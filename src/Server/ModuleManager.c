//
//  Created by Li Yinzhe
//  Unified initialization for modules with different functions.
//
#pragma once
#include "ServerDefs.h"
#include "ThreadsManager.c"

DynArr_t(SOCKET) g_clients; // a global container for clients' sockets.
static SOCKET _sock, _cSock;
static SOCKET maxSock;
static fd_set g_fdRead;  // decalre three file description set for reading,writing and exception respectively
static fd_set g_fdWrite;
static fd_set g_fdExp;

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
    LogInfo("Log file is initialized...");
    LogInfo("Config module is launching...");
    return 0;
}

int netModuleInit()
{
    _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // create a server socket
    struct sockaddr_in _sin = {};
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(cJson_GetInt(s_jRoot, "port"));
    _sin.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt( _sock, SOL_SOCKET,SO_REUSEADDR, (const void *)&opt, sizeof(opt) ); // set port reusable
                    
    if(-1 == bind(_sock, (struct sockaddr*)&_sin, sizeof(_sin))) // bind socket with network address
    {
        LogErr("Bind failed.");
    }
    else
    {
        LogInfo("Bind succeeded.");
    }

    int ret = listen(_sock, cJson_GetInt(s_jRoot, "max_listen_num")); // set this process as a server process
    if(-1 == ret)
    {
        LogErr("Listen failed.");
    }
    else
    {
        LogInfo("Listen success.");
    }

    _cSock = -1; // declare a socket name to refer to a client.
    dynarrInitZero(&g_clients); // initializing the container of clients
    return ret;
}

void fdSetOperation()
{
    FD_ZERO(&g_fdRead); // clear each fd set
    FD_ZERO(&g_fdWrite);
    FD_ZERO(&g_fdExp);

    FD_SET(_sock, &g_fdRead); // put server socket into each fd set
    FD_SET(_sock, &g_fdWrite);
    FD_SET(_sock, &g_fdExp);
}

void selectInit(struct timeval* tv, fd_set* fdr, fd_set* fdw, fd_set* fde)
{
    
    maxSock = _sock; // specify a max socket number for select() 
    // nfds is a int value that refers to the range of all sockets rather than the number
    for(int n = (int)g_clients.len-1; n >=0; n--)
    {
        FD_SET(g_clients.buf[n], fdr);
        if(maxSock < g_clients.buf[n])
        {
            maxSock = g_clients.buf[n];
        }
    }
    tv->tv_sec = 1; // specify a timeout period
    tv->tv_usec = 0;

    int ret = select(maxSock+1, fdr, fdw, fde, tv);
    if(ret < 0)
    {
        LogErr("select task ends.");
    }
    // return;
    if(FD_ISSET(_sock, &g_fdRead)) // select() filters out sockets that cannot be operated,
        // use the macro to check whether the server socket is still in the set
        {
            FD_CLR(_sock, &g_fdRead); //remove socket from the set for next epoch 
            struct sockaddr_in clientAddr = {};
            int nAddrLen = (int)(sizeof(struct sockaddr_in));
            _cSock = accept(_sock, (struct sockaddr*)&clientAddr, (socklen_t*)&nAddrLen); // fetch the first client from server's queue and design this name to it
            if(-1 == _cSock)
            {
                LogErr("receive invalid client.");
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
                LogInfo("found a new client, ip: %s", inet_ntoa(clientAddr.sin_addr));
            }
        }
    return;
}

void netModuleUninit()
{
    for(int n = g_clients.len-1; n >= 0; n--)
    {
        close(g_clients.buf[n]); // close all clients' sockets
    }
    close(_sock); // close local socket
}

void threadsInit(pthread_t* id1)
{
    pthread_create(id1, NULL, cmdRecvThread, &g_clients);
    // pthread_create(id2, NULL, )
    pthread_detach((pthread_t)(&id1));
}