#ifndef CAMERA6467_H
#define CAMERA6467_H
#include "CameraResult.h"
#include "CommonDef_VPR.h"
#include <string>
#include <list>
#include <comdef.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma  comment(lib, "gdiplus.lib")

#define CMD_DEL_VEH_HEAD 1
#define CMD_GET_VEH_LENGTH 2
#define  CMD_DEL_ALL_VEH 3
#define CMD_GET_VHE_HEAD 4
//
//struct CameraInfo
//{
//	char chIP[20];                          //deviceIP
//	char chDeviceID[3];
//	char chStationID[7];
//	char chLaneID[3];
//	bool bLogEnable;
//	bool bSynTimeEnable;
//	bool bIsInit;
//	int iDirection;
//	CameraInfo()
//	{
//		memset(chIP, 0, 20);
//		memset(chDeviceID, 0, 3);
//		memset(chStationID, 0, 7);
//		memset(chLaneID, 0, 3);
//		bLogEnable = true;
//		bSynTimeEnable = false;
//		bIsInit = false;
//		iDirection = 0;
//	}
//};
//
//struct SaveModeInfo
//{
//	int iSafeModeEnable;
//	char chBeginTime[260];
//	char chEndTime[260];
//	int iIndex;
//	int iDataType;
//	SaveModeInfo()
//	{
//		iSafeModeEnable = 0;
//		iIndex = 0;
//		iDataType = 0;
//		memset(chBeginTime, 0, 260);
//		memset(chEndTime, 0, 260);
//	}
//};
//
//struct DeviceTime
//{
//	int iYear;
//	int iMonth;
//	int iDay;
//	int iHour;
//	int iMinutes;
//	int iSecond;
//	int iMilisecond;
//	
//	DeviceTime()
//	{
//		iYear = 0;
//		iMonth = 0;
//		iDay = 0;
//		iHour = 0;
//		iMinutes = 0;
//		iSecond = 0;
//		iMilisecond = 0;
//	}
//};
//
//typedef struct _BasicInfo
//{
//    char szIP[64];              //设备IP，适用于所有设备类型
//    char szMask[64];            //设备子网掩码，适用于所有设备类型
//    char szGateway[64];         //设备网关，适用于所有设备类型
//    char szMac[128];            //设备物理地址，适用于所有设备类型
//    char szModelVersion[128]; //设备模型版本，此字段保留
//    char szSN[128];             //设备编号，适用于所有设备类型
//    char szWorkMode[128];       //设备工作模式，仅适用于PCC200、PCC600、PCC200A
//    char szDevType[128];        //设备类型，仅适用于PCC200、PCC600、PCC200A
//    char szDevVersion[128];  //设备版本，仅适用于PCC200、PCC600、PCC200A
//    char szMode[128];           //设备运行模式，如正常模式，适用于所有设备类型
//    char szRemark[128];    //保留字段，仅适用于PCC200、PCC600
//    char szBackupVersion[128];  //备份版本，仅适用于PCC200、PCC600 
//    char szFPGAVersion[128]; //FPGA版本，仅适用于PCC200、PCC600
//    char szKernelVersion[128];  //Kernel版本，仅适用于PCC200、PCC600
//    char szUbootVersion[128]; //Uboot版本，仅适用于PCC200、PCC600
//    char szUBLVersion[128];//UBL版本，仅适用于PCC200、PCC600
//}BasicInfo;

class Camera6467
{
public:
	Camera6467();
	Camera6467(const char* chIP, HWND  hWnd,int Msg);
	//Camera6467(const char* chIP, HWND*  hWnd,int Msg);
	~Camera6467();

	//void SetList(Result_lister* ResultList);
	bool SetCameraInfo(CameraInfo& camInfo);
	void SetLoginID(int iID);

	int GetCamStatus();
	int GetLoginID();
	char* GetStationID();
	char* GetDeviceID();
	char*  GetLaneID();
	const char* GetCameraIP();
	void SetCameraIP(char* ipAddress);
	void SetWindowsWnd(HWND  hWnd);
	CameraResult* GetOneResult();
	CameraResult* GetOneResultByIndex(int iIndex);
	int GetResultCount();
	void DeleteOneResult();
	bool GetDeviceTime(DeviceTime& deviceTime);
	void ClearAllResult();

	int ConnectToCamera();
	
	void ReadConfig();
	bool WriteLog(const char* chlog);
	bool TakeCapture();
	bool SynTime();
	bool SynTime(int Year, int Month, int Day, int Hour, int Minute, int Second, int MilientSecond);
	bool SaveImgToDisk( char* chImgPath, BYTE* pImgData, DWORD dwImgSize);
	bool SaveImgToDisk( char* chImgPath, BYTE* pImgData, DWORD dwImgSize, int iWidth, int iHeight, int iType = 0);
	void SetResultToUnready();
	void SetMsg(UINT iConMsg, UINT iDsiConMsg);
	void SaveResultToBuffer(CameraResult* pResult);
//	bool GetFileNameAndPathFromDisk(char* pChFileName, char* pChFilePath);
	void ExcuteCMD(char* pChCommand);
	bool IsFileExist(const char* FilePath);
	bool MyMakeDir(const char*);
	bool Img_ScaleJpg(PBYTE pbSrc, int iSrcLen, PBYTE pbDst, DWORD *iDstLen, int iDstWidth, int iDstHeight, int compressQuality);
    
	void CompressImg(CameraIMG& camImg, DWORD requireSize);
	void SendConnetStateMsg(bool isConnect);

	void SendMessageToPlateServer(int iMessageType = 1);
	bool SenMessageToCamera( int iMessageType , int& iReturnValue, int& iErrorCode , int iArg=0);

	void SetDisConnectCallback(void* funcDisc, void* pUser);
	void SetReConnectCallback(void* funcReco, void* pUser);
	void SetBigImgCallback(void* funcBigImg, void* pUser);
	void SetBigImg_OSD_Callback(void* funcBig_OSD_Img, void* pUser);
	void SetPlateNo_Callback(void* funcPlateNo, void* pUser);
	void SetPlateImg_Callback(void* funcPlateImg, void* pUser);
	void SetPlateBin_Callback(void* funcPlateBin, void* pUser);

    void SetCameraIndex(int iIndex);
    void SetResult_Callback(void* funcPlateBin, void* pUser);
    void SetConnectStatus_Callback(void* func, void* pUser, int TimeInterval);
    int GetTimeInterval();

	bool GetOneJpegImg(CameraIMG &destImg);	

	bool SetOverlayVedioFont(int iFontSize, int iColor);

    bool GetHardWareInfo(BasicInfo& info);

	bool m_bStatusCheckThreadExit;
private:
	void* m_hHvHandle;
	HANDLE m_hPlayH264;
	//void* m_hWnd;
	HWND m_hWnd;
	int m_iMsg;
	int m_iConnectMsg;
	int m_iDisConMsg;
	std::string m_strIP;
	std::string m_strDeviceID;
	std::string m_strLastPlateNo;
	int m_iConnectStatus;
	int m_iLoginID;

	char m_chDeviceID[3];
	char m_chStationID[7];
	char m_chLaneID[3];
	bool m_bLogEnable;
	bool m_bSynTime;
	bool m_bResultComplete;
	bool m_bSaveToBuffer;
	bool m_bJpegComplete;
	unsigned long m_dwConnectStatus;
	int m_iCompressQuality;
	int m_iDirection;
	DWORD m_dwLastCarID;
	DWORD m_dwReplaceCarID;
	int m_iSuperLenth;
	int m_iVedioWidth;
	int m_iVedioHeight;

    int m_iTimeInvl;
    int m_iIndex;

	char m_qstrBackupPath[260];
	char m_qstrRootPath[260];

	CLSID m_jpgClsid;
	CLSID m_bmpClsid;
	
	SaveModeInfo m_SaveModelInfo;

	CameraResult* m_CameraResult;
	CameraResult* m_BufferResult;
	std::list<CameraResult*>  m_ResultList;
	CRITICAL_SECTION m_csLog;
	CRITICAL_SECTION m_csResult;
	CRITICAL_SECTION m_csFuncCallback;

	HANDLE m_hStatusCheckThread;			//状态检测线程
	CameraIMG m_CIMG_StreamJPEG;
	CameraIMG m_Bin_IMG_Temp;
	CameraIMG m_Small_IMG_Temp;
	PBYTE m_pTempBin;
	PBYTE m_CaptureImg;
	PBYTE m_pTempBig;

    CBFun_GetRegResult g_ResultCallback;
    CBFun_GetDevStatus g_ConnectStatusCallback;

	void ReadHistoryInfo();
	void WriteHistoryInfo(SaveModeInfo& SaveInfo);	
	void AnalysisAppendInfo(CameraResult* CamResult);
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	bool GetStreamLength( IStream* pStream, ULARGE_INTEGER* puliLenth );
	//bool SaveImgToDiskByQt( CameraIMG& camImageStruct);
	//void SetImgPath(CameraResult* camResult);
	void	InterruptionConnection();

	void* g_pUser;

	inline void CopyDataToIMG(CameraIMG& DestImg, PBYTE pImgData, unsigned long width, unsigned long height, unsigned long dataLenth, WORD wImgType)
	{
		if(NULL != DestImg.pbImgData)
		{
			delete[] DestImg.pbImgData;
			DestImg.pbImgData = NULL;
		}

		DestImg.pbImgData = new(std::nothrow) BYTE[dataLenth];
		DestImg.dwImgSize = 0;
		if (  NULL != DestImg.pbImgData)
		{
			memcpy(DestImg.pbImgData, pImgData, dataLenth);
			DestImg.wImgWidth = width;
			DestImg.wImgHeight = height;
			DestImg.dwImgSize = dataLenth;
			DestImg.wImgType = wImgType;
		}
	}


public:
	static int  RecordInfoBeginCallBack(PVOID pUserData, DWORD dwCarID)
	{
		if ( pUserData == NULL )
			return 0;

		Camera6467* pThis = (Camera6467*)pUserData;
		return pThis->RecordInfoBegin(dwCarID);
	};
	int RecordInfoBegin(DWORD dwCarID);


	static int  RecordInfoEndCallBack(PVOID pUserData, DWORD dwCarID)
	{
		if ( pUserData == NULL )
			return 0;

		Camera6467* pThis = (Camera6467*)pUserData;
		return pThis->RecordInfoEnd(dwCarID);
	};
	int RecordInfoEnd(DWORD dwCarID);


	static int  RecordInfoPlateCallBack(PVOID pUserData,
		DWORD dwCarID,
		LPCSTR pcPlateNo,
		LPCSTR pcAppendInfo,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS)
	{
		if (pUserData == NULL)
			return 0;

		Camera6467* pThis = (Camera6467*)pUserData;
		return pThis->RecordInfoPlate(dwCarID, pcPlateNo, pcAppendInfo, dwRecordType, dw64TimeMS);
	}
	int RecordInfoPlate(DWORD dwCarID,
		LPCSTR pcPlateNo,
		LPCSTR pcAppendInfo,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS);


	static int  RecordInfoBigImageCallBack(PVOID pUserData,
		DWORD dwCarID,
		WORD  wImgType,
		WORD  wWidth,
		WORD  wHeight,
		PBYTE pbPicData,
		DWORD dwImgDataLen,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS)
	{
		if (pUserData == NULL)
			return 0;

		Camera6467* pThis = (Camera6467*)pUserData;
		return pThis->RecordInfoBigImage(dwCarID, wImgType, wWidth, wHeight, pbPicData, dwImgDataLen, dwRecordType, dw64TimeMS);
	}
	int RecordInfoBigImage(DWORD dwCarID,
		WORD  wImgType,
		WORD  wWidth,
		WORD  wHeight,
		PBYTE pbPicData,
		DWORD dwImgDataLen,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS);



	static int  RecordInfoSmallImageCallBack(PVOID pUserData,
		DWORD dwCarID,
		WORD wWidth,
		WORD wHeight,
		PBYTE pbPicData,
		DWORD dwImgDataLen,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS)
	{
		if (pUserData == NULL)
			return 0;

		Camera6467* pThis = (Camera6467*)pUserData;
		return pThis->RecordInfoSmallImage(dwCarID, wWidth, wHeight, pbPicData, dwImgDataLen, dwRecordType, dw64TimeMS);
	}
	int RecordInfoSmallImage(DWORD dwCarID,
		WORD wWidth,
		WORD wHeight,
		PBYTE pbPicData,
		DWORD dwImgDataLen,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS);


	static int  RecordInfoBinaryImageCallBack(PVOID pUserData,
		DWORD dwCarID,
		WORD wWidth,
		WORD wHeight,
		PBYTE pbPicData,
		DWORD dwImgDataLen,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS)
	{
		if (pUserData == NULL)
			return 0;

		Camera6467* pThis = (Camera6467*)pUserData;
		return pThis->RecordInfoBinaryImage(dwCarID, wWidth, wHeight, pbPicData, dwImgDataLen, dwRecordType, dw64TimeMS);
	}
	int RecordInfoBinaryImage(DWORD dwCarID,
		WORD wWidth,
		WORD wHeight,
		PBYTE pbPicData,
		DWORD dwImgDataLen,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS);


	static int  JPEGStreamCallBack(
		PVOID pUserData,
		PBYTE pbImageData,
		DWORD dwImageDataLen,
		DWORD dwImageType,
		LPCSTR szImageExtInfo
		)
	{
		if ( pUserData == NULL )
			return 0;

		Camera6467* pThis = (Camera6467*)pUserData;
		return pThis->DeviceJPEGStream(pbImageData,dwImageDataLen,dwImageType,szImageExtInfo);

	};
	int DeviceJPEGStream(
		PBYTE pbImageData,
		DWORD dwImageDataLen,
		DWORD dwImageType,
		LPCSTR szImageExtInfo);
};

#endif