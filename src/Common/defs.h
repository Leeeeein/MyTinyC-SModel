#pragma once

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <bits/time.h>
#include <sys/socket.h>
#include <sys/select.h>

#include "../../components/source/utils/json.c"
#include "../../components/source/utils/dynarr.c"
#include "../../components/source/sysapi/file.c"
#include "../../components/source/sysapi/ipc.c"
#include "../../components/source/sysapi/error.c"
#include "../../components/source/utils/log.c"
#include "../../components/source/utils/cstring.c"
#include "../../components/source/datastructs/list.c"
#include "../../components/source/sysapi/ctime.c"

static Log_t s_Log;
static cJSON* s_jRoot;
#define LogErr(str, ...)    logErr(&s_Log, str,  ##__VA_ARGS__);
#define LogWarn(str, ...)   logWarning(&s_Log, str,  ##__VA_ARGS__);
#define LogInfo(str, ...)   logInfo(&s_Log, str,  ##__VA_ARGS__);
#define LogDebug(str, ...)  logDebug(&s_Log, str,  ##__VA_ARGS__);
#define LogNotice(str, ...) logNotice(&s_Log, str,  ##__VA_ARGS__);

#define SOCKET int64_t
enum CMD
{
    CMD_LOGIN = 0,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_NEW_USER_JOIN,
    CMD_ERROR,
    CMD_COMMON_MESSAGE,
    CMD_COMMON_FILE,
};

typedef struct
{
    short dataLength;
    short cmd;
}DataHeader;

typedef struct
{
    DataHeader dh;
    char username[32];
    char password[32];
}Login;
void initLogin(Login* login, const char* username, const char* password)
{
    login->dh.dataLength = sizeof(Login);
    login->dh.cmd = CMD_LOGIN;
    strcpy(login->username, username);
    strcpy(login->password, password);
}

typedef struct
{
    DataHeader dh;
    int result;
}LoginResult;
void initLoginResult(LoginResult* loginResult)
{
    loginResult->dh.dataLength = sizeof(LoginResult);
    loginResult->dh.cmd = CMD_LOGIN_RESULT;
    loginResult->result = 0;
}

typedef struct
{
    DataHeader dh;
    char username[32];
}Logout;
void initLogout(Logout* logout, const char* username)
{
    logout->dh.dataLength = sizeof(Logout);
    logout->dh.cmd = CMD_LOGOUT;
    strcpy(logout->username, username);
}

typedef struct
{
    DataHeader dh;
    int result;
}LogoutResult;
void initLogoutResult(LogoutResult* logoutResult)
{
    logoutResult->dh.dataLength = sizeof(LogoutResult);
    logoutResult->dh.cmd = CMD_LOGOUT_RESULT;
    logoutResult->result = 0;
}

typedef struct
{
    DataHeader dh;
    int result;
    int sock;
}NewUserJoin;
void initNewUserJoin(NewUserJoin* newUserJoin)
{
    newUserJoin->dh.dataLength = sizeof(NewUserJoin);
    newUserJoin->dh.cmd = CMD_NEW_USER_JOIN;
    newUserJoin->sock = 0;
}

typedef struct
{
    DataHeader dh;
    int result;
    char content[8196];
}CommonMessage;
void initCommonMessage(CommonMessage* commMess)
{
    commMess->dh.dataLength = sizeof(CommonMessage);
    commMess->dh.cmd = CMD_COMMON_MESSAGE;
    commMess->result = 1;
}

typedef struct
{
    DataHeader dh;
    char content[2048];
}CommonFile;
void initCommonFile(CommonFile* commFile, int len, const char* src)
{
    strcpy(commFile->content, src);
    commFile->dh.dataLength = sizeof(CommonFile);
    commFile->dh.cmd = CMD_COMMON_FILE;
}