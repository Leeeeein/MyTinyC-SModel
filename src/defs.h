#pragma once
#include <string.h>
#include "../components/source/utils/json.c"
#include "../components/source/utils/dynarr.c"
#include "../components/source/sysapi/file.c"
#include "../components/source/sysapi/ipc.c"
#include "../components/source/sysapi/error.c"
#include "../components/source/utils/log.c"
#include "../components/source/utils/cstring.c"
#include "../components/source/datastructs/list.c"
#include "../components/source/sysapi/ctime.c"

#define SOCKET int64_t
enum CMD
{
    CMD_LOGIN = 0,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_NEW_USER_JOIN,
    CMD_ERROR
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