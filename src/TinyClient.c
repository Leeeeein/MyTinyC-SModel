#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    int _cSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(-1 == _cSock)
    {
        printf("creating socket failed.\n");
    }
    else
    {
        printf("creating socket success.\n");
    }
    struct sockaddr_in _sin = {};
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(4599);
    _sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = connect(_cSock, (struct sockaddr*)&_sin, sizeof(struct sockaddr_in));
    if(-1 == ret)
    {
        printf("establishing connection failed.\n");
    }
    else
    {
        printf("establishing connection success, ret: %d.\n", ret);
    }
    
    while (1)
    {
        /* code */
        char cmdBuf[128] = {};
        scanf("%s", cmdBuf);  
        if(0 == strcmp(cmdBuf, "exit"))
        {
            break;
        }
        else
        {
            write(_cSock, cmdBuf, strlen(cmdBuf)+1);
        }

        char recvBuf[128] = {};
        ret = read(_cSock, recvBuf, 128);
        if(ret > 0)
        {
            printf("received data: %s", recvBuf);
        }
    }
    
    close(_cSock);
    printf("close socket.\n");
    return 0;


}
