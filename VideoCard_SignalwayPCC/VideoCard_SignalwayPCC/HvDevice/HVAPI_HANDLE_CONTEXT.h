#ifndef _HVAPI_HANDLE_CONTEXT_
#define _HVAPI_HANDLE_CONTEXT_

#include "HvDeviceDLL.h"

/* HVAPI������� */
typedef struct _HVAPI_HANDLE_CONTEXT
{
	DWORD	dwOpenType;
	CHAR szVersion[8];
	CHAR szIP[16];
	CHAR szDevSN[256];  //�豸���

	SOCKET sktImage;
	LPSTR szImageConnCmd[128];
	DWORD dwImageConnStatus;
	SOCKET sktVideo;
	LPSTR szVideoConnCmd[128];
	DWORD dwVideoConnStatus;
	SOCKET sktRecord;
	LPSTR szRecordConnCmd[128];
	DWORD dwRecordConnStatus;

	HANDLE hThreadRecvImage;
	BOOL fThreadImageExit;
	HANDLE hThreadRecvVideo;
	BOOL fThreadVideoExit;
	HANDLE hThreadRecvRecord;
	BOOL fThreadRecordExit;

	HANDLE hThreadSocketStatusMonitor;
	BOOL fThreadSocketStatusMonitorExit;

	HVAPI_CALLBACK_IMAGE pFuncCallbackJpeg;
	PVOID pCallbackJpegUserData;
	CRITICAL_SECTION csCallbackJpeg;
	HVAPI_CALLBACK_VIDEO pFuncCallbackH264;
	PVOID pCallbackH264UserData;
	CRITICAL_SECTION csCallbackH264;
	HVAPI_CALLBACK_RECORD pFuncCallbackRecord;
	PVOID pCallbackRecordUserData;
	CRITICAL_SECTION csCallbackRecord;
	HVAPI_CALLBACK_GATHER_INFO pFuncCallbackGatherInfo;
	PVOID pCallbackGatherInfoData;
	CRITICAL_SECTION csCallbackGatherInfo;

	DWORD dwJpegStreamRecvThreadLastTick;
	DWORD dwH264StreamRecvThreadLastTick;
	DWORD dwRecordStreamRecvThreadLastTick;

	bool  fNewProtocol;
	//�����������
	BOOL fAutoLink;								//�������ӱ�־
	BOOL fVailPackResumeCache;				//����������� ʹ�ܱ�־
	PACK_RESUME_CACHE* pPackResumeCache;	//�����������

	WORD wAddPort;	//Ϊ����OpenChannel�������ͨ���ţ����豸���������ֶ����ڼ�¼���ӵĶ˿���


	_HVAPI_HANDLE_CONTEXT()
		: sktImage(INVALID_SOCKET)
		, dwImageConnStatus(CONN_STATUS_UNKNOWN)
		, sktVideo(INVALID_SOCKET)
		, dwVideoConnStatus(CONN_STATUS_UNKNOWN)
		, sktRecord(INVALID_SOCKET)
		, dwRecordConnStatus(CONN_STATUS_UNKNOWN)
		, hThreadRecvImage(NULL)
		, fThreadImageExit(TRUE)
		, hThreadRecvVideo(NULL)
		, fThreadVideoExit(TRUE)
		, hThreadRecvRecord(NULL)
		, fThreadRecordExit(TRUE)
		, hThreadSocketStatusMonitor(NULL)
		, fThreadSocketStatusMonitorExit(TRUE)
		, pFuncCallbackJpeg(NULL)
		, pCallbackJpegUserData(NULL)
		, pFuncCallbackH264(NULL)
		, pCallbackH264UserData(NULL)
		, pFuncCallbackRecord(NULL)
		, pCallbackRecordUserData(NULL)
		, pFuncCallbackGatherInfo(NULL)
		, pCallbackGatherInfoData(NULL)
		, fNewProtocol(false)
		, dwOpenType(0)
		, fAutoLink(FALSE)
		, pPackResumeCache(NULL)
		, fVailPackResumeCache(FALSE)
		, wAddPort(0)
	{
		InitializeCriticalSection(&csCallbackRecord);
		InitializeCriticalSection(&csCallbackH264);
		InitializeCriticalSection(&csCallbackJpeg);
		InitializeCriticalSection(&csCallbackGatherInfo);

		ZeroMemory(szVersion, sizeof(szVersion));
		ZeroMemory(szIP, sizeof(szIP));
		ZeroMemory(szImageConnCmd, sizeof(szImageConnCmd));
		ZeroMemory(szVideoConnCmd, sizeof(szVideoConnCmd));
		ZeroMemory(szRecordConnCmd, sizeof(szRecordConnCmd));
		dwJpegStreamRecvThreadLastTick = 0;
		dwH264StreamRecvThreadLastTick = 0;
		dwRecordStreamRecvThreadLastTick = 0;		
	};

	~_HVAPI_HANDLE_CONTEXT()
	{
		DeleteCriticalSection(&csCallbackRecord);
		DeleteCriticalSection(&csCallbackH264);
		DeleteCriticalSection(&csCallbackJpeg);
		DeleteCriticalSection(&csCallbackGatherInfo);
	}

} HVAPI_HANDLE_CONTEXT;

class CCSLock
{
public:
	CCSLock(CRITICAL_SECTION * pCS, bool fLock = true)
	{
		m_pCS = pCS;
		m_fLock = false;
		if (fLock)
		{
			Lock();
		}
	}
	virtual ~CCSLock()
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

#endif // _HVAPI_HANDLE_CONTEXT_
