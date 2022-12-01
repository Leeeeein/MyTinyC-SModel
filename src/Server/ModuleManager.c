//
//  Created by Li Yinzhe
//  Unified initialization for modules with different functions.
//

#include "../Common/defs.h"

int cfgModuleInit()
{
    s_jRoot = cJSON_FromFile("../../config/configServer.json"); // start cJson module
    return 0;
}

int logModuleInit()
{
    time_t tt = time(0);
    char s[32];
    strftime(s, sizeof(s), "%Y%m%d%H%M%S", localtime(&tt));
    char* path = cJson_GetCharArr(s_jRoot, "log_path");
    strcat(path, s);
    if (!logInit(&s_Log, "", path)) {
         printf("log initialize failed. \n");
	 	return -1;
	}
    LogInfo("Log file is initialized...");
    LogInfo("Config module is launching...");
    return 0;
}