#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "../defs.h"

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
        else if(0 == strcmp(cmdBuf, "login"))
        {
            Login login;
            initLogin(&login, "liyinzhe", "19940901");
            write(_cSock, (const char*)&login, sizeof(login));
            LoginResult loginResult;
            read(_cSock, (char*)&loginResult, sizeof(LoginResult));
            printf("Login result: %d. \n", loginResult.result);
        }
        else if(0 == strcmp(cmdBuf, "logout"))
        {
            Logout logout;
            initLogout(&logout, "liyinzhe");
            write(_cSock, (const char*)&logout, sizeof(logout));
            LogoutResult logoutResult;
            read(_cSock, (char*)&logoutResult, sizeof(LogoutResult));
            printf("Logout result: %d. \n", logoutResult.result);
        }
        else
        {
            printf("Input error, please try again. \n");
        }
    }
    
    close(_cSock);
    printf("close socket.\n");
    return 0;


}
