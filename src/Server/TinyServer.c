#include "ServerDefs.h"
#include "ModuleManager.c"
#include "ThreadsManager.c"
#include "Tools.c"

int main()
{
    cfgModuleInit();
    logModuleInit();
    netModuleInit();
    LogInfo("Server is launching...");
    pthread_t idExitServer;
    threadsInit(&idExitServer);
    while(!s_exitFlag)
    {
        
        fdSetOperation();
        struct timeval t;
        selectInit(&t, &g_fdRead, &g_fdWrite, &g_fdExp);
        extClientFilter(&g_clients, &g_fdRead);
    }
    netModuleUninit();

    return 0;
}
