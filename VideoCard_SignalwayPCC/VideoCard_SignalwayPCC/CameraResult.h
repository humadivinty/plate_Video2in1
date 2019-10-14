#ifndef CAMERRESULT
#define CAMERRESULT

/* 识别结果大图类型定义 */
#define RECORD_BIGIMG_BEST_SNAPSHOT			0x0001	/**< 最清晰识别图 */
#define RECORD_BIGIMG_LAST_SNAPSHOT			0x0002	/**< 最后识别图 */
#define RECORD_BIGIMG_BEGIN_CAPTURE			0x0003	/**< 开始抓拍图 */
#define RECORD_BIGIMG_BEST_CAPTURE			0x0004	/**< 最清晰抓拍图 */
#define RECORD_BIGIMG_LAST_CAPTURE			0x0005	/**<  最后抓拍图 */

#define  MAX_IMG_SIZE 10*1024*1024



struct CameraInfo
{
    char chIP[20];                          //deviceIP
    char chDeviceID[3];
    char chStationID[7];
    char chLaneID[3];
    bool bLogEnable;
    bool bSynTimeEnable;
    bool bIsInit;
    int iDirection;
    CameraInfo()
    {
        memset(chIP, 0, 20);
        memset(chDeviceID, 0, 3);
        memset(chStationID, 0, 7);
        memset(chLaneID, 0, 3);
        bLogEnable = true;
        bSynTimeEnable = false;
        bIsInit = false;
        iDirection = 0;
    }
};

struct SaveModeInfo
{
    int iSafeModeEnable;
    char chBeginTime[260];
    char chEndTime[260];
    int iIndex;
    int iDataType;
    SaveModeInfo()
    {
        iSafeModeEnable = 0;
        iIndex = 0;
        iDataType = 0;
        memset(chBeginTime, 0, 260);
        memset(chEndTime, 0, 260);
    }
};

struct DeviceTime
{
    int iYear;
    int iMonth;
    int iDay;
    int iHour;
    int iMinutes;
    int iSecond;
    int iMilisecond;

    DeviceTime()
    {
        iYear = 0;
        iMonth = 0;
        iDay = 0;
        iHour = 0;
        iMinutes = 0;
        iSecond = 0;
        iMilisecond = 0;
    }
};

typedef struct _BasicInfo
{
    char szIP[64];              //设备IP，适用于所有设备类型
    char szMask[64];            //设备子网掩码，适用于所有设备类型
    char szGateway[64];         //设备网关，适用于所有设备类型
    char szMac[128];            //设备物理地址，适用于所有设备类型
    char szModelVersion[128]; //设备模型版本，此字段保留
    char szSN[128];             //设备编号，适用于所有设备类型
    char szWorkMode[128];       //设备工作模式，仅适用于PCC200、PCC600、PCC200A
    char szDevType[128];        //设备类型，仅适用于PCC200、PCC600、PCC200A
    char szDevVersion[128];  //设备版本，仅适用于PCC200、PCC600、PCC200A
    char szMode[128];           //设备运行模式，如正常模式，适用于所有设备类型
    char szRemark[128];    //保留字段，仅适用于PCC200、PCC600
    char szBackupVersion[128];  //备份版本，仅适用于PCC200、PCC600 
    char szFPGAVersion[128]; //FPGA版本，仅适用于PCC200、PCC600
    char szKernelVersion[128];  //Kernel版本，仅适用于PCC200、PCC600
    char szUbootVersion[128]; //Uboot版本，仅适用于PCC200、PCC600
    char szUBLVersion[128];//UBL版本，仅适用于PCC200、PCC600
}BasicInfo;


class CameraIMG
{
public:
	CameraIMG();
	CameraIMG(const CameraIMG& CaIMG);
	~CameraIMG();
	unsigned long wImgWidth;
	unsigned long wImgHeight;
	DWORD dwImgSize;
	unsigned long  wImgType;
	char chSavePath[256];
	PBYTE pbImgData;

	CameraIMG& operator = (const CameraIMG& CaIMG);
};

class CameraResult
{
public:

	CameraResult();
	CameraResult(const CameraResult& CaRESULT);
	~CameraResult();

	DWORD dwCarID;
	int iDeviceID;
	int iPlateColor;
	int iPlateTypeNo;
	DWORD64 dw64TimeMS;
	int iSpeed;
	int iResultNo;
	int iVehTypeNo;		//车型代码: 客1--1 。。。客4--4， 货1--5  。。。货4--8
	int iVehBodyColorNo;	
	int iVehBodyDeepNo;	
	int iAreaNo;
	int iRoadNo;
	int iLaneNo;
	int iDirection;
	int iWheelCount;		//轮数
	long iAxletreeCount;		//轴数
	int iAxletreeGroupCount;//轴组数
	int iAxletreeType;		//轴型
    int iReliability;       //可信度
	float fVehLenth;			//车长
	float fDistanceBetweenAxles;		//轴距
	float fVehHeight;		//车高
	bool bBackUpVeh;		//是否倒车


	char chDeviceIp[32];
	char chPlateNO[32];
	char chPlateColor[10];
	char chListNo[256];
	char chPlateTime[256];
	char chSignStationID[50];
	char chSignStationName[50];
	char pcAppendInfo[2048];

	char chDeviceID[3];
	char chLaneID[3];

	CameraIMG CIMG_BestSnapshot;	/**< 最清晰识别图 */
	CameraIMG CIMG_LastSnapshot;	/**< 最后识别图 */
	CameraIMG CIMG_BeginCapture;	/**< 开始抓拍图 */
	CameraIMG CIMG_BestCapture;		/**< 最清晰抓拍图 */
	CameraIMG CIMG_LastCapture;		/**< 最后抓拍图 */
	CameraIMG CIMG_PlateImage;		/**< 车牌小图 */
	CameraIMG CIMG_BinImage;			/**< 车牌二值图 */

	CameraResult& operator = (const CameraResult& CaRESULT);

	friend bool SerializationResultToDisk(const char* chFilePath, const CameraResult& CaResult);
	friend bool SerializationFromDisk(const char* chFilePath, CameraResult& CaResult);

	friend bool SerializationAsConfigToDisk(const char* chFilePath, const CameraResult& CaResult);
	friend bool SerializationAsConfigFromDisk(const char* chFilePath, CameraResult& CaResult);
};

typedef struct _tagSafeModeInfo
{
	int iEableSafeMode;
	char chBeginTime[256];
	char chEndTime[256];
	int index;
	int DataInfo;
	_tagSafeModeInfo()
	{
		iEableSafeMode = 0;
		memset(chBeginTime, 0, sizeof(chBeginTime));
		memset(chEndTime, 0, sizeof(chEndTime));
		index = 0;
		DataInfo = 0;
	}
}_tagSafeModeInfo;

#endif