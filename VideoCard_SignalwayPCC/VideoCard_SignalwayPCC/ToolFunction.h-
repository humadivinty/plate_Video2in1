#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"
#include <string>

//#define MY_SPRINTF sprintf_s


//通过节点名查找并返回相应节点
//注：XMLTYPE 为1时，InputInfo为XML路径，当为2时,InputInfo为二进制文件内容
TiXmlElement SelectElementByName(const char* InputInfo, char* pName, int iXMLType);

TiXmlElement* ReadElememt(TiXmlElement* InputElement, char* pName);

void Tool_ReadKeyValueFromConfigFile(const char* FileName, const char* nodeName, const char* keyName, char* keyValue, int bufferSize);

void g_WriteKeyValueFromConfigFile(const char* FileName, const char* nodeName, const char* keyName, char* keyValue, int bufferSize);

//检查IP的有效性
int g_checkIP(const char* p);

bool Tool_IsFileExist(const char* FilePath);

bool Tool_MakeDir(const char* chImgPath);

long Tool_GetFileSize(const char *FileName);

bool PingIPaddress(const char* IpAddress);

bool Tool_Img_ScaleJpg(PBYTE pbSrc, int iSrcLen, PBYTE pbDst, DWORD *iDstLen, int iDstWidth, int iDstHeight, int compressQuality);

int Tool_GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

void Tool_ExcuteShellCMD(char* pChCommand);

bool Tool_OverlayStringToImg(unsigned char** pImgsrc, long srcSize,
    unsigned char** pImgDest, long& DestSize,
    const wchar_t* DestString, int FontSize,
    int x, int y, int colorR, int colorG, int colorB,
    int compressQuality);

bool GetDataFromAppenedInfo(char *pszAppendInfo, std::string strItemName, char *pszRstBuf, int *piRstBufLen);

void ExcuteCMD(char* pChCommand);

std::wstring Img_string2wstring(std::string strSrc);

//删除指定文件夹
bool DeleteDirectory(char* strDirName);

//删除指定目录中以 'yyyy-mm-dd' 方式命名的文件夹，其中限定的条件为时间早于指定天数
int CirclelaryDelete(char* folderPath, int iBackUpDays);

bool Tool_SaveFileToDisk(char* chImgPath, void* pImgData, DWORD dwImgSize);

#ifdef WIN32
std::string GetSoftVersion(const char* exepath);
#endif