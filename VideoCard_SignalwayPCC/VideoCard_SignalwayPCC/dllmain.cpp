// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "Camera6467_plate.h"
#include "coredump/MiniDumper.h"
#include "utilityTool/ToolFunction.h"
#include "Camera6467.h"

bool  g_bLogEnable = false;
int g_iBackupCount = 1;
CRITICAL_SECTION g_csDLL;
CMiniDumper g_MiniDumper(true);

Camera6467_plate* g_CameraArray[CAM_COUNT];
PBYTE g_pImageBuffer = NULL;


bool g_ReadConfig();
void g_WriteConfig();
bool g_IsFileExist( const char* FilePath );
long g_GetFileSize(const char *FileName);
void g_WriteLog(const char*);
void g_ReadKeyValueFromConfigFile(const char* nodeName, const char* keyName, char* keyValue, int bufferSize);

Camera6467* g_CameraArray_Plate[CAM_COUNT];


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    char chLog[256] = { 0 };
    DWORD TID = 0;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
        TID = GetCurrentThreadId();
        
        sprintf_s(chLog, "Video_car DLL_PROCESS_ATTACH current thread ID = %lu\n", TID);
        OutputDebugStringA(chLog);

        for (int i = 0; i < CAM_COUNT; i++)
        {
            g_CameraArray_Plate[i] = NULL;
        }

        for (int i = 0; i < CAM_COUNT; i++)
        {
            g_CameraArray[i] = NULL;
        }
		g_ReadConfig();
		InitializeCriticalSection(&g_csDLL);
        if (!g_pImageBuffer)
        {
            g_pImageBuffer = new BYTE[MAX_IMG_SIZE];
            memset(g_pImageBuffer, 0, MAX_IMG_SIZE);
        }

		break;
	case DLL_THREAD_ATTACH:
        TID = GetCurrentThreadId();
        sprintf_s(chLog, "Video_car  DLL_THREAD_ATTACH current thread ID = %lu\n", TID);
        OutputDebugStringA(chLog);
		break;
	case DLL_THREAD_DETACH:
        TID = GetCurrentThreadId();
        sprintf_s(chLog, "Video_car  DLL_THREAD_DETACH current thread ID = %lu\n", TID);
        OutputDebugStringA(chLog);
		break;
	case DLL_PROCESS_DETACH:
        TID = GetCurrentThreadId();
        sprintf_s(chLog, "Video_car  DLL_PROCESS_DETACH current thread ID = %lu\n", TID);
        OutputDebugStringA(chLog);

        for (int i = 0; i < CAM_COUNT; i++)
        {
            if (g_CameraArray[i])
            {
                g_CameraArray[i]->SetExitStatusVideo();
            }
        }

        //如果在这里delete, 且析构函数中带了stopVideoPlay()，则会发生死锁，原因可能和在dllMain函数中操作有关
        for (int i = 0; i < CAM_COUNT; i++)
        {
            if (g_CameraArray[i])
            {
                delete g_CameraArray[i];
                g_CameraArray[i] = NULL;
            }
        }

        for (int i = 0; i < CAM_COUNT; i++)
        {
            if (g_CameraArray_Plate[i])
            {
                delete g_CameraArray_Plate[i];
                g_CameraArray_Plate[i] = NULL;
            }
        }

		g_WriteConfig();
		DeleteCriticalSection(&g_csDLL);
        if (g_pImageBuffer)
        {
            delete[] g_pImageBuffer;
            g_pImageBuffer = NULL;            
        }
		break;
	}
	return TRUE;
}


long g_GetFileSize(const char *FileName)
{
	//FILE* tmpFile = fopen(FileName, "rb");
	FILE* tmpFile = NULL;
	fopen_s(&tmpFile, FileName, "rb");
	if(tmpFile)
	{
		fseek(tmpFile, 0, SEEK_END);
		long fileSize = ftell(tmpFile);
		fclose(tmpFile);
		tmpFile = NULL;
		return fileSize;
	}
	else
	{
		//"open file failed.";
		return 0;
	}
}

bool g_IsFileExist( const char* FilePath )
{
	if (FilePath == NULL)
	{
		return false;
	}
	FILE* tempFile = NULL;
	bool bRet = false;
	//tempFile = fopen(FilePath, "r");
	fopen_s(&tempFile, FilePath, "r");
	if (tempFile)
	{
		bRet = true;
		fclose(tempFile);
		tempFile = NULL;
	}
	return bRet;
}

bool g_ReadConfig()
{
	char FileName[MAX_PATH];
	GetModuleFileNameA(NULL, FileName, MAX_PATH-1);

	PathRemoveFileSpecA(FileName);
	char iniFileName[MAX_PATH] = {0};
	char iniDeviceInfoName[MAX_PATH] = {0};
	strcat_s(iniFileName, FileName);
	//strcat_s(iniFileName,"\\XLW_Plate.ini");
    strcat_s(iniFileName, INI_FILE_NAME);

	//读取可靠性配置文件
	int iLogEnable =  GetPrivateProfileIntA("Log", "Enable", 0, iniFileName);
	if (iLogEnable == 0 )
	{
		g_bLogEnable = false;
	}
	else
	{
		g_bLogEnable = true;
	}
	g_iBackupCount = GetPrivateProfileIntA("Log", "BackupCount", 1, iniFileName);
	return true;
}

void g_WriteLog(const char* chLog)
{
	g_ReadConfig();
	if (!g_bLogEnable)
	{
		return;
	}
	//取得当前的精确毫秒的时间
	static time_t starttime = time(NULL);
	static DWORD starttick = GetTickCount(); 
	DWORD dwNowTick = GetTickCount() - starttick;
	time_t nowtime = starttime + (time_t)(dwNowTick / 1000);
	//struct tm *pTM = localtime(&nowtime);
	struct tm pTM;
	localtime_s(&pTM, &nowtime);
	DWORD dwMS = dwNowTick % 1000;

	const int MAXPATH = 260;

	TCHAR szFileName[ MAXPATH] = {0};
	GetModuleFileName(NULL, szFileName, MAXPATH);	//取得包括程序名的全路径
	PathRemoveFileSpec(szFileName);				//去掉程序名

	char chLogPath[MAXPATH] = {0};
	//sprintf_s(chLogPath, "%s\\XLWLog\\%d-%02d-%02d\\", szFileName, pTM.tm_year + 1900, pTM.tm_mon +1, pTM.tm_mday);

    char chLogRoot[256] = { 0 };
    Tool_ReadKeyValueFromConfigFile(INI_FILE_NAME, "Log", "Path", chLogRoot, sizeof(chLogRoot));
    if (strlen(chLogRoot) <=0)
    {
        sprintf_s(chLogRoot, sizeof(chLogRoot), "%s\\SW_Log\\", szFileName);
        Tool_WriteKeyValueFromConfigFile(INI_FILE_NAME, "Log", "Path", chLogRoot, sizeof(chLogRoot));
    }
    sprintf_s(chLogPath, "%s\\%d-%02d-%02d\\", chLogRoot, pTM.tm_year + 1900, pTM.tm_mon + 1, pTM.tm_mday);
    //sprintf_s(chLogPath, "D:\\XLWLog\\%d-%02d-%02d\\",  pTM.tm_year + 1900, pTM.tm_mon + 1, pTM.tm_mday);
	MakeSureDirectoryPathExists(chLogPath);

	char chLogFileName[MAXPATH] = {0};
    sprintf_s(chLogFileName, "%s\\%s", chLogPath, DLL_LOG_NAME);

	FILE *file = NULL;
	//file = fopen(chLogFileName, "a+");
	 fopen_s(&file,chLogFileName, "a+");
	if (file)
	{
		fprintf(file,"%04d-%02d-%02d %02d:%02d:%02d:%03d [Version:%s]: %s\n",  pTM.tm_year + 1900, pTM.tm_mon+1, pTM.tm_mday,
			pTM.tm_hour, pTM.tm_min, pTM.tm_sec, dwMS, DLL_VERSION, chLog);
		fclose(file);
		file = NULL;
	}
}

void g_WriteConfig()
{
	char FileName[MAX_PATH];
	GetModuleFileNameA(NULL, FileName, MAX_PATH-1);

	PathRemoveFileSpecA(FileName);
	char iniFileName[MAX_PATH] = {0};
	char iniDeviceInfoName[MAX_PATH] = {0};
	strcat_s(iniFileName, FileName);
	//strcat_s(iniFileName,"\\XLW_Plate.ini");
    strcat_s(iniFileName, INI_FILE_NAME);

	char chTemp[260] = {0};
	//sprintf_s(chTemp, "%d", g_iBackupCount);
	//WritePrivateProfileStringA("Log", "BackupCount", chTemp, iniFileName);

	memset(chTemp, 0 , sizeof(chTemp));
	sprintf_s(chTemp, "%d", g_bLogEnable);
	WritePrivateProfileStringA("Log", "Enable", chTemp, iniFileName);
}

void g_ReadKeyValueFromConfigFile(const char* nodeName, const char* keyName, char* keyValue, int bufferSize)
{
	if (strlen(keyValue) > bufferSize)
	{
		return;
	}
	char FileName[MAX_PATH];
	GetModuleFileNameA(NULL, FileName, MAX_PATH-1);

	PathRemoveFileSpecA(FileName);
	char iniFileName[MAX_PATH] = {0};
	char iniDeviceInfoName[MAX_PATH] = {0};
	strcat_s(iniFileName, FileName);
	//strcat_s(iniFileName,"\\XLW_Plate.ini");
    strcat_s(iniFileName, INI_FILE_NAME);

	GetPrivateProfileStringA(nodeName, keyName, "", keyValue, bufferSize, iniFileName);

	WritePrivateProfileStringA(nodeName, keyName, keyValue, iniFileName);
};

void g_WriteLog_plate(const char* chLog)
{
    g_ReadConfig();
    if (!g_bLogEnable)
    {
        return;
    }
    //取得当前的精确毫秒的时间
    static time_t starttime = time(NULL);
    static DWORD starttick = GetTickCount();
    DWORD dwNowTick = GetTickCount() - starttick;
    time_t nowtime = starttime + (time_t)(dwNowTick / 1000);
    //struct tm *pTM = localtime(&nowtime);
    struct tm pTM;
    localtime_s(&pTM, &nowtime);
    DWORD dwMS = dwNowTick % 1000;

    const int MAXPATH = 260;

    TCHAR szFileName[MAXPATH] = { 0 };
    GetModuleFileName(NULL, szFileName, MAXPATH);	//取得包括程序名的全路径
    PathRemoveFileSpec(szFileName);				//去掉程序名

    char chLogPath[MAXPATH] = { 0 };
    sprintf_s(chLogPath, "%s\\XLWLog\\%d-%02d-%02d\\", szFileName, pTM.tm_year + 1900, pTM.tm_mon + 1, pTM.tm_mday);
    MakeSureDirectoryPathExists(chLogPath);

    char chLogFileName[MAXPATH] = { 0 };
    sprintf_s(chLogFileName, "%s\\XLW_Plate.log", chLogPath);

    FILE *file = NULL;
    //file = fopen(chLogFileName, "a+");
    fopen_s(&file, chLogFileName, "a+");
    if (file)
    {
        fprintf(file, "%04d-%02d-%02d %02d:%02d:%02d:%03d [Version:%s]: %s\n", pTM.tm_year + 1900, pTM.tm_mon + 1, pTM.tm_mday,
            pTM.tm_hour, pTM.tm_min, pTM.tm_sec, dwMS, DLL_VERSION, chLog);
        fclose(file);
        file = NULL;
    }
}
