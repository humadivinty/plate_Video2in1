#ifndef COMMONDEF_VPR_H
#define COMMONDEF_VPR_H

/*
1.11类型定义
*/
typedef struct _vlp_info
{
	int vlpInfoSize;				//识别结构体大小
	char vlpTime[20];          //识别时间，格式“yyyyMMddHHmmsszzz”
	int vlpCarClass;				//车型
	unsigned char vlpColor[2];	//车牌颜色(数字编码)
	//“00”蓝色，“01”黄色，“02”黑色，“03”白色
	unsigned char vlpText[16];	//车牌文字，GBK编码
	unsigned int vlpReliability; //识别车牌可信度(采用四位表示9999表示为99.99%)
	unsigned int imageLength[3];	//识别图片长度:[0]=场景图长度，[1]=车牌图长度，[2]=二值化图长度
	unsigned char* image[3];		//识别图片：[0]=场景图，[1]=车牌图，[2]=二值化图
}T_VLPINFO;


/* 设备状态回调定义CBFun_GetDevStatus
返回值	返回值类型	返回值说明
int	--
参数	出入	参数名称	类型	长度	含义
输入	nHandle		int 	4	设备句柄
nStatus		int		4	错误码,0表示正常
pUser		void*	4	用户自定义数据
功能	解析设备状态信息
*/
//typedef void(WINAPI *CBFun_GetDevStatus)(int nHandle, int nStatus, void* pUser);
typedef void( *CBFun_GetDevStatus)(int nHandle, int nStatus, void* pUser);

/* 识别结果回调定义CBFun_GetRegResult
函数描述
返回值	返回值类型	返回值说明
void	--
参数	出入	参数名称	类型		长度	含义
输入	nHandle		int 		4		设备句柄
pVlpResult	T_VLPINFO*	4		识别结果结构体
pUser		void*		4		用户自定义数据
功能	解析抓拍识别结果
*/
//typedef void(WINAPI *CBFun_GetRegResult)(int nHandle, T_VLPINFO* pVlpResult, void *pUser);
typedef void( *CBFun_GetRegResult)(int nHandle, T_VLPINFO* pVlpResult, void *pUser);

#endif