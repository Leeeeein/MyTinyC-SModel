#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "../components/header/utils/json.h"

int main()
{
    // cJSON* cjson = cJSON_FromFile("../config/configServer.json");
    // printf("======== %s ===========\n", cjson->child->next->name);
    int _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

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
        printf("found a new client, ip: %s\n", inet_ntoa(clientAddr.sin_addr));
    }

    char _recvBuf[128] = {};
    while(1)
    {
        int nLen = read(_cSock, _recvBuf, 128);
        if(nLen <= 0)
        {
            printf("client exit. task finish.");
            break;
        }

        if(0 == strcmp(_recvBuf, "getName"))
        {
            char msgBuf[] = "louis";
            write(_cSock, msgBuf, strlen(msgBuf)+1);
        }
        else if(0 == strcmp(_recvBuf, "getAge"))
        {
            char msgBuf[] = "15";
            write(_cSock, msgBuf, strlen(msgBuf)+1);
        }
        else
        {
            char msgBuf[] = "???";
            write(_cSock, msgBuf, strlen(msgBuf)+1);
        }

    }
    close(_sock);
    return 0;
}
