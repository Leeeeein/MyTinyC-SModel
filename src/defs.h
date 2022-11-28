#include <string.h>

#define SOCKET int64_t
enum CMD
{
    CMD_LOGIN = 0,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
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
