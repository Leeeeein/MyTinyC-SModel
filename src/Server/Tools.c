#pragma once
#include "../Common/defs.h"
#include "ModuleManager.c"

int processor(SOCKET _cSock)
{
    char szRecv[1024] = {};
    int nLen = read(_cSock, (char*)szRecv, sizeof(DataHeader));
    DataHeader* header = (DataHeader*)szRecv;
    if(nLen <= 0)
    {
        LogInfo("client <socket = %ld> exited. task finish.", _cSock);
        return -1;
    }
    switch(header->cmd)
    {
        case CMD_LOGIN:
        {
            read(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader));
            Login* login = (Login*)szRecv;
            LogInfo("Command received from <socket = %ld>: login. username: %s, password: %s.", 
                _cSock, login->username, login->password);
            LoginResult ret;
            initLoginResult(&ret);
            int retN = send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
            LogInfo("%d bytes was written.", retN);
            break;
        }
        case CMD_LOGOUT:
        {
            read(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader));
            Logout* logout = (Logout*)szRecv;
            LogInfo("Command received from <socket = %ld>: logout, username: %s.", 
                _cSock, logout->username);
            LogoutResult ret;
            initLogoutResult(&ret);
            send(_cSock, (char*)&ret, sizeof(LogoutResult), 0);
            break;
        }
        case CMD_COMMON_MESSAGE:
        {
            // read()
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

void extClientFilter(DynArr_t(SOCKET)* arrClients, fd_set* fds)
{
    #pragma region FILTER_EXITED_CLIENTS_ID
        /*
        the block below is a procedure that filter out the idx of clients that have exited.
        locating elements and remove in a dynArray.
        */
        DynArr_t(int) t_idxToRm;
        dynarrInitZero(&t_idxToRm);
        int32_t t_ret = 0, t_cnt = 0, t_ptr = 0;
        for(int n = 0; n < (int)arrClients->len; n++)
        {
            if(FD_ISSET(arrClients->buf[n], fds))
            {
                LogInfo("A client is processing. %d", arrClients->len);
                if(-1 == processor(arrClients->buf[n]))
                {
                    dynarrInsert(&t_idxToRm, t_idxToRm.len, n, t_ret);
                }
            }
        }
        if(t_idxToRm.len == 0)
            return;
        for(int n = 0; n < (int)arrClients->len; n++)
        {
            arrClients->buf[n-t_cnt] = arrClients->buf[n];
            if(t_idxToRm.len != 0 && n == t_idxToRm.buf[t_ptr])
            {
                t_cnt += 1;
                t_ptr += 1;
            }
        }
        dynarrResize(arrClients, arrClients->len-t_cnt);
#pragma endregion FILTER_EXITED_CLIENTS_ID
}

void showTerminalPath(char* path)
{
    char pt[128];
    strcpy(path, "ls -a ");
    scanf("%s", pt);
    strcat(path, pt);
    system(path);
}

void selcFileToSend(char* path, DynArr_t(char*)* setOfFileAndDir, int ret)
{
    LogInfo("Input paths of files to send, enter 'EOS' to end.");
    scanf("%s", path);
    dynarrInsert(setOfFileAndDir, setOfFileAndDir->len, path, ret);
}

bool scanFileAndSend(const char* path, FILE* fp, char* dataBuf, DynArr_t(SOCKET)* clients)
{
    fp = fopen(path, "rb");
    if(fp == NULL)
    {
        LogErr("read file failed.");
        return false;
    }
    else
    {
        LogDebug("read file succeeded.");
    }
    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    dataBuf = (char *)malloc((length + 1) * sizeof(char));
    rewind(fp);
    length = fread(dataBuf, 1, length, fp);
    dataBuf[length] = '\0';
    fclose(fp);
    // LogInfo("\n %s", dataBuf);
    LogInfo("QAQAQAQAQAQAQAQAQAQ file length: %d", length);
    CommonMessage cmf; 
    initCommonMessage(&cmf);
    //strcpy(cmf.content, (const char*)dataBuf);

    for(SOCKET s = 0; s < clients->len; s++)
    {
        LogInfo("Message is sent to a client: <Socket: %d>", clients->buf[s]);
        // send(clients->buf[s], (char*)&cmf, sizeof(CommonMessage), 0);
    }
    return true;
}

void scanFilesAndSend(DynArr_t(char*)* sof, FILE* fp, char* dataBuf, DynArr_t(SOCKET)* clients)
{
    for(int i = 0; i < sof->len; i++)
    {
        bool ret = scanFileAndSend(sof->buf[i], fp, dataBuf, (DynArr_t(SOCKET)*)clients);
    }
    dynarrFreeMemory(sof);
}