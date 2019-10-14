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


//链路类型
#define AL_LINK_TYPE_JPEG_VIDEO  0xffff0001   /*JPEG 视频流链路*/
#define AL_LINK_TYPE_H264_VIDEO   0xffff0002  /*H264 视频流链路*/

//链路状态
#define AL_LINK_STATE_UNKNOWN         0xffff0400	/**< 未知 */
#define AL_LINK_STATE_NORMAL  0xffff0401  /*连接*/
#define AL_LINK_STATE_DISCONN 0xffff0402  /*断开*/
#define AL_LINK_STATE_RECONN  0xffff0403  /*重连*/


typedef PVOID HVAPI_OPERATE_HANDLE;

//回调函数的定义

/**
* @brief               视频回调函数
* @param[out]          pUserData          由HVAPI_StartRecvH264Video设置的传入回调函数的用户数据指针
* @param[out]          dwVedioFlag        视频标记 ：有效数据标记、无效数据标记、历史结束标记
* @param[out]          dwVideoType        视频类型 ：历史视频、实时视频
* @param[out]          dwWidth            宽度
* @param[out]          dwHeight			  高度
* @param[out]          dw64TimeMS         视频时间
* @param[out]          pbVideoData        视频帧数据
* @param[out]          dwVideoDataLen     视频帧长度
* @param[out]          szVideoExtInfo     视频附加信息
* @return              成功S_OK 失败 E_FAILE
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
* @brief               jpeg流回调函数
* @param[out]          pUserData          由HVAPI_StartRecvH264Video设置的传入回调函数的用户数据指针
* @param[out]          dwImageFlag        图片标记 ：有效图片数据、无效图片数据
* @param[out]          dwImageType        图片类型 ：调试码流、正常码流
* @param[out]          dwWidth            宽度
* @param[out]          dwHeight			  高度
* @param[out]          dw64TimeMS         JPEG时间
* @param[out]          pbImageData        图片帧数据
* @param[out]          dwImageDataLen     图片帧长度
* @param[out]          szImageExtInfo     JPEG附加信息
* @return              成功S_OK 失败 E_FAILE
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
@Brief	  初始化客流统计主动连接模块    
@Return:  成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT CDECL HVAPI_AL_InitActiveModule();


/**
@Brief	  卸载客户统计主动连接模块   
@Return:  成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT CDECL HVAPI_AL_UnInitActiveModule();

// 客户统计主动连接设备上线回调
typedef INT (CDECL* HVAPI_AL_ONLINE_NOTICE)(PVOID pUserData, HVAPI_OPERATE_HANDLE handle, char*szDeviceNo, char*szAddr,int iPort,char *szOtherXmlInfo);

// 客户统计主动连接设备下线回调
typedef INT (CDECL* HVAPI_AL_OFFLINE_NOTICE)(PVOID pUserData, HVAPI_OPERATE_HANDLE handle, char* szDeviceNo);


/**
@Brief	  断开设备连接   
@Param[in]:   handle	有效的设备连接句柄
@Return:  成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_DisConnect(HVAPI_OPERATE_HANDLE handle);


/**
@Brief	  增加设备操作句柄引用次数   
@Param[in]:   handle		有效的设备操作句柄
@Return:  成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_AddRef(HVAPI_OPERATE_HANDLE handle);


/**
@Brief	  减少设备操作句柄引用次数   
@Param[in]:   handle	有效的设备操作句柄
@Return: 成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_ReleaseRef(HVAPI_OPERATE_HANDLE handle);



/**
@Brief	  注册设备上线回调   
@Param[in]:   onLineNotice	回调函数地址
@Param[in]:   pUserData		回调函数参数
@Return:  成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_RegDevOnLineNotice(HVAPI_AL_ONLINE_NOTICE onLineNotice, void* pUserData);



/**
@Brief	  注册设备下线回调   
@Param[in]:   offLineNotice		回调函数地址
@Param[in]:   pUserData			回调函数参数
@Return:  成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_RegDevOffLineNotice(HVAPI_AL_OFFLINE_NOTICE offLineNotice, void* pUserData);




/**
@Brief	  启动主动连接监听   
@Param:   int iPort			要监听的端口
@Param:   int iMaxCount		最大监听数
@Return:   成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_OpenDevCtrlServer(int iPort, int iMaxCount);


/**
@Brief	  停止主动连接监听   
@Return:  成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_CloseDevCtrlServer();



/**
@Brief	  获取设备配置信息   
@Param[in]:			handle				设备操作句柄
@Param[in/out]:		LPSTR szParamDoc	存放获取到的设备配置信息缓冲区
@Param[in]:			INT nBufLen			szParamDoc缓冲区的长度
@Param[in/out]:     INT * pnRetLen		实际获取到的设备配置信息长度
@Return:  成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_GetDevConfigInfo(HVAPI_OPERATE_HANDLE handle, LPSTR szParamDoc, INT nBufLen, INT* pnRetLen);



/**
@Brief	  上传设备配置信息   
@Param[in]:   handle			设备操作句柄
@Param[in]:   szParamDoc		要上传的设备配需信息
@Return:  成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT HVAPI_AL_UploadDevConfigInfo(HVAPI_OPERATE_HANDLE handle, LPCSTR szParamDoc);


/**
@Brief	  执行命令   
@Param[in]:		  handle		设备句柄
@Param[in]:		  szCmd			要执行的命令
@Param[in/out]:   szRetBuf		存储命令执行结果缓冲区
@Param[in]:		  nBufLen		szRetBuf缓冲区的长度
@Param[in/out]:   pnRetLen		实际szRetBuf长度
@Return:  成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT CDECL HVAPI_AL_ExecCmdOnDev(HVAPI_OPERATE_HANDLE handle,LPCSTR szCmd,LPSTR szRetBuf,INT nBufLen,INT* pnRetLen);

/**
* @brief              取行人卡口人流量(最大只能取一个月数据)
* @param[in]          hHandle          设备句柄
* @param[in]          dw64StartTime    对应获取客流量信息的起始时间,为从1970-01-01 00:00:00开始至目前所经过的毫秒数
* @param[in]          dw64EndTime      对应获取客流量信息的结束时间,为从1970-01-01 00:00:00开始至目前所经过的毫秒数
* @param[out]         szRetInfo		   对应使用者申请的内存区域，函数调用成功后客流量信息更新于此
* @param[in,out]      iLen		       szRetInfo缓冲区长度，函数返回后为字符串实际长度
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT CDECL HVAPI_AL_GetPCSHumanTrafficInfo(HVAPI_OPERATE_HANDLE hHandle, DWORD64 dw64StartTime, DWORD64 dw64EndTime, CHAR* szRetInfo, INT* iLen);


/**
@Brief	  抓拍图像   
@Param[in]:		  hHandle			设备句柄
@Param[in]:       nTimeM			nTimeM=0表示当前图片， nTimeM为前nTimeM分钟有结果的最后一张图片， 在行人卡口中有效，其他无效果
@Param[in/out]:   pImageBuffLen		存储抓拍图缓冲区 
@Param[in]:		  iBuffLen			pImageBuffLen缓冲区长度 
@Param[in/out]:   iImageLen			实际抓拍图大小
@Param[in/out]:   dwTime			抓拍图时间
@Param[in/out]:   dwImageWidth		抓拍图宽度
@Param[in/out]:   dwImageHeigh		抓拍图高度
@Return:  成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT CDECL HVAPI_AL_GetCaptureImage(HVAPI_OPERATE_HANDLE hHandle,int nTimeM, BYTE *pImageBuffLen, int iBuffLen, int *iImageLen, DWORD64 *dwTime,  DWORD *dwImageWidth, DWORD *dwImageHeigh);


/**
* @brief              启动H264接收
* @param[in]          hHandle				设备句柄
* @param[in]          pFunc				    回调函数指针
* @param[in]          pUserData			    用户数据
* @param[in]          iVideoID				视频通道，目前只使用0
* @param[in]          dw64BeginTimeMS       开始时间
* @param[in]          dw64EndTimeMS         结束时间 
* @param[in]          dwRecvFlag            接收标志 :实时视频、历史视频
* @return             成功：S_OK；失败：E_FAIL
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
* @brief              停止H264接收
* @param[in]          hHandle          设备句柄
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT CDECL  HVAPI_AL_StopRecvH264Video(HVAPI_OPERATE_HANDLE hHandle);



/**
* @brief              启动MJPEG接收
* @param[in]          hHandle				 设备句柄
* @param[in]          pFunc					 回调函数指针
* @param[in]          pUserData				 用户数据
* @param[in]          iVideoID				 视频通道，目前只使用0
* @param[in]          dwRecvFlag			 接收标志：调试码流、正常码流
* @return             成功：S_OK；失败：E_FAIL
*/

HV_API_AL HRESULT CDECL HVAPI_AL_StartRecvMJPEG(
					 HVAPI_OPERATE_HANDLE hHandle,
					 PVOID pFunc,
					 PVOID pUserData,
					 INT iVideoID ,
					 DWORD dwRecvFlag
					 );
/**
* @brief              停止MJPEG接收
* @param[in]          hHandle          设备句柄
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT CDECL  HVAPI_AL_StopRecvMJPEG(HVAPI_OPERATE_HANDLE hHandle);


/**
@Brief	  获取设备链路状态   
@Param[in]:		  handle		设备操作句柄
@Param[in]:		  dwLinkType	链路类型，如：AL_LINK_TYPE_JPEG_VIDEO（JPEG视频流链路） 、AL_LINK_TYPE_H264_VIDEO（H264视频流链路）  
@Param[in/oug]:   dwStatus		存储获取到的链路状态
@Return:  成功：S_OK；失败：E_FAIL
*/
HV_API_AL HRESULT CDECL HVAPI_AL_GetDevLinkStatus(HVAPI_OPERATE_HANDLE handle, DWORD dwLinkType, DWORD *dwStatus);


#endif