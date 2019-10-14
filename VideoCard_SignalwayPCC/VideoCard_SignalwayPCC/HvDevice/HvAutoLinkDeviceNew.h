#ifndef _HV_ALLINK_DEVICE_NEW_H_
#define _HV_ALLINK_DEVICE_NEW_H_

#include "HvDeviceBaseType.h"
#include "HvDeviceCommDef.h"

#ifdef HVDEVICE_EXPORTS
#define HV_API_AL extern "C" __declspec(dllexport)
#elif HVDEVICE_LIB
#define HV_API_EX
#else
#define HV_API_AL extern "C" __declspec(dllimport)
#endif


//��·����
#define AL_LINK_TYPE_JPEG_VIDEO  0xffff0001   /*JPEG ��Ƶ����·*/
#define AL_LINK_TYPE_H264_VIDEO   0xffff0002  /*H264 ��Ƶ����·*/

//��·״̬
#define AL_LINK_STATE_UNKNOWN         0xffff0400	/**< δ֪ */
#define AL_LINK_STATE_NORMAL  0xffff0401  /*����*/
#define AL_LINK_STATE_DISCONN 0xffff0402  /*�Ͽ�*/
#define AL_LINK_STATE_RECONN  0xffff0403  /*����*/


typedef PVOID HVAPI_OPERATE_HANDLE;

//�ص������Ķ���

/**
* @brief               ��Ƶ�ص�����
* @param[out]          pUserData          ��HVAPI_StartRecvH264Video���õĴ���ص��������û�����ָ��
* @param[out]          dwVedioFlag        ��Ƶ��� ����Ч���ݱ�ǡ���Ч���ݱ�ǡ���ʷ�������
* @param[out]          dwVideoType        ��Ƶ���� ����ʷ��Ƶ��ʵʱ��Ƶ
* @param[out]          dwWidth            ���
* @param[out]          dwHeight			  �߶�
* @param[out]          dw64TimeMS         ��Ƶʱ��
* @param[out]          pbVideoData        ��Ƶ֡����
* @param[out]          dwVideoDataLen     ��Ƶ֡����
* @param[out]          szVideoExtInfo     ��Ƶ������Ϣ
* @return              �ɹ�S_OK ʧ�� E_FAILE
*/
typedef INT (CDECL*  HVAPI_AL_CALLBACK_H264)(
												PVOID pUserData,  
												DWORD dwVedioFlag,
												DWORD dwVideoType, 
												DWORD dwWidth,
												DWORD dwHeight,
												DWORD64 dw64TimeMS,
												PBYTE pbVideoData, 
												DWORD dwVideoDataLen,
												LPCSTR szVideoExtInfo
												);

/**
* @brief               jpeg���ص�����
* @param[out]          pUserData          ��HVAPI_StartRecvH264Video���õĴ���ص��������û�����ָ��
* @param[out]          dwImageFlag        ͼƬ��� ����ЧͼƬ���ݡ���ЧͼƬ����
* @param[out]          dwImageType        ͼƬ���� ��������������������
* @param[out]          dwWidth            ���
* @param[out]          dwHeight			  �߶�
* @param[out]          dw64TimeMS         JPEGʱ��
* @param[out]          pbImageData        ͼƬ֡����
* @param[out]          dwImageDataLen     ͼƬ֡����
* @param[out]          szImageExtInfo     JPEG������Ϣ
* @return              �ɹ�S_OK ʧ�� E_FAILE
*/

typedef INT (CDECL*  HVAPI_AL_CALLBACK_JPEG)(
											PVOID pUserData,  
											DWORD dwImageFlag,
											DWORD dwImageType, 
											DWORD dwWidth,
											DWORD dwHeight,
											DWORD64 dw64TimeMS,
											PBYTE pbImageData, 
											DWORD dwImageDataLen,
											LPCSTR szImageExtInfo
											);






/**
@Brief	  ��ʼ������ͳ����������ģ��    
@Return:  �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT CDECL HVAPI_AL_InitActiveModule();


/**
@Brief	  ж�ؿͻ�ͳ����������ģ��   
@Return:  �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT CDECL HVAPI_AL_UnInitActiveModule();

// �ͻ�ͳ�����������豸���߻ص�
typedef INT (CDECL* HVAPI_AL_ONLINE_NOTICE)(PVOID pUserData, HVAPI_OPERATE_HANDLE handle, char*szDeviceNo, char*szAddr,int iPort,char *szOtherXmlInfo);

// �ͻ�ͳ�����������豸���߻ص�
typedef INT (CDECL* HVAPI_AL_OFFLINE_NOTICE)(PVOID pUserData, HVAPI_OPERATE_HANDLE handle, char* szDeviceNo);


/**
@Brief	  �Ͽ��豸����   
@Param[in]:   handle	��Ч���豸���Ӿ��
@Return:  �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_DisConnect(HVAPI_OPERATE_HANDLE handle);


/**
@Brief	  �����豸����������ô���   
@Param[in]:   handle		��Ч���豸�������
@Return:  �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_AddRef(HVAPI_OPERATE_HANDLE handle);


/**
@Brief	  �����豸����������ô���   
@Param[in]:   handle	��Ч���豸�������
@Return: �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_ReleaseRef(HVAPI_OPERATE_HANDLE handle);



/**
@Brief	  ע���豸���߻ص�   
@Param[in]:   onLineNotice	�ص�������ַ
@Param[in]:   pUserData		�ص���������
@Return:  �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_RegDevOnLineNotice(HVAPI_AL_ONLINE_NOTICE onLineNotice, void* pUserData);



/**
@Brief	  ע���豸���߻ص�   
@Param[in]:   offLineNotice		�ص�������ַ
@Param[in]:   pUserData			�ص���������
@Return:  �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_RegDevOffLineNotice(HVAPI_AL_OFFLINE_NOTICE offLineNotice, void* pUserData);




/**
@Brief	  �����������Ӽ���   
@Param:   int iPort			Ҫ�����Ķ˿�
@Param:   int iMaxCount		��������
@Return:   �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_OpenDevCtrlServer(int iPort, int iMaxCount);


/**
@Brief	  ֹͣ�������Ӽ���   
@Return:  �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_CloseDevCtrlServer();



/**
@Brief	  ��ȡ�豸������Ϣ   
@Param[in]:			handle				�豸�������
@Param[in/out]:		LPSTR szParamDoc	��Ż�ȡ�����豸������Ϣ������
@Param[in]:			INT nBufLen			szParamDoc�������ĳ���
@Param[in/out]:     INT * pnRetLen		ʵ�ʻ�ȡ�����豸������Ϣ����
@Return:  �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_GetDevConfigInfo(HVAPI_OPERATE_HANDLE handle, LPSTR szParamDoc, INT nBufLen, INT* pnRetLen);



/**
@Brief	  �ϴ��豸������Ϣ   
@Param[in]:   handle			�豸�������
@Param[in]:   szParamDoc		Ҫ�ϴ����豸������Ϣ
@Return:  �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_UploadDevConfigInfo(HVAPI_OPERATE_HANDLE handle, LPCSTR szParamDoc);


/**
@Brief	  ִ������   
@Param[in]:		  handle		�豸���
@Param[in]:		  szCmd			Ҫִ�е�����
@Param[in/out]:   szRetBuf		�洢����ִ�н��������
@Param[in]:		  nBufLen		szRetBuf�������ĳ���
@Param[in/out]:   pnRetLen		ʵ��szRetBuf����
@Return:  �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT CDECL HVAPI_AL_ExecCmdOnDev(HVAPI_OPERATE_HANDLE handle,LPCSTR szCmd,LPSTR szRetBuf,INT nBufLen,INT* pnRetLen);

/**
* @brief              ȡ���˿���������(���ֻ��ȡһ��������)
* @param[in]          hHandle          �豸���
* @param[in]          dw64StartTime    ��Ӧ��ȡ��������Ϣ����ʼʱ��,Ϊ��1970-01-01 00:00:00��ʼ��Ŀǰ�������ĺ�����
* @param[in]          dw64EndTime      ��Ӧ��ȡ��������Ϣ�Ľ���ʱ��,Ϊ��1970-01-01 00:00:00��ʼ��Ŀǰ�������ĺ�����
* @param[out]         szRetInfo		   ��Ӧʹ����������ڴ����򣬺������óɹ����������Ϣ�����ڴ�
* @param[in,out]      iLen		       szRetInfo���������ȣ��������غ�Ϊ�ַ���ʵ�ʳ���
* @return             �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT CDECL HVAPI_AL_GetPCSHumanTrafficInfo(HVAPI_OPERATE_HANDLE hHandle, DWORD64 dw64StartTime, DWORD64 dw64EndTime, CHAR* szRetInfo, INT* iLen);


/**
@Brief	  ץ��ͼ��   
@Param[in]:		  hHandle			�豸���
@Param[in]:       nTimeM			nTimeM=0��ʾ��ǰͼƬ�� nTimeMΪǰnTimeM�����н�������һ��ͼƬ�� �����˿�������Ч��������Ч��
@Param[in/out]:   pImageBuffLen		�洢ץ��ͼ������ 
@Param[in]:		  iBuffLen			pImageBuffLen���������� 
@Param[in/out]:   iImageLen			ʵ��ץ��ͼ��С
@Param[in/out]:   dwTime			ץ��ͼʱ��
@Param[in/out]:   dwImageWidth		ץ��ͼ���
@Param[in/out]:   dwImageHeigh		ץ��ͼ�߶�
@Return:  �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT CDECL HVAPI_AL_GetCaptureImage(HVAPI_OPERATE_HANDLE hHandle,int nTimeM, BYTE *pImageBuffLen, int iBuffLen, int *iImageLen, DWORD64 *dwTime,  DWORD *dwImageWidth, DWORD *dwImageHeigh);


/**
* @brief              ����H264����
* @param[in]          hHandle				�豸���
* @param[in]          pFunc				    �ص�����ָ��
* @param[in]          pUserData			    �û�����
* @param[in]          iVideoID				��Ƶͨ����Ŀǰֻʹ��0
* @param[in]          dw64BeginTimeMS       ��ʼʱ��
* @param[in]          dw64EndTimeMS         ����ʱ�� 
* @param[in]          dwRecvFlag            ���ձ�־ :ʵʱ��Ƶ����ʷ��Ƶ
* @return             �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT CDECL HVAPI_AL_StartRecvH264Video(
	HVAPI_OPERATE_HANDLE hHandle,
	PVOID pFunc,
	PVOID pUserData,
	INT iVideoID ,       
	DWORD64 dw64BeginTimeMS,
	DWORD64 dw64EndTimeMS,
	DWORD dwRecvFlag
);
/**
* @brief              ֹͣH264����
* @param[in]          hHandle          �豸���
* @return             �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT CDECL  HVAPI_AL_StopRecvH264Video(HVAPI_OPERATE_HANDLE hHandle);



/**
* @brief              ����MJPEG����
* @param[in]          hHandle				 �豸���
* @param[in]          pFunc					 �ص�����ָ��
* @param[in]          pUserData				 �û�����
* @param[in]          iVideoID				 ��Ƶͨ����Ŀǰֻʹ��0
* @param[in]          dwRecvFlag			 ���ձ�־��������������������
* @return             �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/

HV_API_AL HRESULT CDECL HVAPI_AL_StartRecvMJPEG(
					 HVAPI_OPERATE_HANDLE hHandle,
					 PVOID pFunc,
					 PVOID pUserData,
					 INT iVideoID ,
					 DWORD dwRecvFlag
					 );
/**
* @brief              ֹͣMJPEG����
* @param[in]          hHandle          �豸���
* @return             �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT CDECL  HVAPI_AL_StopRecvMJPEG(HVAPI_OPERATE_HANDLE hHandle);


/**
@Brief	  ��ȡ�豸��·״̬   
@Param[in]:		  handle		�豸�������
@Param[in]:		  dwLinkType	��·���ͣ��磺AL_LINK_TYPE_JPEG_VIDEO��JPEG��Ƶ����·�� ��AL_LINK_TYPE_H264_VIDEO��H264��Ƶ����·��  
@Param[in/oug]:   dwStatus		�洢��ȡ������·״̬
@Return:  �ɹ���S_OK��ʧ�ܣ�E_FAIL
*/
HV_API_AL HRESULT CDECL HVAPI_AL_GetDevLinkStatus(HVAPI_OPERATE_HANDLE handle, DWORD dwLinkType, DWORD *dwStatus);


#endif