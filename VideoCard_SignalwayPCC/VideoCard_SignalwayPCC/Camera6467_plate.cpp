#include "stdafx.h"
#include "Camera6467_plate.h"
#include "HvDevice/HvDeviceBaseType.h"
#include "HvDevice/HvDeviceCommDef.h"
#include "HvDevice/HvDeviceNew.h"
#include "HvDevice/HvCamera.h"
#include "ToolFunction.h"
#include "H264_Api/H264.h"
#pragma comment(lib, "H264_Api/H264.lib")

#define  COLOR_UNKNOW 9
#define  COLOR_BLUE 0
#define  COLOR_YELLOW 1
#define  COLOR_BLACK 2
#define  COLOR_WHITE 3
#define  COLOR_RED 4
#define  COLOR_GREEN 5

#define  BUFFERLENTH 256


Camera6467_plate::Camera6467_plate() :
BaseCamera(),
m_iTimeInvl(3),
m_iVideoWidth(720),
m_iVideoHeight(576),
m_iCompressBigImgSize(COMPRESS_BIG_IMG_SIZE),
m_iCompressSamllImgSize(COMPRESS_PLATE_IMG_SIZE),
m_pTempBin(NULL),
m_pTempBig1(NULL),
g_pUser(NULL),
g_func_ReconnectCallback(NULL),
g_ConnectStatusCallback(NULL),
g_func_DisconnectCallback(NULL),
m_CameraResult(NULL),
m_BufferResult(NULL),
m_hVideoWindow(NULL),
m_hVideo(NULL),
m_pVideoImgBuffer(NULL),
m_bResultComplete(false),
m_bJpegComplete(false),
m_bSaveToBuffer(false),
m_bOverlay(false),
m_bCompress(false)
{
    ReadConfig();
    InitializeCriticalSection(&m_csResult);
}

Camera6467_plate::Camera6467_plate(const char* chIP, HWND hWnd, int Msg) : 
BaseCamera( chIP,   hWnd,  Msg),
m_iTimeInvl(3),
m_iVideoWidth(720),
m_iVideoHeight(576),
m_iCompressBigImgSize(COMPRESS_BIG_IMG_SIZE),
m_iCompressSamllImgSize(COMPRESS_PLATE_IMG_SIZE),
m_pTempBin(NULL),
m_pTempBig1(NULL),
g_pUser(NULL),
g_func_ReconnectCallback(NULL),
g_ConnectStatusCallback(NULL),
g_func_DisconnectCallback(NULL),
m_CameraResult(NULL),
m_BufferResult(NULL),
m_hVideoWindow(NULL),
m_hVideo(NULL),
m_pVideoImgBuffer(NULL),
m_bResultComplete(false),
m_bJpegComplete(false),
m_bSaveToBuffer(false),
m_bOverlay(false),
m_bCompress(false)
{
    //SetConnectStatus_Callback(NULL, NULL, 10);
    ReadConfig();

    if (m_pTempBig1 == NULL)
    {
        m_pTempBig1 = new BYTE[MAX_IMG_SIZE];
    }

    InitializeCriticalSection(&m_csResult);
}

Camera6467_plate::~Camera6467_plate()
{
    SetConnectStatus_Callback(NULL, NULL, 10);
    //StopPlayVideo();
    InterruptionConnection(); 

    if (NULL != m_CameraResult)
    {
        delete m_CameraResult;
        m_CameraResult = NULL;
    }
    if (NULL != m_BufferResult)
    {
        delete m_BufferResult;
        m_BufferResult = NULL;
    }
    if (m_pTempBin)
    {
        delete[] m_pTempBin;
        m_pTempBin = NULL;
    }

    if (m_pTempBig1 != NULL)
    {
        delete[] m_pTempBig1;
        m_pTempBig1 = NULL;
    }

    if (m_pVideoImgBuffer != NULL)
    {
        delete[] m_pVideoImgBuffer;
        m_pVideoImgBuffer = NULL;
    }

    DeleteCriticalSection(&m_csResult);
}

void Camera6467_plate::AnalysisAppendInfo(CameraResult* record)
{
    if (strstr(record->chPlateNO, "无车牌"))
    {
        record->iPlateColor = COLOR_UNKNOW;
        record->iPlateTypeNo = 0;
    }
    else
    {
        char tmpPlateColor[20] = { 0 };
        strncpy_s(tmpPlateColor, record->chPlateNO, 2);
        strncpy_s(record->chPlateColor, record->chPlateNO, 2);
        if (strstr(tmpPlateColor, "蓝"))
        {
            record->iPlateColor = COLOR_BLUE;
#if GUANGXI_JXY
            record->iPlateColor = 2;
#endif
            record->iPlateTypeNo = 2;
        }
        else if (strstr(tmpPlateColor, "黄"))
        {
            record->iPlateColor = COLOR_YELLOW;
#if GUANGXI_JXY
            record->iPlateColor = 1;
#endif
            record->iPlateTypeNo = 1;
        }
        else if (strstr(tmpPlateColor, "黑"))
        {
            record->iPlateColor = COLOR_BLACK;
#if GUANGXI_JXY
            record->iPlateColor = 4;
#endif
            record->iPlateTypeNo = 3;
        }
        else if (strstr(tmpPlateColor, "白"))
        {
            record->iPlateColor = COLOR_WHITE;
#if GUANGXI_JXY
            record->iPlateColor = 3;
#endif
            record->iPlateTypeNo = 6;
        }
        else if (strstr(tmpPlateColor, "红"))
        {
            record->iPlateColor = COLOR_RED;
#if GUANGXI_JXY
            record->iPlateColor = 5;
#endif
            record->iPlateTypeNo = 0;
        }
        else if (strstr(tmpPlateColor, "绿"))
        {
            record->iPlateColor = COLOR_GREEN;
#if GUANGXI_JXY
            record->iPlateColor = 6;
#endif
            record->iPlateTypeNo = 0;
        }
        else
        {
            record->iPlateColor = 0;
            record->iPlateTypeNo = 0;
        }

        //char chFinalGreenPlate[32] = {0};
        //if (strstr(record->chPlateNO, "绿") && strlen(record->chPlateNO) == 11)
        //{
        //    int iLength = strlen(record->chPlateNO);
        //    char chPlateNo[32] = { 0 };
        //    sprintf_s(chPlateNo, "%s", record->chPlateNO);
        //    if (chPlateNo[5] == 'D' || chPlateNo[5] == 'F')
        //    {
        //        //如果第三位为'D'或'F'则定义为渐变绿
        //        printf("第三位为'D'或'F'则定义为渐变绿");
        //        sprintf_s(chFinalGreenPlate, "白绿%s", record->chPlateNO + 2);
        //    }
        //    else if (chPlateNo[iLength - 1] == 'D' || chPlateNo[iLength - 1] == 'F')
        //    {
        //        //如果最后一位是'D'或'F'则定义为黄绿
        //        printf("最后一位是'D'或'F'则定义为黄绿");
        //        sprintf_s(chFinalGreenPlate, "黄绿%s", record->chPlateNO + 2);
        //    }
        //    else
        //    {
        //        sprintf_s(chFinalGreenPlate, "%s", record->chPlateNO);
        //        printf("找不到的话，给不确定");
        //    }

        //    memset(record->chPlateNO, 0, sizeof(record->chPlateNO));
        //    sprintf_s(record->chPlateNO, "%s", chFinalGreenPlate);
        //}
        //else
        //{
        //    //获取车牌号
        //    char sztempPlate[20] = { 0 };
        //    //sprintf_s(sztempPlate, "%s", record->chPlateNO + 2);
        //    sprintf_s(sztempPlate, "%s", record->chPlateNO);
        //    if (NULL != sztempPlate)
        //    {
        //        memset(record->chPlateNO, 0, sizeof(record->chPlateNO));
        //        sprintf_s(record->chPlateNO, "%s", sztempPlate);
        //    }
        //}


        //获取车牌号
        char sztempPlate[20] = { 0 };
        //sprintf_s(sztempPlate, "%s", record->chPlateNO + 2);
        sprintf_s(sztempPlate, "%s", record->chPlateNO);
        if (NULL != sztempPlate)
        {
            memset(record->chPlateNO, 0, sizeof(record->chPlateNO));
            sprintf_s(record->chPlateNO, "%s", sztempPlate);
        }
    }
    char * pchObj = NULL;
    pchObj = (char *)strstr(record->pcAppendInfo, "车道");
    if (pchObj)
    {
        sscanf_s(pchObj, "车道:%d", &(record->iLaneNo));
    }
    else
    {
        record->iLaneNo = 0;
    }
    pchObj = (char *)strstr(record->pcAppendInfo, "路口方向");
    if (pchObj)
    {
        sscanf_s(pchObj, "路口方向:%d", &(record->iDirection));
    }
    else
    {
        record->iDirection = 0;
    }
    //if (0 != record->dw64TimeMS)
    //{
    //	CTime tm(record->dw64TimeMS/1000);
    //	//sprintf_s(record->chPlateTime, "%04d%02d%02d%02d%02d%02d%03d", tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour(), tm.GetMinute(), tm.GetSecond(), record->dw64TimeMS%1000);
    //	sprintf_s(record->chPlateTime, "%04d%02d%02d%02d%02d%02d", tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour(), tm.GetMinute(), tm.GetSecond());
    //}
    //else
    //{
    //	SYSTEMTIME st;
    //	GetLocalTime(&st);
    //	//sprintf_s(record->chPlateTime, "%04d%02d%02d%02d%02d%02d%03d", st.wYear, st.wMonth, st.wDay	,st.wHour, st.wMinute,st.wSecond, st.wMilliseconds);
    //	sprintf_s(record->chPlateTime, "%04d%02d%02d%02d%02d%02d%03d", st.wYear, st.wMonth, st.wDay	,st.wHour, st.wMinute,st.wSecond);
    //}

    //pchObj = (char *)strstr(record->pcAppendInfo, "车尾检测时间");
    pchObj = (char *)strstr(record->pcAppendInfo, "车身结束时间");
    if (pchObj)
    {
        char chTemp[MAX_PATH] = { 0 };
        int iYear = 0, iMonth = 0, iDay = 0, iHour = 0, iMin = 0, iSecond = 0, iMSecond = 0;
        sscanf_s(pchObj, "车身结束时间:%d-%d-%d %d:%d:%d:%d", &iYear, &iMonth, &iDay, &iHour, &iMin, &iSecond, &iMSecond);
        sprintf_s(record->chPlateTime, "%04d%02d%02d%02d%02d%02d", iYear, iMonth, iDay, iHour, iMin, iSecond);
    }
    else
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        //sprintf_s(record->chPlateTime, "%04d%02d%02d%02d%02d%02d%03d", st.wYear, st.wMonth, st.wDay	,st.wHour, st.wMinute,st.wSecond, st.wMilliseconds);
        sprintf_s(record->chPlateTime, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    }

    //------------车道
    pchObj = (char*)strstr(record->pcAppendInfo, "车辆类型");
    if (pchObj)
    {
        char szCarType[20] = { 0 };
        sscanf_s(pchObj, "车辆类型:%s", szCarType, sizeof(szCarType));

        if (strstr(szCarType, "大"))
        {
            record->iVehTypeNo = 2;
        }
        if (strstr(szCarType, "中"))
        {
            record->iVehTypeNo = 2;
        }
        if (strstr(szCarType, "小"))
        {
            record->iVehTypeNo = 1;
        }
        if (strstr(szCarType, "客车"))
        {
            record->iVehTypeNo = 1;
        }
        if (strstr(szCarType, "货车"))
        {
            record->iVehTypeNo = 2;
        }
        if (strstr(szCarType, "牵引车"))
        {
            record->iVehTypeNo = 3;
        }
        if (strstr(szCarType, "专项作业车"))
        {
            record->iVehTypeNo = 4;
        }
        if (strstr(szCarType, "电车"))
        {
            record->iVehTypeNo = 5;
        }
        if (strstr(szCarType, "摩托车"))
        {
            record->iVehTypeNo = 6;
        }
        if (strstr(szCarType, "三轮汽车"))
        {
            record->iVehTypeNo = 7;
        }
        if (strstr(szCarType, "拖拉机"))
        {
            record->iVehTypeNo = 8;
        }
        if (strstr(szCarType, "轮式机械"))
        {
            record->iVehTypeNo = 9;
        }
        if (strstr(szCarType, "全挂车"))
        {
            record->iVehTypeNo = 10;
        }
        if (strstr(szCarType, "半挂车"))
        {
            record->iVehTypeNo = 11;
        }
        if (strstr(szCarType, "其他"))
        {
            record->iVehTypeNo = 12;
        }
    }

    //---------------车身颜色
    pchObj = (char*)strstr(record->pcAppendInfo, "车身颜色:");

    record->iVehBodyColorNo = 0;
    if (pchObj)
    {
        char szBodyColour[256] = { 0 };
        sscanf_s(pchObj, "车身颜色:%s", szBodyColour, sizeof(szBodyColour));

        if (strstr(szBodyColour, "白"))
            record->iVehBodyColorNo = 1;

        if (strstr(szBodyColour, "灰"))
            record->iVehBodyColorNo = 2;

        if (strstr(szBodyColour, "黄"))
            record->iVehBodyColorNo = 3;

        if (strstr(szBodyColour, "粉"))
            record->iVehBodyColorNo = 4;

        if (strstr(szBodyColour, "红"))
            record->iVehBodyColorNo = 5;

        if (strstr(szBodyColour, "紫"))
            record->iVehBodyColorNo = 6;

        if (strstr(szBodyColour, "绿"))
            record->iVehBodyColorNo = 7;

        if (strstr(szBodyColour, "蓝"))
            record->iVehBodyColorNo = 8;

        if (strstr(szBodyColour, "棕"))
            record->iVehBodyColorNo = 9;

        if (strstr(szBodyColour, "黑"))
            record->iVehBodyColorNo = 10;
    }

    //---------车身深浅度
    pchObj = (char*)(strstr(record->pcAppendInfo, "深浅度:"));
    if (pchObj)
    {
        char szBodyColourDeep[256] = { 0 };
        sscanf_s(pchObj, "深浅度:%s", szBodyColourDeep, sizeof(szBodyColourDeep));

        if (strstr(szBodyColourDeep, "深"))
            record->iVehBodyDeepNo = 1;

        if (strstr(szBodyColourDeep, "浅"))
            record->iVehBodyDeepNo = 0;
    }
    //--------------------测速
    pchObj = (char*)(strstr(record->pcAppendInfo, "测速:"));
    if (pchObj)
    {
        int iCarSpeed = 0;
        sscanf_s(pchObj, "测速:%d Km/h", &iCarSpeed);
        record->iSpeed = iCarSpeed;
    }
    //--------------车型----------------------------
    pchObj = (char*)(strstr(record->pcAppendInfo, "车型:"));
    if (pchObj)
    {
        char chTemp[128] = { 0 };
        sscanf_s(pchObj, "车型:%s", chTemp, sizeof(chTemp));
        if (strstr(chTemp, "客1"))
        {
            record->iVehTypeNo = 1;
        }
        else if (strstr(chTemp, "客2"))
        {
            record->iVehTypeNo = 2;
        }
        else if (strstr(chTemp, "客3"))
        {
            record->iVehTypeNo = 3;
        }
        else if (strstr(chTemp, "客4"))
        {
            record->iVehTypeNo = 4;
        }
        else if (strstr(chTemp, "货1"))
        {
            //record->iVehTypeNo = 5;
            record->iVehTypeNo = 11;
        }
        else if (strstr(chTemp, "货2"))
        {
            //record->iVehTypeNo = 6;
            record->iVehTypeNo = 12;
        }
        else if (strstr(chTemp, "货3"))
        {
            //record->iVehTypeNo = 7;
            record->iVehTypeNo = 13;
        }
        else if (strstr(chTemp, "货4"))
        {
            //record->iVehTypeNo = 8;
            record->iVehTypeNo = 14;
        }
        else if (strstr(chTemp, "货5"))
        {
            //record->iVehTypeNo = 8;
            record->iVehTypeNo = 15;
        }
        char chLog[260] = { 0 };
        sprintf_s(chLog, "iVehTypeNo= %d", record->iVehTypeNo);
        WriteLog(chLog);
    }
    //--------------轮数----------------------------
    pchObj = (char*)(strstr(record->pcAppendInfo, "轮数:"));
    if (pchObj)
    {
        int iTemp = 0;
        sscanf_s(pchObj, "轮数:%d", &iTemp);
        record->iWheelCount = iTemp;

        char chLog[260] = { 0 };
        sprintf_s(chLog, "iWheelCount= %d", record->iWheelCount);
        WriteLog(chLog);
    }
    //--------------轴数----------------------------
    pchObj = (char*)(strstr(record->pcAppendInfo, "轴数:"));
    if (pchObj)
    {
        int iTemp = 0;
        sscanf_s(pchObj, "轴数:%d", &iTemp);
        record->iAxletreeCount = iTemp;

        char chLog[260] = { 0 };
        sprintf_s(chLog, "iAxletreeCount= %d", record->iAxletreeCount);
        WriteLog(chLog);
    }
    //--------------轴距----------------------------
    pchObj = (char*)(strstr(record->pcAppendInfo, "轴距:"));
    if (pchObj)
    {
        float fTemp = 0.0;
        sscanf_s(pchObj, "轴距:%f", &fTemp);
        record->fDistanceBetweenAxles = fTemp;

        char chLog[260] = { 0 };
        sprintf_s(chLog, "fDistanceBetweenAxles= %f", record->fDistanceBetweenAxles);
        WriteLog(chLog);
    }
    //--------------车长----------------------------
    pchObj = (char*)(strstr(record->pcAppendInfo, "车长:"));
    if (pchObj)
    {
        float fTemp = 0.0;
        sscanf_s(pchObj, "车长:%f", &fTemp);
        record->fVehLenth = fTemp;

        char chLog[260] = { 0 };
        sprintf_s(chLog, "fVehLenth= %f", record->fVehLenth);
        WriteLog(chLog);
    }
    //--------------车高----------------------------
    pchObj = (char*)(strstr(record->pcAppendInfo, "车高:"));
    if (pchObj)
    {
        float fTemp = 0.0;
        sscanf_s(pchObj, "车高:%f", &fTemp);
        record->fVehHeight = fTemp;

        char chLog[260] = { 0 };
        sprintf_s(chLog, "fVehHeight= %f", record->fVehHeight);
        WriteLog(chLog);
    }

    pchObj = (char*)(strstr(record->pcAppendInfo, "倒车:"));
    if (pchObj)
    {
        record->bBackUpVeh = true;
    }
}

void Camera6467_plate::ReadConfig()
{
    char FileName[MAX_PATH];
    GetModuleFileNameA(NULL, FileName, MAX_PATH - 1);

    PathRemoveFileSpecA(FileName);
    char iniFileName[MAX_PATH] = { 0 };
    char iniDeviceInfoName[MAX_PATH] = { 0 };
#ifdef GUANGXI_DLL
    sprintf_s(iniFileName, "..\\DevInterfaces\\HVCR_Signalway_V%d_%d\\HVCR_Config\\HVCR_Signalway_V%d_%d.ini", PROTOCAL_VERSION, DLL_VERSION, PROTOCAL_VERSION, DLL_VERSION);
#else
    strcat_s(iniFileName, FileName);
    strcat_s(iniFileName, INI_FILE_NAME);
#endif

    //读取可靠性配置文件
    int iValue = GetPrivateProfileIntA("SaveToBufferPath", "Enable", 0, iniFileName);
    m_bSaveToBuffer = (iValue == 1) ? true : false;

    char chTemp[256] = { 0 };
    //sprintf_s(chTemp, "%d", iLog);
    sprintf_s(chTemp, "%d", iValue);
    WritePrivateProfileStringA("SaveToBufferPath", "Enable", chTemp, iniFileName);

    //------------------------------Overlay setting  enable--------------
    iValue = GetPrivateProfileIntA("Overlay", "Enable", 0, iniFileName);
    m_bOverlay = (iValue == 1) ? true : false;

    memset(chTemp, 0, sizeof(chTemp));
    sprintf_s(chTemp, "%d", iValue);
    WritePrivateProfileStringA("Overlay", "Enable", chTemp, iniFileName);

    //------------------------------Compress setting  enable--------------
    iValue = GetPrivateProfileIntA("Compress", "Enable", 0, iniFileName);
    m_bCompress = (iValue == 1) ? true : false;

    memset(chTemp, 0, sizeof(chTemp));
    sprintf_s(chTemp, "%d", iValue);
    WritePrivateProfileStringA("Compress", "Enable", chTemp, iniFileName);

    //-----------------------Compress setting : Big image size--------------
    iValue = GetPrivateProfileIntA("Compress", "BigImgSize", COMPRESS_BIG_IMG_SIZE, iniFileName);
    m_iCompressBigImgSize = (iValue > 0) ? iValue : COMPRESS_BIG_IMG_SIZE;

    memset(chTemp, 0, sizeof(chTemp));
    sprintf_s(chTemp, "%d", iValue);
    WritePrivateProfileStringA("Compress", "BigImgSize", chTemp, iniFileName);

    //-----------------------Compress setting : Small image size--------------
    iValue = GetPrivateProfileIntA("Compress", "SmallImgSize", COMPRESS_BIG_IMG_SIZE, iniFileName);
    m_iCompressSamllImgSize = (iValue > 0) ? iValue : COMPRESS_BIG_IMG_SIZE;

    memset(chTemp, 0, sizeof(chTemp));
    sprintf_s(chTemp, "%d", iValue);
    WritePrivateProfileStringA("Compress", "SmallImgSize", chTemp, iniFileName);

    BaseCamera::ReadConfig();
}

void Camera6467_plate::SetExitStatusVideo()
{
    WriteLog("SetExitStatusVideo begin.");
    if (m_hVideo)
    {
        H264_SetExitStatus((HANDLE)m_hVideo);
    }
    WriteLog("SetExitStatusVideo end.");
}

int Camera6467_plate::RecordInfoBegin(DWORD dwCarID)
{
    char chlogInfo[260] = { 0 };
    sprintf_s(chlogInfo, "RecordInfoBegin -begin- dwCarID = %d", dwCarID);
    WriteLog(chlogInfo);
    if (dwCarID == m_dwLastCarID)
    {
        WriteLog("相同carID,丢弃该结果");
        return 0;
    }
    SetResultComplete(false);
    if (NULL != m_CameraResult)
    {
        delete m_CameraResult;
        m_CameraResult = NULL;
    }
    m_CameraResult = new CameraResult();
    sprintf_s(m_CameraResult->chDeviceIp, "%s", m_strIP.c_str());
    m_CameraResult->dwCarID = dwCarID;
    char chlogInfo2[260] = { 0 };
    sprintf_s(chlogInfo2, "RecordInfoBegin -end- dwCarID = %d", dwCarID);
    WriteLog(chlogInfo2);
    return 0;
}

int Camera6467_plate::RecordInfoEnd(DWORD dwCarID)
{
    char chLog[260] = { 0 };
    sprintf_s(chLog, "RecordInfoEnd begin, dwCarID = %d", dwCarID);
    WriteLog(chLog);
    if (dwCarID != m_dwLastCarID)
    {
        m_dwLastCarID = dwCarID;
    }
    else
    {
        WriteLog("相同CarID, 丢弃该结果");
        return 0;
    }

    if (NULL == m_CameraResult)
    {
        return 0;
    }

    CTime tm(m_CameraResult->dw64TimeMS / 1000);
    SaveModeInfo TempSaveModeInfo;
    sprintf_s(TempSaveModeInfo.chBeginTime, "%d.%02d.%02d_%02d", tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour());
    //WriteHistoryInfo(TempSaveModeInfo);
    
    /**************************************队列模式*****begin*****************************************

    if (m_CameraResult->fVehLenth > (float)m_iSuperLenth)
    {
    char chLog[260] = {0};
    sprintf_s(chLog, "接收到车长%f的车， 超过设定超长车长%d, 清除当前车辆队列",m_CameraResult->fVehLenth, m_iSuperLenth );
    WriteLog(chLog);
    ClearAllResult();
    }

    if (m_CameraResult->bBackUpVeh)
    {
    ClearAllResult();
    delete m_CameraResult;
    m_CameraResult = NULL;
    WriteLog("接收倒车结果， 清除包括倒车结果在内的当前车辆队列。");
    }

    if (NULL != m_CameraResult)
    {
    EnterCriticalSection(&m_csResult);
    if (m_ResultList.size() < 255)
    {
    char chLogList[260] = {0};
    sprintf_s(chLogList, "now the size is %d, put one result in list.", m_ResultList.size());
    WriteLog(chLogList);
    m_ResultList.push_back(m_CameraResult);
    LeaveCriticalSection(&m_csResult);
    }
    else
    {
    LeaveCriticalSection(&m_csResult);
    //todo 将结果缓存的本地缓存目录
    //SaveResultToBuffer(m_CameraResult);

    delete m_CameraResult;
    //WriteLog("The list is full , save the result to buffer path.");
    WriteLog("The list is full , delete the result.");
    }
    m_CameraResult = NULL;
    }
    **************************************队列模式*****begin*****************************************/

    if (m_bOverlay)
    {
        if (m_pTempBig1 == NULL)
        {
            m_pTempBig1 = new BYTE[MAX_IMG_SIZE];
            memset(m_pTempBig1, 0, MAX_IMG_SIZE);
        }
        else
        {
            memset(m_pTempBig1, 0, MAX_IMG_SIZE);
        }

        char chOverlayInfo[256] = { 0 };
        if (strstr(m_CameraResult->chPlateNO, "无"))
        {
            sprintf_s(chOverlayInfo, "时间:%s   车牌号: *****", m_CameraResult->chPlateTime);
        }
        else
        {
            sprintf_s(chOverlayInfo, "时间:%s   车牌号: %s", m_CameraResult->chPlateTime, m_CameraResult->chPlateNO);
        }
        std::string strOverlayInfo(chOverlayInfo);
        std::wstring wstrOverlayIno = Img_string2wstring(strOverlayInfo);

        if (m_CameraResult->CIMG_BestSnapshot.dwImgSize > 0 && m_CameraResult->CIMG_BestSnapshot.pbImgData)
        {
            if (m_pTempBig1)
            {
                memset(m_pTempBig1, 0, MAX_IMG_SIZE);
                long iDestSize = MAX_IMG_SIZE;

                memset(chLog, 0, sizeof(chLog));
                MY_SPRINTF(chLog, "BestSnapshot开始字符叠加， text = %s", chOverlayInfo);
                WriteLog(chLog);
                bool bOverlay = Tool_OverlayStringToImg(&m_CameraResult->CIMG_BestSnapshot.pbImgData, m_CameraResult->CIMG_BestSnapshot.dwImgSize,
                    &m_pTempBig1, iDestSize,
                    wstrOverlayIno.c_str(), 32,
                    10, 30, 255, 255, 255,
                    50);
                if (bOverlay)
                {
                    memset(chLog, 0, sizeof(chLog));
                    MY_SPRINTF(chLog, "字符叠加成功, size = %ld, begin copy..", iDestSize);
                    WriteLog(chLog);

                    delete[] m_CameraResult->CIMG_BestSnapshot.pbImgData;
                    m_CameraResult->CIMG_BestSnapshot.pbImgData = new BYTE[iDestSize];
                    memcpy(m_CameraResult->CIMG_BestSnapshot.pbImgData, m_pTempBig1, iDestSize);
                    m_CameraResult->CIMG_BestSnapshot.dwImgSize = iDestSize;

                    WriteLog("finish copy.");
                }
                else
                {
                    WriteLog("字符叠加失败,使用原图数据.");
                }

            }
        }

        if (m_CameraResult->CIMG_LastSnapshot.dwImgSize > 0 && m_CameraResult->CIMG_LastSnapshot.pbImgData)
        {
            if (m_pTempBig1)
            {
                memset(m_pTempBig1, 0, MAX_IMG_SIZE);
                long iDestSize = MAX_IMG_SIZE;

                memset(chLog, 0, sizeof(chLog));
                MY_SPRINTF(chLog, "LastSnapshot 开始字符叠加， text = %s", chOverlayInfo);
                WriteLog(chLog);
                bool bOverlay = Tool_OverlayStringToImg(&m_CameraResult->CIMG_LastSnapshot.pbImgData, m_CameraResult->CIMG_LastSnapshot.dwImgSize,
                    &m_pTempBig1, iDestSize,
                    wstrOverlayIno.c_str(), 32,
                    10, 10, 255, 255, 255,
                    50);
                if (bOverlay)
                {
                    memset(chLog, 0, sizeof(chLog));
                    MY_SPRINTF(chLog, "字符叠加成功, size = %ld, begin copy..", iDestSize);
                    WriteLog(chLog);

                    delete[] m_CameraResult->CIMG_LastSnapshot.pbImgData;
                    m_CameraResult->CIMG_LastSnapshot.pbImgData = new BYTE[iDestSize];
                    memcpy(m_CameraResult->CIMG_LastSnapshot.pbImgData, m_pTempBig1, iDestSize);
                    m_CameraResult->CIMG_LastSnapshot.dwImgSize = iDestSize;

                    WriteLog("finish copy.");
                }
                else
                {
                    WriteLog("字符叠加失败,使用原图数据.");
                }
            }
        }
    }

    if (m_bSaveToBuffer)
    {
        SaveResultToBufferPath(m_CameraResult);
    }

    if (m_bCompress)
    {
        WriteLog("开始压缩车牌小图.");
        CompressImg(m_CameraResult->CIMG_PlateImage, m_iCompressSamllImgSize);
        WriteLog("开始压缩最清晰大图.");
        CompressImg(m_CameraResult->CIMG_BestSnapshot, m_iCompressBigImgSize);
        WriteLog("开始压缩最后大图.");
        CompressImg(m_CameraResult->CIMG_LastSnapshot, m_iCompressBigImgSize);
        WriteLog("全部压缩结束.");
    }

    EnterCriticalSection(&m_csResult);
    if (NULL != m_BufferResult)
    {
        delete m_BufferResult;
        m_BufferResult = NULL;
    }
    m_BufferResult = new CameraResult(*m_CameraResult);
    m_bResultComplete = true;
    LeaveCriticalSection(&m_csResult);

    SYSTEMTIME st;
    GetLocalTime(&st);

    if (NULL != m_hWnd)
    {
    	WriteLog("PostMessage");
    	//::PostMessage(*((HWND*)m_hWnd),m_iMsg, 1, 0);
    	::PostMessage(m_hWnd,m_iMsg, (WPARAM)1,0);
    }

    char chLog2[260] = { 0 };
    sprintf_s(chLog2, sizeof(chLog2), "RecordInfoEnd end, dwCarID = %d", dwCarID);
    WriteLog(chLog2);
    return 0;
}

int Camera6467_plate::RecordInfoPlate(DWORD dwCarID, LPCSTR pcPlateNo, LPCSTR pcAppendInfo, DWORD dwRecordType, DWORD64 dw64TimeMS)
{
    SetResultComplete(false);

    if (NULL == m_CameraResult)
    {
        return -1;
    }
    char chlogInfo[260] = { 0 };
    sprintf_s(chlogInfo, "RecordInfoPlate -begin- dwCarID = %d", dwCarID);
    WriteLog(chlogInfo);
    if (dwCarID == m_dwLastCarID)
    {
        WriteLog("相同carID,丢弃该结果");
        return 0;
    }


    if (m_CameraResult->dwCarID == dwCarID)
    {
        m_CameraResult->dw64TimeMS = dw64TimeMS;
        //if (!strstr(pcPlateNo, "无"))
        //{
        //	std::string strPlate(pcPlateNo);
        //	if (m_strLastPlateNo == pcPlateNo && dwCarID != m_dwLastCarID)
        //	{
        //		m_dwReplaceCarID = dwCarID;
        //		//sprintf_s(m_CameraResult->chPlateNO, "蓝贵A%05d",GetTickCount()%100000);
        //		sprintf_s(m_CameraResult->chPlateNO, " 无车牌");
        //		m_strLastPlateNo = std::string(m_CameraResult->chPlateNO);
        //		memset(chlogInfo, 0, sizeof(chlogInfo));
        //		sprintf_s(chlogInfo," The plate is same as last one %s ,replace it to %s", pcPlateNo,m_CameraResult->chPlateNO);
        //		WriteLog(chlogInfo);
        //	}
        //	else if(dwCarID == m_dwReplaceCarID)
        //	{
        //		sprintf_s(m_CameraResult->chPlateNO, "%s",m_strLastPlateNo.c_str());

        //		memset(chlogInfo, 0, sizeof(chlogInfo));
        //		sprintf_s(chlogInfo," The car id is same as replace one %s ,use it", m_CameraResult->chPlateNO);
        //		WriteLog(chlogInfo);
        //	}
        //	else
        //	{
        //		sprintf_s(m_CameraResult->chPlateNO, "%s",pcPlateNo);
        //		m_strLastPlateNo= strPlate;
        //	}
        //}
        //else
        {
            sprintf_s(m_CameraResult->chPlateNO, "%s", pcPlateNo);
        }
        //sprintf_s(m_CameraResult->chPlateNO, "%s",pcPlateNo);
        //sprintf_s(m_CameraResult->chPlateNO, "%s",pcPlateNo+3);        
        memset(m_CameraResult->pcAppendInfo, 0, sizeof(m_CameraResult->pcAppendInfo));
        HVAPIUTILS_ParsePlateXmlStringEx(pcAppendInfo, m_CameraResult->pcAppendInfo, 2048);
        WriteLog(m_CameraResult->chPlateNO);
        WriteLog(m_CameraResult->pcAppendInfo);
        AnalysisAppendInfo(m_CameraResult);

        {
            char chTemp[BUFFERLENTH] = { 0 };
            int iLenth = BUFFERLENTH;

            memset(chTemp, '\0', BUFFERLENTH);
            if (GetDataFromAppenedInfo((char*)pcAppendInfo, "Confidence", chTemp, &iLenth))
            {
                float fConfidence = 0.0;
                sscanf_s(chTemp, "%f", &fConfidence);
                m_CameraResult->iReliability = (int)(fConfidence * 10000);
            }
        }

        char qstrPlateTime[260] = { 0 };
        sprintf_s(qstrPlateTime, "the plate time is %s, %lld", m_CameraResult->chPlateTime, dw64TimeMS);
        WriteLog(qstrPlateTime);
    }
    char chlogInfo2[260] = { 0 };
    sprintf_s(chlogInfo2, "RecordInfoPlate -end- dwCarID = %d", dwCarID);
    WriteLog(chlogInfo2);
    return 0;
}

int Camera6467_plate::RecordInfoBigImage(DWORD dwCarID, WORD wImgType, WORD wWidth, WORD wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dw64TimeMS)
{
    SetResultComplete(false);

    if (NULL == m_CameraResult)
    {
        return -1;
    }
    char chlogInfo[260] = { 0 };
    sprintf_s(chlogInfo, "RecordInfoBigImage -begin- dwCarID = %ld, dwRecordType = %#x， ImgType=%d, size = %ld", dwCarID, dwRecordType, wImgType, dwImgDataLen);
    WriteLog(chlogInfo);
    if (dwCarID == m_dwLastCarID)
    {
        WriteLog("相同carID,丢弃该结果");
        return 0;
    }
    if (m_CameraResult->dwCarID == dwCarID)
    {
        if (wImgType == RECORD_BIGIMG_BEST_SNAPSHOT)
        {
            WriteLog("RecordInfoBigImage BEST_SNAPSHO  ");

            CopyDataToIMG(m_CameraResult->CIMG_BestSnapshot, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);
        }
        else if (wImgType == RECORD_BIGIMG_LAST_SNAPSHOT)
        {
            WriteLog("RecordInfoBigImage LAST_SNAPSHOT  ");

            CopyDataToIMG(m_CameraResult->CIMG_LastSnapshot, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);
        }
        else if (wImgType == RECORD_BIGIMG_BEGIN_CAPTURE)
        {
            WriteLog("RecordInfoBigImage BEGIN_CAPTURE  ");

            CopyDataToIMG(m_CameraResult->CIMG_BeginCapture, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);
        }
        else if (wImgType == RECORD_BIGIMG_BEST_CAPTURE)
        {
            WriteLog("RecordInfoBigImage BEST_CAPTURE  ");

            CopyDataToIMG(m_CameraResult->CIMG_BestCapture, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);
        }
        else if (wImgType == RECORD_BIGIMG_LAST_CAPTURE)
        {
            WriteLog("RecordInfoBigImage LAST_CAPTURE  ");

            CopyDataToIMG(m_CameraResult->CIMG_LastCapture, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);
        }
        else
        {
            WriteLog("RecordInfoBigImage other Img, put it to  LAST_CAPTURE .");
            CopyDataToIMG(m_CameraResult->CIMG_LastCapture, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);
        }
    }
    char chlogInfo2[260] = { 0 };
    sprintf_s(chlogInfo2, "RecordInfoBigImage -end- dwCarID = %d", dwCarID);
    WriteLog(chlogInfo2);
    return 0;
}

int Camera6467_plate::RecordInfoSmallImage(DWORD dwCarID, WORD wWidth, WORD wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dw64TimeMS)
{
    SetResultComplete(false);
    if (NULL == m_CameraResult)
    {
        return -1;
    }
    char chlogInfo[260] = { 0 };
    sprintf_s(chlogInfo, "RecordInfoSmallImage  -begin- dwCarID = %d", dwCarID);
    WriteLog(chlogInfo);
    if (dwCarID == m_dwLastCarID)
    {
        WriteLog("相同carID,丢弃该结果");
        return 0;
    }

    int iBuffLen = 1024 * 1024;
    if (m_CameraResult->dwCarID == dwCarID)
    {
        if (NULL != m_CameraResult->CIMG_PlateImage.pbImgData)
        {
            delete[] m_CameraResult->CIMG_PlateImage.pbImgData;
            m_CameraResult->CIMG_PlateImage.pbImgData = NULL;
        }
        m_CameraResult->CIMG_PlateImage.pbImgData = new BYTE[iBuffLen];
        WriteLog("RecordInfoSmallImage 内存申请.");
        if (m_CameraResult->CIMG_PlateImage.pbImgData != NULL)
        {
            WriteLog("RecordInfoSmallImage 内存申请成功.");
            memset(m_CameraResult->CIMG_PlateImage.pbImgData, 0, iBuffLen);
            HRESULT Hr = HVAPIUTILS_SmallImageToBitmapEx(pbPicData,
                wWidth,
                wHeight,
                m_CameraResult->CIMG_PlateImage.pbImgData,
                &iBuffLen);
            if (Hr == S_OK)
            {
                m_CameraResult->CIMG_PlateImage.wImgWidth = wWidth;
                m_CameraResult->CIMG_PlateImage.wImgHeight = wHeight;
                m_CameraResult->CIMG_PlateImage.dwImgSize = iBuffLen;
                if (m_Small_IMG_Temp.pbImgData == NULL)
                {
                    m_Small_IMG_Temp.pbImgData = new BYTE[MAX_IMG_SIZE];
                    memset(m_Small_IMG_Temp.pbImgData, 0, MAX_IMG_SIZE);
                }
                if (m_Small_IMG_Temp.pbImgData)
                {
                    DWORD iDestLenth = MAX_IMG_SIZE;
                    memset(m_Small_IMG_Temp.pbImgData, 0, MAX_IMG_SIZE);
                    WriteLog("convert bmp to jpeg , begin .");
                    bool bScale = Tool_Img_ScaleJpg(m_CameraResult->CIMG_PlateImage.pbImgData,
                        m_CameraResult->CIMG_PlateImage.dwImgSize,
                        m_Small_IMG_Temp.pbImgData,
                        &iDestLenth,
                        m_CameraResult->CIMG_PlateImage.wImgWidth,
                        m_CameraResult->CIMG_PlateImage.wImgHeight,
                        80);
                    if (bScale)
                    {
                        WriteLog("convert bmp to jpeg success, begin copy.");
                        memset(m_CameraResult->CIMG_PlateImage.pbImgData, 0, m_CameraResult->CIMG_PlateImage.dwImgSize);
                        memcpy(m_CameraResult->CIMG_PlateImage.pbImgData, m_Small_IMG_Temp.pbImgData, iDestLenth);
                        m_CameraResult->CIMG_PlateImage.dwImgSize = iDestLenth;
                        WriteLog("convert bmp to jpeg success, finish copy.");
                    }
                    else
                    {
                        WriteLog("convert bmp to jpeg failed, use default.");
                    }
                }
            }
            else
            {
                WriteLog("HVAPIUTILS_SmallImageToBitmapEx 失败.");
            }
        }
        else
        {
            WriteLog("RecordInfoSmallImage 内存申请失败.");
        }
    }
    char chlogInfo2[260] = { 0 };
    sprintf_s(chlogInfo2, "RecordInfoSmallImage  -end- dwCarID = %d", dwCarID);
    WriteLog(chlogInfo2);
    return 0;
}

int Camera6467_plate::RecordInfoBinaryImage(DWORD dwCarID, WORD wWidth, WORD wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dw64TimeMS)
{
    SetResultComplete(false);

    if (NULL == m_CameraResult)
    {
        return -1;
    }
    char chlogInfo[260] = { 0 };
    sprintf_s(chlogInfo, "RecordInfoBinaryImage -begin- dwCarID = %d", dwCarID);
    WriteLog(chlogInfo);

    if (dwCarID == m_dwLastCarID)
    {
        WriteLog("相同carID,丢弃该结果");
        return 0;
    }
    //int iBufferlength = 1024 * 1024;
    //if (m_pTempBin == NULL)
    //{
    //    m_pTempBin = new BYTE[1024 * 1024];
    //    memset(m_pTempBin, 0x00, iBufferlength);
    //}
    //if (m_pTempBin)
    //{
    //    memset(m_pTempBin, 0x00, iBufferlength);

    //    HRESULT hRet = HVAPIUTILS_BinImageToBitmapEx(pbPicData, m_pTempBin, &iBufferlength);
    //    if (hRet == S_OK)
    //    {
    //        if (m_Bin_IMG_Temp.pbImgData == NULL)
    //        {
    //            m_Bin_IMG_Temp.pbImgData = new BYTE[MAX_IMG_SIZE];
    //            memset(m_Bin_IMG_Temp.pbImgData, 0x00, MAX_IMG_SIZE);
    //        }
    //        if (m_Bin_IMG_Temp.pbImgData)
    //        {
    //            DWORD iDestLenth = MAX_IMG_SIZE;
    //            memset(m_Bin_IMG_Temp.pbImgData, 0x00, MAX_IMG_SIZE);
    //            WriteLog("bin, convert bmp to jpeg , begin .");
    //            bool bScale = Tool_Img_ScaleJpg(m_pTempBin,
    //                iBufferlength,
    //                m_Bin_IMG_Temp.pbImgData,
    //                &iDestLenth,
    //                wWidth,
    //                wHeight,
    //                90);
    //            if (bScale)
    //            {
    //                WriteLog("bin, convert bmp to jpeg success, begin copy.");
    //                CopyDataToIMG(m_CameraResult->CIMG_BinImage, m_Bin_IMG_Temp.pbImgData, wWidth, wHeight, iDestLenth, 0);
    //                WriteLog("bin, convert bmp to jpeg success, finish copy.");
    //            }
    //            else
    //            {
    //                WriteLog("bin, convert bmp to jpeg failed, use default.");
    //            }
    //        }
    //        else
    //        {
    //            WriteLog("m_Bin_IMG_Temp  is null.");
    //        }
    //    }
    //    else
    //    {
    //        WriteLog("HVAPIUTILS_BinImageToBitmapEx, failed, use default.");
    //        CopyDataToIMG(m_CameraResult->CIMG_BinImage, pbPicData, wWidth, wHeight, dwImgDataLen, 0);
    //    }
    //}
    //else
    {
        WriteLog("m_pTempBin is NULL ,  use default.");
        CopyDataToIMG(m_CameraResult->CIMG_BinImage, pbPicData, wWidth, wHeight, dwImgDataLen, 0);
    }
    char chlogInfo2[260] = { 0 };
    sprintf_s(chlogInfo2, "RecordInfoBinaryImage -end- dwCarID = %d", dwCarID);
    WriteLog(chlogInfo2);
    return 0;
}

int Camera6467_plate::DeviceJPEGStream(PBYTE pbImageData, DWORD dwImageDataLen, DWORD dwImageType, LPCSTR szImageExtInfo)
{
    static int iCout = 0;
    if (iCout++ > 100)
    {
        WriteLog("receive one jpeg frame.");
        iCout = 0;
    }

    EnterCriticalSection(&m_csResult);
    m_bJpegComplete = false;

    m_CIMG_StreamJPEG.dwImgSize = dwImageDataLen;
    m_CIMG_StreamJPEG.wImgWidth = 1920;
    m_CIMG_StreamJPEG.wImgHeight = 1080;
    if (NULL == m_CIMG_StreamJPEG.pbImgData)
    {
        m_CIMG_StreamJPEG.pbImgData = new unsigned char[MAX_IMG_SIZE];
        memset(m_CIMG_StreamJPEG.pbImgData, 0, MAX_IMG_SIZE);
    }
    if (m_CIMG_StreamJPEG.pbImgData)
    {
        memset(m_CIMG_StreamJPEG.pbImgData, 0, MAX_IMG_SIZE);
        memcpy(m_CIMG_StreamJPEG.pbImgData, pbImageData, dwImageDataLen);
        m_bJpegComplete = true;
    }
    LeaveCriticalSection(&m_csResult);

    return 0;
}

void Camera6467_plate::SetDisConnectCallback(void* funcDisc, void* pUser)
{
    EnterCriticalSection(&m_csFuncCallback);
    g_func_DisconnectCallback = funcDisc;
    g_pUser = pUser;
    LeaveCriticalSection(&m_csFuncCallback);
}

void Camera6467_plate::SetReConnectCallback(void* funcReco, void* pUser)
{
    EnterCriticalSection(&m_csFuncCallback);
    g_func_ReconnectCallback = funcReco;
    g_pUser = pUser;
    LeaveCriticalSection(&m_csFuncCallback);
}

bool Camera6467_plate::GetOneJpegImg(CameraIMG &destImg)
{
    WriteLog("GetOneJpegImg::begin.");
    bool bRet = false;

    if (!destImg.pbImgData)
    {
        WriteLog("GetOneJpegImg:: allocate memory.");
        destImg.pbImgData = new unsigned char[MAX_IMG_SIZE];
        memset(destImg.pbImgData, 0, MAX_IMG_SIZE);
        WriteLog("GetOneJpegImg:: allocate memory success.");
    }

    EnterCriticalSection(&m_csResult);
    if (m_bJpegComplete)
    {
        if (destImg.pbImgData)
        {
            memset(destImg.pbImgData, 0, MAX_IMG_SIZE);
            memcpy(destImg.pbImgData, m_CIMG_StreamJPEG.pbImgData, m_CIMG_StreamJPEG.dwImgSize);

            destImg.dwImgSize = m_CIMG_StreamJPEG.dwImgSize;
            destImg.wImgHeight = m_CIMG_StreamJPEG.wImgHeight;
            destImg.wImgWidth = m_CIMG_StreamJPEG.wImgWidth;
            bRet = true;
            WriteLog("GetOneJpegImg success.");
            m_bJpegComplete = false;
        }
        else
        {
            WriteLog("GetOneJpegImg:: allocate memory failed.");
        }
    }
    else
    {
        WriteLog("GetOneJpegImg the image is not ready.");
    }
    LeaveCriticalSection(&m_csResult);
    WriteLog("GetOneJpegImg:: end.");

    return bRet;
}

void Camera6467_plate::SendConnetStateMsg(bool isConnect)
{
    //if (m_hWnd == NULL)
    //	return;

    if (isConnect)
    {
        //EnterCriticalSection(&m_csFuncCallback);
        //if (g_ConnectStatusCallback)
        //{
        //    LeaveCriticalSection(&m_csFuncCallback);
        //    //char chIP[32] = { 0 };
        //    //sprintf_s(chIP, "%s", m_strIP.c_str());
        //    //g_ConnectStatusCallback(m_iIndex, 0, g_pUser);
        //}
        //else
        //{
        //    LeaveCriticalSection(&m_csFuncCallback);
        //}

        EnterCriticalSection(&m_csFuncCallback);
        ::PostMessage(m_hWnd,m_iConnectMsg, NULL,NULL);
        LeaveCriticalSection(&m_csFuncCallback);
    }
    else
    {
        //EnterCriticalSection(&m_csFuncCallback);
        //if (g_ConnectStatusCallback)
        //{
        //    LeaveCriticalSection(&m_csFuncCallback);
        //    char chIP[32] = { 0 };
        //    sprintf_s(chIP, "%s", m_strIP.c_str());
        //    //g_ConnectStatusCallback(m_iIndex, -100, g_pUser);
        //}
        //else
        //{
        //    LeaveCriticalSection(&m_csFuncCallback);
        //}

        EnterCriticalSection(&m_csFuncCallback);
        ::PostMessage(m_hWnd,m_iDisConMsg, NULL,NULL);
        LeaveCriticalSection(&m_csFuncCallback);
    }
}

void Camera6467_plate::SetConnectStatus_Callback(void* func, void* pUser, int TimeInterval)
{
    EnterCriticalSection(&m_csFuncCallback);
    //g_ConnectStatusCallback = (CBFun_GetDevStatus)func;
    g_pUser = pUser;
    m_iTimeInvl = TimeInterval;
    LeaveCriticalSection(&m_csFuncCallback);
}

bool Camera6467_plate::GetResultComplete()
{
    bool bFinish = false;
    EnterCriticalSection(&m_csResult);
    bFinish = m_bResultComplete;
    LeaveCriticalSection(&m_csResult);
    return bFinish;
}

CameraResult* Camera6467_plate::GetOneResult()
{
    CameraResult* tempResult = NULL;
    //EnterCriticalSection(&m_csResult);	
    //if (m_ResultList.size() > 0)
    //{
    //	tempResult = m_ResultList.front();
    //	m_ResultList.pop_front();
    //}
    //LeaveCriticalSection(&m_csResult);

    if (GetResultComplete())
    {
        EnterCriticalSection(&m_csResult);
        tempResult = new CameraResult(*m_BufferResult);
        LeaveCriticalSection(&m_csResult);
    }
    return tempResult;
}

void Camera6467_plate::SetVideoPlayHandle(HWND handle, int Width, int Height)
{
    EnterCriticalSection(&m_csFuncCallback);
    m_hVideoWindow = handle;
    m_iVideoHeight = Height;
    m_iVideoWidth = Width;
    LeaveCriticalSection(&m_csFuncCallback);
}

bool Camera6467_plate::StartPlayVideo()
{
    if (m_hVideo)
    {
        WriteLog("StartPlayVideo, the video handle is already exist, please close it first.");
        return false;
    }
    else
    {
        char chUrl[256] = {0};
        sprintf_s(chUrl, sizeof(chUrl), "rtsp://%s:554/h264ESVideoTest", m_strIP.c_str());
        WriteLog(chUrl);
        EnterCriticalSection(&m_csFuncCallback);
        m_hVideo = H264_Play(m_hVideoWindow, chUrl);
        LeaveCriticalSection(&m_csFuncCallback);
        if (m_hVideo)
        {
            WriteLog("play video success.");
            return true;
        }
        else
        {
            WriteLog("play video failed.");
            return false;
        }
    }
}

void Camera6467_plate::StopPlayVideo()
{
    WriteLog("StopPlayVideo begin.");
    if (m_hVideo)
    {
        H264_Destroy((HANDLE)m_hVideo);
        m_hVideo = NULL;
    }
    WriteLog("StopPlayVideo end.");
}

bool Camera6467_plate::GetOneImgFromVideo(int format, PBYTE dataBuffer, int* bufferLenght)
{
    char chLog[256] = {0};
    sprintf_s(chLog, sizeof(chLog), "GetOneImgFromVideo, begin, format =%d, dataBuffer = %p, buffer length = %d", format, dataBuffer, bufferLenght);
    WriteLog(chLog);
    if (!dataBuffer || *bufferLenght <= 0)
    {
        WriteLog("GetOneImgFromVideo, dataBuffer is NULL, please input correct parameter.");
        return false;
    }
    if (m_pVideoImgBuffer == NULL)
    {
        WriteLog("GetOneImgFromVideo, apply buffer space.");
        m_pVideoImgBuffer = new BYTE[MAX_IMG_SIZE];
    }
    if (m_pVideoImgBuffer)
    {
        WriteLog("GetOneImgFromVideo, Initialization buffer space.");
        memset(m_pVideoImgBuffer, 0, MAX_IMG_SIZE);

        if (!m_hVideo)
        {
            WriteLog("GetOneImgFromVideo,the video is not connect, please connect first.");
            return false;
        }
        int iImgBufferSize = MAX_IMG_SIZE;
        int iImgWidth = 1920, iImgHeight = 1080;
        if (H264_GetOneBmpImg((HANDLE)m_hVideo, m_pVideoImgBuffer, iImgBufferSize, iImgWidth, iImgHeight))
        {
            memset(chLog, 0, sizeof(chLog));
            sprintf_s(chLog, "GetOneImgFromVideo, get image success, image length = %d, width = %d, height= %d", iImgBufferSize, iImgWidth, iImgHeight);
            WriteLog(chLog);
            if (format == VIDEO_IMG_FORMAT_BMP)
            {
                if (*bufferLenght < iImgBufferSize)
                {
                    memset(chLog, 0, sizeof(chLog));
                    sprintf_s(chLog, sizeof(chLog), "the format is bmp, but the buffer size is not enough");
                    WriteLog(chLog);
                    *bufferLenght = iImgBufferSize;
                    return false;
                }
                WriteLog("copy the data to buffer.");
                memcpy(dataBuffer, m_pVideoImgBuffer, iImgBufferSize);
                *bufferLenght = iImgBufferSize;
                WriteLog("finish copy.");
                return true;
            }
            else
            {
                CameraIMG camImg;
                camImg.pbImgData = m_pVideoImgBuffer;
                camImg.wImgWidth = iImgWidth;
                camImg.wImgHeight = iImgHeight;
                camImg.dwImgSize = iImgBufferSize;

                int iOrigeLength = *bufferLenght;
                CompressImg(camImg, iOrigeLength);
                if (camImg.dwImgSize > iOrigeLength)
                {
                    WriteLog("the format is jpeg, but compress size is still larger than buffer size, compress image failed.");
                    camImg.pbImgData = NULL;
                    *bufferLenght = camImg.dwImgSize;
                    return false;
                }
                else
                {
                    WriteLog("the format is jpeg, compress image success, copy the data to buffer.");
                    memcpy(dataBuffer, camImg.pbImgData, camImg.dwImgSize);
                    *bufferLenght = camImg.dwImgSize;
                    WriteLog("finish copy.");
                    camImg.pbImgData = NULL;
                    return true;
                }
            }
        }
        else
        {
            WriteLog("GetOneImgFromVideo, get image failed.");
            return false;
        }
    }
    else
    {
        WriteLog("GetOneImgFromVideo, the buffer space is apply failed.");
        return false;
    }
}

void Camera6467_plate::SetResultComplete(bool bfinish)
{
    EnterCriticalSection(&m_csResult);
    m_bResultComplete = bfinish;
    LeaveCriticalSection(&m_csResult);
}

int Camera6467_plate::GetTimeInterval()
{
    int iTimeInterval = 1;
    EnterCriticalSection(&m_csFuncCallback);
    iTimeInterval = m_iTimeInvl;
    LeaveCriticalSection(&m_csFuncCallback);
    return iTimeInterval;
}

void Camera6467_plate::ThreadFunc_CheckStatus()
{
    int iLastStatus = -1;
    INT64 iLastTick = 0, iCurrentTick = 0;
    int iFirstConnctSuccess = -1;

    while (!GetCheckThreadExit())
    {
        Sleep(50);
        iCurrentTick = GetTickCount();
        int iTimeInterval = GetTimeInterval();
        if ((iCurrentTick - iLastTick) >= (iTimeInterval * 1000))
        {
            int iStatus = GetCamStatus();
            if (iStatus !=iLastStatus)
            {
                if (iStatus == 0)
                {
                    //if (iStatus != iLastStatus)
                    //{
                    //	pThis->SendConnetStateMsg(true);
                    //}
                    SendConnetStateMsg(true);
                    WriteLog("设备连接正常.");
                    iFirstConnctSuccess = 0;
                }
                else
                {
                    SendConnetStateMsg(false);
                    WriteLog("设备连接失败, 尝试重连");

                    if (iFirstConnctSuccess == -1)
                    {
                        //pThis->ConnectToCamera();
                    }
                }
            }
            iLastStatus = iStatus;

            iLastTick = iCurrentTick;
        }
    }
}
