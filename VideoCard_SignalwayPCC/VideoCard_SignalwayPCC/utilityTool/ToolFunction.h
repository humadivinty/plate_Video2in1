#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"
#include <string>
#include <list>
#include <map>
#include<windef.h>
//#define MY_SPRINTF sprintf_s

#define SAFE_DELETE_OBJ(obj) \
if (NULL != obj)                                  \
{                                           \
    delete obj;                        \
    obj = NULL;                      \
}

#define SAFE_DELETE_ARRAY(arrayObj) \
if (NULL != arrayObj)                                  \
{                                           \
    delete[] arrayObj;                        \
    arrayObj = NULL;                      \
}

#define USE_GID_PLUS 1
#define  USE_MFC

//ͨ���ڵ������Ҳ�������Ӧ�ڵ�
//ע��XMLTYPE Ϊ1ʱ��InputInfoΪXML·������Ϊ2ʱ,InputInfoΪ�������ļ�����
TiXmlElement Tool_SelectElementByName(const char* InputInfo, const char* pName, int iXMLType);

TiXmlElement* Tool_ReadElememt(TiXmlElement* InputElement, const char* pName);

bool Tool_GetElementTextByName(const char* InputInfo, const char* pName, int iXMLType, char* chTextValueBuffer, size_t& bufferLength);

bool Tool_InsertElementByName(const char* InputInfo, const char* pName, int iXMLType, const char* nodeName, const char* textValue, std::string& outputString);

void Tool_ReadKeyValueFromConfigFile(const char* FileName, const char* nodeName, const char* keyName, char* keyValue, size_t bufferSize);

void Tool_ReadIntValueFromConfigFile(const char* FileName, const char* nodeName, const char* keyName, int&keyValue);

void Tool_WriteKeyValueFromConfigFile(const char* FileName, const char* nodeName, const char* keyName, char* keyValue, size_t bufferSize);

//���IP����Ч��
int Tool_checkIP(const char* p);

bool Tool_IsFileExist(const char* FilePath);

bool Tool_IsDirExist(const char* Dir);

bool Tool_MakeDir(const char* chImgPath);

long Tool_GetFileSize(const char *FileName);

bool Tool_PingIPaddress(const char* IpAddress);
#ifdef USE_GID_PLUS
bool Tool_Img_ScaleJpg(PBYTE pbSrc, int iSrcLen, PBYTE pbDst, size_t *iDstLen, int iDstWidth, int iDstHeight, int compressQuality);
#endif

#ifdef USE_GID_PLUS
int Tool_GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
#endif
void Tool_ExcuteShellCMD(char* pChCommand);

bool Tool_ExcuteCMDbyCreateProcess(const char* CmdName);

#ifdef USE_GID_PLUS
bool Tool_OverlayStringToImg(unsigned char** pImgsrc, long srcSize,
    unsigned char** pImgDest, long& DestSize,
    const wchar_t* DestString, int FontSize,
    int x, int y, int colorR, int colorG, int colorB,
    int compressQuality);
#endif

bool Tool_GetDataFromAppenedInfo(char *pszAppendInfo, std::string strItemName, char *pszRstBuf, int *piRstBufLen);

void Tool_ExcuteCMD(char* pChCommand);

std::wstring Tool_string2wstring(std::string strSrc);

#ifdef  USE_MFC
//ɾ��ָ���ļ���
bool DeleteDirectory(const char* strDirName);

//ɾ��ָ��Ŀ¼���� 'yyyy-mm-dd' ��ʽ�������ļ��У������޶�������Ϊʱ������ָ������
int CirclelaryDelete(const char* folderPath, int iBackUpDays);
#endif


int Tool_SafeCloseThread(HANDLE& threadHandle);

const char* Tool_GetCurrentPath();
#ifdef USE_MSVC
SYSTEMTIME Tool_GetCurrentTime();
#endif

bool Tool_DimCompare(const char *szSrcPlateNo, const char *szDesPlateNo);

void Tool_WriteLog(const char*);

void Tool_WriteFormatLog(const char* szfmt, ...);

bool Tool_SaveFileToPath(const char* szPath, void* fileData, size_t fileSize);

std::list<std::string> getFilesPath(const std::string& cate_dir, const std::string& filter);

bool Tool_LoadFile(const char* fileName, void* pBuffer, size_t& fileSize);

bool Tool_LoadCamerXml(const char* fileName, std::map<std::string, int>& myMap);

void Tool_DeleteFileByCMD(const char* chFileName);

bool Tool_FindMapAndGetValue(std::map<std::string, int>& myMap, std::string keyName, int& value);

#ifdef WIN32
std::string Tool_GetSoftVersion(const char* exepath);

//************************************
// Method:        Tool_pingIp_win
// Describe:
// FullName:      Tool_pingIp_win
// Access:          public 
// Returns:        int  0 success, 1 failed
// Returns Describe:
// Parameter:    const char * ipAddress
//************************************
int Tool_pingIp_win(const char* ipAddress);
#endif
int Tool_AnalysisPlateColorNo(const char *szPlateNo);

bool Tool_ProcessPlateNo(const char* pSrcPlateNum, char* destPlateNum, int bufLen);