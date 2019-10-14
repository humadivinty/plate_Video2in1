#ifndef _HVAPI_HANDLE_CONTEXT_H__
#define _HVAPI_HANDLE_CONTEXT_H__

#include "HvDeviceDLL.h"
#include "HvDeviceNew.h"
#include "refactored/HvDeviceAgent.h"
#include <list>

/* �ص������� */
typedef struct _HVAPI_CALLBACK_SET
{
	struct _HVAPI_CALLBACK_SET* pNext;
	INT iVideoID;	//��Ƶ�����

	HVAPI_CALLBACK_RECORD_INFOBEGIN pOnRecordBegin;	//ʶ������ʼ�ص�
	PVOID pOnRecordBeginParam; //ʶ������ʼ�ص��û�����

	HVAPI_CALLBACK_RECORD_INFOEND pOnRecordEnd;	//ʶ���������ص�
	PVOID pOnRecordEndParam; //ʶ���������ص��û�����

	HVAPI_CALLBACK_RECORD_PLATE pOnPlate;	//���ƻص����ϴ������ִ���ʶ����������Ϣ
	PVOID pOnPlateParam;	//���ƻص��û�����

	HVAPI_CALLBACK_RECORD_BIGIMAGE pOnBigImage;	//ʶ������ͼ�ص�
	PVOID pOnBigImageParam;	//ʶ������ͼ�ص��û�����

	HVAPI_CALLBACK_RECORD_SMALLIMAGE pOnSmallImage;	//����Сͼ�ص�
	PVOID pOnSmallImageParam;	//����Сͼ�ص��û�����

	HVAPI_CALLBACK_RECORD_BINARYIMAGE pOnBinaryImage;	//���ƶ�ֵͼ�ص�
	PVOID pOnBinaryImageParam;	//���ƶ�ֵͼ�ص��û�����

	HVAPI_CALLBACK_STRING pOnString;	//�ַ���Ϣ�ص�
	PVOID pOnStringParam;	//�ַ���Ϣ�ص��û�����

	HVAPI_CALLBACK_RADAR pOnRadar;	//�״���Ϣ�ص�
	PVOID pOnRadarParam;	   //�״���Ϣ�ص��û�����

	HVAPI_CALLBACK_TFD_STRING pOnTFDString;	//��������Ʒ�ַ���Ϣ�ص�  
	PVOID pOnTFDStringParam;	//��������Ʒ�ַ���Ϣ�ص��û�����

	HVAPI_CALLBACK_JPEG pOnJpegFrame;	//Image����ͼƬ��ص�
	PVOID pOnJpegFrameParam;	//Image����ͼƬ��ص��û�����

	HVAPI_CALLBACK_H264 pOnH264; //H264��Ƶ���ص�
	PVOID pOnH264Param;	//H264��Ƶ���ص��û�����

	HVAPI_CALLBACK_HISTORY_VIDEO pOnHistoryVideo; //��ʷ��Ƶ�ص�
	PVOID pOnHistoryVideoParam;	//��ʷ��Ƶ�ص��û�����

	HVAPI_CALLBACK_RECORD_ILLEGALVideo pOnIllegalVideo;//Υ����Ƶ�ص�
	PVOID pOnIllegalVideoParam;//Υ����Ƶ�ص��û�����

	HVAPI_CALLBACK_REDEVELOP_RESULT pOnReDevelopResult;	//���ο����ûص�  
	PVOID pOnReDevelopResultParam;	//���ο����ûص��û�����

	HVAPI_CALLBACK_RESULT  pOnRecordResult;// ʶ������չ�ص��� ������ͼ Сͼ ��λͼ �ַ���Ϣ�ص� ��ͨ������Ϣ
	HVAPI_CALLBACK_H264_EX pOnRecoreH264Video;		// H264��չ�ص��� 
	HVAPI_CALLBACK_H264_EX pOnRecoreH264HistoryVideo;//��ʷH264��չ�ص�
	HVAPI_CALLBACK_JPEG_EX pOnMJPEG;				// ͼƬ��չ�ص�

	_HVAPI_CALLBACK_SET()
		: pNext(NULL)
		, pOnPlate(NULL)
		, pOnPlateParam(NULL)
		, pOnBigImage(NULL)
		, pOnBigImageParam(NULL)
		, pOnSmallImage(NULL)
		, pOnSmallImageParam(NULL)
		, pOnBinaryImage(NULL)
		, pOnBinaryImageParam(NULL)
		, pOnString(NULL)
		, pOnStringParam(NULL)
		, pOnTFDString(NULL)
		, pOnTFDStringParam(NULL)
		, pOnJpegFrame(NULL)
		, pOnJpegFrameParam(NULL)
		, pOnH264(NULL)
		, pOnH264Param(NULL)
		, pOnHistoryVideo(NULL)
		, pOnHistoryVideoParam(NULL)
		, pOnRecordResult(NULL)
		, pOnRecoreH264Video(NULL)
		, pOnRecoreH264HistoryVideo(NULL)
		, pOnMJPEG(NULL)
		, pOnIllegalVideo(NULL)
		, pOnIllegalVideoParam(NULL)
		, pOnReDevelopResult(NULL)
		, pOnReDevelopResultParam(NULL)
		, iVideoID(0)
	{

	}

}HVAPI_CALLBACK_SET;

/* �ص���Ϣ��ʱ���� */
typedef struct _HVAPI_CALLBACK_TEMPT
{
	 // �ص���ͬ����
   DWORD dwRecvFlag;					// ���ձ�־
   INT  iVideoID;                                 // ��Ƶͨ��

	// ʶ�����ص�
	PVOID  pOnRecordResult;// ʶ������չ�ص�
	PVOID pOnRecordResultUserData;//Υ����Ƶ�ص��û�����
	DWORD dwStartIndex;					// ��ʼ֡��

   // �½ӿڹ�������
   DWORD64 dw64BeginTimeMS;	// ��ʼʱ��
   DWORD64 dw64EndTimeMS;		// ����ʱ��

   //  �ɽӿڲ���
    PVOID pOnLegacyCallback;		// �ص�����ָ��
	PVOID pOnLegacyUserData;		// �ص��û�����ָ��
	CHAR szConnCmd[512];

	// �Ƿ��� HVAPI_StartRecvMsg�ص�
	BOOL ISRecvMsgCallBack;
	// �Ƿ���HVAPI_StartRecvRedevelopResult�ص�
	BOOL ISRecvRedevelopResultCallBack;

   _HVAPI_CALLBACK_TEMPT()
	   :pOnRecordResult(NULL),
	   pOnRecordResultUserData(NULL),
	   dwStartIndex(0),
	   dwRecvFlag(0),
	   iVideoID(0),
	   dw64BeginTimeMS(0),
	   dw64EndTimeMS(0),
	   pOnLegacyCallback(NULL),
	   pOnLegacyUserData(NULL),
	   ISRecvMsgCallBack(FALSE),
	   ISRecvRedevelopResultCallBack(FALSE)
	 {
		 	ZeroMemory(szConnCmd, sizeof(szConnCmd));
		   szConnCmd[0] ='\0';
	 }
   ~_HVAPI_CALLBACK_TEMPT()
   {
	   if (NULL!=szConnCmd)
	   {

	   }
   }
}HVAPI_CALLBACK_TEMPT;

/* HVAPI ������� */
typedef struct _HVAPI_HANDLE_CONTEXT_EX
{
	DWORD	dwOpenType;
	CHAR	szVersion[8]; //�汾			
	CHAR	szIP[16];	//�豸IP
	CHAR	szDevSN[256];  //�豸���

	SOCKET	sktRecord;						//�������SOCKET
	CHAR	szRecordConnCmd[256];			//�����������
	DWORD	dwRecordConnStatus;				//�����������״̬
	BOOL	fIsRecvHistoryRecord;			//�Ƿ�Ϊ������ʷ���
	SOCKET	sktImage;						//ͼƬ����SOCKET
	CHAR	szImageConnCmd[128];			//ͼƬ��������
	DWORD	dwImageConnStatus;				//ͼƬ��������״̬
	SOCKET	sktVideo;						//��Ƶ����SOCKET
	CHAR	szVideoConnCmd[128];			//��Ƶ��������
	DWORD	dwVideoConnStatus;				//��Ƶ��������״̬
	BOOL	fIsConnectHistoryVideo;			//�Ƿ�Ϊ������ʷ¼�������
	SOCKET	sktRadar;						//�״����SOCKET
	CHAR	szRadarConnCmd[128];			//�״��������
	DWORD	dwRadarConnStatus;				//�״��������״̬

	HANDLE	hThreadRecvRecord;				//��������߳�
	BOOL	fIsThreadRecvRecordExit;		//��������߳̽������
	DWORD	dwEnhanceRedLightFlag;			//ʶ������ƼӺ��־
	INT		iBigPicBrightness;				//ʶ������ͼ����
	INT		iBigPicHueThrshold;				//ʶ������ͼ��Ʒ�ֵ
	INT		iBigPicCompressRate;			//ʶ������ͼ��ƼӺ�ͼƬѹ��Ʒ��
	HANDLE	hThreadRecvImage;				//ͼƬ�����߳�
	BOOL	fIsThreadRecvImageExit;			//ͼƬ�����߳̽������
	HANDLE	hThreadRecvVideo;				//��Ƶ�����߳�
	BOOL	fIsThreadRecvVideoExit;			//��Ƶ�����߳̽������
	HANDLE	hThreadRecvRadar;				//�״�����߳�
	BOOL	fIsThreadRecvRadarExit;			//�״�����߳̽������

	HANDLE	hThreadSocketStatusMonitor;		//����״̬����߳�
	BOOL	fIsThreadSocketStatusMonitorExit;	//����״̬����߳̽������

	HVAPI_CALLBACK_SET* pCallBackSet;		//�ص���

	DWORD	dwRecordStreamTick;				//��������̼߳�����
	DWORD	dwImageStreamTick;				//ͼƬ�����̼߳�����
	DWORD	dwVideoStreamTick;

	DWORD	dwVideoEnhanceRedLightFlag;		//��ʷ¼���ƼӺ촦��
	INT		iBrightness;					//��ʷ¼������
	INT		iHueThrshold;					//��ʷ¼����ɫ�ȷ�ֵ
	INT		iCompressRate;					//��ʷ¼���ƼӺ�ͼƬѹ��Ʒ��

	char szRecordBeginTimeStr[20];			//������ʷ�����ʼʱ��
	char szRecordEndTimeStr[20];			//������ʷ�������ʱ��
	DWORD dwRecordStartIndex;				//������ʷ�����ʼ������
	DWORD dwRecordEndIndex;					//������ʷ�����ʼ������
	DWORD dwHistoryRecordFlag;				//�Ƿ�Ϊ������ʷ�����־
	INT iTimeRefer; //�Ƿ�ITTS�洢ʱ����в�ѯ
	INT iRecordDataInfo;					//������˱�־
	INT iRealTimeRecordSendIllegalVideo;	//ʵʱ�������Υ����Ƶ��־
	INT iChannelID;                          //ͨ����
	INT iIllegalType;                        //Υ������
	char szVehicleType[14];                  //��������

	char szVideoBeginTimeStr[20];			//������ʷ��Ƶ��ʼʱ��
	char szVideoEndTimeStr[20];				//������ʷ��Ƶ����ʱ��

	DWORD dwRecordReconectTimes;			//���������������
	DWORD dwImageReconnectTimes;			//ͼ�������������
	DWORD dwVideoReconnectTimes;			//��Ƶ������������
	DWORD dwRadarReconnectTimes;			//�״������������

	RECT rcPlate[5];                        //��������
	RECT rcFacePos[5][20];                     //��������
	int nFaceCount[5];                         //������

	bool  fNewProtocol;
	PROTOCOL_VERSION emProtocolVersion;     // Э��汾��
	DWORD CropImageEnable;
	DWORD CropImageHeight;
	DWORD CropImageWeight;

	DWORD NoneImageEnable;
	DWORD NoneImageUpx;
	DWORD NoneImageUpy;


	BOOL fIsRecordPlateExit;                //����ص��رձ��
	BOOL fIsRecordStringExit;                //���״̬��Ϣ�رձ��

	//�����������
	BOOL fAutoLink;							//�������ӱ�־
	BOOL fVailPackResumeCache;				//����������� ʹ�ܱ�־
	PACK_RESUME_CACHE* pPackResumeCache;	//�����������
	CRITICAL_SECTION csCallbackJpeg;
	CRITICAL_SECTION csCallbackRecord;
	CRITICAL_SECTION csCallbackH264;
	CRITICAL_SECTION csCallbackRadar;
	DWORD64 AutoLinkSetCallBackTime;
	DWORD64 AutiLinkOverTime;
	//�û���֤���
	CHAR szUserName[128];
	CHAR szPassword[128];
	INT nAuthority;

	//20150521 ���� Ϊץ��ͼ׼��
	BYTE *pCaputureImage;
	int iCaptureImgeSize;
	bool fCapureFlag;
	CRITICAL_SECTION csLockGetImage;
	CHvDeviceAgent * pAgent;	//SDK�ع��������豸����Ĵ�����

	WORD wAddPort;	//Ϊ����OpenChannel�������ͨ���ţ����豸���������ֶ����ڼ�¼���ӵĶ˿���
	
	// �Զ������豸����׼��
	list<HVAPI_CALLBACK_TEMPT> * TempCallbackList; // ������ʱ�ص���������
	BOOL fInitFlag;                                      // �Ƿ��ʼ����
	BOOL fConnect;                                  // �Ƿ������豸״̬��
	HANDLE	hThreadReconnect;				//�Զ��������ûص������߳�
	BOOL	fIshThreadReconnectExit;		//�Զ��������ûص������߳̽������
	CRITICAL_SECTION csCallbackList;  

	_HVAPI_HANDLE_CONTEXT_EX()
		: hThreadSocketStatusMonitor(NULL)
		, hThreadRecvRecord(NULL)
		, hThreadRecvImage(NULL)
		, hThreadRecvVideo(NULL)
		, pCallBackSet(NULL)
		, fIsThreadSocketStatusMonitorExit(TRUE)
		, sktRecord(INVALID_SOCKET)
		, dwRecordConnStatus(CONN_STATUS_CONNFIRST)
		, dwRecordStreamTick(0)
		, fIsRecvHistoryRecord(FALSE)
		, sktImage(INVALID_SOCKET)
		, dwImageConnStatus(CONN_STATUS_UNKNOWN)
		, dwImageStreamTick(0)
		, sktVideo(INVALID_SOCKET)
		, dwVideoConnStatus(CONN_STATUS_UNKNOWN)
		, dwVideoStreamTick(0)
		, fIsConnectHistoryVideo(0)
		, fIsThreadRecvRecordExit(TRUE)
		, fIsThreadRecvImageExit(TRUE)
		, fIsThreadRecvVideoExit(TRUE)
		, dwEnhanceRedLightFlag(0)
		, dwVideoEnhanceRedLightFlag(0)
		, dwRecordStartIndex(0)
		, dwHistoryRecordFlag(0)
		, iRecordDataInfo(-1)
		, iRealTimeRecordSendIllegalVideo(-1)
		, dwRecordReconectTimes(0)
		, dwImageReconnectTimes(0)
		, dwVideoReconnectTimes(0)
		, dwOpenType(0)
		, fNewProtocol(false)
		, emProtocolVersion(PROTOCOL_UNKNOWN)
		, fAutoLink(FALSE)
		, pPackResumeCache(NULL)
		, fVailPackResumeCache(FALSE)
		, AutoLinkSetCallBackTime(0)
		, AutiLinkOverTime(0)
		, CropImageEnable(0)
		, CropImageHeight(600)
		, CropImageWeight(800)
		, NoneImageEnable(0)
		, NoneImageUpy(300)
		, NoneImageUpx(400)
		, fIsRecordPlateExit(TRUE)
		, fIsRecordStringExit(TRUE)
		, nAuthority(-1)
		, pAgent(NULL)
		, pCaputureImage(NULL)
		, iCaptureImgeSize(0)
		, fCapureFlag(FALSE)
		,TempCallbackList(NULL)
		,fInitFlag(FALSE)
		,fConnect(FALSE)
		,fIshThreadReconnectExit(TRUE)
		,iChannelID(0)
		,iIllegalType(1) 
		,wAddPort(0)
	{
		InitializeCriticalSection(&csCallbackRecord);
		InitializeCriticalSection(&csCallbackH264);
		InitializeCriticalSection(&csCallbackJpeg);
		InitializeCriticalSection(&csCallbackRadar);
		InitializeCriticalSection(&csLockGetImage);
		InitializeCriticalSection(&csCallbackList);
		
		ZeroMemory(szVersion, sizeof(szVersion));
		ZeroMemory(szIP, sizeof(szIP));
		ZeroMemory(rcFacePos, sizeof(rcFacePos));
		szUserName[0] = '\0';
		szPassword[0] = '\0';
		szVehicleType[0]='\0';
	}
	~_HVAPI_HANDLE_CONTEXT_EX()
	{
		if ( pCaputureImage != NULL )
		{
			delete[] pCaputureImage;
			pCaputureImage = NULL;
		}
		DeleteCriticalSection(&csCallbackRecord);
		DeleteCriticalSection(&csCallbackH264);
		DeleteCriticalSection(&csCallbackJpeg);
		DeleteCriticalSection(&csCallbackRadar);
		DeleteCriticalSection(&csLockGetImage);
	}
}HVAPI_HANDLE_CONTEXT_EX;

/* HVAPI ������� */
typedef struct _HVAPI_LISTEN_HANDLE_CONTEXT
{			
	CHAR	szIP[16];	//�豸IP
	PVOID	pUserData;
	HANDLE  hThread;
	CRITICAL_SECTION csCallback;
	HVAPI_CALLBACK_LISTEN_CMD pCallBackFun;		//�غ���
	HVAPI_CALLBACK_LISTEN_CMDEX pCallBackFunEx;		//�غ���
	DWORD		historyTrigerID;
	_HVAPI_LISTEN_HANDLE_CONTEXT()
		: hThread(NULL)
		, pCallBackFun(NULL)
		, pUserData(NULL)
		, historyTrigerID(-1)
		, pCallBackFunEx(NULL)
		
	{
		memset(szIP,0,sizeof(szIP));
		//memset(historyTrigerID,-1,sizeof(historyTrigerID));
		InitializeCriticalSection(&csCallback);
	}
	~_HVAPI_LISTEN_HANDLE_CONTEXT()
	{
		DeleteCriticalSection(&csCallback);
	}
}HVAPI_LISTEN_HANDLE_CONTEXT;


class CCSLockEx
{
public:
	CCSLockEx(CRITICAL_SECTION * pCS, bool fLock = true)
	{
		m_pCS = pCS;
		m_fLock = false;
		if (fLock)
		{
			Lock();
		}
	}
	virtual ~CCSLockEx()
	{
		Unlock();
	}
	void Lock(void)
	{
		if (!m_fLock)
		{
			m_fLock = true;
			EnterCriticalSection(m_pCS);
		}
	}
	void Unlock(void)
	{
		if (m_fLock)
		{
			LeaveCriticalSection(m_pCS);
			m_fLock = false;
		}
	}
private:
	CRITICAL_SECTION *m_pCS;
	bool m_fLock;
};

#endif
