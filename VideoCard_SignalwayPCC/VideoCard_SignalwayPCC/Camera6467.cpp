#include "stdafx.h"
#include "Camera6467.h"
//#include "HvDevice/HvDeviceBaseType.h"
//#include "HvDevice/HvDeviceCommDef.h"
//#include "HvDevice/HvDeviceNew.h"
#include "tinyxml/tinyxml.h"
#include<math.h>
#include<shellapi.h>
#include <process.h>
//#pragma comment(lib, "./lib/hvdevice/HvDevice.lib")
//注意：这里要把HvDeviceDLL.h 里面的 #include "HvDeviceUtils.h" 注释掉,否则无法编译通过
#include "HvDevice/HvDeviceBaseType.h"
#include "HvDevice/HvDeviceCommDef.h"
#include "HvDevice/HvDeviceNew.h"
#include "HvDevice/HvCamera.h"
#pragma comment(lib, "HvDevice/HvDevice.lib")
#define  VEHICLE_LISTEN_PORT 99999
#include <WinSock2.h>
#pragma comment(lib,"WS2_32.lib")
//#pragma comment(lib, "H264API/H264.lib")
#include "utilityTool/ToolFunction.h"


//#define  GUANGXI_JXY 1
#define GUANGXI_DLL 1

//#define  COLOR_UNKNOW 9
//#define  COLOR_BLUE 0
//#define  COLOR_YELLOW 1
//#define  COLOR_BLACK 2
//#define  COLOR_WHITE 3
//#define  COLOR_RED 4
//#define  COLOR_GREEN 5

#define  BUFFERLENTH 256
unsigned int __stdcall  StatusCheckThread_plate(LPVOID lpParam);


Camera6467::Camera6467()
{
	m_CameraResult = NULL;
	m_BufferResult = NULL;
	m_hHvHandle = NULL;
	m_hWnd = NULL;
	m_hPlayH264 = NULL;
	m_strIP = std::string("");	
	m_iCompressQuality = 20;
	m_bResultComplete = false;
	m_dwLastCarID = -1;
	m_bSaveToBuffer = false;
	m_iSuperLenth = 13;
	m_iVedioWidth = 1920;
	m_iVedioHeight = 1080;
    m_iTimeInvl = 20;

    g_ResultCallback = NULL;
    g_ConnectStatusCallback = NULL;

    m_pBigImageCallback = NULL;
    m_pBigImageCallbackUserData = NULL;

	m_CIMG_StreamJPEG.pbImgData = NULL;
	//合成图片初始化
	GetEncoderClsid(L"image/jpeg", &m_jpgClsid);
	GetEncoderClsid(L"image/bmp", &m_bmpClsid);

	ReadConfig();
	InitializeCriticalSection(&m_csLog);
	InitializeCriticalSection(&m_csResult);
	InitializeCriticalSection(&m_csFuncCallback);

	g_pUser = NULL;
	m_pTempBin = NULL;
	m_hStatusCheckThread =NULL;
	m_CaptureImg = NULL;
	m_pTempBig = NULL;
	m_bStatusCheckThreadExit = false;
    m_hStatusCheckThread = (HANDLE)_beginthreadex(NULL, 0, StatusCheckThread_plate, this, 0, NULL);
}

Camera6467::Camera6467( const char* chIP, HWND  hWnd, int Msg)
//Camera6467::Camera6467( const char* chIP, HWND*  hWnd, int Msg)
{
	m_strIP = std::string(chIP);
	m_hWnd = hWnd;
	m_iMsg = Msg;

	m_hHvHandle = NULL;
	m_iCompressQuality = 20;
	m_CameraResult = NULL;
	m_BufferResult = NULL;
	m_CIMG_StreamJPEG.pbImgData = NULL;
	m_bResultComplete = false;
	m_bSaveToBuffer = false;
	m_dwLastCarID = -1;
	m_iSuperLenth =13;
	m_iVedioWidth = 1920;
	m_iVedioHeight = 1080;
    m_iTimeInvl = 20;

    g_ResultCallback = NULL;
    g_ConnectStatusCallback = NULL;

    m_pBigImageCallback = NULL;
    m_pBigImageCallbackUserData = NULL;

	//m_bStatusCheckThreadExit = false;
	//合成图片初始化
	GetEncoderClsid(L"image/jpeg", &m_jpgClsid);
	GetEncoderClsid(L"image/bmp", &m_bmpClsid);

	ReadConfig();
	InitializeCriticalSection(&m_csLog);
	InitializeCriticalSection(&m_csResult);
	InitializeCriticalSection(&m_csFuncCallback);

	g_pUser = NULL;
	m_pTempBin = NULL;
	m_CaptureImg = NULL;
	m_pTempBig = NULL;

	m_bStatusCheckThreadExit = false;
	m_hStatusCheckThread = (HANDLE)_beginthreadex(NULL, 0, StatusCheckThread_plate, this, 0, NULL);
}

Camera6467::~Camera6467()
{
    SetResult_Callback(NULL, NULL);
    SetConnectStatus_Callback(NULL, NULL, 0);

	m_bStatusCheckThreadExit = true;
	InterruptionConnection();

	//结束状态获取线程
	if (m_hStatusCheckThread != NULL)
	{
		if (WaitForSingleObject(m_hStatusCheckThread, INFINITE) == WAIT_OBJECT_0)
		{
		}
		//TerminateThread(m_hStatusCheckThread, 0);
		CloseHandle(m_hStatusCheckThread);
		m_hStatusCheckThread = NULL;
	}

	if(NULL != m_hHvHandle)
	{
		HVAPI_CloseEx((HVAPI_HANDLE_EX)m_hHvHandle);
		m_hHvHandle = NULL;
	}
	if(NULL != m_CameraResult)
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
	if (m_CaptureImg)
	{
		delete[] m_CaptureImg;
		m_CaptureImg = NULL;
	}
	if (m_pTempBig)
	{
		delete[] m_pTempBig;
		m_pTempBig = NULL;
	}
	
	m_bResultComplete = false;
	//m_strIP.clear();
	ClearAllResult();
	m_hWnd = NULL;
	WriteLog("finish delete Camera");
	DeleteCriticalSection(&m_csLog);
	DeleteCriticalSection(&m_csResult);
	DeleteCriticalSection(&m_csFuncCallback);
}

void Camera6467::ReadHistoryInfo()
{
	char FileName[MAX_PATH];
	GetModuleFileNameA(NULL, FileName, MAX_PATH-1);

	PathRemoveFileSpecA(FileName);
	char iniFileName[MAX_PATH] = {0};
	char iniDeviceInfoName[MAX_PATH] = {0};
	//strcat_s(iniFileName, FileName);
	//strcat_s(iniFileName,"\\SafeModeConfig.ini");
	strcat_s(iniFileName, FileName);
	strcat_s(iniFileName,"\\SafeModeConfig.ini");

	//读取可靠性配置文件
	m_SaveModelInfo.iSafeModeEnable = GetPrivateProfileIntA(m_strIP.c_str(), "SafeModeEnable", 0, iniFileName);
	GetPrivateProfileStringA(m_strIP.c_str(),"BeginTime", "0", m_SaveModelInfo.chBeginTime, 256, iniFileName);
	GetPrivateProfileStringA(m_strIP.c_str(), "EndTime", "0", m_SaveModelInfo.chEndTime, 256, iniFileName);
	m_SaveModelInfo.iIndex = GetPrivateProfileIntA(m_strIP.c_str(), "Index", 0, iniFileName);
	m_SaveModelInfo.iDataType = GetPrivateProfileIntA(m_strIP.c_str(), "DataType", 0, iniFileName);
}

void Camera6467::WriteHistoryInfo( SaveModeInfo& SaveInfo )
{
	char fileName[MAX_PATH];
	GetModuleFileNameA(NULL, fileName, MAX_PATH-1);

	PathRemoveFileSpecA(fileName);
	char iniFileName[MAX_PATH] = {0};
	char iniDeviceInfoName[MAX_PATH] = {0};
	//strcat_s(iniFileName, fileName);
	//strcat_s(iniFileName, "\\SafeModeConfig.ini");
	strcat_s(iniFileName, fileName);
	strcat_s(iniFileName, "\\SafeModeConfig.ini");

	//读取配置文件
	char chTemp[256] = {0};
	//sprintf_s(chTemp, "%d", m_SaveModelInfo.iSafeModeEnable);
	sprintf_s(chTemp, "%d", m_SaveModelInfo.iSafeModeEnable);

	//if(m_SaveModelInfo.iSafeModeEnable == 0)
	//{
	//	SYSTEMTIME st;	
	//	GetLocalTime(&st);
	//	sprintf_s(m_SaveModelInfo.chBeginTime, "%d.%d.%d_%d", st.wYear, st.wMonth, st.wDay, st.wHour);
	//}
	WritePrivateProfileStringA(m_strIP.c_str(), "SafeModeEnable", chTemp, iniFileName);
	WritePrivateProfileStringA(m_strIP.c_str(), "BeginTime", SaveInfo.chBeginTime, iniFileName);
	WritePrivateProfileStringA(m_strIP.c_str(), "EndTime", SaveInfo.chEndTime, iniFileName);
	memset(chTemp, 0, sizeof(chTemp));
	//sprintf_s(chTemp, "%d", SaveInfo.iIndex);
	sprintf_s(chTemp, "%d", SaveInfo.iIndex);
	WritePrivateProfileStringA(m_strIP.c_str(), "Index", chTemp, iniFileName);
}

bool Camera6467::SaveImgToDisk( char* chImgPath, BYTE* pImgData, DWORD dwImgSize )
{
	WriteLog("begin SaveImgToDisk");
	if(NULL == pImgData)
	{
		WriteLog("end1 SaveImgToDisk");
		return false;
	}
	bool bRet = false;
	size_t iWritedSpecialSize = 0;	
	std::string tempFile(chImgPath);
	size_t iPosition = tempFile.rfind("\\");
	std::string tempDir = tempFile.substr(0, iPosition+1);
	if (MakeSureDirectoryPathExists(tempDir.c_str()))
	{
		FILE* fp = NULL;
		//fp = fopen(chImgPath, "wb+");
		fopen_s(&fp, chImgPath, "wb+");
		if(fp)
		{
			//iWritedSpecialSize = fwrite(pImgData, dwImgSize , 1, fp);
			iWritedSpecialSize = fwrite(pImgData, 1 , dwImgSize, fp);
			fclose(fp);
			fp = NULL;
			bRet = true;
		}
		if (iWritedSpecialSize == dwImgSize)
		{
			char chLogBuff[MAX_PATH] = {0};
			//sprintf_s(chLogBuff, "%s save success", chImgPath);
			sprintf_s(chLogBuff, "%s save success", chImgPath);
			WriteLog(chLogBuff);
		}
	}
	else
	{
		char chLogBuff[MAX_PATH] = {0};
		//sprintf_s(chLogBuff, "%s save failed", chImgPath);
		sprintf_s(chLogBuff, "%s save failed", chImgPath);
		WriteLog(chLogBuff);
		bRet = false;
	}
	WriteLog("end SaveImgToDisk");
	return bRet;
}

bool Camera6467::SaveImgToDisk( char* chImgPath, BYTE* pImgData, DWORD dwImgSize, int iWidth, int iHeight, int iType /*= 0*/ )
{
	//iType 为0时压缩图像，1时不压缩
	if (pImgData == NULL || dwImgSize < 0 || iWidth < 0 || iHeight < 0)
	{
		return false;
	}	
	IStream* pStream = NULL;
	CreateStreamOnHGlobal(NULL, TRUE, &pStream);
	if (NULL == pStream)
	{
		WriteLog("SaveImgToDisk:: Stream 流创建失败. reture false");
		return false;		//流创建失败
	}
	LARGE_INTEGER LiTemp = {0};
	ULARGE_INTEGER ULiZero = {0};
	ULONG ulRealSize = 0;
	pStream->Seek(LiTemp, STREAM_SEEK_SET, NULL);
	pStream->SetSize(ULiZero);

	//将图片写入流中
	pStream->Write(pImgData, dwImgSize, &ulRealSize);
	//创建位图
	Bitmap bmpSrc(pStream);

	Bitmap bmpDest(iWidth, iHeight);
	Graphics grCompress(&bmpDest);
	Rect RCompress(0, 0 , iWidth, iHeight);
	Status statuDraw = grCompress.DrawImage(&bmpSrc, RCompress,0, 0, bmpSrc.GetWidth(), bmpSrc.GetHeight(), UnitPixel);
	if (statuDraw != Ok)
	{
		char chLog[260] = {0};
		sprintf_s(chLog, "SaveImgToDisk:: DrawImage failed, the error code = %d", statuDraw);
		WriteLog(chLog);

		if (pStream) 
		{
			pStream->Release();
			pStream = NULL;
		}
		return false;
	}
	Status statusDest;
	bool bRet = false;
	if (iType == 0)
	{
		ULONG quality = 50;
		EncoderParameters encoderParameters;
		encoderParameters.Count = 1;
		encoderParameters.Parameter[0].Guid = EncoderQuality;
		encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
		encoderParameters.Parameter[0].NumberOfValues = 1;

		pStream->Seek(LiTemp, STREAM_SEEK_SET, NULL);
		pStream->SetSize(ULiZero);
		encoderParameters.Parameter[0].Value = &quality;
		statusDest = bmpDest.Save(pStream, &m_jpgClsid, &encoderParameters);
		if (statusDest != Ok)
		{
			char chLog[260] = {0};
			//sprintf_s(chLog, "SaveImgToDisk:: failed, the error code = %d", statusDest);
			sprintf_s(chLog, "SaveImgToDisk:: failed, the error code = %d", statusDest);
			WriteLog(chLog);

			if (pStream) 
			{
				pStream->Release();
				pStream = NULL;
			}
			return false;
		}

		ULARGE_INTEGER uiLength;
		ULONG iLastSize = 0;
		if (GetStreamLength(pStream, &uiLength))
		{
			iLastSize = (int)uiLength.QuadPart;
		}
		BYTE* pDestImg = NULL;
		if (iLastSize > 0)
		{
			pDestImg = new BYTE[iLastSize];
		}
		pStream->Seek(LiTemp, STREAM_SEEK_SET, NULL);
		if(S_OK != pStream->Read(pDestImg, iLastSize, &iLastSize))
		{
			WriteLog("压缩图片保存失败");

			if (NULL != pDestImg)
			{
				delete[] pDestImg;
				pDestImg = NULL;
			}			
			return false;
		}
		bRet = SaveImgToDisk(chImgPath, pDestImg, iLastSize);
		if (NULL != pDestImg)
		{
			delete[] pDestImg;
			pDestImg = NULL;
		}

		if (pStream) 
		{
			pStream->Release();
			pStream = NULL;
		}
		return bRet;
	}
	else
	{
		wchar_t tempPath[260];
		MultiByteToWideChar(CP_ACP, NULL, chImgPath, 260, tempPath, 260); 
		statusDest = bmpDest.Save(tempPath, &m_bmpClsid);
		if (statusDest ==  Ok)
		{
			bRet = true;
		}
		else
		{
			char chLog[260] = {0};
			//sprintf_s(chLog, "SaveImgToDisk:: Save failed, the error code = %d", statusDest);
			sprintf_s(chLog, "SaveImgToDisk:: Save failed, the error code = %d", statusDest);
			WriteLog(chLog);
		}

		if (pStream) 
		{
			pStream->Release();
			pStream = NULL;
		}
	}
	return bRet;
}

bool Camera6467::SetCameraInfo( CameraInfo& camInfo )
{
	m_strIP = std::string(camInfo.chIP);
	m_strDeviceID = std::string(camInfo.chDeviceID);
	//sprintf_s(m_chDeviceID, "%s", camInfo.chDeviceID);
	//sprintf_s(m_chLaneID, "%s", camInfo.chLaneID);
	//sprintf_s(m_chStationID, "%s", camInfo.chStationID);
	sprintf_s(m_chDeviceID, "%s", camInfo.chDeviceID);
	sprintf_s(m_chLaneID, "%s", camInfo.chLaneID);
	sprintf_s(m_chStationID, "%s", camInfo.chStationID);
	m_bLogEnable = camInfo.bLogEnable;
	m_bSynTime = camInfo.bSynTimeEnable;
	m_iDirection = camInfo.iDirection;

	return true;
}

int Camera6467::GetCamStatus()
{
	//if (NULL == m_hHvHandle)
	//	return 1;

	//int iStatus = 1;
	//CDevState pState;
	////HVAPI_GetDevState(m_hHvHandle, &pState);

	//if (HVAPI_GetDevState(m_hHvHandle, &pState) != S_OK)
	//{
	//	iStatus = 1;
	//	char chCaptureLog3[MAX_PATH] = {0};
	//	//sprintf_s(chCaptureLog3, "Camera: %s SoftTriggerCapture failed", m_strIP.c_str());
	//	sprintf_s(chCaptureLog3, "Camera: %s GetDevState failed", m_strIP.c_str());
	//	WriteLog(chCaptureLog3);
	//}
	//else
	//{
	//	iStatus = 0;
	//	char chCaptureLog4[MAX_PATH] = {0};
	//	//sprintf_s(chCaptureLog4, "Camera: %s SoftTriggerCapture success", m_strIP.c_str());
	//	sprintf_s(chCaptureLog4, "Camera: %s GetDevState success", m_strIP.c_str());
	//	WriteLog(chCaptureLog4);
	//}
	//return iStatus;


	if(NULL == m_hHvHandle)
		return 1;
	DWORD dwStatus = 1;

	if ( HVAPI_GetConnStatusEx((HVAPI_HANDLE_EX)m_hHvHandle, CONN_TYPE_RECORD, &dwStatus) == S_OK )
	{
		if (dwStatus == CONN_STATUS_NORMAL
			/*|| dwStatus == CONN_STATUS_RECVDONE*/)
		{
			m_iConnectStatus = 0;
			m_dwConnectStatus = 0;
		}
		else if (dwStatus == CONN_STATUS_RECONN)
		{
			m_iConnectStatus = 1;
			m_dwConnectStatus = -1;
		}
		else
		{
			m_iConnectStatus = 1;
			m_dwConnectStatus = -1;
		}
	}
	else
	{
		m_iConnectStatus = 1;
		m_dwConnectStatus = -1;
	}
	return m_iConnectStatus;
}

char* Camera6467::GetStationID()
{
	return m_chStationID;
}

char* Camera6467::GetDeviceID()
{
	return m_chDeviceID;
}

char* Camera6467::GetLaneID()
{
	return m_chLaneID;
}

const char* Camera6467::GetCameraIP()
{
	return m_strIP.c_str();
}

CameraResult* Camera6467::GetOneResult()
{
	CameraResult* tempResult = NULL;
	//EnterCriticalSection(&m_csResult);	
	//if (m_ResultList.size() > 0)
	//{
	//	tempResult = m_ResultList.front();
	//	m_ResultList.pop_front();
	//}
	//LeaveCriticalSection(&m_csResult);


	if (NULL != m_BufferResult && m_bResultComplete)
	{
		tempResult = new CameraResult(*m_BufferResult);
	}
	return tempResult;
}

int Camera6467::ConnectToCamera()
{
	if(m_strIP.empty())
	{
		WriteLog("ConnectToCamera:: please finish the camera ip address");
		return -1;
	}
	if(NULL != m_hHvHandle)
	{
		InterruptionConnection();
		HVAPI_CloseEx((HVAPI_HANDLE_EX)m_hHvHandle);
		m_hHvHandle = NULL;
	}
	m_hHvHandle = HVAPI_OpenEx(m_strIP.c_str(), NULL);
	//m_hHvHandle = HVAPI_OpenChannel(m_strIP.c_str(), NULL, 0);
	if(NULL == m_hHvHandle)
	{
		WriteLog("ConnectToCamera:: Open CameraHandle failed!");
		return -2;
	}

	ReadHistoryInfo();
	char chCommand[1024] = {0};
	sprintf_s(chCommand, "DownloadRecord,BeginTime[%s],Index[%d],Enable[%d],EndTime[%s],DataInfo[%d]", m_SaveModelInfo.chBeginTime, m_SaveModelInfo.iIndex, m_SaveModelInfo.iSafeModeEnable, m_SaveModelInfo.chEndTime, m_SaveModelInfo.iDataType);
	//sprintf_s(chCommand, "DownloadRecord,BeginTime[%s],Index[%d],Enable[%d],EndTime[%s],DataInfo[%d]", m_SaveModelInfo.chBeginTime, m_SaveModelInfo.iIndex, m_SaveModelInfo.iSafeModeEnable, m_SaveModelInfo.chEndTime, m_SaveModelInfo.iDataType);
	
	WriteLog(chCommand);

	if ( (HVAPI_SetCallBackEx(m_hHvHandle, (PVOID)RecordInfoBeginCallBack, this, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, NULL) != S_OK) ||
		 (HVAPI_SetCallBackEx(m_hHvHandle, (PVOID)RecordInfoPlateCallBack, this, 0, CALLBACK_TYPE_RECORD_PLATE, NULL) != S_OK) ||
		 (HVAPI_SetCallBackEx(m_hHvHandle, (PVOID)RecordInfoBigImageCallBack, this, 0, CALLBACK_TYPE_RECORD_BIGIMAGE, chCommand) != S_OK) ||
		 (HVAPI_SetCallBackEx(m_hHvHandle, (PVOID)RecordInfoSmallImageCallBack, this, 0, CALLBACK_TYPE_RECORD_SMALLIMAGE, chCommand) != S_OK) ||
		 (HVAPI_SetCallBackEx(m_hHvHandle, (PVOID)RecordInfoBinaryImageCallBack, this, 0, CALLBACK_TYPE_RECORD_BINARYIMAGE, chCommand) != S_OK) ||
		 (HVAPI_SetCallBackEx(m_hHvHandle, (PVOID)RecordInfoEndCallBack, this, 0, CALLBACK_TYPE_RECORD_INFOEND, NULL) != S_OK)
		// ||
		//(HVAPI_SetCallBackEx(m_hHvHandle, (PVOID)JPEGStreamCallBack, this, 0, CALLBACK_TYPE_JPEG_FRAME, NULL) != S_OK)
		)
	{		
		WriteLog("ConnectToCamera:: SetCallBack failed.");
		HVAPI_CloseEx(m_hHvHandle);
		m_hHvHandle = NULL;
		m_dwConnectStatus = -1;
		return -3;
	}
	else
	{
		m_dwConnectStatus = 0;
		WriteLog("ConnectToCamera:: SetCallBack success.");
	}

	return 0;
}

void Camera6467::ReadConfig()
{
	char FileName[MAX_PATH];
	GetModuleFileNameA(NULL, FileName, MAX_PATH-1);

	PathRemoveFileSpecA(FileName);
	char iniFileName[MAX_PATH] = {0};
	char iniDeviceInfoName[MAX_PATH] = {0};
#ifdef GUANGXI_DLL
	strcat_s(iniFileName, FileName);
    strcat_s(iniFileName, INI_FILE_NAME);
#else
	sprintf_s(iniFileName, "..\\DevInterfaces\\HVCR_Signalway_V%d_%d\\HVCR_Config\\HVCR_Signalway_V%d_%d.ini", PROTOCAL_VERSION, DLL_VERSION,PROTOCAL_VERSION, DLL_VERSION);
#endif


	//读取可靠性配置文件
	int iLog =  GetPrivateProfileIntA("Log", "Enable", 1, iniFileName);
	m_bLogEnable = (iLog==1 ) ? true : false;	

	int iSaveBuffer =  GetPrivateProfileIntA("SaveBuffer", "Enable", 0, iniFileName);
	m_bSaveToBuffer = (iSaveBuffer==1 ) ? true : false;	

	m_iSuperLenth =  GetPrivateProfileIntA("Filter", "SuperLongVehicleLenth", 13, iniFileName);

	m_iVedioWidth = GetPrivateProfileIntA("Video", "Width", 1920, iniFileName);
	m_iVedioHeight = GetPrivateProfileIntA("Video", "height", 1080, iniFileName);

	char chTemp[256] = {0};
	//sprintf_s(chTemp, "%d", iLog);
	sprintf_s(chTemp, "%d", iLog);

	WritePrivateProfileStringA("Log", "Enable", chTemp, iniFileName);

	memset(chTemp, 0, sizeof(chTemp));
	sprintf_s(chTemp, "%d", m_iSuperLenth);
	WritePrivateProfileStringA("Filter", "SuperLongVehicleLenth", chTemp, iniFileName);
	
	memset(chTemp, 0, sizeof(chTemp));
	sprintf_s(chTemp, "%d", m_iVedioWidth);
	WritePrivateProfileStringA("Video", "Width", chTemp, iniFileName);

	memset(chTemp, 0, sizeof(chTemp));
	sprintf_s(chTemp, "%d", m_iVedioHeight);
	WritePrivateProfileStringA("Video", "height", chTemp, iniFileName);
}

bool Camera6467::WriteLog( const char* chlog )
{
	ReadConfig();
	if(!m_bLogEnable || NULL == chlog)
		return false;

	//取得当前的精确毫秒的时间
	static time_t starttime = time(NULL);
	static DWORD starttick = GetTickCount(); 
	DWORD dwNowTick = GetTickCount() - starttick;
	time_t nowtime = starttime + (time_t)(dwNowTick / 1000);
	//struct tm *pTM = localtime(&nowtime);
	struct tm pTM;
	localtime_s(&pTM,&nowtime);
	DWORD dwMS = dwNowTick % 1000;

	const int MAXPATH = 260;

	TCHAR szFileName[ MAXPATH] = {0};
	GetModuleFileName(NULL, szFileName, MAXPATH);	//取得包括程序名的全路径
	PathRemoveFileSpec(szFileName);				//去掉程序名

	char chLogPath[512] = {0};
	//sprintf_s(chLogPath, "%s\\XLWLog\\%04d-%02d-%02d\\%s\\",  szFileName, pTM->tm_year + 1900, pTM->tm_mon+1, pTM->tm_mday, m_strIP.c_str());
#ifdef GUANGXI_DLL
		sprintf_s(chLogPath, "%s\\XLWLog\\%04d-%02d-%02d\\%s\\",  szFileName, pTM.tm_year + 1900, pTM.tm_mon+1, pTM.tm_mday, m_strIP.c_str());
#else
		sprintf_s(chLogPath, "..\\DevInterfaces\\HVCR_Signalway_V%d_%d\\HVCR_Logs\\XLWLog\\%04d-%02d-%02d\\%s\\", PROTOCAL_VERSION, DLL_VERSION,pTM.tm_year + 1900, pTM.tm_mon +1, pTM.tm_mday, m_strIP.c_str());
#endif

	MakeSureDirectoryPathExists(chLogPath);

	//每次只保留10天以内的日志文件
	CTime tmCurrentTime = CTime::GetCurrentTime();
	CTime tmLastMonthTime = tmCurrentTime - CTimeSpan(10, 0, 0, 0);
	int Last_Year = tmLastMonthTime.GetYear() ;
	int Last_Month = tmLastMonthTime.GetMonth();
	int Last_Day = tmLastMonthTime.GetDay();

	char chOldLogFileName[MAXPATH] = {0};
	//sprintf_s(chOldLogFileName, "%s\\XLWLog\\%04d-%02d-%02d\\",szFileName, Last_Year, Last_Month, Last_Day);
	sprintf_s(chOldLogFileName, "%s\\XLWLog\\%04d-%02d-%02d\\",szFileName, Last_Year, Last_Month, Last_Day);
	if (PathFileExists(chOldLogFileName))
	{
		char chCommand[512] = {0};
		//sprintf_s(chCommand, "/c rd /s/q %s", chOldLogFileName);
		sprintf_s(chCommand, "/c rd /s/q %s", chOldLogFileName);
		ExcuteCMD(chCommand);
	}

	char chLogFileName[512] = {0};
	//sprintf_s(chLogFileName, "%s\\CameraLog-%d-%02d_%02d.log",chLogPath, pTM->tm_year + 1900, pTM->tm_mon+1, pTM->tm_mday);
	sprintf_s(chLogFileName, "%s\\CameraLog-%d-%02d_%02d_PLATE.log",chLogPath, pTM.tm_year + 1900, pTM.tm_mon+1, pTM.tm_mday);

	EnterCriticalSection(&m_csLog);

	FILE *file = NULL;
	//file = fopen(chLogFileName, "a+");
	fopen_s(&file, chLogFileName, "a+");
	if (file)
	{
		fprintf(file,"%04d-%02d-%02d %02d:%02d:%02d:%03d [%s]: %s\n",  
            pTM.tm_year + 1900, 
            pTM.tm_mon+1, 
            pTM.tm_mday,
			pTM.tm_hour, 
            pTM.tm_min, 
            pTM.tm_sec,
            dwMS,
            DLL_VERSION,
            chlog);
		fclose(file);
		file = NULL;
	}

	LeaveCriticalSection(&m_csLog);


	return true;
}

bool Camera6467::TakeCapture()
{
	if (NULL == m_hHvHandle)
		return false;

	bool bRet = true;
	char chRetBuf[1024] = {0};
	int nRetLen = 0;

	if (HVAPI_ExecCmdEx(m_hHvHandle, "SoftTriggerCapture", chRetBuf, 1024, &nRetLen) != S_OK)
	{
		bRet = false;
		char chCaptureLog3[MAX_PATH] = {0};
		//sprintf_s(chCaptureLog3, "Camera: %s SoftTriggerCapture failed", m_strIP.c_str());
		sprintf_s(chCaptureLog3, "Camera: %s SoftTriggerCapture failed", m_strIP.c_str());
		WriteLog(chCaptureLog3);
	}
	else
	{
		char chCaptureLog4[MAX_PATH] = {0};
		//sprintf_s(chCaptureLog4, "Camera: %s SoftTriggerCapture success", m_strIP.c_str());
		sprintf_s(chCaptureLog4, "Camera: %s SoftTriggerCapture success", m_strIP.c_str());
		WriteLog(chCaptureLog4);
	}
	return bRet;
}

bool Camera6467::SynTime()
{
	if (NULL == m_hHvHandle || m_dwConnectStatus != 0)
		return false;
	//if (!m_bSynTime)
	//{
	//	return false;
	//}
	WriteLog("SynTime begin");

	static time_t starttime = time(NULL);
	static DWORD starttick = GetTickCount(); 
	DWORD dwNowTick = GetTickCount() - starttick;
	time_t nowtime = starttime + (time_t)(dwNowTick / 1000);
	//struct tm *pTM = localtime(&nowtime);
	struct tm pTM;
	localtime_s(&pTM, &nowtime);
	DWORD dwMS = dwNowTick % 1000;

	char chTemp[256]={ 0 };
	//sprintf_s(chTemp, "SetTime,Date[%d.%02d.%02d],Time[%02d:%02d:%02d]",
	//	pTM->tm_year + 1900,  pTM->tm_mon, pTM->tm_mday,
	//	pTM->tm_hour, pTM->tm_min, pTM->tm_sec);
	sprintf_s(chTemp, "SetTime,Date[%d.%02d.%02d],Time[%02d:%02d:%02d]",
		pTM.tm_year + 1900,  pTM.tm_mon, pTM.tm_mday,
		pTM.tm_hour, pTM.tm_min, pTM.tm_sec);
	WriteLog(chTemp);
	char szRetBuf[1024] = {0};
	int nRetLen = 0;
	if (m_hHvHandle != NULL)
	{
		try
		{
			if (HVAPI_ExecCmdEx(m_hHvHandle, chTemp, szRetBuf, 1024, &nRetLen) != S_OK)
			{
				char chSynTimeLogBuf1[MAX_PATH] = {0};
				//sprintf_s(chSynTimeLogBuf1, "Camera: %s SynTime failed", m_strIP.c_str());
				sprintf_s(chSynTimeLogBuf1, "Camera: %s SynTime failed", m_strIP.c_str());
				WriteLog(chSynTimeLogBuf1);
				return false;
			}
			else
			{
				char chSynTimeLogBuf2[MAX_PATH] = {0};
				sprintf_s(chSynTimeLogBuf2, "Camera: %s SynTime success", m_strIP.c_str());
				WriteLog(chSynTimeLogBuf2);
			}
		} catch (...)
		{
			char chSynTimeLogBuf3[MAX_PATH] = {0};
			sprintf_s(chSynTimeLogBuf3, "Camera: %s SynTime take exception", m_strIP.c_str());
			WriteLog(chSynTimeLogBuf3);
		}
	}
	WriteLog("SynTime end");
	return 0;
}

bool Camera6467::SynTime( int Year, int Month, int Day, int Hour, int Minute, int Second, int MilientSecond )
{
	if (NULL == m_hHvHandle || m_dwConnectStatus != 0)
	{
		WriteLog("SynTime  failed: ConnectStatus != 0.");
		return false;
	}
	//if (!m_bSynTime)
	//{
	//	WriteLog("SynTime  failed: SynTime is not open.");
	//	return false;
	//}
	if (abs(Month) > 12 || abs(Day) > 31 || abs(Hour) > 24 || abs(Minute) > 60 || abs(Second) > 60)
	{
		WriteLog("SynTime  failed: time value is invalid.");
		return false;
	}
	WriteLog("SynTime begin");

	
	char chTemp[256]={ 0 };
	sprintf_s(chTemp, "SetTime,Date[%d.%02d.%02d],Time[%02d:%02d:%02d]",
		abs(Year),  abs(Month), abs(Day),
		abs(Hour), abs(Minute), abs(Second));
	WriteLog(chTemp);
	char szRetBuf[1024] = {0};
	int nRetLen = 0;
	if (m_hHvHandle != NULL)
	{
		try
		{
			//if (HVAPI_ExecCmdEx(m_hHvHandle, chTemp, szRetBuf, 1024, &nRetLen) != S_OK)
			if(HVAPI_SetTime(m_hHvHandle, Year, Month, Day, Hour, Minute, Second, 0) != S_OK)
			{
				char chSynTimeLogBuf1[MAX_PATH] = {0};
				sprintf_s(chSynTimeLogBuf1, "Camera: %s SynTime failed", m_strIP.c_str());
				WriteLog(chSynTimeLogBuf1);
				return false;
			}
			else
			{
				char chSynTimeLogBuf2[MAX_PATH] = {0};
				sprintf_s(chSynTimeLogBuf2, "Camera: %s SynTime success", m_strIP.c_str());
				WriteLog(chSynTimeLogBuf2);
			}
		} catch (...)
		{
			char chSynTimeLogBuf3[MAX_PATH] = {0};
			sprintf_s(chSynTimeLogBuf3, "Camera: %s SynTime take exception", m_strIP.c_str());
			WriteLog(chSynTimeLogBuf3);
		}
	}
	WriteLog("SynTime end");
	return true;
}

void Camera6467::AnalysisAppendInfo( CameraResult* record )
{
    record->iPlateColor = Tool_AnalysisPlateColorNo(record->chPlateNO);	

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
		char chTemp[MAX_PATH] = {0};
		int iYear = 0, iMonth =0, iDay = 0, iHour=0, iMin =0, iSecond =0, iMSecond = 0;
		sscanf_s(pchObj, "车身结束时间:%d-%d-%d %d:%d:%d:%d", &iYear, &iMonth, &iDay, &iHour, &iMin, &iSecond, &iMSecond);
		sprintf_s(record->chPlateTime,"%04d%02d%02d%02d%02d%02d",   iYear, iMonth, iDay, iHour, iMin, iSecond);
	}
	else
	{
			SYSTEMTIME st;
			GetLocalTime(&st);
			//sprintf_s(record->chPlateTime, "%04d%02d%02d%02d%02d%02d%03d", st.wYear, st.wMonth, st.wDay	,st.wHour, st.wMinute,st.wSecond, st.wMilliseconds);
			sprintf_s(record->chPlateTime, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay	,st.wHour, st.wMinute, st.wSecond);
	}

	//------------车道
	pchObj = (char*)strstr(record->pcAppendInfo, "车辆类型");
	if(pchObj)
	{
		char szCarType[20] = {0};
		sscanf_s(pchObj, "车辆类型:%s", szCarType, sizeof(szCarType));

		if (strstr(szCarType,"大"))
		{
			record->iVehTypeNo=2;
		}
		if (strstr(szCarType,"中"))
		{
			record->iVehTypeNo=2;
		}
		if (strstr(szCarType,"小"))
		{
			record->iVehTypeNo=1;
		}
		if(strstr(szCarType,"客车"))
		{
			record->iVehTypeNo=1;
		}
		if(strstr(szCarType,"货车"))
		{
			record->iVehTypeNo=2;
		}
		if(strstr(szCarType,"牵引车"))
		{
			record->iVehTypeNo=3;
		}
		if(strstr(szCarType,"专项作业车"))
		{
			record->iVehTypeNo=4;
		}
		if(strstr(szCarType,"电车"))
		{
			record->iVehTypeNo=5;
		}
		if(strstr(szCarType,"摩托车"))
		{
			record->iVehTypeNo=6;
		}
		if(strstr(szCarType,"三轮汽车"))
		{
			record->iVehTypeNo=7;
		}
		if(strstr(szCarType,"拖拉机"))
		{
			record->iVehTypeNo=8;
		}
		if(strstr(szCarType,"轮式机械"))
		{
			record->iVehTypeNo=9;
		}
		if(strstr(szCarType,"全挂车"))
		{
			record->iVehTypeNo=10;
		}
		if(strstr(szCarType,"半挂车"))
		{
			record->iVehTypeNo=11;
		}
		if(strstr(szCarType,"其他"))
		{
			record->iVehTypeNo=12;
		}
	}	

	//---------------车身颜色
	pchObj= (char*)strstr(record->pcAppendInfo,"车身颜色:");

	record->iVehBodyColorNo=0;
	if(pchObj)
	{
		char szBodyColour[256]={0};
		sscanf_s(pchObj,"车身颜色:%s",szBodyColour, sizeof(szBodyColour));

		if(strstr(szBodyColour,"白"))
			record->iVehBodyColorNo=1;

		if(strstr(szBodyColour,"灰"))
			record->iVehBodyColorNo=2;

		if(strstr(szBodyColour,"黄"))
			record->iVehBodyColorNo=3;

		if(strstr(szBodyColour,"粉"))
			record->iVehBodyColorNo=4;

		if(strstr(szBodyColour,"红"))
			record->iVehBodyColorNo=5;

		if(strstr(szBodyColour,"紫"))
			record->iVehBodyColorNo=6;

		if(strstr(szBodyColour,"绿"))
			record->iVehBodyColorNo=7;

		if(strstr(szBodyColour,"蓝"))
			record->iVehBodyColorNo=8;

		if(strstr(szBodyColour,"棕"))
			record->iVehBodyColorNo=9;

		if(strstr(szBodyColour,"黑"))
			record->iVehBodyColorNo=10;
	}

	//---------车身深浅度
	pchObj = (char*)(strstr(record->pcAppendInfo,"深浅度:"));
	if(pchObj)
	{
		char szBodyColourDeep[256]={0};
		sscanf_s(pchObj,"深浅度:%s",szBodyColourDeep, sizeof(szBodyColourDeep));

		if(strstr(szBodyColourDeep,"深"))
			record->iVehBodyDeepNo=1;

		if(strstr(szBodyColourDeep,"浅"))
			record->iVehBodyDeepNo=0;
	}
	//--------------------测速
	pchObj = (char*)(strstr(record->pcAppendInfo,"测速:"));
	if(pchObj)
	{
		int iCarSpeed = 0;
		sscanf_s(pchObj,"测速:%d Km/h",&iCarSpeed);
		record->iSpeed = iCarSpeed;
	}
	//--------------车型----------------------------
	pchObj = (char*)(strstr(record->pcAppendInfo,"车型:"));
	if(pchObj)
	{
		char chTemp[128] = {0};
		sscanf_s(pchObj,"车型:%s",chTemp, sizeof(chTemp));
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
		char chLog[260] = {0};
		sprintf_s(chLog, "iVehTypeNo= %d", record->iVehTypeNo);
		WriteLog(chLog);
	}
	//--------------轮数----------------------------
	pchObj = (char*)(strstr(record->pcAppendInfo,"轮数:"));
	if(pchObj)
	{
		int iTemp = 0;
		sscanf_s(pchObj, "轮数:%d", &iTemp);
		record->iWheelCount = iTemp;

		char chLog[260] = {0};
		sprintf_s(chLog, "iWheelCount= %d", record->iWheelCount);
		WriteLog(chLog);
	}
	//--------------轴数----------------------------
	pchObj = (char*)(strstr(record->pcAppendInfo,"轴数:"));
	if(pchObj)
	{
		int iTemp = 0;
		sscanf_s(pchObj, "轴数:%d", &iTemp);
		record->iAxletreeCount = iTemp;

		char chLog[260] = {0};
		sprintf_s(chLog, "iAxletreeCount= %d", record->iAxletreeCount);
		WriteLog(chLog);
	}
	//--------------轴距----------------------------
	pchObj = (char*)(strstr(record->pcAppendInfo,"轴距:"));
	if(pchObj)
	{
		float fTemp = 0.0;
		sscanf_s(pchObj, "轴距:%f", &fTemp);
		record->fDistanceBetweenAxles = fTemp;

		char chLog[260] = {0};
		sprintf_s(chLog, "fDistanceBetweenAxles= %f", record->fDistanceBetweenAxles);
		WriteLog(chLog);
	}
	//--------------车长----------------------------
	pchObj = (char*)(strstr(record->pcAppendInfo,"车长:"));
	if(pchObj)
	{
		float fTemp = 0.0;
		sscanf_s(pchObj, "车长:%f", &fTemp);
		record->fVehLenth = fTemp;

		char chLog[260] = {0};
		sprintf_s(chLog, "fVehLenth= %f", record->fVehLenth);
		WriteLog(chLog);
	}
	//--------------车高----------------------------
	pchObj = (char*)(strstr(record->pcAppendInfo,"车高:"));
	if(pchObj)
	{
		float fTemp = 0.0;
		sscanf_s(pchObj, "车高:%f", &fTemp);
		record->fVehHeight = fTemp;

		char chLog[260] = {0};
		sprintf_s(chLog, "fVehHeight= %f", record->fVehHeight);
		WriteLog(chLog);
	}

	pchObj = (char*)(strstr(record->pcAppendInfo,"倒车:"));
	if(pchObj)
	{
		record->bBackUpVeh = true;
	}
}

int Camera6467::RecordInfoBegin(DWORD dwCarID)
{
	char chlogInfo[260] = {0};
	sprintf_s(chlogInfo, "RecordInfoBegin -begin- dwCarID = %d", dwCarID);
	WriteLog(chlogInfo);
	if (dwCarID == m_dwLastCarID)
	{
		WriteLog("相同carID,丢弃该结果");
		return 0;
	}
	m_bResultComplete = false;
	if(NULL != m_CameraResult)
	{
		delete m_CameraResult;
		m_CameraResult = NULL;
	}
	m_CameraResult = new CameraResult();
	sprintf_s(m_CameraResult->chDeviceIp, "%s", m_strIP.c_str());
	m_CameraResult->dwCarID = dwCarID;
	char chlogInfo2[260] = {0};
	sprintf_s(chlogInfo2, "RecordInfoBegin -end- dwCarID = %d", dwCarID);
	WriteLog(chlogInfo2);
	return 0;
}

int Camera6467::RecordInfoEnd( DWORD dwCarID )
{
	char chLog[260] = {0};
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

	CTime tm(m_CameraResult->dw64TimeMS/1000);
	SaveModeInfo TempSaveModeInfo;
	sprintf_s(TempSaveModeInfo.chBeginTime, "%d.%02d.%02d_%02d", tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour());
	//WriteHistoryInfo(TempSaveModeInfo);

	if (m_bSaveToBuffer)
	{
		SaveResultToBuffer(m_CameraResult);
	}
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

	if (NULL != m_BufferResult)
	{
		delete m_BufferResult;
		m_BufferResult = NULL;
	}
	m_BufferResult = new CameraResult(*m_CameraResult);
    m_bResultComplete = true;

    SendBigImgByCallback(m_BufferResult->CIMG_LastSnapshot);
    SendResultByCallback(m_BufferResult);    

	//if (NULL != m_hWnd)
	//{
	//	WriteLog("PostMessage");
	//	//::PostMessage(*((HWND*)m_hWnd),m_iMsg, 1, 0);
	//	::PostMessage(m_hWnd,m_iMsg, (WPARAM)1,0);
	//}

	char chLog2[260] = {0};
	sprintf_s(chLog2, "RecordInfoEnd end, dwCarID = %d", dwCarID);
	WriteLog(chLog2);
	return 0;
}

int Camera6467::RecordInfoPlate( DWORD dwCarID, 
    LPCSTR pcPlateNo, 
    LPCSTR pcAppendInfo,
    DWORD dwRecordType,
    DWORD64 dw64TimeMS )
{
	m_bResultComplete = false;

	if (NULL == m_CameraResult)
	{
		return -1;
	}
	char chlogInfo[260] = {0};
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
			sprintf_s(m_CameraResult->chPlateNO, "%s",pcPlateNo);
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
            if (Tool_GetDataFromAppenedInfo((char* )pcAppendInfo, "Confidence", chTemp, &iLenth))
            {
                float fConfidence = 0.0;
                sscanf_s(chTemp, "%f", &fConfidence);
                m_CameraResult->iReliability = (int)(fConfidence * 10000);
            }
        }

		char qstrPlateTime[260] = {0};
		sprintf_s(qstrPlateTime, "the plate time is %s, %lld", m_CameraResult->chPlateTime, dw64TimeMS);
		WriteLog(qstrPlateTime);
	}
	char chlogInfo2[260] = {0};
	sprintf_s(chlogInfo2, "RecordInfoPlate -end- dwCarID = %d", dwCarID);
	WriteLog(chlogInfo2);
	return 0;
}

int Camera6467::RecordInfoBigImage( DWORD dwCarID, 
    WORD wImgType,
    WORD wWidth, 
    WORD wHeight, 
    PBYTE pbPicData,
    DWORD dwImgDataLen,
    DWORD dwRecordType, 
    DWORD64 dw64TimeMS )
{
	m_bResultComplete = false;

	if (NULL == m_CameraResult)
	{
		return -1;
	}
	char chlogInfo[260] = {0};
	sprintf_s(chlogInfo, "RecordInfoBigImage -begin- dwCarID = %ld, dwRecordType = %#x， ImgType=%d", dwCarID,  dwRecordType, wImgType);
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
	char chlogInfo2[260] = {0};
	sprintf_s(chlogInfo2, "RecordInfoBigImage -end- dwCarID = %d",dwCarID);
	WriteLog(chlogInfo2);
	return 0;
}

int Camera6467::RecordInfoSmallImage( DWORD dwCarID, 
    WORD wWidth,
    WORD wHeight,
    PBYTE pbPicData,
    DWORD dwImgDataLen,
    DWORD dwRecordType,
    DWORD64 dw64TimeMS )
{
	m_bResultComplete = false;
	if (NULL == m_CameraResult)
	{
		return -1;
	}
	char chlogInfo[260] = {0};
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
			memset(m_CameraResult->CIMG_PlateImage.pbImgData, 0 ,iBuffLen);
			HRESULT Hr = HVAPIUTILS_SmallImageToBitmapEx(pbPicData,
				wWidth,
				wHeight,
				m_CameraResult->CIMG_PlateImage.pbImgData,
				&iBuffLen);
			if ( Hr == S_OK)
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
					bool bScale = Img_ScaleJpg(m_CameraResult->CIMG_PlateImage.pbImgData,
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
	char chlogInfo2[260] = {0};
	sprintf_s(chlogInfo2, "RecordInfoSmallImage  -end- dwCarID = %d", dwCarID);	
	WriteLog(chlogInfo2);
	return 0;
}

int Camera6467::RecordInfoBinaryImage( DWORD dwCarID,
    WORD wWidth,
    WORD wHeight,
    PBYTE pbPicData,
    DWORD dwImgDataLen,
    DWORD dwRecordType,
    DWORD64 dw64TimeMS )
{
	m_bResultComplete = false;

	if (NULL == m_CameraResult)
	{
		return -1;
	}
	char chlogInfo[260] = {0};
	sprintf_s(chlogInfo, "RecordInfoBinaryImage -begin- dwCarID = %d", dwCarID);
	WriteLog(chlogInfo);

	if (dwCarID == m_dwLastCarID)
	{
		WriteLog("相同carID,丢弃该结果");
		return 0;
	}
	int iBufferlength = 1024*1024;
	if (m_pTempBin == NULL)
	{
		m_pTempBin = new BYTE[1024*1024];
		memset(m_pTempBin, 0x00, iBufferlength);
	}
	if (m_pTempBin)
	{
		memset(m_pTempBin, 0x00, iBufferlength);

		HRESULT hRet = HVAPIUTILS_BinImageToBitmapEx(pbPicData,	m_pTempBin,&iBufferlength);
		if (hRet == S_OK)
		{
			if (m_Bin_IMG_Temp.pbImgData == NULL)
			{
				m_Bin_IMG_Temp.pbImgData = new BYTE[MAX_IMG_SIZE];
				memset(m_Bin_IMG_Temp.pbImgData, 0x00, MAX_IMG_SIZE);
			}
			if (m_Bin_IMG_Temp.pbImgData)
			{
				DWORD iDestLenth = MAX_IMG_SIZE;
				memset(m_Bin_IMG_Temp.pbImgData, 0x00, MAX_IMG_SIZE);
				WriteLog("bin, convert bmp to jpeg , begin .");
				bool bScale = Img_ScaleJpg(m_pTempBin,
					iBufferlength,
					m_Bin_IMG_Temp.pbImgData,
					&iDestLenth,
					wWidth,
					wHeight,
					90);
				if (bScale)
				{
					WriteLog("bin, convert bmp to jpeg success, begin copy.");
					CopyDataToIMG(m_CameraResult->CIMG_BinImage, m_Bin_IMG_Temp.pbImgData, wWidth, wHeight, iDestLenth, 0);
					WriteLog("bin, convert bmp to jpeg success, finish copy.");
				}
				else
				{
					WriteLog("bin, convert bmp to jpeg failed, use default.");
				}
			}
			else
			{
				WriteLog("m_Bin_IMG_Temp  is null.");
			}
		}
		else
		{
			WriteLog("HVAPIUTILS_BinImageToBitmapEx, failed, use default.");
			CopyDataToIMG(m_CameraResult->CIMG_BinImage, pbPicData, wWidth, wHeight, dwImgDataLen, 0);
		}
	}
	else
	{
		WriteLog("m_pTempBin is NULL ,  use default.");
		CopyDataToIMG(m_CameraResult->CIMG_BinImage, pbPicData, wWidth, wHeight, dwImgDataLen, 0);
	}
	char chlogInfo2[260] = {0};
	sprintf_s(chlogInfo2, "RecordInfoBinaryImage -end- dwCarID = %d", dwCarID);
	WriteLog(chlogInfo2);
	return 0;
}

void Camera6467::SetResultToUnready()
{
	m_bResultComplete = false;
}

bool Camera6467::GetDeviceTime( DeviceTime& deviceTime )
{
	if (NULL == m_hHvHandle)
		return false;
	
	char chRetBuf[1024] = {0};
	int nRetLen = 0;

	if (HVAPI_ExecCmdEx(m_hHvHandle, "DateTime", chRetBuf, 1024, &nRetLen) != S_OK)
	{
		WriteLog("GetDeviceTime:: failed.");
		return false;
	}
	WriteLog(chRetBuf);
	bool bRet = false;
	const char* chFileName = "./DateTime.xml";
	DeleteFileA(chFileName);

	//FILE* file_L = fopen(chFileName, "wb");
	FILE* file_L = NULL;
	fopen_s(&file_L, chFileName, "wb");
	if(file_L)
	{
		size_t size_Read =  fwrite(chRetBuf, 1, nRetLen, file_L);		
		fclose(file_L);
		file_L = NULL;
		char chFileLog[260] = {0};
		sprintf_s(chFileLog, "GetDeviceTime:: DateTime.xml create success, size =%d ", size_Read);
		WriteLog(chFileLog);
		bRet = true;
	}
	if (!bRet)
	{
		WriteLog("GetDeviceTime:: DateTime.xml create failed.");
		return false;
	}

	const char* pDate = NULL;
	const char* pTime = NULL;
	TiXmlDocument cXmlDoc;
	//    if(cXmlDoc.Parse(chRetBuf))
	if(cXmlDoc.LoadFile(chFileName))
	{
		TiXmlElement* pSectionElement = cXmlDoc.RootElement();
		if(pSectionElement)
		{
			TiXmlElement* pChileElement = pSectionElement->FirstChildElement();
			pDate = pChileElement->Attribute("Date");
			pTime = pChileElement->Attribute("Time");
		}
		else
		{
			WriteLog("find Root element failed.");
		}
	}
	else
	{
		WriteLog("parse failed");
	}
	int iYear = 0, iMonth = 0, iDay = 0, iHour = 0, iMinute = 0, iSecond = 0, iMiliSecond = 0;
	sscanf_s(pDate, "%04d.%02d.%02d", &iYear, &iMonth, &iDay);
	sscanf_s(pTime, "%02d:%02d:%02d %03d", &iHour, &iMinute, &iSecond, &iMiliSecond);

	deviceTime.iYear =iYear;
	deviceTime.iMonth =iMonth;
	deviceTime.iDay =iDay;
	deviceTime.iHour =iHour;
	deviceTime.iMinutes =iMinute;
	deviceTime.iSecond =iSecond;
	deviceTime.iMilisecond =iMiliSecond;

	return true;
}

int Camera6467::GetEncoderClsid( const WCHAR* format, CLSID* pClsid )
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}
	free(pImageCodecInfo);
	return -1;  // Failure
}

bool Camera6467::GetStreamLength( IStream* pStream, ULARGE_INTEGER* puliLenth )
{
	if (pStream == NULL)
		return false;

	LARGE_INTEGER liMov;
	liMov.QuadPart = 0;

	ULARGE_INTEGER uliEnd, uliBegin;

	HRESULT hr = S_FALSE;

	hr = pStream->Seek(liMov, STREAM_SEEK_END, &uliEnd);
	if (FAILED(hr))
		return false;

	hr = pStream->Seek(liMov, STREAM_SEEK_SET, &uliBegin);
	if (FAILED(hr))
		return false;

	// 差值即是流的长度
	puliLenth->QuadPart = uliEnd.QuadPart - uliBegin.QuadPart;

	return TRUE;
}

void Camera6467::SaveResultToBuffer( CameraResult* pResult )
{
	if (NULL == pResult)
	{
		return;
	}
	//将图片缓存到缓存目录
	DWORD64 dwPlateTime = 0;
	//char chBigImgFileName[260] = {0};
	//char chBinImgFileName[260] = {0};
	char chPlateColor[32] = {0};

	dwPlateTime = pResult->dw64TimeMS/1000;
	if (strstr(pResult->chPlateNO, "无"))
	{
		sprintf_s(chPlateColor, "无");
	}
	else
	{
		sprintf_s(chPlateColor, "%s", pResult->chPlateColor);
	}

	TCHAR szFileName[260] = {0};
	GetModuleFileName(NULL, szFileName, 260);	//取得包括程序名的全路径
	PathRemoveFileSpec(szFileName);				//去掉程序名	
	
	//构造文件名称，格式： Unix时间-车牌号-车牌颜色
	//二值图
	sprintf_s(pResult->CIMG_BinImage.chSavePath,"%s\\Buffer\\%s\\%d-%s-%s-%s-车型%d-车轴%d-%d-bin.bin", szFileName, m_strIP.c_str(), dwPlateTime, pResult->chPlateNO, chPlateColor, pResult->chPlateTime, pResult->iVehTypeNo, pResult->iAxletreeCount, pResult->dwCarID);
	//车牌图
	sprintf_s(pResult->CIMG_PlateImage.chSavePath,"%s\\Buffer\\%s\\%d-%s-%s-%s-车型%d-车轴%d-%d-Plate.jpg", szFileName, m_strIP.c_str(), dwPlateTime, pResult->chPlateNO, chPlateColor, pResult->chPlateTime, pResult->iVehTypeNo, pResult->iAxletreeCount, pResult->dwCarID);
	//bestCapture
	sprintf_s(pResult->CIMG_BestCapture.chSavePath, "%s\\Buffer\\%s\\%d-%s-%s-%s-车型%d-车轴%d-%d-BestCapture.jpg", szFileName, m_strIP.c_str(), dwPlateTime, pResult->chPlateNO, chPlateColor, pResult->chPlateTime, pResult->iVehTypeNo, pResult->iAxletreeCount, pResult->dwCarID);
	//lastCapture
	sprintf_s(pResult->CIMG_LastCapture.chSavePath,"%s\\Buffer\\%s\\%d-%s-%s-%s-车型%d-车轴%d-%d-LastCapture.jpg", szFileName, m_strIP.c_str(), dwPlateTime, pResult->chPlateNO, chPlateColor, pResult->chPlateTime, pResult->iVehTypeNo, pResult->iAxletreeCount, pResult->dwCarID);
	//BEGIN_CAPTURE
	sprintf_s(pResult->CIMG_BeginCapture.chSavePath,"%s\\Buffer\\%s\\%d-%s-%s-%s-车型%d-车轴%d-%d-BeginCapture.jpg", szFileName, m_strIP.c_str(), dwPlateTime, pResult->chPlateNO, chPlateColor, pResult->chPlateTime, pResult->iVehTypeNo, pResult->iAxletreeCount, pResult->dwCarID);
	//BEST_SNAPSHOT
	sprintf_s(pResult->CIMG_BestSnapshot.chSavePath,"%s\\Buffer\\%s\\%d-%s-%s-%s-车型%d-车轴%d-%d-BestSnapshot.jpg", szFileName, m_strIP.c_str(), dwPlateTime, pResult->chPlateNO, chPlateColor, pResult->chPlateTime, pResult->iVehTypeNo, pResult->iAxletreeCount, pResult->dwCarID);
	//LAST_SNAPSHOT
	sprintf_s(pResult->CIMG_LastSnapshot.chSavePath,"%s\\Buffer\\%s\\%d-%s-%s-%s-车型%d-车轴%d-%d-LastSnapshot.jpg", szFileName, m_strIP.c_str(), dwPlateTime, pResult->chPlateNO, chPlateColor, pResult->chPlateTime, pResult->iVehTypeNo, pResult->iAxletreeCount, pResult->dwCarID);

	if (pResult->CIMG_BinImage.pbImgData)
	{
		bool bSave = SaveImgToDisk(pResult->CIMG_BinImage.chSavePath, pResult->CIMG_BinImage.pbImgData, pResult->CIMG_BinImage.dwImgSize);
	}

	if (pResult->CIMG_PlateImage.pbImgData)
	{
		bool bSave = SaveImgToDisk(pResult->CIMG_PlateImage.chSavePath, pResult->CIMG_PlateImage.pbImgData, pResult->CIMG_PlateImage.dwImgSize);
	}

	if (pResult->CIMG_BeginCapture.pbImgData)
	{
		bool bSave = SaveImgToDisk(pResult->CIMG_BeginCapture.chSavePath, pResult->CIMG_BeginCapture.pbImgData, pResult->CIMG_BeginCapture.dwImgSize);
	}

	if (pResult->CIMG_BestCapture.pbImgData)
	{
		bool bSave = SaveImgToDisk(pResult->CIMG_BestCapture.chSavePath, pResult->CIMG_BestCapture.pbImgData, pResult->CIMG_BestCapture.dwImgSize);
		//bool bSave  = SaveImgToDisk(pResult->CIMG_BestCapture.chSavePath, pResult->CIMG_BestCapture.pbImgData, pResult->CIMG_BestCapture.dwImgSize, 768, 576, 1); 
	}

	if (pResult->CIMG_LastCapture.pbImgData)
	{
		bool bSave = SaveImgToDisk(pResult->CIMG_LastCapture.chSavePath, pResult->CIMG_LastCapture.pbImgData, pResult->CIMG_LastCapture.dwImgSize);
	}

	if (pResult->CIMG_BestSnapshot.pbImgData)
	{
		bool bSave = SaveImgToDisk(pResult->CIMG_BestSnapshot.chSavePath, pResult->CIMG_BestSnapshot.pbImgData, pResult->CIMG_BestSnapshot.dwImgSize);
	}

	if (pResult->CIMG_LastSnapshot.pbImgData)
	{
		bool bSave = SaveImgToDisk(pResult->CIMG_LastSnapshot.chSavePath, pResult->CIMG_BestSnapshot.pbImgData, pResult->CIMG_LastSnapshot.dwImgSize);
		//bool bSave  = SaveImgToDisk(pResult->CIMG_LastSnapshot.chSavePath, pResult->CIMG_LastSnapshot.pbImgData, pResult->CIMG_LastSnapshot.dwImgSize, 768, 576, 0); 
	}
}

//bool Camera6467::GetFileNameAndPathFromDisk( char* pChFileName, char* pChFilePath )
//{
//	if (pChFileName == NULL || pChFilePath == NULL)
//	{
//		return false;
//	}
//	bool bFindFile = false;
//	TCHAR szFileName[260] = {0};
//	GetModuleFileName(NULL, szFileName, 260);	//取得包括程序名的全路径
//	PathRemoveFileSpec(szFileName);				//去掉程序名
//	CString strDir;
//	strDir.Format("%s\\Buffer\\%s\\*.jpg", szFileName, m_strIP.c_str());
//
//	CFileFind tempFind;
//	BOOL IsFind = tempFind.FindFile(strDir);
//
//	int iFindCout = 5;	
//	CString strFileName;
//	CString strFilePath;
//	while(IsFind && (iFindCout > 0) )
//	{
//		IsFind = tempFind.FindNextFileA();
//		if (!tempFind.IsDots())
//		{
//			strFileName = tempFind.GetFileName();
//			strFilePath = tempFind.GetFilePath();
//			sprintf_s(pChFileName, "%s", strFileName.GetBuffer());
//			sprintf_s(pChFilePath, "%s", strFilePath.GetBuffer());
//
//			strFileName.ReleaseBuffer();
//			strFilePath.ReleaseBuffer();
//			bFindFile = true;
//			break;
//		}
//		iFindCout--;
//		Sleep(50);		
//	}
//	tempFind.Close();
//	return bFindFile;
//}

void Camera6467::ExcuteCMD( char* pChCommand )
{
	if (NULL == pChCommand)
	{
		return;
	}
	ShellExecute(NULL, "open", "C:\\WINDOWS\\system32\\cmd.exe", pChCommand, "", SW_HIDE);
}

bool Camera6467::IsFileExist( const char* FilePath )
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

bool Camera6467::MyMakeDir( const char* chImgPath)
{
	if(NULL == chImgPath)
	{
		WriteLog("the path is null ,Create Dir failed.");
		return false;
	}
	std::string tempFile(chImgPath);
	size_t iPosition = tempFile.rfind("\\");
	std::string tempDir = tempFile.substr(0, iPosition+1);
	if (MakeSureDirectoryPathExists(tempDir.c_str()))
	{
		return true;
	}
	else
	{
		WriteLog("Create Dir failed.");
		return false;
	}
}

CameraResult* Camera6467::GetOneResultByIndex( int iIndex )
{
	CameraResult* tempResult = NULL;
	if (iIndex < 0 || iIndex > 100)
	{
		return tempResult;
	}
	EnterCriticalSection(&m_csResult);	
	if (m_ResultList.size() > iIndex)
	{
		int idex = 0;
		for (std::list<CameraResult*>::iterator it = m_ResultList.begin(); it != m_ResultList.end(); it++)
		{
			if (idex == iIndex)
			{
				tempResult = new CameraResult(**it);
				break;
			}
			idex++;
		}		
	}
	LeaveCriticalSection(&m_csResult);

	return tempResult;
}

void Camera6467::DeleteOneResult()
{
	CameraResult* tempResult = NULL;

	EnterCriticalSection(&m_csResult);	
	if (m_ResultList.size() > 0)
	{
		tempResult = m_ResultList.front();
		m_ResultList.pop_front();
	}
	LeaveCriticalSection(&m_csResult);
	if (tempResult != NULL)
	{
		delete tempResult;
		tempResult = NULL;
	}
}

void Camera6467::InterruptionConnection()
{
	if(NULL == m_hHvHandle)
	{
		return;
	}

	if ( (HVAPI_SetCallBackEx(m_hHvHandle, NULL, this, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, NULL) != S_OK) ||
		(HVAPI_SetCallBackEx(m_hHvHandle, NULL, this, 0, CALLBACK_TYPE_RECORD_PLATE, NULL) != S_OK) ||
		(HVAPI_SetCallBackEx(m_hHvHandle, NULL, this, 0, CALLBACK_TYPE_RECORD_BIGIMAGE, NULL) != S_OK) ||
		(HVAPI_SetCallBackEx(m_hHvHandle, NULL, this, 0, CALLBACK_TYPE_RECORD_SMALLIMAGE, NULL) != S_OK)||
		(HVAPI_SetCallBackEx(m_hHvHandle, NULL, this, 0, CALLBACK_TYPE_RECORD_BINARYIMAGE,NULL) != S_OK)||
		(HVAPI_SetCallBackEx(m_hHvHandle, NULL, this, 0, CALLBACK_TYPE_RECORD_INFOEND, NULL) != S_OK)||
		(HVAPI_SetCallBackEx(m_hHvHandle, NULL, this, 0, CALLBACK_TYPE_JPEG_FRAME, NULL) != S_OK)
		)
	{		
		WriteLog("DisConnectToCamera:: SetCallBack NULL failed.");
		//return false;
	}
	else
	{
		WriteLog("DisConnectToCamera:: SetCallBack NULL success.");
	}
}

int Camera6467::GetResultCount()
{
	int iCount = 0;
	EnterCriticalSection(&m_csResult);
	iCount = m_ResultList.size();
	LeaveCriticalSection(&m_csResult);
	return iCount;
}

void Camera6467::ClearAllResult()
{
	EnterCriticalSection(&m_csResult);
	CameraResult * ptempResult = NULL;
	size_t iCount = m_ResultList.size();
	while( iCount > 0)
	{
		ptempResult = m_ResultList.front();
		m_ResultList.pop_front();
		if (ptempResult != NULL)
		{
			delete ptempResult;
			ptempResult = NULL;
		}
		iCount = m_ResultList.size();
	}
	LeaveCriticalSection(&m_csResult);
}

void Camera6467::SetMsg( UINT iConMsg, UINT iDsiConMsg )
{
	m_iConnectMsg = (iConMsg < 0x402) ? 0x402 : iConMsg;
	m_iDisConMsg = (iDsiConMsg < 0x403) ? 0x403 : iDsiConMsg;
}

bool Camera6467::Img_ScaleJpg(PBYTE pbSrc,
    int iSrcLen,
    PBYTE pbDst,
    DWORD *iDstLen,
    int iDstWidth,
    int iDstHeight,
    int compressQuality)
{
	if (pbSrc == NULL || iSrcLen <= 0)
	{
		return false;
	}
	if (pbDst == NULL || iDstLen == NULL || *iDstLen <= 0)
	{
		return false;
	}
	if (iDstWidth <= 0 || iDstHeight <= 0)
	{
		return false;
	}

	// init gdi+
	ULONG_PTR gdiplusToken = NULL;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// 创建流
	IStream *pstmp = NULL;
	CreateStreamOnHGlobal(NULL, TRUE, &pstmp);
	if (pstmp == NULL)
	{
		GdiplusShutdown(gdiplusToken);
		gdiplusToken = NULL;
		return false;
	}

	// 初始化流
	LARGE_INTEGER liTemp = {0};
	ULARGE_INTEGER uLiZero = {0};
	pstmp->Seek(liTemp, STREAM_SEEK_SET,NULL);
	pstmp->SetSize(uLiZero);

	// 将图像放入流中
	ULONG ulRealSize = 0;
	pstmp->Write(pbSrc, iSrcLen, &ulRealSize);

	// 从流创建位图
	Bitmap bmpSrc(pstmp);
	Bitmap bmpDst(iDstWidth, iDstHeight, PixelFormat24bppRGB);

	// 创建画图对象
	Graphics grDraw(&bmpDst);

	// 绘图
	grDraw.DrawImage(&bmpSrc, 0, 0, bmpSrc.GetWidth(), bmpSrc.GetHeight());
	if (Ok != grDraw.GetLastStatus())
	{
		pstmp->Release();
		pstmp = NULL;
		GdiplusShutdown(gdiplusToken);
		gdiplusToken = NULL;
		return false;
	}

	// 创建输出流
	IStream* pStreamOut = NULL;
	if (CreateStreamOnHGlobal(NULL, TRUE, &pStreamOut) != S_OK)
	{
		pstmp->Release();
		pstmp = NULL;
		GdiplusShutdown(gdiplusToken);
		gdiplusToken = NULL;
		return false;
	}

	CLSID jpgClsid;
	GetEncoderClsid(L"image/jpeg", &jpgClsid);

	// 初始化输出流
	pStreamOut->Seek(liTemp, STREAM_SEEK_SET, NULL ); 
	pStreamOut->SetSize(uLiZero); 

	// 将位图按照JPG的格式保存到输出流中
	int iQuality = compressQuality%100;
	EncoderParameters encoderParameters;
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	encoderParameters.Parameter[0].Value = &iQuality;
	bmpDst.Save(pStreamOut, &jpgClsid, &encoderParameters);
	//bmpDst.Save(pStreamOut, &jpgClsid, 0);

	// 获取输出流大小
	bool bRet = false;
	ULARGE_INTEGER libNewPos = {0};
	pStreamOut->Seek(liTemp, STREAM_SEEK_END, &libNewPos);      // 将流指针指向结束位置，从而获取流的大小 
	if (*iDstLen < (int)libNewPos.LowPart)                     // 用户分配的缓冲区不足
	{
		*iDstLen = libNewPos.LowPart;
		bRet = false;
	}
	else
	{
		pStreamOut->Seek(liTemp, STREAM_SEEK_SET, NULL);                   // 将流指针指向开始位置
		pStreamOut->Read(pbDst, libNewPos.LowPart, &ulRealSize);           // 将转换后的JPG图片拷贝给用户
		*iDstLen = ulRealSize;
		bRet = true;
	}


	// 释放内存
	if (pstmp != NULL)
	{
		pstmp->Release();
		pstmp = NULL;
	}
	if (pStreamOut != NULL)
	{
		pStreamOut->Release();
		pStreamOut = NULL;
	}

	GdiplusShutdown(gdiplusToken);
	gdiplusToken = NULL;

	return bRet;
}

void Camera6467::CompressImg( CameraIMG& camImg, DWORD requireSize )
{
	if (camImg.dwImgSize <= 0 || camImg.dwImgSize <= requireSize || !(camImg.pbImgData) )
	{
		WriteLog("图片大小为0，或原图已满足要求大小，结束压缩.");
		return;
	}

    DWORD iImgSize = 5 * 1024 * 1024;
	int iCompressQuality = 80;
	PBYTE pDestImg = new BYTE[iImgSize];
	do
	{
		iImgSize = 5*1024*1024;
		memset(pDestImg, 0, iImgSize);		
		bool bRet = Img_ScaleJpg(camImg.pbImgData, camImg.dwImgSize, pDestImg, &iImgSize, camImg.wImgWidth, camImg.wImgHeight, iCompressQuality);		
		iCompressQuality -= 10;
	}while(iCompressQuality >= 10 && iImgSize > requireSize);

 	if (iImgSize <= requireSize)
	{
		delete[] camImg.pbImgData;
		camImg.pbImgData = NULL;

		camImg.pbImgData = new BYTE[iImgSize];
		memcpy(camImg.pbImgData, pDestImg, iImgSize);
		camImg.dwImgSize = iImgSize;

		char chaLog[MAX_PATH] = {0};
		sprintf_s(chaLog, "图片压缩成功, 最后大小为%d", iImgSize);
		WriteLog(chaLog);
	}
	else
	{
		WriteLog("图片压缩失败，继续使用原图.");
	}
	if (pDestImg)
	{
		delete[] pDestImg;
		pDestImg = NULL;
	}
}

void Camera6467::SendConnetStateMsg( bool isConnect )
{
	//if (m_hWnd == NULL)
	//	return;

	if (isConnect)
	{
        EnterCriticalSection(&m_csFuncCallback);
        if (g_ConnectStatusCallback)
        {
            LeaveCriticalSection(&m_csFuncCallback);
            //char chIP[32] = { 0 };
            //sprintf_s(chIP, "%s", m_strIP.c_str());
            g_ConnectStatusCallback(m_iIndex, 0, g_pUser);
        }
        else
        {
            LeaveCriticalSection(&m_csFuncCallback);
        }
		
		//::PostMessage(m_hWnd,m_iConnectMsg, NULL,NULL);
	}
	else
	{
        EnterCriticalSection(&m_csFuncCallback);
        if (g_ConnectStatusCallback)
        {
            LeaveCriticalSection(&m_csFuncCallback);
            char chIP[32] = { 0 };
            sprintf_s(chIP, "%s", m_strIP.c_str());
            g_ConnectStatusCallback(m_iIndex, -100, g_pUser);
        }
        else
        {
            LeaveCriticalSection(&m_csFuncCallback);
        }
		//::PostMessage(m_hWnd,m_iDisConMsg, NULL,NULL);
	}
}

void Camera6467::SendMessageToPlateServer( int iMessageType /*= 1*/ )
{
	char chMessage[50] = {0};
	if (iMessageType == 1)
	{
		sprintf_s(chMessage, "deleteOneResult");
	}
	else if (iMessageType == 2)
	{
		sprintf_s(chMessage, "deleteALLResult");
	}
	else
	{
		sprintf_s(chMessage, "hello.");
	}
	WriteLog("send 'deleteOneResult' to Server.");
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return;
	}

	if (LOBYTE( wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return;
	}

	SOCKET sockClient = socket(AF_INET,SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port= htons(VEHICLE_LISTEN_PORT);
	connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	int iSendLenth = send(sockClient, chMessage, strlen(chMessage)+1, 0);

	char chLog[260] = {0};
	sprintf_s(chLog, "send buffer =%s, lenth = %d", chMessage,iSendLenth);
	WriteLog(chLog);
	//char recvBuf[50] = {0};
	//recv(sockClient, recvBuf, 50, 0);
	//printf("%s\n", recvBuf);
	//WriteLog(recvBuf);

	closesocket(sockClient);
	WSACleanup();
	getchar();
}

bool Camera6467::SenMessageToCamera( int iMessageType , int& iReturnValue, int& iErrorCode , int iArg)
{
	if (NULL == m_hHvHandle)
	{
		iErrorCode = -1;
		return false;
	}		

	char chRetBuf[1024] = {0};
	char chSendBuf[256] ={0};
	char chLog[256] = {0};
	int nRetLen = 0;
	const char* pAttribute1 = NULL;
	const char* pAttribute2 = NULL;
	const char* pAttribute3 = NULL;
	const char* pAttribute4 = NULL;

	if (iMessageType == CMD_DEL_VEH_HEAD)
	{
		sprintf_s(chSendBuf, "DeleteHead_ZHWL");
	}
	else if (iMessageType == CMD_GET_VEH_LENGTH)
	{
		sprintf_s(chSendBuf, "GetQueueSize_ZHWL");
	}
	else if (iMessageType == CMD_DEL_ALL_VEH)
	{
		sprintf_s(chSendBuf, "DeleteAll_ZHWL");
	}
	else if (iMessageType == CMD_GET_VHE_HEAD)
	{
		int iValue = 0;
		if (iArg < 0)
		{
			iValue = 0;
		}
		else
		{
			iValue = iArg;
		}
		sprintf_s(chSendBuf, "GetResult_ZHWL, Value[%d]", iValue);
	}

	if (strlen(chSendBuf) <= 0)
	{
		WriteLog("SenMessageToCamera, please input the right command");
		return false;
	}

	if (HVAPI_ExecCmdEx(m_hHvHandle, chSendBuf, chRetBuf, 1024, &nRetLen) != S_OK)
	{
		memset(chLog, 0, sizeof(chLog));
		sprintf_s(chLog, "%s  send failed.", chSendBuf);
		WriteLog(chLog);

		iErrorCode = -2;
		return false;
	}
	else
	{
		memset(chLog, 0, sizeof(chLog));
		sprintf_s(chLog, "%s  send success.", chSendBuf);
		WriteLog(chLog);
	}

	if (iMessageType == CMD_GET_VEH_LENGTH)
	{
		int iLength = 0;
		sscanf_s(chRetBuf, "%d", &iLength);
		iReturnValue = iLength;
	}

	iErrorCode = 0;
	return true;
}

int Camera6467::GetLoginID()
{
	return m_iLoginID;
}

void Camera6467::SetLoginID( int iID )
{
	m_iLoginID = iID;
}

void Camera6467::SetCameraIP( char* ipAddress )
{
	m_strIP = std::string(ipAddress);
}

void Camera6467::SetWindowsWnd( HWND hWnd )
{
	m_hWnd = hWnd;
}

void Camera6467::SetDisConnectCallback( void* funcDisc, void* pUser )
{
	EnterCriticalSection(&m_csFuncCallback);
	//g_func_DisconnectCallback = (DISCONNECT_CALLBACK)funcDisc;
	g_pUser = pUser;
	LeaveCriticalSection(&m_csFuncCallback);
}

int Camera6467::DeviceJPEGStream( PBYTE pbImageData, DWORD dwImageDataLen, DWORD dwImageType, LPCSTR szImageExtInfo )
{
	static int iCout = 0;
	if(iCout++ > 100)
	{
		WriteLog("receive one jpeg frame.");
		iCout = 0;
	}

	//if (g_funcBigImg_OSD_Callback)
	//{
	//	WriteLog("CAPTURE_IMAGE_CALLBACK begin.");
	//	g_funcBigImg_OSD_Callback(g_pUser, '2', 1920, 1080, dwImageDataLen, pbImageData, GetTickCount());
	//	WriteLog("CAPTURE_IMAGE_CALLBACK end.");
	//	g_funcBigImg_OSD_Callback = NULL;
	//}

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

bool Camera6467::GetOneJpegImg( CameraIMG &destImg )
{
	WriteLog("GetOneJpegImg::begin.");
	bool bRet = false;

	if(!destImg.pbImgData)
	{
		WriteLog("GetOneJpegImg:: allocate memory.");
		destImg.pbImgData = new unsigned char[MAX_IMG_SIZE];
		memset(destImg.pbImgData, 0, MAX_IMG_SIZE);
		WriteLog("GetOneJpegImg:: allocate memory success.");
	}

	EnterCriticalSection(&m_csResult);
	if(m_bJpegComplete)
	{
		if(destImg.pbImgData)
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

void Camera6467::SetReConnectCallback( void* funcReco, void* pUser )
{
	EnterCriticalSection(&m_csFuncCallback);
	//g_func_ReconnectCallback = (RECONNECT_CALLBACK)funcReco;
	g_pUser = pUser;
	LeaveCriticalSection(&m_csFuncCallback);
}

void Camera6467::SetBigImgCallback( void* funcBigImg, void* pUser )
{
	EnterCriticalSection(&m_csFuncCallback);
    m_pBigImageCallback = funcBigImg;
	m_pBigImageCallbackUserData = pUser;
	LeaveCriticalSection(&m_csFuncCallback);
}

void Camera6467::SetBigImg_OSD_Callback( void* funcBig_OSD_Img, void* pUser )
{
	EnterCriticalSection(&m_csFuncCallback);
	//g_funcBigImg_OSD_Callback = (CAPTURE_IMAGE_CALLBACK)funcBig_OSD_Img;
	g_pUser = pUser;
	LeaveCriticalSection(&m_csFuncCallback);
}

void Camera6467::SetPlateNo_Callback( void* funcPlateNo, void* pUser )
{
	EnterCriticalSection(&m_csFuncCallback);
	//g_func_PlateNoCallback = (PLATE_CALLBACK)funcPlateNo;
	g_pUser = pUser;
	LeaveCriticalSection(&m_csFuncCallback);
}

void Camera6467::SetPlateImg_Callback( void* funcPlateImg, void* pUser )
{
	EnterCriticalSection(&m_csFuncCallback);
	//g_funcPlateImgCallback = (PLATE_IMAGE_CALLBACK)funcPlateImg;
	g_pUser = pUser;
	LeaveCriticalSection(&m_csFuncCallback);
}

void Camera6467::SetPlateBin_Callback( void* funcPlateBin, void* pUser )
{
	EnterCriticalSection(&m_csFuncCallback);
	//g_func_PlateBinImgCallback = (PLATE_BIN_IMAGE_CALLBACK)funcPlateBin;
	g_pUser = pUser;
	LeaveCriticalSection(&m_csFuncCallback);
}


void Camera6467::SetCameraIndex(int iIndex)
{
    EnterCriticalSection(&m_csFuncCallback);
    m_iIndex = iIndex;
    LeaveCriticalSection(&m_csFuncCallback);
}

int Camera6467::GetCameraIndex()
{
    int iValue = 0;
    EnterCriticalSection(&m_csFuncCallback);
    iValue =  m_iIndex;
    LeaveCriticalSection(&m_csFuncCallback);
    return iValue;
}

void Camera6467::SetResult_Callback(void* funcPlate, void* pUser)
{
    EnterCriticalSection(&m_csFuncCallback);
    g_ResultCallback = (CBFun_GetRegResult)funcPlate;
    g_pUser = pUser;
    LeaveCriticalSection(&m_csFuncCallback);
}

void Camera6467::SetConnectStatus_Callback(void* func, void* pUser, int TimeInterval)
{
    EnterCriticalSection(&m_csFuncCallback);
    g_ConnectStatusCallback = (CBFun_GetDevStatus)func;
    g_pUser = pUser;
    m_iTimeInvl = TimeInterval;
    LeaveCriticalSection(&m_csFuncCallback);
}

int Camera6467::GetTimeInterval()
{
    int iTimeInterval = 1;
    EnterCriticalSection(&m_csFuncCallback);
    iTimeInterval = m_iTimeInvl;
    LeaveCriticalSection(&m_csFuncCallback);
    return iTimeInterval;
}

void Camera6467::SendResultByCallback(CameraResult* pResult)
{
    char chLog[256] = {0};
    EnterCriticalSection(&m_csFuncCallback);
    if (g_ResultCallback)
    {
        LeaveCriticalSection(&m_csFuncCallback);
        
        SYSTEMTIME st;
        GetLocalTime(&st);

        //填充结构体
        T_VLPINFO PLTE_INFO;
        memset(&PLTE_INFO, 0, sizeof(T_VLPINFO));

        PLTE_INFO.vlpInfoSize = sizeof(T_VLPINFO);
        sprintf_s(PLTE_INFO.vlpTime, "%04d%02d%02d%02d%02d%02d%03d",
            st.wYear,
            st.wMonth,
            st.wDay,
            st.wHour,
            st.wMinute,
            st.wSecond,
            st.wMilliseconds);
        PLTE_INFO.vlpCarClass = 0;
        char chTemp[32] = { 0 };
        MY_SPRINTF(chTemp, "%02d", m_BufferResult->iPlateColor);
        memcpy(PLTE_INFO.vlpColor, chTemp, 2);
        //PLTE_INFO.vlpColor[0] = 0;
        //PLTE_INFO.vlpColor[1] = m_BufferResult->iPlateColor;

        memset(chTemp, '\0', sizeof(chTemp));
        //MY_SPRINTF(chTemp, "%s", m_BufferResult->chPlateNO);
        //memcpy(PLTE_INFO.vlpText, chTemp, strlen(m_BufferResult->chPlateNO)+1);
        Tool_ProcessPlateNo(m_BufferResult->chPlateNO, chTemp, sizeof(chTemp));
        sprintf_s((char*)(PLTE_INFO.vlpText), 16, "%s", chTemp);

        PLTE_INFO.vlpReliability = m_BufferResult->iReliability;
        //大图
        PLTE_INFO.imageLength[0] = m_BufferResult->CIMG_LastSnapshot.dwImgSize;
        PLTE_INFO.image[0] = m_BufferResult->CIMG_LastSnapshot.pbImgData;
        //小图
        PLTE_INFO.imageLength[1] = m_BufferResult->CIMG_PlateImage.dwImgSize;
        PLTE_INFO.image[1] = m_BufferResult->CIMG_PlateImage.pbImgData;
        //二值图
        PLTE_INFO.imageLength[2] = m_BufferResult->CIMG_BinImage.dwImgSize;
        PLTE_INFO.image[2] = m_BufferResult->CIMG_BinImage.pbImgData;

        memset(chLog, '\0', sizeof(chLog));
        sprintf_s(chLog, sizeof(chLog), "Result callback begin ,g_ResultCallback = %p,  g_pUser = %p, plate number = %s", 
            g_ResultCallback,
            g_pUser,
            m_BufferResult->chPlateNO);
        WriteLog(chLog);

        int iHandleID = GetCameraIndex();

        EnterCriticalSection(&m_csFuncCallback);
        g_ResultCallback(iHandleID, &PLTE_INFO, g_pUser);
        LeaveCriticalSection(&m_csFuncCallback);

        WriteLog("Result callback end.");

        PLTE_INFO.imageLength[0] = 0;
        PLTE_INFO.image[0] = NULL;
        //小图
        PLTE_INFO.imageLength[1] = 0;
        PLTE_INFO.image[1] = NULL;
        //二值图
        PLTE_INFO.imageLength[2] = 0;
        PLTE_INFO.image[2] = NULL;
    }
    else
    {
        LeaveCriticalSection(&m_csFuncCallback);
        WriteLog("Result callback is NULL.");
    }
}

void Camera6467::SendBigImgByCallback(CameraIMG &destImg)
{
    char chLog[256] = { 0 };
    EnterCriticalSection(&m_csFuncCallback);
    if (m_pBigImageCallback)
    {
        LeaveCriticalSection(&m_csFuncCallback);

        int iHandleID = GetCameraIndex();
        unsigned char* pImg = NULL;
        int iImageLength = 0;
        if (destImg.dwImgSize > 0
            && destImg.pbImgData != NULL)
        {
            pImg = destImg.pbImgData;
            iImageLength = destImg.dwImgSize;
        }

        memset(chLog, '\0', sizeof(chLog));
        sprintf_s(chLog, sizeof(chLog), "big image callback begin ,m_pBigImageCallback = %p,  m_pBigImageCallbackUserData = %p, image data = %p, image length = %d",
            m_pBigImageCallback,
            m_pBigImageCallbackUserData, 
            pImg,
            iImageLength);
        WriteLog(chLog);

        EnterCriticalSection(&m_csFuncCallback);
        ((CBFun_GetImageResult)m_pBigImageCallback)(iHandleID, 1, (char*)pImg, iImageLength, m_pBigImageCallbackUserData);
        LeaveCriticalSection(&m_csFuncCallback);

        WriteLog("big image callback finish.");
    }
    else
    {
        LeaveCriticalSection(&m_csFuncCallback);
        WriteLog("big image callback is NULL.");
    }
}

bool Camera6467::SetOverlayVedioFont(int iFontSize, int iColor)
{
	char chLog[260] ={0};
	sprintf_s(chLog, "SetOverlayVedioFont, size = %d, color = %d", iFontSize, iColor);
	WriteLog(chLog);
	HRESULT hRet =  S_FALSE;
	HRESULT hRet2 = S_FALSE;
	int iR = 255, iG = 255, iB = 255;
	switch (iColor)
	{
	case 0:		//白
		iR = 255, iG = 255, iB = 255;
		break;
	case 1:		//红
		iR = 255, iG = 0, iB = 0;
		break;
	case 2:		//黄
		iR = 255, iG = 255, iB = 0;
		break;
	case 3:		//蓝
		iR = 0, iG = 0, iB = 255;
		break;
	case 4:		//黑
		iR = 0, iG = 0, iB = 0;
		break;
	case 5:		//绿
		iR = 0, iG = 255, iB = 0;
		break;
	case 6:		//紫
		iR = 138, iG = 43, iB = 226;
		break;
	default:
		iR = 255, iG = 255, iB = 255;
		break;
	}


	if (m_hHvHandle)
	{
		hRet = HVAPI_SetOSDFont((HVAPI_HANDLE_EX)m_hHvHandle, 0 , iFontSize, iR, iG, iB);
		//hRet2 = HVAPI_SetOSDFont((HVAPI_HANDLE_EX)m_hHvHandle, 1 , iFontSize, iR, iG, iB);
		hRet2 = HVAPI_SetOSDFont((HVAPI_HANDLE_EX)m_hHvHandle, 2 , iFontSize, iR, iG, iB);
		if (hRet)
		{
			WriteLog("set H264 Font  success.");
		}
		if (hRet2)
		{
			WriteLog("set JPEG Font  success");
		}
	}
	else
	{
		WriteLog("set time text, but the handle is invalid.");
	}
	if(hRet!= S_OK && hRet2 != S_OK)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool Camera6467::GetHardWareInfo(BasicInfo& bsinfo)
{
    if (NULL == m_hHvHandle)
    {
        return false;
    }
    CDevBasicInfo tempInfo;
    HRESULT hr = HVAPI_GetDevBasicInfo((HVAPI_HANDLE_EX)m_hHvHandle, &tempInfo);
    if (hr == S_OK)
    {
        memcpy(&bsinfo, &tempInfo, sizeof(BasicInfo));
        return true;
    }
    else
    {
        return false;
    }
}

unsigned int __stdcall StatusCheckThread_plate(LPVOID lpParam)
{
	if (!lpParam)
	{
		return -1;
	}
	Camera6467* pThis = (Camera6467*)lpParam;
	int iLastStatus = -1;
	INT64 iLastTick = 0, iCurrentTick = 0;
	int iFirstConnctSuccess = -1;

	while(!pThis->m_bStatusCheckThreadExit)
	{
		if (NULL == pThis)
		{
			break;
		}        
		Sleep(50);
        iCurrentTick = GetTickCount();
        int iTimeInterval = pThis->GetTimeInterval();
        if ( (iCurrentTick - iLastTick) >= (iTimeInterval * 1000))
		{
			int iStatus =  pThis->GetCamStatus();
			if ( iStatus == 0 )
			{
				//if (iStatus != iLastStatus)
				//{
				//	pThis->SendConnetStateMsg(true);
				//}
                pThis->SendConnetStateMsg(true);
				pThis->WriteLog("设备连接正常.");
				iFirstConnctSuccess = 0;
			}
			else
			{
				pThis->SendConnetStateMsg(false);
				pThis->WriteLog("设备连接失败, 尝试重连");

				if (iFirstConnctSuccess == -1)
				{
					//pThis->ConnectToCamera();
				}
			}
			iLastStatus = iStatus;

            iLastTick = iCurrentTick;
		}

	}
	return 0;
}