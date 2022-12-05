//
//  Created by Li Yinzhe
//  Unified management for thread functions of server.
//
#pragma once
#include "ServerDefs.h"
#include "dirent.h"
#include "Tools.c"

void* cmdRecvThread(DynArr_t(SOCKET)* clientsPtr)
{
    char sendBuf[64];
    char path[128];
    int ret;
    char ch;
    DynArr_t(char*) setOfFileAndDir;
    dynarrInitZero(&setOfFileAndDir);
    FILE* fp = NULL;
    char * data;
    while(1)
    {   
        scanf("%s", sendBuf);
        if(0 == strcmp(sendBuf, "exit"))
            s_exitFlag = true;
        else if(0 == strcmp(sendBuf, "show"))
        {
            showTerminalPath(path);
        }
        else if(0 == strcmp(sendBuf, "select"))
        {
            selcFileToSend(path, &setOfFileAndDir, ret);
        }
        else if(0 == strcmp(sendBuf, "send"))
        {
            scanFilesAndSend(&setOfFileAndDir, fp, data, clientsPtr);
        }
        else
        {   LogInfo(" 一一一一一一一一一一一一一一一一一一一一");
            LogInfo("|    A wrong command is entered.");
            LogInfo("|    Enter 'exit' for exiting.");
            LogInfo("|    Enter 'show' for showing all files in the specified path.");
            LogInfo(" 一一一一一一一一一一一一一一一一一一一一");
        }
    }
    free(fp);
    free(data);
}

void* fileOpThread()
{

}