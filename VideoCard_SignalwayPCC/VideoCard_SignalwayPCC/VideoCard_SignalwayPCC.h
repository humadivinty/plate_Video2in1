// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 VIDEOCARD_SIGNALWAYPCC_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// VIDEOCARD_SIGNALWAYPCC_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef VIDEOCARD_SIGNALWAYPCC_EXPORTS
#define VIDEOCARD_SIGNALWAYPCC_API __declspec(dllexport)
#else
#define VIDEOCARD_SIGNALWAYPCC_API __declspec(dllimport)
#endif

// 此类是从 VideoCard_SignalwayPCC.dll 导出的
//class VIDEOCARD_SIGNALWAYPCC_API CVideoCard_SignalwayPCC {
//public:
//	CVideoCard_SignalwayPCC(void);
//	// TODO:  在此添加您的方法。
//};
//
//extern VIDEOCARD_SIGNALWAYPCC_API int nVideoCard_SignalwayPCC;
//
//VIDEOCARD_SIGNALWAYPCC_API int fnVideoCard_SignalwayPCC(void);


#if defined(__cplusplus)  
#define D_EXTERN_C extern "C"
#else
#define D_EXTERN_C
#endif

#ifdef __OS_WIN
//Windows 环境定义
#define D_CALLTYPE __stdcall
#define D_DECL_EXPORT __declspec(dllexport)
#define D_DECL_IMPORT __declspec(dllimport)
#else //非 Windows环境定义
#define D_CALLTYPE
#define D_DECL_EXPORT __attribute__((visibility("default")))
#define D_DECL_IMPORT __attribute__((visibility("default")))
#endif

#ifdef __SHARE_EXPORT
#define D_SHARE_EXPORT D_DECL_EXPORT
#else
#define D_SHARE_EXPORT D_DECL_IMPORT
#endif

/*
视频卡接口错误码
错误码   错误描述
  0         正常
- 100   设备无响应
- 1000  传入参数错误
- 1001 设备被占用
- 1002  打开失败
*/

/*
返回值类型 返回值说明
int   >0    打开设备成功，返回值为设备句柄号
		-100         设备无响应
        -1000   传入参数错误?
        -1001   设备被占用
        -1002   设备打开失败
		-2000   其它错误
参数
出入	参数名称	类型	长度	含义

输入 nType	Int	4 	连接方式：0=板卡，1=网络连接，2=车牌识别共用
sParas	char* 4 连接信息：-串口：填“板卡”例“VideCard0” -网络：填“网址，端口，用户名，密码”
例“ 192.168.0.11,8000,admin,password”
-车牌识别共用
例“车牌识别句柄”
功能		初始化视频资源
*/
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_Init(int nType, char* sParas);


//************************************
// Method:    VC_Deinit
// Describe:    释放视频资源
// FullName:  VC_Deinit
// Access:    public 
// Returns:   int
//                    0    操作成功
//                  - 100     设备无响应
//                  - 1000  传入参数错误
 //                 - 2000  其它错误
// Qualifier:
// Parameter: int nHandle       视频卡句柄
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_Deinit(int nHandle);

#ifdef __OS_WIN
//************************************
// Method:        VC_StartDisplay
// Describe:        启动显示视频
// FullName:      VC_StartDisplay
// Access:          public 
// Returns:        int
//                        0    操作成功
//                     - 100     设备无响应
//                     - 1000  传入参数错误
//                     - 2000  其它错误
// Qualifier:      
// Parameter:    int nHandle    视频卡句柄
// Parameter:    int nWidth      视频宽度
// Parameter:    int nHeight     视频高度
// Parameter:    int nFHandle   窗体句柄
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_StartDisplay(int nHandle, int nWidth, int nHeight, int nFHandle);

#else
//************************************
// Method:        VC_StartDisplay
// Describe:       启动显示视频
// FullName:      VC_StartDisplay
// Access:          public 
// Returns:        int
// Qualifier:      
// Parameter:    int nHandle    视频卡句柄
// Parameter:    int nWidth     视频宽度
// Parameter:    int nHeight    视频高度
// Parameter:    int nTop        显示屏起始点
// Parameter:    int nLeft        显示屏起始点
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_StartDisplay(int nHandle, int nWidth, int nHeight, int nTop, int nLeft);
#endif

//************************************
// Method:        VC_StopDisplay
// Describe:       停止显示图像
// FullName:      VC_StopDisplay
// Access:          public 
// Returns:        int
//                        0    操作成功
//                     - 100     设备无响应
//                     - 1000  传入参数错误
//                     - 1005  停止显示错误
//                     - 2000  其它错误
// Qualifier:      
// Parameter:    int nHandle    (输入)视频卡句柄
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_StopDisplay(int nHandle);


//************************************
// Method:        VC_GetImage
// Describe:       获取图片
// FullName:      VC_GetImage
// Access:          public 
// Returns:        int
//                        0    操作成功
//                     - 100     设备无响应
//                     - 1000  传入参数错误
//                     - 1006  获取图片错误
//                     - 2000  其它错误
// Qualifier:      
// Parameter:    int nHandle       (输入)设备句柄
// Parameter:    int nFormat       (输入)获取图片的格式，0:bmp, 1 : jpeg,其他保留
// Parameter:    char * sImage    (输出)存放抓拍图片的缓存，由外部申请和释放
// Parameter:    int * nLength     (输入输出)输入为缓存的最大长度，输出为抓拍图片实际长度
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_GetImage(int nHandle, int nFormat, char* sImage, int* nLength);


//************************************
// Method:        VC_GetImageFile
// Describe:       获取图片保存到指定的文件
// FullName:      VC_GetImageFile
// Access:          public 
// Returns:        int
//                        0    操作成功
//                     - 100     设备无响应
//                     - 1000  传入参数错误
//                     - 1007  获取图片错误
//                     - 2000  其它错误
// Qualifier:      
// Parameter:    int nHandle            (输入)设备句柄
// Parameter:    int nFormat            (输入)获取图片的格式，0:bmp, 1 : jpeg,其他保留
// Parameter:    char * sFileName    (输出)抓拍图片文件名
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_GetImageFile(int nHandle, int nFormat, char* sFileName);


//************************************
// Method:        VLPR_TVPDisplay
// Describe:       在屏幕上叠加文字(叠加内容编码方式为 GBK )
// FullName:      VLPR_TVPDisplay
// Access:          public 
// Returns:        int
//                        0    操作成功
//                     - 100     设备无响应
//                     - 1000  传入参数错误
//                     - 2000  其它错误
// Qualifier:      
// Parameter:    int nHandle            (输入)设备句柄
// Parameter:    int nRow                (输入)行位置，从1 开始
// Parameter:    int nCol                  (输入)列位置，从1 开始
// Parameter:    char * sText            (输入)叠加内容
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VLPR_TVPDisplay(int nHandle, int nRow, int nCol, char* sText);


//************************************
// Method:        VC_TVPClear
// Describe:        清除屏幕叠加内容
// FullName:      VC_TVPClear
// Access:          public 
// Returns:        int
//                        0    操作成功
//                     - 100     设备无响应
//                     - 1000  传入参数错误
//                     - 2000  其它错误
// Qualifier:      
// Parameter:    int nHandle    (输入)设备句柄
// Parameter:    int nRow        (输入)从 (row,col) 位置开始清除length个字符；row = 0时表示清屏；col = 0时表示清除一行
// Parameter:    int nCol
// Parameter:    int nLength
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_TVPClear(int nHandle, int nRow, int nCol, int nLength = 1);


//************************************
// Method:        VC_SyncTime
// Describe:        与主机同步时间(该函数只负责同步时间，不会显示时间)
// FullName:      VC_SyncTime
// Access:          public 
// Returns:        int
//                        0    操作成功
//                     - 100     设备无响应
//                     - 1000  传入参数错误
//                     - 2000  其它错误
// Qualifier:      
// Parameter:    int nHandle            (输入)设备句柄
// Parameter:    char * sSysTime      (输入)输入时间格式：yyyyMMddHHmmss
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_SyncTime(int nHandle, char* sSysTime);


//************************************
// Method:        VC_ShowTime
// Describe:       设置时间显示格式
// FullName:      VC_ShowTime
// Access:          public 
// Returns:        int
//                        0    操作成功
//                     - 100     设备无响应
//                     - 1000  传入参数错误
//                     - 2000  其它错误
// Qualifier:      
// Parameter:    int nHandle        (输入)设备句柄
// Parameter:    int nStyle            (输入)显示格式：
//                                              0 = 不显示
//                                              1 =显示日期,格式为 "yyyy-MM-dd"
//                                              2 =显示时间,格式为 "HH : mm : ss"
//                                              3 = 显示日期和时间,格式为  "yyyy-MM-dd HH : mm : ss"
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_ShowTime(int nHandle, int nStyle);


//************************************
// Method:        VC_GetStatus
// Describe:       获取设备运行状态
// FullName:      VC_GetStatus
// Access:          public 
// Returns:        int
//                        0    操作成功
//                     - 100     设备无响应
//                     - 1000  传入参数错误
//                     - 2000  其它错误
// Qualifier:      
// Parameter:    int nHandle                (输入)设备句柄
// Parameter:    int * pStatusCode       (输出)设备状态错误码，0表示正常
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_GetStatus(int nHandle, int* pStatusCode);


//************************************
// Method:        VC_GetStatusMsg
// Describe:        查询错误码详细描述
// FullName:      VC_GetStatusMsg
// Access:          public 
// Returns:        int
//                        0    操作成功
//                     - 100     设备无响应
//                     - 1000  传入参数错误
//                     - 2000  其它错误
// Qualifier:      
// Parameter:    int nStatusCode        (输入)错误码
// Parameter:    char * sStatusMsg      (输出)错误码信息缓存地址
// Parameter:    int nStatusMsgLen      (输入)错误码信息缓存长度
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_GetStatusMsg(int nStatusCode, char* sStatusMsg, int nStatusMsgLen);


//************************************
// Method:        VC_GetDevInfo
// Describe:        获取设备厂家信息
// FullName:      VC_GetDevInfo
// Access:          public 
// Returns:        int
//                        0    操作成功
//                     - 100     设备无响应
//                     - 1000  传入参数错误
//                     - 2000  其它错误
// Qualifier:      
// Parameter:    char * sCompany        (输出)厂家信息缓存地址
// Parameter:    int nCompanyLen       (输入)厂家信息缓存长度 
// Parameter:    char * sDevMode        (输出)设备型号信息缓存地址
// Parameter:    int nModeLen             (输入)设备型号信息缓存长度
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_GetDevInfo(char* sCompany, int nCompanyLen, char* sDevMode, int nModeLen);


//************************************
// Method:        VC_GetVersion
// Describe:        获取设备及接口版本信息
// FullName:      VC_GetVersion
// Access:          public 
// Returns:        int
//                        0    操作成功
//                     - 100     设备无响应
//                     - 1000  传入参数错误
//                     - 2000  其它错误
// Qualifier:      
// Parameter:    char * sDevVersion         (输出)设备版本信息缓存地址
// Parameter:    int nDevVerLen              (输入)设备版本信息缓存长度 
// Parameter:    char * sAPIVersion         (输出)接口库版本信息缓存地址
// Parameter:    int nAPIVerLen              (输入)接口库版本信息缓存长度
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_GetVersion(char* sDevVersion, int nDevVerLen, char* sAPIVersion, int nAPIVerLen);


//************************************
// Method:        VC_TVPDisplay
// Describe:       在屏幕上叠加文字(叠加内容编码方式为 GBK )
// FullName:      VLPR_TVPDisplay
// Access:          public 
// Returns:        int
//                        0    操作成功
//                     - 100     设备无响应
//                     - 1000  传入参数错误
//                     - 2000  其它错误
// Qualifier:      
// Parameter:    int nHandle            (输入)设备句柄
// Parameter:    int nRow                (输入)行位置，从0 开始
// Parameter:    int nCol                  (输入)列位置，从1 开始
// Parameter:    char * sText            (输入)叠加内容
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_TVPDisplay(int nHandle, int nRow, int nCol, char* sText);

//************************************
// Method:        VC_GetHWVersion
// Describe:       获取设备版本信息
// FullName:      VC_GetHWVersion
// Access:          public 
// Returns:        int
//                        0    操作成功
//                     - 100     设备无响应
//                     - 1000  传入参数错误
//                     - 2000  其它错误
// Qualifier:      
// Parameter:    int nHandle  设备句柄号
// Parameter:    char * sHWVersion  硬件版本信息
// Parameter:    int nHWVerMaxLen  硬件版本信息缓存最大长度
// Parameter:    char * sAPIVersion  设备固件版本信息
// Parameter:    int nAPIVerMaxLen  设备固件版本信息缓存最大长度
//************************************
D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_GetHWVersion(int nHandle, char* sHWVersion, int nHWVerMaxLen, char* sAPIVersion, int nAPIVerMaxLen);

D_EXTERN_C D_DECL_EXPORT int D_CALLTYPE VC_GetVideoFile(int nHandle, int nFormat, int time,  char* sVideoFimeName);



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "CommonDef_VPR.h"
#ifdef    VPR_SIGNALWAY_HLJ_EXPORTS
//#define XLW_GZ_VFR_API extern "C" __declspec(dllexport)
//#define XLW_VPR_API __stdcall
#define XLW_VPR_API  __declspec(dllexport)
#else
//#define XLW_GZ_VFR_API extern "C" __declspec(dllimport)
//#define XLW_VPR_API __stdcall
#define XLW_VPR_API  __declspec(dllimport)
#endif


/*
1车牌识别控制接口
功能：获取车牌识别结果；
1.1资源初始化
函数描述
返回值	返回值类型	返回值说明
int		0		    操作成功
-100	    设备无响应
-2000	其它错误
参数	出入	参数名称	类型	长度	含义

功能	申请足够的内存空间，保证后续运作
备注
*/
XLW_VPR_API int WINAPI VLPR_Init();

/* 1.2释放资源
函数描述
返回值	返回值类型	返回值说明
int	 	0		    操作成功
-100	    设备无响应
-2000	其它错误
参数	出入	参数名称	类型	长度	含义

功能	释放内存空间
备注	 */
XLW_VPR_API int WINAPI VLPR_Deinit();

/* 1.3连接设备
函数描述
返回值	返回值类型	返回值说明
int		>0	打开设备成功，返回值为设备句柄号
-100		设备无响应
-1000	传入参数错误
-1001	设备被占用
-1002	设备打开失败
-2000	其它错误
参数	出入	参数名称	类型	长度	含义
输入	nType		int		4		连接方式：0=串口，1=网络连接
sParas		char*	N		连接信息：-串口：填“串口号”
例“COM1”
-网络：填“网址，端口，用户名，密码”
例“192.168.0.11,8000,admin,password”
功能	主机与设备建立连接
*/
XLW_VPR_API int WINAPI VLPR_Login(int nType, char* sParas);

/* 1.4断开设备连接
函数描述
返回值	返回值 类型	返回值说明
int	 	0		    关闭设备成功
-100	    设备无响应
-1000	传入参数错误
-2000	其它错误
参数	出入	参数名称	类型	长度	含义
输入	nHandle		int 	4	设备句柄

功能	主机与设备断开连接
*/
XLW_VPR_API int WINAPI VLPR_Logout(int nHandle);

/* 1.5设置识别结果回调
函数描述
参数	出入	参数名称	类型	长度	含义
输入	nHandle		int 	4	设备句柄
返回值	返回值类型	返回值说明
int	 		0		        操作成功
-100	        设备无响应
-1000	    传入参数错误
-2000	    其它错误

pFunc	CBFun_RspRegResult 	4	识别结果回调函数NULL时，取消回调
pUser	void*		4	用户自定义数据

功能	获得识别结果触发回调函数
*/

XLW_VPR_API int WINAPI VLPR_SetResultCallBack(int nHandle, CBFun_GetRegResult pFunc, void* pUser);

/* 1.6设置设备状态回调
函数描述
返回值	返回值类型	返回值说明
int	 	0	操作成功
-100	设备无响应
-1000传入参数错误
-2000其它错误
参数	出入	参数名称	类型	长度	含义
输入	nHandle		int 	4	设备句柄
nTimeInvl	    int		4	状态汇报时间间隔
pFunc		CBFun_GetDevStatus	4	异常消息回调函数func=NULL时，取消回调
pUser		void*	4	用户自定义数据
功能	设置回调，定时汇报设备运行状态或当设备发生异常时则立即回调
备注	参考CBFun_GetDevStatus
*/
XLW_VPR_API int WINAPI VLPR_SetStatusCallBack(int nHandle, int nTimeInvl, CBFun_GetDevStatus pFunc, void* pUser);

/*
1.7手动触发抓拍
返回值	返回值类型	返回值说明
Int	 	0	操作成功
-100	设备无响应
-1000传入参数错误
-2000其它错误

参数	出入	参数名称	类型	长度	含义
输入	nHandle		int 	4	设备句柄

功能	手动触发抓拍识别，在识别回调函数中返回结果
*/
XLW_VPR_API int WINAPI VLPR_ManualSnap(int nhandle);

/*
1.8获取设备状态
函数描述
返回值	返回值类型	返回值说明
int	 	0		操作成功
-100	设备无响应
-1000	传入参数错误
-2000	其它错误

参数	出入	参数名称	        类型	长度	含义
输入	nHandle		    int		4		设备句柄
输出	pStatusCode	    int*	    4		设备状态错误码，0表示正常

版本功能	获取设备运行状态
*/
/*
1.12车牌识别接口错误码
错误码	错误描述
0		    正常
-100	    设备无响应
-1000	传入参数错误
-1001	设备被占用
-1002	打开失败
-2000	其他错误
-2000以上	预留
*/

XLW_VPR_API int WINAPI VLPR_GetStatus(int nHandle, int* pStatusCode);

/*
1.9获取错误码详细描述
函数描述
返回值	返回值类型	返回值说明
Int	 	0	操作成功
-100	设备无响应
-1000传入参数错误
-2000其它错误
参数	出入	参数名称		类型	长度	含义
输入	nStatusCode		 Int		4		错误码
输出	sStatusMsg		 char*	N		错误码信息缓存地址
输入	nStatusMsgLen	 Int		4		错误码信息缓存长度
功能	查询错误码详细描述
*/

XLW_VPR_API int WINAPI VLPR_GetStatusMsg(int nStatusCode, char* sStatusMsg, int nStatusMsgLen);

/*
1.10获取设备版本信息
函数描述
返回值	返回值类型	返回值说明
int	 	0	            操作成功
-100	        设备无响应
-1000       传入参数错误
-2000       其它错误
参数	出入	参数名称	类型	长度	含义
输出	sVersion	char*	N	版本信息缓存地址
输入	nVerLen		int 	4	版本信息缓存长度
功能	获取顶棚牌版本信息
*/

XLW_VPR_API int WINAPI VLPR_GetVersion(char* sVersion, int nVerLen);


/*
函数描述 :获取设备版本信息 获取设备的硬件版本和固件版本
参数:
nHandle 设备句柄号
sHWVersion  硬件版本信息
nHWVerMaxLen 硬件版本信息缓存最大长度
sAPIVersion 设备固件版本信息
nAPIVerMaxLen 设备固件版本信息缓存最大长度

返回值 返回值类型 返回值说明
>0 打开设备成功，返回值为设备句柄号
-100 设备无响应
-1000 传入参数错误
-1001 设备被占用
-1002 设备打开失败
-2000 其它错误
*/
XLW_VPR_API int WINAPI VLPR_GetHWVersion(int nHandle, char *sHWVersion, int nHWVerMaxLen, char *sAPIVersion, int nAPIVerMaxLen);