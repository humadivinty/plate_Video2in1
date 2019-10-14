#ifndef _HV_AUTOLINK_NEW_H_
#define _HV_AUTOLINK_NEW_H_

#include "HvDeviceDLL.h"
//#include "HvDeviceNew.h"

#include "HvAutoLinkDeviceNew.h"



typedef struct _HVAPI_AL_CALLBACK_SET
{
	HVAPI_AL_CALLBACK_H264 pOnRecoreH264Video;		// H264��չ�ص��� 
	PVOID pOnH264Param;
	HVAPI_AL_CALLBACK_H264 pOnRecoreH264HistoryVideo;//��ʷH264��չ�ص�
	PVOID pOnHistroyH264Param;

	 HVAPI_AL_CALLBACK_JPEG pOnMJPEG;				// ͼƬ��չ�ص�
	PVOID pOnJpegParam;

	_HVAPI_AL_CALLBACK_SET()
	{
		pOnRecoreH264Video = NULL;
		pOnH264Param = NULL;
		pOnRecoreH264HistoryVideo = NULL;
		pOnMJPEG = NULL;
		pOnJpegParam = NULL;
	}

}_HVAPI_AL_CALLBACK_SET;


//��Ƶ����
typedef struct _HVAPI_AL_VIDEO_SERVER
{
	SOCKET sktVideo;
	HANDLE	hThreadServer;
	bool fExit;

	bool fHistoryEnable;
	DWORD64 dwBeginTime;
	DWORD64 dwEndTime;

	DWORD dwRecvType;

	char szAddr[32];
	char szDeviceNo[32]; 

	_HVAPI_AL_VIDEO_SERVER()
		:sktVideo(INVALID_SOCKET)
		,hThreadServer(NULL)
		,dwBeginTime(0)
		,dwEndTime(0)
		,fHistoryEnable(false)
		,dwRecvType(0)
	{
	}
}_HVAPI_AL_VIDEO_SERVER;



enum NOTICESTATUS{
	NONOTICE=0,    //δ֪ͨ
	ONLINE,		   //����֪ͨ
	OFFLINE,	   //����֪ͨ
};

typedef struct _HVAPI_AL_CONTEXT
{
	_HVAPI_AL_CALLBACK_SET CallBackSet;
	CRITICAL_SECTION csH264Call;
	CRITICAL_SECTION csJpegCall;

	//CRITICAL_SECTION csCallBack;

	//������·
	SOCKET	sktCmd;
	HANDLE	hThreadCmd;
	bool fCmdThExit;   //���ڱ��������߳��Ƿ����˳���ͬʱ�����豸�Ƿ����ߣ����Ϊ�棬 �����豸�����ߣ� ����������ˣ� ���Ϊ�٣� ��ʾ�豸���á�
	bool fBusySktCmd;
	CRITICAL_SECTION csSktCmd;
	DWORD dwCmdTick;

	char szAddr[32];
	char szDeviceNo[32];
	char szDeviceInfo[1024];
	int iPort;

	_HVAPI_AL_VIDEO_SERVER videoH264;
	_HVAPI_AL_VIDEO_SERVER videoJpeg;


	NOTICESTATUS eNotieType;

	DWORD dwRef;  // ���ڼ����� ����ʱ�Զ���1�� ��Ӧ�ü���Ϊ0 ʱ�� �Ҿ���Ѳ������ˣ� ��̬���Զ�ɾ���˾����   ��λ���ӵ�����֪ͨ�� �����API �����ص�������ص�ʱ�� Ӧ�ü���Ϊ0,��
	CRITICAL_SECTION csRef;

	
	_HVAPI_AL_CONTEXT()
		:sktCmd(INVALID_SOCKET)
		,hThreadCmd(NULL)
		//,pCallBackSet(NULL)
		,fCmdThExit(false)
		,fBusySktCmd(false)
		,dwCmdTick(0)
		,eNotieType(NONOTICE)
		,dwRef(0)
	{
		InitializeCriticalSection(&csSktCmd);
		InitializeCriticalSection(&csH264Call);
		InitializeCriticalSection(&csJpegCall);
		InitializeCriticalSection(&csRef);
	}


	~_HVAPI_AL_CONTEXT()
	{
		DeleteCriticalSection(&csSktCmd);
		DeleteCriticalSection(&csH264Call);
		DeleteCriticalSection(&csJpegCall);
		DeleteCriticalSection(&csRef);
	}

}_HVAPI_AL_CONTEXT;






typedef struct _tagNoticeFunc
{
	HVAPI_AL_ONLINE_NOTICE pOnOnLine;	//ʶ������ʼ�ص�
	PVOID pOnOnLineParam; //ʶ������ʼ�ص��û�����

	HVAPI_AL_OFFLINE_NOTICE pOnOffLine;	//ʶ���������ص�
	PVOID pOnOffLineParam; //ʶ���������ص��û�����

	_tagNoticeFunc()
		:pOnOnLine(NULL)
		,pOnOnLineParam(NULL)
		,pOnOffLine(NULL)
		,pOnOffLineParam(NULL)
	{
	}

}NoticeFunc;




typedef struct _tagnNetServer
{
	CHAR szVersion[8];

	_HVAPI_AL_CONTEXT **pAutoContext;
	NoticeFunc noticeFunc;

	CRITICAL_SECTION csContext;

	//�������
	INT iCmdPort;
	int iMaxCnt;
	int iLinkCnt;
	SOCKET	sktCmdSvr;
	HANDLE	hThreadCmdSvr;
	bool fCmdSvrStop;

	//���ݷ���
	INT iDatePort;
	int iMaxDateLinkCnt;
	int iDateLinkCnt;
	SOCKET	sktDateSvr;
	HANDLE	hThreadDatedSvr;
	bool fDateSvrStop;

	//�����豸���ߺ����ߵ��߳�(ͨ���ص�֪ͨ�ϲ㺯��)
	HANDLE	hNteThread; 

	_tagnNetServer()
		:sktCmdSvr(INVALID_SOCKET)
		,hThreadCmdSvr(NULL)
		,iCmdPort(0)
		,iMaxCnt(0)
		,iLinkCnt(0)
		,fCmdSvrStop(false)
		,pAutoContext(NULL)
		,iDatePort(0)
		,iMaxDateLinkCnt(0)
		,iDateLinkCnt(0)
		,sktDateSvr(INVALID_SOCKET)
		,hThreadDatedSvr(NULL)
		,fDateSvrStop(false)
		,hNteThread(NULL)
	{
		memset(szVersion, 0, 8);
		InitializeCriticalSection(&csContext);
	}
	~_tagnNetServer()
	{
		DeleteCriticalSection(&csContext);
	}


}NetServer;

#endif