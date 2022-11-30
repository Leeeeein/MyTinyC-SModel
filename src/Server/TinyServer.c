#include <sys/time.h>
#include <bits/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "../defs.h"

DynArr_t(SOCKET) g_clients; // a global container for clients' sockets.

int processor(SOCKET _cSock)
{
    char szRecv[1024] = {};
    int nLen = read(_cSock, (char*)szRecv, sizeof(DataHeader));
    DataHeader* header = (DataHeader*)szRecv;
    if(nLen <= 0)
    {
        printf("client <socket = %ld> exited. task finish. \n", _cSock);
        return -1;
    }
    switch(header->cmd)
    {
        case CMD_LOGIN:
        {
            read(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader));
            Login* login = (Login*)szRecv;
            printf("Command received from <socket = %ld>: login. username: %s, password: %s. \n", 
                _cSock, login->username, login->password);
            LoginResult ret;
            initLoginResult(&ret);
            int retN = send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
            printf("%d bytes was written. \n", retN);
            break;
        }
        case CMD_LOGOUT:
        {
            read(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader));
            Logout* logout = (Logout*)szRecv;
            printf("Command received from <socket = %ld>: logout, username: %s. \n", 
                _cSock, logout->username);
            LogoutResult ret;
            initLogoutResult(&ret);
            send(_cSock, (char*)&ret, sizeof(LogoutResult), 0);
            break;
        }
        default:
        {
            DataHeader header;
            header.cmd = CMD_ERROR;
            header.dataLength = 0;
            send(_cSock, (char*)&header, sizeof(DataHeader), 0);
        }
    }
}

static Log_t s_Log;
static cJSON* s_jRoot;
int cfgModuleInit()
{
    s_jRoot = cJSON_FromFile("../../config/configServer.json"); // start cJson module
    return 0;
}

int logModuleInit()
{
    time_t tt = time(0);
    char s[32];
    strftime(s, sizeof(s), "%Y%m%d%H%M%S", localtime(&tt));
    char* path = cJson_GetCharArr(s_jRoot, "log_path");
    strcat(path, s);
    if (!logInit(&s_Log, "", path)) {
        printf("log initialize failed. \n");
		return -1;
	}
    s_Log.m_maxfilesize = 1;//cJson_GetInt(s_jRoot, "max_log_length");
    logInfo(&s_Log, "log file is initialized. \n");
    return 0;
}

int main():
{
    printf("main entry ready.\n");
    cfgModuleInit();
    logModuleInit();
    Log_t t_Log, *t2;
    
    dynarrInitZero(&g_clients); // initializing the container of clients

    SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // create a server socket
    struct sockaddr_in _sin = {};
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(cJson_GetInt(s_jRoot, "port"));
    _sin.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt( _sock, SOL_SOCKET,SO_REUSEADDR, (const void *)&opt, sizeof(opt) ); // set port reusable
                    
    if(-1 == bind(_sock, (struct sockaddr*)&_sin, sizeof(_sin))) // bind socket with network address
    {
        printf("bind failed.\n");
    }
    else
    {
        printf("bind success.\n");
    }

    int ret = listen(_sock, cJson_GetInt(s_jRoot, "max_listen_num")); // set this process as a server process
    if(-1 == ret)
    {
        printf("listen failed.\n");
    }
    else
    {
        printf("listen success.\n");
    }

    int _cSock = -1; // declare a socket name to refer to a client.
    while(1)
    {
        fd_set fdRead;  // decalre three file description set for reading,writing and exception respectively
        fd_set fdWrite;
        fd_set fdExp;

        FD_ZERO(&fdRead); // clear each fd set
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);

        FD_SET(_sock, &fdRead); // put server socket into each fd set
        FD_SET(_sock, &fdWrite);
        FD_SET(_sock, &fdExp);

        SOCKET maxSock = _sock; // specify a max socket number for select() 
        // nfds is a int value that refers to the range of all sockets rather than the number
        for(int n = (int)g_clients.len-1; n >=0; n--)
        {
            FD_SET(g_clients.buf[n], &fdRead);
            if(maxSock < g_clients.buf[n])
            {
                maxSock = g_clients.buf[n];
            }
        }

        struct timeval t = {2, 0}; // specify a timeout period
        int ret = select(maxSock+1, &fdRead, &fdWrite, &fdExp, &t);
        if(ret < 0)
        {
            printf("select task ends. \n");
            break;
        }

        if(FD_ISSET(_sock, &fdRead)) // select() filters out sockets that cannot be operated,
        // use this macro to check whether the server socket is still in the set
        {
            FD_CLR(_sock, &fdRead); //remove socket from the set for next epoch 
            struct sockaddr_in clientAddr = {};
            int nAddrLen = (int)(sizeof(struct sockaddr_in));
            _cSock = accept(_sock, (struct sockaddr*)&clientAddr, (socklen_t*)&nAddrLen); // fetch the first client from server's queue and design this name to it
            if(-1 == _cSock)
            {
                printf("receive invalid client. \n");
            }
            else
            {
                for(int n = (int)g_clients.len - 1; n >= 0; n--) //inform all connected clients that a new client joins 
                {
                    NewUserJoin userJoin;
                    initNewUserJoin(&userJoin);
                    send(g_clients.buf[n], (const char*)&userJoin, sizeof(NewUserJoin), 0);
                }
                int appendRet = 0;
                dynarrInsert(&g_clients, g_clients.len, _cSock, appendRet);
                printf("found a new client, ip: %s\n", inet_ntoa(clientAddr.sin_addr));
            }
        }
#pragma region FILTER_EXITED_CLIENTS_ID
        /*
        the block below is a procedure that filter out the idx of clients that have exited.
        locating elements and remove in a dynArray.
        */
        DynArr_t(int) t_idxToRm;
        dynarrInitZero(&t_idxToRm);
        int32_t t_ret = 0, t_cnt = 0, t_ptr = 0;
        for(int n = 0; n < (int)g_clients.len; n++)
        {
            if(FD_ISSET(g_clients.buf[n], &fdRead))
            {
                printf("a client is processing. \n");
                if(-1 == processor(g_clients.buf[n]))
                {
                    dynarrInsert(&t_idxToRm, t_idxToRm.len, n, t_ret);
                }
            }
        }
        for(int n = 0; n < (int)g_clients.len; n++)
        {
            g_clients.buf[n-t_cnt] = g_clients.buf[n];
            if(t_idxToRm.len != 0 && n == t_idxToRm.buf[t_ptr])
            {
                t_cnt += 1;
                t_ptr += 1;
            }
        }
        dynarrResize(&g_clients, g_clients.len-t_cnt);
#pragma endregion FILTER_EXITED_CLIENTS_ID


        printf("other tasks are processed during the free time. \n");
    }
    for(int n = g_clients.len-1; n >= 0; n--)
    {
        close(g_clients.buf[n]); // close all clients' sockets
    }
    close(_sock); // close local socket
    printf("server exited. \n");
    return 0;
}
