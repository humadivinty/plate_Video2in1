#include "stdafx.h"
#include "CameraResult.h"

CameraIMG& CameraIMG::operator=( const CameraIMG& CaIMG )
{
	if (this != &CaIMG)
	{
		wImgWidth = CaIMG.wImgWidth;
		wImgHeight = CaIMG.wImgHeight;
		dwImgSize = CaIMG.dwImgSize;
		wImgType = CaIMG.wImgType;
		for (unsigned int i = 0; i< sizeof(chSavePath); i++)
		{
			chSavePath[i] = CaIMG.chSavePath[i];
		}
		if (NULL != CaIMG.pbImgData)
		{
			pbImgData = new BYTE[CaIMG.dwImgSize];
			for (DWORD j = 0; j< dwImgSize; j++)
			{
				pbImgData[j] = CaIMG.pbImgData[j];
			}
		}
		else
		{
			pbImgData = NULL;
		}
	}
	return *this;
}

CameraIMG::CameraIMG()
{
	wImgWidth = 0;
	wImgHeight = 0;
	pbImgData = NULL;
	dwImgSize = 0;
	for (unsigned int i = 0; i< sizeof(chSavePath); i++)
	{
		chSavePath[i] = 0;
	}
}

CameraIMG::CameraIMG( const CameraIMG& CaIMG )
{
	wImgWidth = CaIMG.wImgWidth;
	wImgHeight = CaIMG.wImgHeight;
	dwImgSize = CaIMG.dwImgSize;
	wImgType = CaIMG.wImgType;
	for (unsigned int i = 0; i< sizeof(chSavePath); i++)
	{
		chSavePath[i] = CaIMG.chSavePath[i];
	}
	if (NULL != CaIMG.pbImgData)
	{
		pbImgData = new BYTE[CaIMG.dwImgSize];
		for (DWORD j = 0; j< dwImgSize; j++)
		{
			pbImgData[j] = CaIMG.pbImgData[j];
		}
	}
	else
	{
		pbImgData = NULL;
	}
}

CameraIMG::~CameraIMG()
{
	wImgWidth = 0;
	wImgHeight = 0;
	if (NULL != pbImgData)
	{
		delete[] pbImgData;
		pbImgData = NULL;
	}
	dwImgSize = 0;
	memset(chSavePath, 0, sizeof(chSavePath));
}
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

CameraResult::CameraResult()
{
	dwCarID = 0;
	iDeviceID = 0;
	iPlateColor = 0;
	iPlateTypeNo = 0;
	dw64TimeMS = 0;
	iSpeed = 0;
	iResultNo = 0;
	iVehTypeNo = 0;		//车型代码: 客1--1 。。。客4--4， 货1--5  。。。货4--8
	iVehBodyColorNo = 0;	
	iVehBodyDeepNo = 0;	
	iAreaNo = 0;
	iRoadNo = 0;
	iLaneNo = 0;
	iDirection = 0;
	iWheelCount = 0;		//轮数
	iAxletreeCount = 0;		//轴数
	iAxletreeType = 0;		//轴型
	iAxletreeGroupCount = 2;
    iReliability = 0;
	fVehLenth = 0;			//车长
	fDistanceBetweenAxles = 0.0;		//轴距
	fVehHeight = 0.0;		//车高
	bBackUpVeh = false;		//是否倒车

	//pcAppendInfo = NULL;
	for (unsigned int i = 0; i < sizeof(pcAppendInfo); i++)
	{
		pcAppendInfo[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chDeviceIp); i++)
	{
		chDeviceIp[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chPlateNO); i++)
	{
		chPlateNO[i] = 0;
	}
	for (unsigned int i = 0; i < sizeof(chPlateColor); i++)
	{
		chPlateColor[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chListNo); i++)
	{
		chListNo[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chPlateTime); i++)
	{
		chPlateTime[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chSignStationID); i++)
	{
		chSignStationID[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chSignStationName); i++)
	{
		chSignStationName[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chLaneID); i++)
	{
		chLaneID[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chDeviceID); i++)
	{
		chDeviceID[i] = 0;
	}
}

CameraResult::CameraResult( const CameraResult& CaRESULT )
{
	dwCarID = CaRESULT.dwCarID;
	dw64TimeMS = CaRESULT.dw64TimeMS;
	iLaneNo = CaRESULT.iLaneNo;
	iPlateColor = CaRESULT.iPlateColor;
	iVehTypeNo = CaRESULT.iVehTypeNo;
	iDirection = CaRESULT.iDirection;
	iAreaNo = CaRESULT.iAreaNo;
	iVehBodyColorNo = CaRESULT.iVehBodyColorNo;
	iPlateTypeNo = CaRESULT.iPlateTypeNo;
	iVehBodyDeepNo = CaRESULT.iVehBodyDeepNo;
	iDeviceID = CaRESULT.iDeviceID;
	iRoadNo = CaRESULT.iRoadNo;
	iSpeed = CaRESULT.iSpeed;
	iAxletreeType = CaRESULT.iAxletreeType;
	iAxletreeCount = CaRESULT.iAxletreeCount;
	iAxletreeGroupCount = CaRESULT.iAxletreeGroupCount;
	iResultNo = CaRESULT.iResultNo;
	iWheelCount = CaRESULT.iWheelCount;
    iReliability = CaRESULT.iReliability;
	fVehHeight = CaRESULT.fVehHeight;
	fVehLenth = CaRESULT.fVehLenth;
	fDistanceBetweenAxles = CaRESULT.fDistanceBetweenAxles;
	bBackUpVeh = CaRESULT.bBackUpVeh;

	//pcAppendInfo = NULL;
	for (unsigned int i = 0; i < sizeof(pcAppendInfo); i++)
	{
		pcAppendInfo[i] = CaRESULT.pcAppendInfo[i];
	}

	for (unsigned int i = 0; i < sizeof(chDeviceIp); i++)
	{
		chDeviceIp[i] = CaRESULT.chDeviceIp[i];
	}

	for (unsigned int i = 0; i < sizeof(chPlateNO); i++)
	{
		chPlateNO[i] = CaRESULT.chPlateNO[i];
	}

	for (unsigned int i = 0; i < sizeof(chPlateColor); i++)
	{
		chPlateColor[i] = CaRESULT.chPlateColor[i];
	}

	for (unsigned int i = 0; i < sizeof(chListNo); i++)
	{
		chListNo[i] = CaRESULT.chListNo[i];
	}

	for (unsigned int i = 0; i < sizeof(chPlateTime); i++)
	{
		chPlateTime[i] = CaRESULT.chPlateTime[i];
	}

	for (unsigned int i = 0; i < sizeof(chSignStationID); i++)
	{
		chSignStationID[i] = CaRESULT.chSignStationID[i];
	}

	for (unsigned int i = 0; i < sizeof(chSignStationName); i++)
	{
		chSignStationName[i] = CaRESULT.chSignStationName[i];
	}

	for (unsigned int i = 0; i < sizeof(chLaneID); i++)
	{
		chLaneID[i] = CaRESULT.chLaneID[i];
	}

	for (unsigned int i = 0; i < sizeof(chDeviceID); i++)
	{
		chDeviceID[i] = CaRESULT.chDeviceID[i];
	}

	CIMG_BestCapture = CaRESULT.CIMG_BestCapture;
	CIMG_BeginCapture = CaRESULT.CIMG_BeginCapture;
	CIMG_LastCapture = CaRESULT.CIMG_LastCapture;
	CIMG_BestSnapshot = CaRESULT.CIMG_BestSnapshot;
	CIMG_LastSnapshot = CaRESULT.CIMG_LastSnapshot;
	CIMG_PlateImage = CaRESULT.CIMG_PlateImage;
	CIMG_BinImage = CaRESULT.CIMG_BinImage;
}

CameraResult::~CameraResult()
{
	dwCarID = 0;
	iDeviceID = 0;
	iPlateColor = 0;
	iPlateTypeNo = 0;
	dw64TimeMS = 0;
	iSpeed = 0;
	iResultNo = 0;
	iVehTypeNo = 0;		//车型代码: 客1--1 。。。客4--4， 货1--5  。。。货4--8
	iVehBodyColorNo = 0;	
	iVehBodyDeepNo = 0;	
	iAreaNo = 0;
	iRoadNo = 0;
	iLaneNo = 0;
	iDirection = 0;
	iWheelCount = 0;		//轮数
	iAxletreeCount = 0;		//轴数
	iAxletreeGroupCount = 0;
	iAxletreeType = 0;		//轴型
    iReliability = 0;
	fVehLenth = 0;			//车长
	fDistanceBetweenAxles = 0.0;		//轴距
	fVehHeight = 0.0;		//车高
	bBackUpVeh = false;		//是否倒车

	memset(chDeviceIp, 0, sizeof(chDeviceIp));
	memset(chPlateNO, 0, sizeof(chPlateNO));
	memset(chPlateColor, 0, sizeof(chPlateColor));
	memset(chListNo, 0, sizeof(chListNo));
	memset(chPlateTime, 0, sizeof(chPlateTime));
	memset(chSignStationID, 0, sizeof(chSignStationID));
	memset(chSignStationName, 0, sizeof(chSignStationName));
	memset(pcAppendInfo, 0, sizeof(pcAppendInfo));
	memset(chLaneID, 0, sizeof(chLaneID));
	memset(chDeviceID, 0, sizeof(chDeviceID));
}

CameraResult& CameraResult::operator=( const CameraResult& CaRESULT )
{
	dwCarID = CaRESULT.dwCarID;
	dw64TimeMS = CaRESULT.dw64TimeMS;
	iLaneNo = CaRESULT.iLaneNo;
	iPlateColor = CaRESULT.iPlateColor;
	iVehTypeNo = CaRESULT.iVehTypeNo;
	iDirection = CaRESULT.iDirection;
	iAreaNo = CaRESULT.iAreaNo;
	iVehBodyColorNo = CaRESULT.iVehBodyColorNo;
	iPlateTypeNo = CaRESULT.iPlateTypeNo;
	iVehBodyDeepNo = CaRESULT.iVehBodyDeepNo;
	iDeviceID = CaRESULT.iDeviceID;
	iRoadNo = CaRESULT.iRoadNo;
	iSpeed = CaRESULT.iSpeed;
	iAxletreeType = CaRESULT.iAxletreeType;
	iAxletreeCount = CaRESULT.iAxletreeCount;
	iAxletreeGroupCount = CaRESULT.iAxletreeGroupCount;

	iResultNo = CaRESULT.iResultNo;
	iWheelCount = CaRESULT.iWheelCount;
    iReliability = CaRESULT.iReliability;
	fVehHeight = CaRESULT.fVehHeight;
	fVehLenth = CaRESULT.fVehLenth;
	fDistanceBetweenAxles = CaRESULT.fDistanceBetweenAxles;
	bBackUpVeh = CaRESULT.bBackUpVeh;

	//pcAppendInfo = NULL;

	for(unsigned int i =0; i < sizeof(pcAppendInfo); i++)
	{
		pcAppendInfo[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chDeviceIp); i++)
	{
		chDeviceIp[i] = CaRESULT.chDeviceIp[i];
	}

	for (unsigned int i = 0; i < sizeof(chPlateNO); i++)
	{
		chPlateNO[i] = CaRESULT.chPlateNO[i];
	}

	for (unsigned int i = 0; i < sizeof(chPlateColor); i++)
	{
		chPlateColor[i] = CaRESULT.chPlateColor[i];
	}

	for (unsigned int i = 0; i < sizeof(chListNo); i++)
	{
		chListNo[i] = CaRESULT.chListNo[i];
	}

	for (unsigned int i = 0; i< sizeof(chPlateTime); i++ )
	{
		chPlateTime[i] = CaRESULT.chPlateTime[i];
	}

	for (unsigned int i = 0; i < sizeof(chSignStationID); i++)
	{
		chSignStationID[i] = CaRESULT.chSignStationID[i];
	}

	for (unsigned int i = 0; i < sizeof(chSignStationName); i++)
	{
		chSignStationName[i] = CaRESULT.chSignStationName[i];
	}

	for (unsigned int i = 0; i < sizeof(chLaneID); i++)
	{
		chLaneID[i] = CaRESULT.chLaneID[i];
	}

	for (unsigned int i = 0; i < sizeof(chDeviceID); i++)
	{
		chDeviceID[i] = CaRESULT.chDeviceID[i];
	}

	CIMG_BestCapture = CaRESULT.CIMG_BestCapture;
	CIMG_BeginCapture = CaRESULT.CIMG_BeginCapture;
	CIMG_LastCapture = CaRESULT.CIMG_LastCapture;
	CIMG_BestSnapshot = CaRESULT.CIMG_BestSnapshot;
	CIMG_LastSnapshot = CaRESULT.CIMG_LastSnapshot;
	CIMG_PlateImage = CaRESULT.CIMG_PlateImage;
	CIMG_BinImage = CaRESULT.CIMG_BinImage;

	return *this;
}
