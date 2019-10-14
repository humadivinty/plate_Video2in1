// VideoCard_SignalwayPCC.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "VideoCard_SignalwayPCC.h"
#include "Camera6467_plate.h"
#include "Camera6467.h"
#include "ToolFunction.h"
#include <process.h>

#define OVERLAY_MAX_ROW 8
#define OVERLAY_MAX_COL 32

extern void g_WriteLog_plate(const char*);
extern void g_WriteLog(const char* chLog);
extern void g_ReadKeyValueFromConfigFile(const char* nodeName, const char* keyName, char* keyValue, int bufferSize);

extern Camera6467_plate* g_CameraArray[];
extern PBYTE g_pImageBuffer;

HANDLE g_hThreadDeleteLog = NULL;
bool g_bExit = false;
unsigned int __stdcall  StatusDeleteLogThread(LPVOID lpParam);

bool g_bIsInit;
void CheckInitCamera();

char g_chOverlay[OVERLAY_MAX_ROW][OVERLAY_MAX_COL] = { 0 };

typedef struct ThreadParameter
{
    ~ThreadParameter()
    {
        g_bIsInit = false;
    }
};
ThreadParameter ThreadExit;

void CheckInitCamera()
{
    if (!g_bIsInit)
    {
        for (int i = 0; i < CAM_COUNT; i++)
        {
            g_CameraArray[i] = NULL;
        }
        g_bIsInit = true;
    }
}
void InitializationOverlayText()
{
    for (int i = 0; i < OVERLAY_MAX_ROW; i++)
    {
        for (int j = 0; j < OVERLAY_MAX_COL; j++)
        {
            g_chOverlay[i][j] = 32;
        }
    }
}

unsigned int __stdcall  StatusDeleteLogThread(LPVOID lpParam)
{
    DWORD TID = GetCurrentThreadId();
    char chLog[256] = { 0 };
    sprintf_s(chLog, "StatusDeleteLogThread current thread ID = %lu\n", TID);
    OutputDebugStringA(chLog);

    char chLogPath[256] = { 0 };
    char chLogHoldDay[256] = { 0 };
    g_ReadKeyValueFromConfigFile("Log", "Path", chLogPath, sizeof(chLogPath));
    g_ReadKeyValueFromConfigFile("Log", "HoldDay", chLogHoldDay, sizeof(chLogPath));
    int iHoldDay = 10;
    if (strlen(chLogHoldDay) > 0)
    {
        iHoldDay = atoi(chLogHoldDay);
    }
    iHoldDay = iHoldDay > 0 ? iHoldDay : 30;

    while (!g_bExit)
    {
        if (strlen(chLogPath) > 0)
        {
            CirclelaryDelete(chLogPath, iHoldDay);
        }
        Sleep(500);
    }
    return 0;
}

extern Camera6467* g_CameraArray_Plate[];
void CheckInitCamera_plate()
{
    if (!g_bIsInit)
    {
        for (int i = 0; i < CAM_COUNT; i++)
        {
            g_CameraArray_Plate[i] = NULL;
        }
        g_bIsInit = true;
    }
}



D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_Init(int nType, char* sParas)
{
    CHAR chLog[256] = { 0 };
    MY_SPRINTF(chLog, sizeof(chLog), "VC_Init begin, nType = %d, sParas = %s", nType, sParas);
    g_WriteLog(chLog);
    CheckInitCamera();
    InitializationOverlayText();

    g_bExit = false;
    if (g_hThreadDeleteLog == NULL)
    {
        g_WriteLog("Begin to create log thread.");
        g_hThreadDeleteLog = (HANDLE)_beginthreadex(NULL, 0, StatusDeleteLogThread, NULL, 0, NULL);
        g_WriteLog("Create log thread finish.");
    }

    if (nType == 0)
    {
        g_WriteLog("This device supports Network mode only.");
        return -1000;
    }
    if (strlen(sParas) < 8)
    {
        g_WriteLog("the Parameters is invalid, length less than 8.");
        return -1000;
    }

    char chDeviceIP[64] = { 0 };
    std::string strSrc(sParas);
    std::size_t iPos = strSrc.find(",");
    if (iPos != std::string::npos)
    {
        std::string strSub = strSrc.substr(0, iPos);
        MY_SPRINTF(chDeviceIP, sizeof(chDeviceIP), "%s", strSub.c_str());
        g_WriteLog(chDeviceIP);
    }
    else
    {
        g_WriteLog("the Parameters is invalid, can`t find IP address.");
        return -1000;
    }
    int iCheckIp = g_checkIP(chDeviceIP);
    if (iCheckIp != 1)
    {
        g_WriteLog("the Parameters is invalid,  IP address is invalid.");
        return -1000;
    }

    Camera6467_plate* pCamera = NULL;
    std::list<Camera6467_plate*>::iterator it;
    bool bFind = false;
    int iIndex = -1;
    for (int i = 0; i < CAM_COUNT; i++)
    {
        if (g_CameraArray[i])
        {
            if (strcmp(g_CameraArray[i]->GetCameraIP(), chDeviceIP) == 0)
            {
                bFind = true;
                iIndex = i;
                g_WriteLog("HVCR_Open, find the camera .");
                break;
            }
        }
    }
    if (!bFind)
    {
        for (int i = 1; i < CAM_COUNT; i++)
        {
            if (!g_CameraArray[i])
            {
                g_CameraArray[i] = new Camera6467_plate();
                g_CameraArray[i]->SetCameraIP(chDeviceIP);
                g_CameraArray[i]->SetCameraIndex(iIndex);
                g_CameraArray[i]->ConnectToCamera();
                iIndex = i;
                break;
            }
        }
        memset(chLog, '\0', sizeof(chLog));
        MY_SPRINTF(chLog, sizeof(chLog), "VC_Init end, create camera %s, index = %d", chDeviceIP, iIndex);
    }
    else
    {
        memset(chLog, '\0', sizeof(chLog));
        MY_SPRINTF(chLog, sizeof(chLog), "VC_Init end, find camera %s, index = %d", chDeviceIP, iIndex);
    }
    g_WriteLog(chLog);
    return iIndex;
}

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_Deinit(int nHandle)
{
    CheckInitCamera();
    InitializationOverlayText();
    CHAR szLog[256] = { 0 };
    MY_SPRINTF(szLog, sizeof(szLog), "VC_Deinit begin, nHandle = %d", nHandle);
    g_WriteLog(szLog);

    g_bExit = true;
    if (g_hThreadDeleteLog != NULL)
    {
        g_WriteLog("begin to exit log thread.");
        if (WaitForSingleObject(g_hThreadDeleteLog, INFINITE) == WAIT_OBJECT_0)
        {
            CloseHandle(g_hThreadDeleteLog);
            g_hThreadDeleteLog = NULL;
        }
        g_WriteLog("Finish exit log thread.");
    }

    if (nHandle > CAM_COUNT || nHandle < 0)
    {
        g_WriteLog("parameters is invalid.");
        return -1000;
    }
    if (g_CameraArray[nHandle])
    {
        g_CameraArray[nHandle]->StopPlayVideo();
        delete g_CameraArray[nHandle];
        g_CameraArray[nHandle] = NULL;
    }

    g_WriteLog("VC_Deinit End.");
    return 0;
}

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_StartDisplay(int nHandle, int nWidth, int nHeight, int nFHandle)
{
    CheckInitCamera();
    CHAR szLog[256] = { 0 };
    MY_SPRINTF(szLog, sizeof(szLog), "VC_StartDisplay begin, nHandle = %d, nWidth= %d, nHeight= %d, nFHandle= %d", \
        nHandle, \
        nWidth, \
        nHeight, \
        nFHandle);
    g_WriteLog(szLog);

    if (nHandle > CAM_COUNT || nHandle < 0)
    {
        g_WriteLog("Camera handle is invalid.");
        return -1000;
    }

    if (g_CameraArray[nHandle])
    {
        g_CameraArray[nHandle]->SetVideoPlayHandle((HWND)nFHandle, nWidth, nHeight);
        if (g_CameraArray[nHandle]->StartPlayVideo())
        {
            g_WriteLog("VC_StartDisplay end ,StartPlayVideo success.");
            return 0;
        }
        else
        {
            g_WriteLog("VC_StartDisplay end ,StartPlayVideo failed.");
            return -100;
        }
    }
    else
    {
        g_WriteLog("VC_StartDisplay end ,the camera of nHandle is invalid.");
        return -2000;
    }
}

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_StopDisplay(int nHandle)
{
    CheckInitCamera();
    CHAR szLog[256] = { 0 };
    MY_SPRINTF(szLog, sizeof(szLog), "VC_StopDisplay begin, nHandle = %d", nHandle);
    g_WriteLog(szLog);

    if (nHandle > CAM_COUNT || nHandle < 0)
    {
        g_WriteLog("Camera handle is invalid.");
        return -1000;
    }

    if (g_CameraArray[nHandle])
    {
        g_CameraArray[nHandle]->StopPlayVideo();
        g_WriteLog("VC_StopDisplay end ,VC_StopDisplay success.");
        return 0;
    }
    else
    {
        g_WriteLog("VC_StopDisplay end ,the camera of nHandle is invalid.");
        return -2000;
    }

    g_WriteLog("VC_StopDisplay End.");
    return 0;
}

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_GetImage(int nHandle, int nFormat, char* sImage, int* nLength)
{
    CheckInitCamera();
    CHAR chLog[256] = { 0 };
    MY_SPRINTF(chLog, sizeof(chLog), "VC_GetImage begin, nHandle = %d, nFormat= %d, sImage= %s, nLength= %d", \
        nHandle, \
        nFormat, \
        sImage, \
        *nLength);
    g_WriteLog(chLog);
    if (!sImage || *nLength <= 0)
    {
        g_WriteLog("VC_GetImage ,sImage==NUll or *nLength <= 0 please input the correct parameter.");
        return -1000;
    }

    if (nHandle > CAM_COUNT || nHandle < 0)
    {
        g_WriteLog("Camera handle is invalid.");
        return -1000;
    }

    int iRetCode = -2000;
    if (g_CameraArray[nHandle])
    {
        if (g_CameraArray[nHandle]->GetNetStatus() != 0)
        {
            g_WriteLog("Camera  is disconnect.");
            return -100;
        }
        if (!g_pImageBuffer)
        {
            g_pImageBuffer = new BYTE[MAX_IMG_SIZE];
        }
        if (g_pImageBuffer)
        {
            memset(g_pImageBuffer, 0, MAX_IMG_SIZE);
            int iBufferLength = MAX_IMG_SIZE;
            if (nFormat == 0)
            {
                //bmp
                if (g_CameraArray[nHandle]->GetOneImgFromVideo(VIDEO_IMG_FORMAT_BMP, g_pImageBuffer, &iBufferLength))
                {
                    memset(chLog, 0, sizeof(chLog));
                    MY_SPRINTF(chLog, sizeof(chLog), "VC_GetImage , get bmp image success, size = %d", iBufferLength);
                    g_WriteLog(chLog);
                    if (*nLength >= iBufferLength)
                    {
                        g_WriteLog("Begin to copy data to buffer.");
                        memcpy(sImage, g_pImageBuffer, iBufferLength);
                        *nLength = iBufferLength;
                        g_WriteLog("Finish copy .");
                        iRetCode = 0;
                    }
                    else
                    {
                        g_WriteLog("VC_GetImage  , the buffer size is not enough to save image data.");
                        iRetCode = -1000;
                    }
                }
                else
                {
                    memset(chLog, 0, sizeof(chLog));
                    MY_SPRINTF(chLog, sizeof(chLog), "VC_GetImage , get bmp image failed");
                    g_WriteLog(chLog);
                    iRetCode = -1006;
                }                
            }
            else
            {
                iBufferLength = *nLength;
                if (g_CameraArray[nHandle]->GetOneImgFromVideo(VIDEO_IMG_FORMAT_JPG, g_pImageBuffer, &iBufferLength))
                {
                    memset(chLog, 0, sizeof(chLog));
                    MY_SPRINTF(chLog, sizeof(chLog), "VC_GetImage , get JPEGE image success, size = %d", iBufferLength);
                    g_WriteLog(chLog);
                    if (*nLength >= iBufferLength)
                    {
                        g_WriteLog("Begin to copy data to buffer.");
                        memcpy(sImage, g_pImageBuffer, iBufferLength);
                        *nLength = iBufferLength;
                        g_WriteLog("Finish copy .");
                        iRetCode = 0;
                    }
                    else
                    {
                        g_WriteLog("VC_GetImage  , the buffer size is not enough to save image data.");
                        iRetCode = -1000;
                    }
                }
                else
                {
                    memset(chLog, 0, sizeof(chLog));
                    MY_SPRINTF(chLog, sizeof(chLog), "VC_GetImage , get JPEGE image failed");
                    g_WriteLog(chLog);
                    iRetCode = -1006;
                }
            }
        }
        else
        {
            g_WriteLog("VC_GetImage ,the image buffer is apply failed.");
            iRetCode = -2000;
        }
    }
    else
    {
        g_WriteLog("VC_GetImage ,the camera of nHandle is invalid.");
        iRetCode = -2000;
    }

    g_WriteLog("VC_GetImage End.");
    return iRetCode;
}

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_GetImageFile(int nHandle, int nFormat, char* sFileName)
{
    CheckInitCamera();
    CHAR chLog[512] = { 0 };
    MY_SPRINTF(chLog, sizeof(chLog), "VC_GetImageFile begin, nHandle = %d, nFormat= %d, sFileName= %s", \
        nHandle, \
        nFormat, \
        sFileName);
    g_WriteLog(chLog);

    if (nHandle > CAM_COUNT || nHandle < 0)
    {
        g_WriteLog("Camera handle is invalid.");
        return -1000;
    }

    char chFilePath[MAX_PATH] = {0};
    std::string strFilePath = sFileName;
    if (std::string::npos != strFilePath.find('\\') || std::string::npos != strFilePath.find('/'))
    {
        sprintf_s(chFilePath, sizeof(chFilePath), "%s", sFileName);
    }
    else
    {
        char chCurrentPath[MAX_PATH] = {0};
        GetModuleFileNameA(NULL, chCurrentPath, MAX_PATH - 1);
        PathRemoveFileSpecA(chCurrentPath);

        sprintf_s(chFilePath, sizeof(chFilePath), "%s\\%s", chCurrentPath, sFileName);
        memset(chLog, 0, sizeof(chLog));
        MY_SPRINTF(chLog, sizeof(chLog), "Current file name is not full path , use current path to completion it, final path = %s.", 
            chFilePath);
        g_WriteLog(chLog);
    }

    int iRetCode = -2000;
    if (g_CameraArray[nHandle])
    {
        if (g_CameraArray[nHandle]->GetNetStatus() != 0)
        {
            g_WriteLog("Camera  is disconnect.");
            return -100;
        }
        if (!g_pImageBuffer)
        {
            g_pImageBuffer = new BYTE[MAX_IMG_SIZE];
        }
        if (g_pImageBuffer)
        {
            memset(g_pImageBuffer, 0, MAX_IMG_SIZE);
            int iBufferLength = MAX_IMG_SIZE;
            if (nFormat == 0)
            {
                if (g_CameraArray[nHandle]->GetOneImgFromVideo(VIDEO_IMG_FORMAT_BMP, g_pImageBuffer, &iBufferLength))
                {
                    memset(chLog, 0, sizeof(chLog));
                    MY_SPRINTF(chLog, sizeof(chLog), "VC_GetImageFile , get bmp image success, size = %d", iBufferLength);
                    g_WriteLog(chLog);
                    if (g_CameraArray[nHandle]->SaveImgToDisk(chFilePath, g_pImageBuffer, iBufferLength))
                    {
                        g_WriteLog("save image success.");
                        iRetCode = 0;
                    }
                    else
                    {
                        g_WriteLog("save image failed.");
                        iRetCode = -2000;
                    }
                }
                else
                {
                    memset(chLog, 0, sizeof(chLog));
                    MY_SPRINTF(chLog, sizeof(chLog), "VC_GetImageFile , get bmp image failed");
                    g_WriteLog(chLog);
                    iRetCode = -1006;
                }
            }
            else
            {
                if (g_CameraArray[nHandle]->GetOneImgFromVideo(VIDEO_IMG_FORMAT_JPG, g_pImageBuffer, &iBufferLength))
                {
                    memset(chLog, 0, sizeof(chLog));
                    MY_SPRINTF(chLog, sizeof(chLog), "VC_GetImageFile , get JPEGE image success, size = %d", iBufferLength);
                    g_WriteLog(chLog);
                    if (g_CameraArray[nHandle]->SaveImgToDisk(chFilePath, g_pImageBuffer, iBufferLength))
                    {
                        g_WriteLog("save image success.");
                        iRetCode = 0;
                    }
                    else
                    {
                        g_WriteLog("save image failed.");
                        iRetCode = -2000;
                    }
                }
                else
                {
                    memset(chLog, 0, sizeof(chLog));
                    MY_SPRINTF(chLog, sizeof(chLog), "VC_GetImageFile , get JPEGE image failed");
                    g_WriteLog(chLog);
                    iRetCode = -1006;
                }
            }
        }
        else
        {
            g_WriteLog(" VC_GetImageFile ,the image buffer is apply failed.");
            iRetCode = -2000;
        }
    }
    else
    {
        g_WriteLog(" VC_GetImageFile ,the camera of nHandle is invalid.");
        iRetCode = -2000;
    }

    g_WriteLog("VC_GetImageFile End.");
    return iRetCode;
}

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VLPR_TVPDisplay(int nHandle, int nRow, int nCol, char* sText)
{
    CheckInitCamera();
    CHAR szLog[256] = { 0 };
    MY_SPRINTF(szLog, sizeof(szLog), "VLPR_TVPDisplay begin, nHandle = %d, nRow= %d, nCol= %d, sText=%s", \
    nHandle, \
    nRow, \
    nCol,\
    sText);
    g_WriteLog(szLog);

    if (nHandle > CAM_COUNT || nHandle < 0)
    {
        g_WriteLog("Camera handle is invalid.");
        return -1000;
    }

    if (nRow > OVERLAY_MAX_ROW || nCol > OVERLAY_MAX_COL 
        || (nCol + strlen(sText) > OVERLAY_MAX_COL)
        || nRow <= 0 || nCol <= 0)
    {
        memset(szLog, 0, sizeof(szLog));
        MY_SPRINTF(szLog, sizeof(szLog), "VLPR_TVPDisplay  the row or col or (col + strlen(sText) larger than max num, max row = %d, max col = %d", OVERLAY_MAX_ROW, OVERLAY_MAX_COL);
        g_WriteLog(szLog);
        return -1000;
    }
    
    int iFinalRow = nRow - 1;
    int iFinalCol = nCol - 1;
    char* pValue = sText;
    //for (int i = 0; i < strlen(sText); i++)
    //{
    //    g_chOverlay[iFinalRow][iFinalCol+i] = *(pValue++);
    //    //g_chOverlay[iFinalRow][OVERLAY_MAX_COL - 1] = '\0';
    //}
    std::string strOrige = sText;
    int iIndex = 0;
    for (int i = 0; i < strOrige.length(); i++)
    {
        if (iFinalCol > 0)
        {
            if (strOrige[i]<255 && strOrige[i]>0)//扩充的ASCII字符范围为0-255,如是,处理一个字节
            {
                iIndex += 1;
            }
            else//<0,>255的是汉字,处理两个字节
            {
                ++i;
                iIndex += 2;
            }
            iFinalCol -= 1;
        }
        else
        {
            break;
        }
    }
    printf("find the position%d\n", iIndex);
    for (int i = 0; i < strlen(sText); i++)
    {
        g_chOverlay[iFinalRow][iIndex + i] = *(pValue++);
        //g_chOverlay[iFinalRow][OVERLAY_MAX_COL - 1] = '\0';
    }

    char chOverlayString[OVERLAY_MAX_COL*OVERLAY_MAX_ROW] = { 0 };
    memset(chOverlayString, 32, sizeof(chOverlayString));
    chOverlayString[OVERLAY_MAX_COL*OVERLAY_MAX_ROW-1] = '\0';
    char* pOverlay = chOverlayString;
    for (int i = 0; i < OVERLAY_MAX_ROW; i++)
    {
        //sprintf_s(pOverlay, 512 - i*OVERLAY_MAX_COL, "%s\r\n", g_chOverlay[i]);
        memcpy(pOverlay, g_chOverlay[i], strlen(g_chOverlay[i]));
        //memcpy((pOverlay + OVERLAY_MAX_COL - strlen("{0D0A}") - 1), "{0D0A}", strlen("{0D0A}"));
        if (i < OVERLAY_MAX_ROW - 1)
        {
            //memcpy((pOverlay + OVERLAY_MAX_COL - strlen("{0D0A}")), "{0D0A}", strlen("{0D0A}"));
            memcpy((pOverlay + OVERLAY_MAX_COL - strlen("\n")), "\n", strlen("\n"));
            pOverlay += OVERLAY_MAX_COL;
        }
        else if (i == OVERLAY_MAX_ROW - 1)
        {
            //memcpy((pOverlay + OVERLAY_MAX_COL - strlen("{0D0A}") - 1), "{0D0A}", strlen("{0D0A}"));
            memcpy((pOverlay + OVERLAY_MAX_COL - strlen("\n") - 1), "\n", strlen("\n"));
        }
    }
    g_WriteLog("Initialization overlay text success.");
    chOverlayString[OVERLAY_MAX_COL*OVERLAY_MAX_ROW - 1] = '\0';
    if (g_CameraArray[nHandle])
    {
        if (g_CameraArray[nHandle]->SetOverlayVideoTextEnable(0, true))
        {
            g_WriteLog(" VLPR_TVPDisplay  ,set overlay enable success.");
        }
        if (g_CameraArray[nHandle]->SetOverlayVideoText(0, chOverlayString, strlen(chOverlayString)))
        {
            g_WriteLog(" VLPR_TVPDisplay end ,overlay success.");
            return 0;
        }
        else
        {
            g_WriteLog(" VLPR_TVPDisplay end ,overlay failed.");
            return -100;
        }
    }
    else
    {
        g_WriteLog(" VLPR_TVPDisplay end ,the camera of nHandle is invalid.");
        return -2000;
    }
}

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_TVPClear(int nHandle, int nRow, int nCol, int nLength /*= 1*/)
{
    CheckInitCamera();
    CHAR szLog[256] = { 0 };
    MY_SPRINTF(szLog, sizeof(szLog), "VC_TVPClear begin, nHandle = %d, nRow= %d, nCol= %d, nLength=%d", \
        nHandle, \
        nRow, \
        nCol, \
        nLength);
    g_WriteLog(szLog);

    if (nHandle > CAM_COUNT || nHandle < 0)
    {
        g_WriteLog("Camera handle is invalid.");
        return -1000;
    }

    if (nRow > OVERLAY_MAX_ROW || nCol > OVERLAY_MAX_COL )
    {
        memset(szLog, 0, sizeof(szLog));
        MY_SPRINTF(szLog, sizeof(szLog), "VC_TVPClear  the row or col larger than max num, max row = %d, max col = %d", OVERLAY_MAX_ROW, OVERLAY_MAX_COL);
        g_WriteLog(szLog);
        return -1000;
    }

    if (nRow == 0 || nCol == 0)
    {
        if (nRow == 0)
        {
            g_WriteLog("nRow ==0, clear all content.");
            InitializationOverlayText();
        }
        if (nCol == 0)
        {
            g_WriteLog("nCol ==0, clear one line.");
            int iFinalRow = (nRow - 1 >= 0) ? (nRow - 1) : nRow;            
            for (int i = 0; i < OVERLAY_MAX_COL; i++)
            {
                g_chOverlay[iFinalRow][i] = char(32);
            }
        }
    }
    else
    {
        int iFinalRow = nRow - 1;
        int iFinalCol = nCol - 1;
        int iLength = (nCol + nLength > OVERLAY_MAX_COL) ? OVERLAY_MAX_COL - nCol : nLength;

        char chTempValue[OVERLAY_MAX_COL] = { 0 };
        memcpy(chTempValue, g_chOverlay[iFinalRow], OVERLAY_MAX_COL);
        chTempValue[OVERLAY_MAX_COL - 1] = '\0';
        std::string strOrige = chTempValue;
        int iIndex = 0;
        for (int i = 0; i < strOrige.length(); i++)
        {
            if (iFinalCol > 0)
            {
                if (strOrige[i]<255 && strOrige[i]>0)//扩充的ASCII字符范围为0-255,如是,处理一个字节
                {
                    iIndex += 1;
                }
                else//<0,>255的是汉字,处理两个字节
                {
                    ++i;
                    iIndex += 2;
                }
                iFinalCol -= 1;
            }
            else
            {
                break;
            }
        }
        printf("find the position%d\n", iIndex);
        for (int i = 0; i < iLength; i++)
        {
            g_chOverlay[iFinalRow][iIndex++] = char(32);
        }
    }

    char chOverlayString[OVERLAY_MAX_ROW * OVERLAY_MAX_COL] = { 0 };
    memset(chOverlayString, 32, sizeof(chOverlayString));
    chOverlayString[OVERLAY_MAX_COL*OVERLAY_MAX_ROW - 1] = '\0';
    char* pOverlay = chOverlayString;
    for (int i = 0; i < OVERLAY_MAX_ROW; i++)
    {
        memcpy(pOverlay, g_chOverlay[i], strlen(g_chOverlay[i]));
        if (i < OVERLAY_MAX_ROW - 1)
        {
            //memcpy((pOverlay + OVERLAY_MAX_COL - strlen("{0D0A}")), "{0D0A}", strlen("{0D0A}"));
            memcpy((pOverlay + OVERLAY_MAX_COL - strlen("\n")), "\n", strlen("\n"));
            pOverlay += OVERLAY_MAX_COL;
        }
        else if (i == OVERLAY_MAX_ROW - 1)
        {
            //memcpy((pOverlay + OVERLAY_MAX_COL - strlen("{0D0A}") - 1), "{0D0A}", strlen("{0D0A}"));
            memcpy((pOverlay + OVERLAY_MAX_COL - strlen("\n") - 1), "\n", strlen("\n"));
        }
    }
    chOverlayString[OVERLAY_MAX_ROW * OVERLAY_MAX_COL - 1] = '\0';
    g_WriteLog("Initialization overlay text success.");

    if (g_CameraArray[nHandle])
    {
        if (g_CameraArray[nHandle]->SetOverlayVideoText(0, chOverlayString, strlen(chOverlayString)))
        {
            g_WriteLog(" VC_TVPClear end ,overlay success.");
            return 0;
        }
        else
        {
            g_WriteLog(" VC_TVPClear end ,overlay failed.");
            return -100;
        }
    }
    else
    {
        g_WriteLog(" VC_TVPClear end ,the camera of nHandle is invalid.");
        return -2000;
    }
}

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_SyncTime(int nHandle, char* sSysTime)
{
    CheckInitCamera();
    CHAR szLog[256] = { 0 };
    MY_SPRINTF(szLog, sizeof(szLog), "VC_SyncTime begin, nHandle = %d, sSysTime= %s", \
        nHandle, \
        sSysTime);
    g_WriteLog(szLog);

    if (nHandle < 0 || nHandle >= CAM_COUNT)
    {
        g_WriteLog("parameters is invalid.");
        return -1000;
    }

    char year[5] = { 0 }, month[3] = { 0 }, day[3] = { 0 }, hour[3] = { 0 }, min[3] = { 0 }, sec[3] = { 0 };
    // int year=0,month=0,day=0,hour=0,min=0,sec=0;
    //if(6 != swpa_sscanf(szBeginTime,"%04hd.%02hd.%02hd_%02hd:%02hd:%02hd", &year, &month, &day, &hour, &min, &sec)) return -1;

    //strncpy(year, sSysTime, 4);
    //strncpy(month, &sSysTime[4], 2);
    //strncpy(day, &sSysTime[6], 2);
    //strncpy(hour, &sSysTime[8], 2);
    //strncpy(min, &sSysTime[10], 2);
    //strncpy(sec, &sSysTime[12], 2);
    strncpy_s(year, sSysTime, 4);
    strncpy_s(month, &sSysTime[4], 2);
    strncpy_s(day, &sSysTime[6], 2);
    strncpy_s(hour, &sSysTime[8], 2);
    strncpy_s(min, &sSysTime[10], 2);
    strncpy_s(sec, &sSysTime[12], 2);

    int iYear = atoi(year);
    int iMonth = atoi(month);
    int iDay = atoi(day);
    int iHour = atoi(hour);
    int iMin = atoi(min);
    int iSec = atoi(sec);
    if (iYear < 2000 || iYear > 2100 || iMonth < 1 || iMonth > 12
        || iHour < 0 || iHour > 23 || iMin < 0 || iMin > 59 || iSec < 0 || iSec > 59)
    {
        return -1000;
    }
    if ((((iYear % 4 == 0 && iYear % 100 != 0) || (iYear % 400 == 0)) && iDay > 29 && iMonth == 2)
        || (!((iYear % 4 == 0 && iYear % 100 != 0) || (iYear % 400 == 0)) && iDay > 28 && iMonth == 2)
        || ((iMonth == 1 || iMonth == 3 || iMonth == 5 || iMonth == 7 || iMonth == 8 || iMonth == 10 || iMonth == 12) && iDay > 31)
        || (!(iMonth == 1 || iMonth == 3 || iMonth == 5 || iMonth == 7 || iMonth == 8 || iMonth == 10 || iMonth == 12) && iDay > 30)
        || iDay < 1)
    {
        return -1000;
    }

    if (g_CameraArray[nHandle])
    {
        if (g_CameraArray[nHandle]->SynTime(iYear, iMonth, iDay, iHour, iMin, iSec, 0))
        {
            g_WriteLog("VC_SyncTime end ,SynTime success.");
            return 0;
        }
        else
        {
            g_WriteLog("VC_SyncTime end ,TakeCapture failed.");
            return -100;
        }
    }
    else
    {
        g_WriteLog("VC_SyncTime end ,the camera of nHandle is invalid.");
        return -2000;
    }
}

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_ShowTime(int nHandle, int nStyle)
{
    CheckInitCamera();
    CHAR szLog[256] = { 0 };
    MY_SPRINTF(szLog, sizeof(szLog), "VC_ShowTime begin, nHandle = %d, nStyle= %d", \
        nHandle, \
        nStyle);
    g_WriteLog(szLog);

    if (nHandle > CAM_COUNT || nHandle < 0)
    {
        g_WriteLog("Camera handle is invalid.");
        return -1000;
    }
    
    if (g_CameraArray[nHandle])
    {
        if (nStyle == 0)
        {
            if (g_CameraArray[nHandle]->SetOverlayTimeEnable(0, false))
            {
                g_WriteLog("VC_ShowTime end , show success.");
                return 0;
            }
            else
            {
                g_WriteLog("VC_ShowTime end , show failed.");
                return -100;
            }
        }
        else
        {
            if (g_CameraArray[nHandle]->SetOverlayVideoTextEnable(0, true))
            {
                g_WriteLog(" VC_ShowTime ,set overlay enable success.");
            }
            int iRet1 = 0, iRet2 = 0;
            if (g_CameraArray[nHandle]->SetOverlayTimeEnable(0, true))
            {
                g_WriteLog("VC_ShowTime end , show success.");
                iRet1 = 1;
            }
            int iStyle = 0;
            switch (nStyle)
            {
            case 0:
                iStyle = 0;
                break;
            case 1:
                iStyle = 1;
                break;
            case 2:
                iStyle = 2;
                break;
            case 3:
                iStyle = 0;
                break;
            default:
                break;
            }
            if (g_CameraArray[nHandle]->SetOverlayTimeFormat(0, iStyle))
            {
                g_WriteLog("VC_ShowTime end , set style success.");
                iRet2 = 1;
            }
            if (iRet1 ==1 && iRet2 ==1)
            {
                g_WriteLog("VC_ShowTime end , set  success.");
                return 0;
            }
            else
            {
                return -2000;
            }
        }
    }
    else
    {
        g_WriteLog("VC_ShowTime end ,the camera of nHandle is invalid.");
        return -2000;
    }
}

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_GetStatus(int nHandle, int* pStatusCode)
{
    CheckInitCamera();
    CHAR szLog[256] = { 0 };
    MY_SPRINTF(szLog, sizeof(szLog), "VC_GetStatus begin, nHandle = %d, pStatusCode= %d", \
        nHandle, \
        *pStatusCode);
    g_WriteLog(szLog);

    if (nHandle > CAM_COUNT || nHandle < 0)
    {
        g_WriteLog("Camera handle is invalid.");
        return -1000;
    }

    if (g_CameraArray[nHandle])
    {
        //if (0 == g_CameraArray[nHandle]->GetCamStatus())
        if (0 == g_CameraArray[nHandle]->GetNetStatus())
        {
            *pStatusCode = 0;
            g_WriteLog("VC_GetStatus end ,GetCamStatus success.");
            return 0;
        }
        else
        {
            *pStatusCode = -100;
            g_WriteLog("VC_GetStatus end , camera is disconnect .");
            return 0;
        }
    }
    else
    {
        g_WriteLog("VC_GetStatus end ,the camera of nHandle is invalid.");
        return -2000;
    }
}

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_GetStatusMsg(int nStatusCode, char* sStatusMsg, int nStatusMsgLen)
{
    CheckInitCamera();
    CHAR szLog[256] = { 0 };
    MY_SPRINTF(szLog, sizeof(szLog), "VC_GetStatusMsg begin, nStatusCode = %d, sStatusMsg= %p, nStatusMsgLen=%d", \
        nStatusCode, \
        sStatusMsg,\
        nStatusMsgLen);
    g_WriteLog(szLog);

    switch (nStatusCode)
    {
    case 0:
        memcpy(sStatusMsg, "正常\0", sizeof("正常\0"));
        break;
    case -100:
        memcpy(sStatusMsg, "设备无响应\0", sizeof("设备无响应\0"));
        break;
    case -1000:
        memcpy(sStatusMsg, "传入参数错误\0", sizeof("传入参数错误\0"));
        break;
    case -1001:
        memcpy(sStatusMsg, "设备被占用\0", sizeof("设备被占用\0"));
        break;
    case -1002:
        memcpy(sStatusMsg, "打开失败\0", sizeof("打开失败\0"));
        break;
    case -2000:
        memcpy(sStatusMsg, "其他错误\0", sizeof("其他错误\0"));
        break;
    default:
        if (nStatusCode >= -2000)
        {
            memcpy(sStatusMsg, "预留\0", sizeof("预留\0"));
        }
        else
        {
            memcpy(sStatusMsg, "未定义\0", sizeof("未定义\0"));
        }
        break;
    }

    memset(szLog, 0, sizeof(szLog));
    MY_SPRINTF(szLog, sizeof(szLog), "VC_GetStatusMsg end, nStatusCode = %d, sStatusMsg= %s, nStatusMsgLen=%d", \
        nStatusCode, \
        sStatusMsg, \
        nStatusMsgLen);
    g_WriteLog(szLog);
    return 0;
}

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_GetDevInfo(char* sCompany, int nCompanyLen, char* sDevMode, int nModeLen)
{
    CheckInitCamera();
    CHAR szLog[256] = { 0 };
    MY_SPRINTF(szLog, sizeof(szLog), "VC_GetDevInfo begin, sCompany = %s, nCompanyLen= %d, sDevMode=%s, sDevMode=%d", \
        sCompany, \
        nCompanyLen, \
        sDevMode,\
        nModeLen);
    g_WriteLog(szLog);

    char chTemp[260] = { 0 };
    g_ReadKeyValueFromConfigFile("DeviceInfo", "Company", chTemp, 260);
    if (strlen(chTemp) < 1)
    {
        memcpy(chTemp, "Signalway\0", sizeof("Signalway\0"));
    }
    memcpy(sCompany, chTemp, strlen(chTemp) + 1);


    memset(chTemp, 0, sizeof(chTemp));
    g_ReadKeyValueFromConfigFile("DeviceInfo", "DevMode", chTemp, 260);
    if (strlen(chTemp) < 1)
    {
        memcpy(chTemp, "PCC\0", sizeof("PCC\0"));
    }
    memcpy(sDevMode, chTemp, strlen(chTemp) + 1);

    memset(szLog, 0, sizeof(szLog));
    MY_SPRINTF(szLog, sizeof(szLog), "VC_GetDevInfo end, sCompany = %s, nCompanyLen= %d, sDevMode=%s, sDevMode=%d， return 0.", \
        sCompany, \
        nCompanyLen, \
        sDevMode, \
        nModeLen);
    g_WriteLog(szLog);
    return 0;
}

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_GetVersion(char* sDevVersion, int nDevVerLen, char* sAPIVersion, int nAPIVerLen)
{
    CheckInitCamera();
    CHAR szLog[256] = { 0 };
    MY_SPRINTF(szLog, sizeof(szLog), "VC_GetVersion begin, sCompany = %p, nDevVerLen= %d, sAPIVersion=%p, nAPIVerLen=%d", \
        sDevVersion, \
        nDevVerLen, \
        sAPIVersion, \
        nAPIVerLen);
    g_WriteLog(szLog);

    char chTemp[260] = { 0 };
    g_ReadKeyValueFromConfigFile("DeviceInfo", "DeviceVersion", chTemp, 260);
    if (strlen(chTemp) < 1)
    {
        memcpy(chTemp, "PCC\0", sizeof("PCC\0"));
    }
    memcpy(sDevVersion, chTemp, strlen(chTemp) + 1);

    //memset(chTemp, 0, sizeof(chTemp));
    //g_ReadKeyValueFromConfigFile("DeviceInfo", "DLLVersion", chTemp, 260);
    //if (strlen(chTemp) < 1)
    //{
    //    memcpy(chTemp, "1.0.0.3\0", sizeof("1.0.0.3\0"));
    //}
    //memcpy(sAPIVersion, chTemp, strlen(chTemp) + 1);

    TCHAR szFileName[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szFileName, MAX_PATH);	//取得包括程序名的全路径
    PathRemoveFileSpec(szFileName);				//去掉程序名
    char chPath[MAX_PATH] = { 0 };
    MY_SPRINTF(chPath, sizeof(chPath), "%s\\VideoCard_SignalwayPCC.dll", szFileName);
    std::string strVerion =  GetSoftVersion(chPath);
    
    if (strVerion.length() <= 0)
    {
        memset(chTemp, 0, sizeof(chTemp));
        g_ReadKeyValueFromConfigFile("DeviceInfo", "DLLVersion", chTemp, 260);
        if (strlen(chTemp) < 1)
        {
            memcpy(chTemp, "1.0.0.4\0", sizeof("1.0.0.4\0"));
        }
        memcpy(sAPIVersion, chTemp, strlen(chTemp) + 1);
    }
    else
    {
        memcpy(sAPIVersion, strVerion.c_str(), strVerion.length());
    }

    memset(szLog, 0, sizeof(szLog));
    MY_SPRINTF(szLog, sizeof(szLog), "VC_GetVersion end, sCompany = %s, nDevVerLen= %d, sAPIVersion=%s, nAPIVerLen=%d， return 0.", \
        sDevVersion, \
        nDevVerLen, \
        sAPIVersion, \
        nAPIVerLen);
    g_WriteLog(szLog);
    return 0;
}

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_TVPDisplay(int nHandle, int nRow, int nCol, char* sText)
{
    CheckInitCamera();
    CHAR szLog[256] = { 0 };
    MY_SPRINTF(szLog, sizeof(szLog), "VC_TVPDisplay begin, nHandle = %d, nRow= %d, nCol= %d, sText=%s", \
        nHandle, \
        nRow, \
        nCol, \
        sText);
    g_WriteLog(szLog);

    if (nHandle > CAM_COUNT || nHandle < 0)
    {
        g_WriteLog("Camera handle is invalid.");
        return -1000;
    }

    if (nRow > OVERLAY_MAX_ROW || nCol > OVERLAY_MAX_COL
        || (nCol + strlen(sText) > OVERLAY_MAX_COL)
        || nRow < 0 || nCol <= 0)
    {
        memset(szLog, 0, sizeof(szLog));
        MY_SPRINTF(szLog, sizeof(szLog), "VC_TVPDisplay  the row or col or (col + strlen(sText) larger than max num, max row = %d, max col = %d", OVERLAY_MAX_ROW, OVERLAY_MAX_COL);
        g_WriteLog(szLog);
        return -1000;
    }

    int iFinalRow = nRow;
    int iFinalCol = nCol - 1;
    char* pValue = sText;
    //for (int i = 0; i < strlen(sText); i++)
    //{
    //    g_chOverlay[iFinalRow][iFinalCol+i] = *(pValue++);
    //    //g_chOverlay[iFinalRow][OVERLAY_MAX_COL - 1] = '\0';
    //}
    std::string strOrige = sText;
    int iIndex = 0;
    for (int i = 0; i < strOrige.length(); i++)
    {
        if (iFinalCol > 0)
        {
            if (strOrige[i]<255 && strOrige[i]>0)//扩充的ASCII字符范围为0-255,如是,处理一个字节
            {
                iIndex += 1;
            }
            else//<0,>255的是汉字,处理两个字节
            {
                ++i;
                iIndex += 2;
            }
            iFinalCol -= 1;
        }
        else
        {
            break;
        }
    }
    printf("find the position%d\n", iIndex);
    for (int i = 0; i < strlen(sText); i++)
    {
        g_chOverlay[iFinalRow][iIndex + i] = *(pValue++);
        //g_chOverlay[iFinalRow][OVERLAY_MAX_COL - 1] = '\0';
    }

    char chOverlayString[OVERLAY_MAX_COL*OVERLAY_MAX_ROW] = { 0 };
    memset(chOverlayString, 32, sizeof(chOverlayString));
    chOverlayString[OVERLAY_MAX_COL*OVERLAY_MAX_ROW - 1] = '\0';
    char* pOverlay = chOverlayString;
    for (int i = 0; i < OVERLAY_MAX_ROW; i++)
    {
        //sprintf_s(pOverlay, 512 - i*OVERLAY_MAX_COL, "%s\r\n", g_chOverlay[i]);
        memcpy(pOverlay, g_chOverlay[i], strlen(g_chOverlay[i]));
        //memcpy((pOverlay + OVERLAY_MAX_COL - strlen("{0D0A}") - 1), "{0D0A}", strlen("{0D0A}"));
        if (i < OVERLAY_MAX_ROW - 1)
        {
            //memcpy((pOverlay + OVERLAY_MAX_COL - strlen("{0D0A}")), "{0D0A}", strlen("{0D0A}"));
            memcpy((pOverlay + OVERLAY_MAX_COL - strlen("\n")), "\n", strlen("\n"));
            pOverlay += OVERLAY_MAX_COL;
        }
        else if (i == OVERLAY_MAX_ROW - 1)
        {
            //memcpy((pOverlay + OVERLAY_MAX_COL - strlen("{0D0A}") - 1), "{0D0A}", strlen("{0D0A}"));
            memcpy((pOverlay + OVERLAY_MAX_COL - strlen("\n") - 1), "\n", strlen("\n"));
        }
    }
    g_WriteLog("Initialization overlay text success.");
    chOverlayString[OVERLAY_MAX_COL*OVERLAY_MAX_ROW - 1] = '\0';
    if (g_CameraArray[nHandle])
    {
        if (g_CameraArray[nHandle]->SetOverlayVideoTextEnable(0, true))
        {
            g_WriteLog(" VC_TVPDisplay  ,set overlay enable success.");
        }
        if (g_CameraArray[nHandle]->SetOverlayVideoText(0, chOverlayString, strlen(chOverlayString)))
        {
            g_WriteLog(" VC_TVPDisplay end ,overlay success.");
            return 0;
        }
        else
        {
            g_WriteLog(" VC_TVPDisplay end ,overlay failed.");
            return -100;
        }
    }
    else
    {
        g_WriteLog(" VC_TVPDisplay end ,the camera of nHandle is invalid.");
        return -2000;
    }
}

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_GetHWVersion(int nHandle, char* sHWVersion, int nHWVerMaxLen, char* sAPIVersion, int nAPIVerMaxLen)
{
    CheckInitCamera();
    CHAR szLog[256] = { 0 };
    MY_SPRINTF(szLog, sizeof(szLog), "VC_GetHWVersion begin, nHandle = %d, sHWVersion= %p, nHWVerMaxLen=%d, sAPIVersion=%p, nAPIVerMaxLen= %d", \
        nHandle, \
        sHWVersion, \
        nHWVerMaxLen, \
        sAPIVersion, \
        nAPIVerMaxLen);
    g_WriteLog(szLog);

    if (nHandle > CAM_COUNT || nHandle < 0)
    {
        g_WriteLog("Camera handle is invalid.");
        return -1000;
    }
    char chTemp[260] = { 0 };

    int iRet = -100;
    if (g_CameraArray[nHandle])
    {
        BasicInfo info;
        if (g_CameraArray[nHandle]->GetHardWareInfo(info))
        {
            g_WriteLog(" VLPR_GetHWVersion  ,GetHardWareInfo success.");
            if (nHWVerMaxLen <= strlen(info.szDevType))
            {
                memcpy(sHWVersion, info.szDevType, nHWVerMaxLen);
            }
            else
            {
                memcpy(sHWVersion, info.szDevType, strlen(info.szDevType));
            }

            if (nAPIVerMaxLen <= strlen(info.szDevVersion))
            {
                memcpy(sAPIVersion, info.szDevVersion, nAPIVerMaxLen);
            }
            else
            {
                memcpy(sAPIVersion, info.szDevVersion, strlen(info.szDevVersion));
            }
            iRet = 0;
        }
        else
        {
            g_ReadKeyValueFromConfigFile("DeviceInfo", "DeviceVersion", chTemp, 260);
            if (strlen(chTemp) < 1)
            {
                memcpy(chTemp, "PCC\0", sizeof("PCC\0"));
            }
            memcpy(sHWVersion, chTemp, strlen(chTemp) + 1);
            memcpy(sAPIVersion, "1.0.0.1\0", sizeof("1.0.0.1\0"));
            g_WriteLog(" VLPR_GetHWVersion  ,GetHardWareInfo failed, use default.");
        }
    }
    else
    {
        g_WriteLog(" VLPR_GetHWVersion end ,the camera of nHandle is invalid.");
        return -1002;
    }

    memset(szLog, 0, sizeof(szLog));
    MY_SPRINTF(szLog, sizeof(szLog), "VC_GetHWVersion end, nHandle = %d, sHWVersion= %s, nHWVerMaxLen=%d, sAPIVersion=%s, nAPIVerMaxLen= %d", \
        nHandle, \
        sHWVersion, \
        nHWVerMaxLen, \
        sAPIVersion, \
        nAPIVerMaxLen);
    g_WriteLog(szLog);
    return iRet;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CheckInitCamera_plate();


XLW_VPR_API int WINAPI VLPR_Init()
{
    CHAR szLog[1024] = { 0 };
    MY_SPRINTF(szLog, "VLPR_Init begin");
    g_WriteLog_plate(szLog);
    for (int i = 0; i < CAM_COUNT; i++)
    {
        g_CameraArray_Plate[i] = NULL;
    }
    CheckInitCamera_plate();
    g_WriteLog_plate("VLPR_Init End.");
    return 0;
}

XLW_VPR_API int WINAPI VLPR_Deinit()
{
    CheckInitCamera_plate();
    g_WriteLog_plate("VLPR_Deinit begin..");
    Camera6467* pCamera = NULL;
    for (int i = 0; i < CAM_COUNT; i++)
    {
        if (g_CameraArray_Plate[i])
        {
            delete g_CameraArray_Plate[i];
            g_CameraArray_Plate[i] = NULL;
        }
    }
    g_WriteLog_plate("VLPR_Deinit finish.");
    return 0;
}

XLW_VPR_API int WINAPI VLPR_Login(int nType, char* sParas)
{
    CheckInitCamera_plate();
    char chLog[512] = { 0 };
    MY_SPRINTF(chLog, "VLPR_Login, nType =%d, sParas=%s", nType, sParas);
    g_WriteLog_plate(chLog);
    if (nType == 0)
    {
        g_WriteLog_plate("This device supports Network mode only.");
        return -1000;
    }
    if (strlen(sParas) < 8)
    {
        g_WriteLog_plate("the Parameters is invalid, length less than 8.");
        return -1000;
    }

    char chDeviceIP[32] = { 0 };
    std::string strSrc(sParas);
    std::size_t iPos = strSrc.find(",");
    if (iPos != std::string::npos)
    {
        std::string strSub = strSrc.substr(0, iPos);
        MY_SPRINTF(chDeviceIP, "%s", strSub.c_str());
        g_WriteLog_plate(chDeviceIP);
    }
    else
    {
        g_WriteLog_plate("the Parameters is invalid, can`t find IP address.");
        return -1000;
    }
    int iCheckIp = g_checkIP(chDeviceIP);
    if (iCheckIp != 1)
    {
        g_WriteLog_plate("the Parameters is invalid,  IP address is invalid.");
        return -1000;
    }

    Camera6467* pCamera = NULL;
    std::list<Camera6467*>::iterator it;
    bool bFind = false;
    int iIndex = -1;
    for (int i = 0; i < CAM_COUNT; i++)
    {
        if (g_CameraArray_Plate[i])
        {
            if (strcmp(g_CameraArray_Plate[i]->GetCameraIP(), chDeviceIP) == 0)
            {
                bFind = true;
                iIndex = i;
                g_WriteLog_plate("HVCR_Open, find the camera .");
                break;
            }
        }
    }
    if (!bFind)
    {
        for (int i = 1; i < CAM_COUNT; i++)
        {
            if (!g_CameraArray_Plate[i])
            {
                g_CameraArray_Plate[i] = new Camera6467();
                g_CameraArray_Plate[i]->SetCameraIP(chDeviceIP);
                g_CameraArray_Plate[i]->SetCameraIndex(iIndex);
                g_CameraArray_Plate[i]->ConnectToCamera();
                iIndex = i;
                break;
            }
        }
        memset(chLog, '\0', sizeof(chLog));
        MY_SPRINTF(chLog, "VLPR_Login end, create camera %s, index = %d", chDeviceIP, iIndex);
    }
    else
    {
        memset(chLog, '\0', sizeof(chLog));
        MY_SPRINTF(chLog, "VLPR_Login end, find camera %s, index = %d", chDeviceIP, iIndex);
    }
    g_WriteLog_plate(chLog);
    return iIndex;
}

XLW_VPR_API int WINAPI VLPR_Logout(int nHandle)
{
    CheckInitCamera_plate();
    CHAR chLog[1024] = { 0 };
    MY_SPRINTF(chLog, "VLPR_Logout begin, Handle = %d", nHandle);
    g_WriteLog_plate(chLog);

    if (nHandle > CAM_COUNT || nHandle < 0)
    {
        g_WriteLog_plate("parameters is invalid.");
        return -1000;
    }
    if (g_CameraArray_Plate[nHandle])
    {
        delete g_CameraArray_Plate[nHandle];
        g_CameraArray_Plate[nHandle] = NULL;
    }
    g_WriteLog_plate("VLPR_Logout finished.");
    return 0;
}

XLW_VPR_API int WINAPI VLPR_SetResultCallBack(int nHandle, CBFun_GetRegResult pFunc, void* pUser)
{
    CheckInitCamera_plate();
    CHAR chLog[1024] = { 0 };
    MY_SPRINTF(chLog, "VLPR_SetResultCallBack begin, Handle = %d, CBFun_GetRegResult = %p, pUser = %p", nHandle, pFunc, pUser);
    g_WriteLog_plate(chLog);
    if (nHandle < 0 || nHandle >= CAM_COUNT)
    {
        g_WriteLog_plate("parameters is invalid.");
        return -1000;
    }

    if (g_CameraArray_Plate[nHandle])
    {
        g_CameraArray_Plate[nHandle]->SetResult_Callback(pFunc, pUser);
        g_WriteLog_plate(" VLPR_SetResultCallBack end ,SetResultCallBack success.");
        return 0;
    }
    else
    {
        g_WriteLog_plate(" VLPR_SetResultCallBack end ,the camera of nHandle is invalid.");
        return -1002;
    }
}

XLW_VPR_API int WINAPI VLPR_SetStatusCallBack(int nHandle, int nTimeInvl, CBFun_GetDevStatus pFunc, void* pUser)
{
    CheckInitCamera_plate();
    CHAR chLog[1024] = { 0 };
    MY_SPRINTF(chLog, "VLPR_SetStatusCallBack begin, Handle = %d, CBFun_GetDevStatus = %p, pUser = %p", nHandle, pFunc, pUser);
    g_WriteLog_plate(chLog);
    if (nHandle < 0 || nHandle >= CAM_COUNT)
    {
        g_WriteLog_plate("parameters is invalid.");
        return -1000;
    }

    if (g_CameraArray_Plate[nHandle])
    {
        g_CameraArray_Plate[nHandle]->SetConnectStatus_Callback(pFunc, pUser, nTimeInvl);
        g_WriteLog_plate(" VLPR_SetStatusCallBack end ,set CBFun_GetDevStatus success.");
        return 0;
    }
    else
    {
        g_WriteLog_plate(" VLPR_SetStatusCallBack end ,the camera of nHandle is invalid.");
        return -1002;
    }
}

XLW_VPR_API int WINAPI VLPR_ManualSnap(int nhandle)
{
    CheckInitCamera_plate();
    CHAR chLog[1024] = { 0 };
    MY_SPRINTF(chLog, "VLPR_ManualSnap begin, Handle = %d", nhandle);
    g_WriteLog_plate(chLog);
    if (nhandle < 0 || nhandle >= CAM_COUNT)
    {
        g_WriteLog_plate("parameters is invalid.");
        return -1000;
    }

    if (g_CameraArray_Plate[nhandle])
    {
        if (g_CameraArray_Plate[nhandle]->TakeCapture())
        {
            g_WriteLog_plate(" VLPR_ManualSnap end ,TakeCapture success.");
            return 0;
        }
        else
        {
            g_WriteLog_plate(" VLPR_ManualSnap end ,TakeCapture failed.");
            return -100;
        }
    }
    else
    {
        g_WriteLog_plate(" VLPR_ManualSnap end ,the camera of nHandle is invalid.");
        return -1002;
    }
}

XLW_VPR_API int WINAPI VLPR_GetStatus(int nHandle, int* pStatusCode)
{
    CheckInitCamera_plate();
    CHAR chLog[1024] = { 0 };
    int iHandle = nHandle;
    MY_SPRINTF(chLog, "VLPR_GetStatus begin, Handle = %d", iHandle);
    g_WriteLog_plate(chLog);
    if (iHandle < 0 || iHandle >= CAM_COUNT)
    {
        g_WriteLog_plate("parameters is invalid.");
        return -1000;
    }

    if (g_CameraArray_Plate[iHandle])
    {
        if (0 == g_CameraArray_Plate[iHandle]->GetCamStatus())
        {
            *pStatusCode = 0;
            g_WriteLog_plate(" VLPR_GetStatus end ,GetCamStatus success.");
            return 0;
        }
        else
        {
            *pStatusCode = -100;
            g_WriteLog_plate(" VLPR_GetStatus end , camera is disconnect .");
            return 0;
        }
    }
    else
    {
        g_WriteLog_plate(" VLPR_GetStatus end ,the camera of nHandle is invalid.");
        return -1002;
    }
}

XLW_VPR_API int WINAPI VLPR_GetStatusMsg(int nStatusCode, char* sStatusMsg, int nStatusMsgLen)
{
    CheckInitCamera_plate();
    CHAR chLog[1024] = { 0 };
    MY_SPRINTF(chLog, "VLPR_GetStatusMsg begin, nStatusCode = %d, sStatusMsg = %s , nStatusMsgLen = %d", nStatusCode, sStatusMsg, nStatusMsgLen);
    g_WriteLog_plate(chLog);

    switch (nStatusCode)
    {
    case 0:
        memcpy(sStatusMsg, "正常\0", sizeof("正常\0"));
        break;
    case -100:
        memcpy(sStatusMsg, "设备无响应\0", sizeof("设备无响应\0"));
        break;
    case -1000:
        memcpy(sStatusMsg, "传入参数错误\0", sizeof("传入参数错误\0"));
        break;
    case -1001:
        memcpy(sStatusMsg, "设备被占用\0", sizeof("设备被占用\0"));
        break;
    case -1002:
        memcpy(sStatusMsg, "打开失败\0", sizeof("打开失败\0"));
        break;
    case -2000:
        memcpy(sStatusMsg, "其他错误\0", sizeof("其他错误\0"));
        break;
    default:
        if (nStatusCode >= -2000)
        {
            memcpy(sStatusMsg, "预留\0", sizeof("预留\0"));
        }
        else
        {
            memcpy(sStatusMsg, "未定义\0", sizeof("未定义\0"));
        }
        break;
    }
    //nStatusMsgLen = strlen(sStatusMsg) + 1;
    memset(chLog, '\0', sizeof(chLog));
    MY_SPRINTF(chLog, "VLPR_GetStatusMsg end, nStatusCode = %d, sStatusMsg = %s , nStatusMsgLen = %d", nStatusCode, sStatusMsg, nStatusMsgLen);
    g_WriteLog_plate(chLog);

    return 0;
}

XLW_VPR_API int WINAPI VLPR_GetVersion(char* sVersion, int nVerLen)
{
    CHAR chLog[1024] = { 0 };
    MY_SPRINTF(chLog, "VLPR_GetVersion begin, sVersion = %s, nVerLen= %d", sVersion, nVerLen);
    g_WriteLog_plate(chLog);

    char chTemp[260] = { 0 };
    g_ReadKeyValueFromConfigFile("DeviceInfo", "DLLVersion", chTemp, 260);
    if (strlen(chTemp) < 1)
    {
        memcpy(chTemp, "1.0.0.3\0", sizeof("1.0.0.3\0"));
    }
    memcpy(sVersion, chTemp, strlen(chTemp) + 1);
    nVerLen = strlen(chTemp) + 1;

    memset(chLog, '\0', sizeof(chLog));
    MY_SPRINTF(chLog, "VLPR_GetVersion end, sVersion = %s, nVerLen= %d", sVersion, nVerLen);
    g_WriteLog_plate(chLog);
    return 0;
}

XLW_VPR_API int WINAPI VLPR_GetHWVersion(int nHandle, char *sHWVersion, int nHWVerMaxLen, char *sAPIVersion, int nAPIVerMaxLen)
{
    CheckInitCamera_plate();
    CHAR chLog[1024] = { 0 };
    MY_SPRINTF(chLog, "VLPR_GetHWVersion begin, Handle = %d", nHandle);
    g_WriteLog_plate(chLog);
    if (nHandle < 0 || nHandle >= CAM_COUNT)
    {
        g_WriteLog_plate("parameters is invalid.");
        return -1000;
    }

    if (g_CameraArray_Plate[nHandle])
    {
        BasicInfo info;
        if (g_CameraArray_Plate[nHandle]->GetHardWareInfo(info))
        {
            g_WriteLog_plate(" VLPR_GetHWVersion end ,GetHardWareInfo success.");
            if (nHWVerMaxLen <= strlen(info.szDevType))
            {
                memcpy(sHWVersion, info.szDevType, nHWVerMaxLen);
            }
            else
            {
                memcpy(sHWVersion, info.szDevType, strlen(info.szDevType));
            }

            if (nAPIVerMaxLen <= strlen(info.szDevVersion))
            {
                memcpy(sAPIVersion, info.szDevVersion, nAPIVerMaxLen);
            }
            else
            {
                memcpy(sAPIVersion, info.szDevVersion, strlen(info.szDevVersion));
            }

            return 0;
        }
        else
        {
            g_WriteLog_plate(" VLPR_GetHWVersion end ,GetHardWareInfo failed.");
            return -100;
        }
    }
    else
    {
        g_WriteLog_plate(" VLPR_GetHWVersion end ,the camera of nHandle is invalid.");
        return -1002;
    }
}
