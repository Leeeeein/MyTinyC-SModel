#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "../defs.h"
#include "../../components/source/utils/json.c"
#include "../../components/source/utils/dynarr.c"

DynArr_t(SOCKET) g_clients; // a global container for clients' sockets.

int processor(SOCKET _cSock)
{
    char szRecv[1024] = {};
    int nLen = read(_cSock, (char*)szRecv, sizeof(DataHeader));
    DataHeader* header = (DataHeader*)szRecv;
    if(nLen <= 0)
    {
        printf("client exit. task finish. \n");
        return -1;
    }
    switch(header->cmd)
    {
        case CMD_LOGIN:
        {
            Login login = {};
            read(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader));
            printf("Command received: login. username: %s, password: %s. \n", login.username, login.password);
            LoginResult ret;
            initLoginResult(&ret);
            write(_cSock, (char*)&ret, sizeof(LoginResult));
            break;
        }
        case CMD_LOGOUT:
        {
            Logout logout = {};
            read(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader));
            printf("Command received: logout, username: %s. \n", logout.username);
            LogoutResult ret;
            initLogoutResult(&ret);
            write(_cSock, (char*)&ret, sizeof(LogoutResult));
            break;
        }
        default:
        {
            DataHeader header;
            header.cmd = CMD_ERROR;
            header.dataLength = 0;
            write(_cSock, (char*)&header, sizeof(DataHeader));
        }
    }
}

int main()
{
    cJSON* cjson = cJSON_FromFile("../../config/configServer.json");
    printf("======== %s ===========\n", cjson->child->next->name);
    dynarrInitZero(&g_clients); // initializing the container
    SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in _sin = {};
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(4599);
    _sin.sin_addr.s_addr = htonl(INADDR_ANY);
    if(-1 == bind(_sock, (struct sockaddr*)&_sin, sizeof(_sin)))
    {
        printf("bind failed.\n");
    }
    else
        printf("bind success.\n");
    int ret = listen(_sock, 5);
    if(-1 == ret)
    {
        printf("listen failed.\n");
    }
    else
    {
        printf("listen success.\n");
    }
    int _cSock = -1;

    char _recvBuf[128] = {};
    while(1)
    {
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExp;

        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);

        FD_SET(_sock, &fdRead);
        FD_SET(_sock, &fdWrite);
        FD_SET(_sock, &fdExp);

        // for(size_t n = 0; n < g_clients.len; n++)
        for(size_t n = g_clients.len-1; n >=0; n--)
        {
            FD_SET(g_clients.buf[n], &fdRead);
        }
        // nfds is a int value that refers to the 
        // range of all sockets rather than the number.
        int ret = select(_sock+1, &fdRead, &fdWrite, &fdExp, NULL);
        if(ret < 0)
        {
            printf("select task ends. \n");
            break;
        }
        if(FD_ISSET(_sock, &fdRead))
        {
            FD_CLR(_sock, &fdRead);
            struct sockaddr_in clientAddr = {};
            int nAddrLen = (int)(sizeof(struct sockaddr_in));
            // char msgBuf[] = "Hello, I'm server.";

            _cSock = accept(_sock, (struct sockaddr*)&clientAddr, &nAddrLen);
            if(-1 == _cSock)
            {
                printf("receive invalid client.\n");
            }
            else
            {
                int appendRet = 0;
                dynarrInsert(&g_clients, g_clients.len, _cSock, appendRet);
                printf("found a new client, ip: %s\n", inet_ntoa(clientAddr.sin_addr));
                printf("this is a test line, length of container: %d, cSock: %d, appendRet: %d. \n", g_clients.len, _cSock, appendRet);
            }
        }
        // for(size_t n = fdRead.__fds_bits.; n >=0; n--)
        // {fdRead
        //     if(-1 == processor(g_clients.buf[n]))
        //     {
        //         SOCKET idxRet = -1;
                
        //     }
        // }
    }
    close(_sock);
    return 0;
}
