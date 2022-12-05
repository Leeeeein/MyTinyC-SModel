#include "ClientDefs.h"

int processor(SOCKET _cSock)
{
    char szRecv[16392] = {};
    int nLen = recv(_cSock, (char*)szRecv, sizeof(DataHeader), 0);
    DataHeader* header = (DataHeader*)szRecv;
    if(nLen <= 0)
    {
        printf("client <socket = %ld> exited. task finish. \n", _cSock);
        return -1;
    }
    switch(header->cmd)
    {
        case CMD_LOGIN_RESULT:
        {
            recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
            LoginResult* loginResult = (LoginResult*)szRecv;
            printf("Result CMD_LOGIN_RESULT received from server: %d, data length: %d. \n", 
                loginResult->result, loginResult->dh.dataLength);
            break;
        }
        case CMD_LOGOUT_RESULT:
        {
            recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
            LogoutResult* logoutResult = (LogoutResult*)szRecv;
            printf("Result CMD_LOGOUT_RESULT received from server: %d, data length: %d \n", 
                logoutResult->result, logoutResult->dh.dataLength);
            break;
        }
        case CMD_NEW_USER_JOIN:
        {
            recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
            NewUserJoin* newUserJoin = (NewUserJoin*)szRecv;
            printf("Result CMD_NEW_USER_JOIN received from server, <SOCKET = %d>, data length: %d \n",
                newUserJoin->sock, newUserJoin->dh.dataLength);
            break;
        }
        case CMD_COMMON_FILE:
        {
            break;
        }
        case CMD_COMMON_MESSAGE:
        {
            recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
            CommonMessage* commonMessage = (CommonMessage*)szRecv;
            printf("Result CMD_COMMON_MESSAGE received from server. \n");
            printf("%s", commonMessage->content);
            
            FILE* fp = fopen("/home/liyinzhe/Workspace/MyTinyC-SModel/src/Client/b.txt","w");
            for(int i = 0; i < strlen(commonMessage->content)-1; i++)
            {
                fprintf(fp, "%c", commonMessage->content[i]);
            }
            // for(int i = 0; i < 100; i++)
            // {
            //     fprintf(fp, "%s", "a");
            // }
            fclose(fp);
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

int g_bExit = 1;
void* commMessThread(SOCKET* _cSock)
{
    char sendBuf[128];
    while(1)
    {
        scanf("%s", sendBuf);
        if(0 == strcmp(sendBuf, "login"))
        {
            Login login;
            initLogin(&login, "leeeeein", "19940901");
            send(*_cSock, (const char*)&login, sizeof(Login), 0);
        }
        else if(0 == strcmp(sendBuf, "logout"))
        {
            Logout logout;
            initLogout(&logout, "liyinzhe");
            send(*_cSock, (char*)&logout, sizeof(Logout), 0);
        }
        else if(0 == strcmp(sendBuf, "exit"))
        {
            g_bExit = 0;
        }
    }
}

int main()
{
    cJSON* t_jRoot = cJSON_FromFile("../../config/configClient.json"); // start cJson module
    int _cSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    pthread_t id;

    pthread_create(&id, NULL, commMessThread, &_cSock);
    pthread_detach(id);
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
    _sin.sin_port = htons(cJson_GetInt(t_jRoot, "port"));
    _sin.sin_addr.s_addr = inet_addr(cJson_GetCharArr(t_jRoot, "ip"));
    int ret = connect(_cSock, (struct sockaddr*)&_sin, sizeof(struct sockaddr_in));
    if(-1 == ret)
    {
        printf("establishing connection failed.\n");
    }
    else
    {
        printf("establishing connection success, ret: %d.\n", ret);
    }
    int32_t t_timer = 0;
    while (g_bExit)
    {
        fd_set fdRead;
        FD_ZERO(&fdRead);
        FD_SET(_cSock, &fdRead);
        struct timeval t = {2, 0};
        int ret = select(_cSock+1, &fdRead, 0, 0, &t);
        if(ret < 0)
        {
            printf("select task finished. \n");
            break;
        }
        if(FD_ISSET(_cSock, &fdRead))
        {
            FD_CLR(_cSock, &fdRead);
            if(-1 == processor(_cSock))
            {
                printf("select task finished. \n");
                break;
            }
        }
        // printf("other tasks are processed during the free time. %d\n", ++t_timer);
    }
    close(_cSock);
    printf("close socket.\n");
    return 0;


}
