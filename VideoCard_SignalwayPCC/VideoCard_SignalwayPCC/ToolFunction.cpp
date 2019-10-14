#include "stdafx.h"
#include "ToolFunction.h"
#include<shellapi.h>
//#include <afx.h>

#include <gdiplus.h>
using namespace Gdiplus;
#pragma  comment(lib, "gdiplus.lib")

#ifdef WIN32
#include <io.h>
#pragma comment(lib, "version.lib")  
#endif

TiXmlElement SelectElementByName(const char* InputInfo, char* pName, int iXMLType)
{
	//注：XMLTYPE 为1时，InputInfo为XML路径，当为2时,InputInfo为二进制文件内容
	TiXmlDocument cXmlDoc;
	TiXmlElement* pRootElement = NULL;
	if (iXMLType == 1)
	{
		if (!cXmlDoc.LoadFile(InputInfo))
		{
			printf("parse XML file failed \n");
			return TiXmlElement("");
		}
	}
	else if (iXMLType == 2)
	{
		if (!cXmlDoc.Parse(InputInfo))
		{
			printf("parse XML failed \n");
			return TiXmlElement("");
		}
	}

	pRootElement = cXmlDoc.RootElement();
	if (NULL == pRootElement)
	{
		printf("no have root Element\n");
		return TiXmlElement("");
	}
	else
	{
		TiXmlElement* pTempElement = NULL;
		pTempElement = ReadElememt(pRootElement, pName);
		if (pTempElement)
		{
			printf("find the Name : %s, Text = %s\n", pTempElement->Value(), pTempElement->GetText());
			return *pTempElement;
		}
		else
		{
			return TiXmlElement("");
		}		
	}
}

TiXmlElement* ReadElememt(TiXmlElement* InputElement, char* pName)
{
	TiXmlElement* ptemp = NULL;
	if (InputElement && 0 == strcmp(pName, InputElement->Value()))
	{
		printf("Find the element :%s \n", InputElement->Value());
		ptemp = InputElement;
		return ptemp;
	}
	else
	{
		printf("%s \n", InputElement->Value());
	}

	TiXmlElement* tmpElement = InputElement;
	if (tmpElement->FirstChildElement())
	{
		ptemp = ReadElememt(tmpElement->FirstChildElement(), pName);
	}
	if (!ptemp)
	{
		tmpElement = tmpElement->NextSiblingElement();
		if (tmpElement)
		{
			ptemp = ReadElememt(tmpElement, pName);
		}
	}
	return ptemp;
}

void Tool_ReadKeyValueFromConfigFile(const char* IniFileName, const char* nodeName, const char* keyName, char* keyValue, int bufferSize)
{
	if (strlen(keyValue) > bufferSize)
	{
		return;
	}
	char FileName[MAX_PATH];
	GetModuleFileNameA(NULL, FileName, MAX_PATH - 1);

	PathRemoveFileSpecA(FileName);
    char iniFileName[MAX_PATH] = { 0 };
	//char iniDeviceInfoName[MAX_PATH] = { 0 };
	MY_SPRINTF(iniFileName, sizeof(iniFileName), "%s\\%s", FileName, IniFileName);

	GetPrivateProfileStringA(nodeName, keyName, "0", keyValue, bufferSize, iniFileName);

	WritePrivateProfileStringA(nodeName, keyName, keyValue, iniFileName);
}

void g_WriteKeyValueFromConfigFile(const char* INIFileName, const char* nodeName, const char* keyName, char* keyValue, int bufferSize)
{
	if (strlen(keyValue) > bufferSize)
	{
		return;
	}
	char FileName[MAX_PATH];
	GetModuleFileNameA(NULL, FileName, MAX_PATH - 1);

	PathRemoveFileSpecA(FileName);
	char iniFileName[MAX_PATH] = { 0 };
	char iniDeviceInfoName[MAX_PATH] = { 0 };
	strcat_s(iniFileName, FileName);
	strcat_s(iniFileName, INIFileName);


	//GetPrivateProfileStringA(nodeName, keyName, "172.18.109.97", keyValue, bufferSize, iniFileName);

	WritePrivateProfileStringA(nodeName, keyName, keyValue, iniFileName);
}

int g_checkIP(const char* p)
{
	int n[4];
	char c[4];
	//if (sscanf(p, "%d%c%d%c%d%c%d%c",
	//	&n[0], &c[0], &n[1], &c[1],
	//	&n[2], &c[2], &n[3], &c[3])
	//	== 7)
	if (sscanf_s(p, "%d%c%d%c%d%c%d%c",
		&n[0], &c[0], 1,
		&n[1], &c[1], 1,
		&n[2], &c[2], 1,
		&n[3], &c[3], 1)
		== 7)
	{
		int i;
		for (i = 0; i < 3; ++i)
		if (c[i] != '.')
			return 0;
		for (i = 0; i < 4; ++i)
		if (n[i] > 255 || n[i] < 0)
			return 0;
        if (n[0] == 0 && n[1] == 0 && n[2] == 0 && n[3] == 0)
		{
            return 0;
		}
		return 1;
	}
	else
		return 0;
}

bool Tool_IsFileExist(const char* FilePath)
{
	if (FilePath == NULL)
	{
		return false;
	}
	FILE* tempFile = NULL;
	bool bRet = false;
	//tempFile = fopen(FilePath, "r");
	fopen_s(&tempFile, FilePath, "r");
	if (tempFile)
	{
		bRet = true;
		fclose(tempFile);
		tempFile = NULL;
	}
	return bRet;
}

bool Tool_MakeDir(const char* chImgPath)
{
    if (NULL == chImgPath)
    {
        //WriteLog("the path is null ,Create Dir failed.");
        return false;
    }
    std::string tempFile(chImgPath);
    size_t iPosition = tempFile.rfind("\\");
    std::string tempDir = tempFile.substr(0, iPosition + 1);
    if (MakeSureDirectoryPathExists(tempDir.c_str()))
    {
        return true;
    }
    else
    {
        //WriteLog("Create Dir failed.");
        return false;
    }
}

long Tool_GetFileSize(const char *FileName)
{
	//FILE* tmpFile = fopen(FileName, "rb");
	FILE* tmpFile = NULL;
	fopen_s(&tmpFile, FileName, "rb");
	if (tmpFile)
	{
		fseek(tmpFile, 0, SEEK_END);
		long fileSize = ftell(tmpFile);
		fclose(tmpFile);
		tmpFile = NULL;
		return fileSize;
	}
	else
	{
		//"open file failed.";
		return 0;
	}
}

bool PingIPaddress(const char* IpAddress)
{
	//FILE* pfile;
	//char chBuffer[1024] = {0};
	char chCMD[256] = { 0 };
	sprintf_s(chCMD, "ping %s -n 1", IpAddress);
	//std::string strPingResult;
	//pfile = _popen(chCMD, "r");
	//if (pfile != NULL)
	//{
	//	while(fgets(chBuffer, 1024, pfile) != NULL)
	//	{
	//		strPingResult.append(chBuffer);
	//	}
	//}
	//else
	//{
	//	printf("popen failed. \n");
	//	return false;
	//}
	//_pclose(pfile);
	//printf("%s", strPingResult.c_str());
	//if (std::string::npos != strPingResult.find("TTL") || std::string::npos != strPingResult.find("ttl"))
	//{
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}


	char pbuf[1024]; // 缓存  
	DWORD len;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	HANDLE hRead1 = NULL, hWrite1 = NULL;  // 管道读写句柄  
	BOOL b;
	SECURITY_ATTRIBUTES saAttr;

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE; // 管道句柄是可被继承的  
	saAttr.lpSecurityDescriptor = NULL;

	// 创建匿名管道，管道句柄是可被继承的  
	b = CreatePipe(&hRead1, &hWrite1, &saAttr, 1024);
	if (!b)
	{
		//MessageBox(hwnd, "管道创建失败。","Information",0);  
		printf("管道创建失败\n");
		return false;
	}

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdOutput = hWrite1; // 设置需要传递到子进程的管道写句柄  


	// 创建子进程，运行ping命令，子进程是可继承的  
	if (!CreateProcess(NULL, chCMD, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
	{
		//itoa(GetLastError(), pbuf, 10); 
		sprintf_s(pbuf, "%d", GetLastError());
		//MessageBox(hwnd, pbuf,"Information",0);
		printf("%s\n", pbuf);
		CloseHandle(hRead1);
		hRead1 = NULL;
		CloseHandle(hWrite1);
		hWrite1 = NULL;
		return false;
	}

	// 写端句柄已被继承，本地则可关闭，不然读管道时将被阻塞  
	CloseHandle(hWrite1);
	hWrite1 = NULL;

	// 读管道内容，并用消息框显示  
	len = 1000;
	DWORD l;

	std::string strInfo;
	while (ReadFile(hRead1, pbuf, len, &l, NULL))
	{
		if (l == 0) break;
		pbuf[l] = '\0';
		//MessageBox(hwnd, pbuf, "Information",0);  
		//printf("Information2:\n%s\n", pbuf);
		strInfo.append(pbuf);
		len = 1000;
	}

	//MessageBox(hwnd, "ReadFile Exit","Information",0);  
	printf("finish ReadFile buffer = %s\n", strInfo.c_str());
	CloseHandle(hRead1);
	hRead1 = NULL;

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hThread);
	pi.hThread = NULL;
	CloseHandle(pi.hProcess);
	pi.hProcess = NULL;

	if (std::string::npos != strInfo.find("TTL") || std::string::npos != strInfo.find("ttl"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Tool_Img_ScaleJpg(PBYTE pbSrc, int iSrcLen, PBYTE pbDst, DWORD *iDstLen, int iDstWidth, int iDstHeight, int compressQuality)
{
    if (pbSrc == NULL || iSrcLen <= 0)
    {
        return false;
    }
    if (pbDst == NULL || iDstLen == NULL || *iDstLen <= 0)
    {
        return false;
    }
    if (iDstWidth <= 0 || iDstHeight <= 0)
    {
        return false;
    }

    // init gdi+
    ULONG_PTR gdiplusToken = NULL;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // 创建流
    IStream *pstmp = NULL;
    CreateStreamOnHGlobal(NULL, TRUE, &pstmp);
    if (pstmp == NULL)
    {
        GdiplusShutdown(gdiplusToken);
        gdiplusToken = NULL;
        return false;
    }

    // 初始化流
    LARGE_INTEGER liTemp = { 0 };
    ULARGE_INTEGER uLiZero = { 0 };
    pstmp->Seek(liTemp, STREAM_SEEK_SET, NULL);
    pstmp->SetSize(uLiZero);

    // 将图像放入流中
    ULONG ulRealSize = 0;
    pstmp->Write(pbSrc, iSrcLen, &ulRealSize);

    // 从流创建位图
    Bitmap bmpSrc(pstmp);
    Bitmap bmpDst(iDstWidth, iDstHeight, PixelFormat24bppRGB);

    // 创建画图对象
    Graphics grDraw(&bmpDst);

    // 绘图
    grDraw.DrawImage(&bmpSrc, 0, 0, bmpSrc.GetWidth(), bmpSrc.GetHeight());
    if (Ok != grDraw.GetLastStatus())
    {
        pstmp->Release();
        pstmp = NULL;
        GdiplusShutdown(gdiplusToken);
        gdiplusToken = NULL;
        return false;
    }

    // 创建输出流
    IStream* pStreamOut = NULL;
    if (CreateStreamOnHGlobal(NULL, TRUE, &pStreamOut) != S_OK)
    {
        pstmp->Release();
        pstmp = NULL;
        GdiplusShutdown(gdiplusToken);
        gdiplusToken = NULL;
        return false;
    }

    CLSID jpgClsid;
    Tool_GetEncoderClsid(L"image/jpeg", &jpgClsid);

    // 初始化输出流
    pStreamOut->Seek(liTemp, STREAM_SEEK_SET, NULL);
    pStreamOut->SetSize(uLiZero);

    // 将位图按照JPG的格式保存到输出流中
    int iQuality = compressQuality % 100;
    EncoderParameters encoderParameters;
    encoderParameters.Count = 1;
    encoderParameters.Parameter[0].Guid = EncoderQuality;
    encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
    encoderParameters.Parameter[0].NumberOfValues = 1;
    encoderParameters.Parameter[0].Value = &iQuality;
    bmpDst.Save(pStreamOut, &jpgClsid, &encoderParameters);
    //bmpDst.Save(pStreamOut, &jpgClsid, 0);

    // 获取输出流大小
    bool bRet = false;
    ULARGE_INTEGER libNewPos = { 0 };
    pStreamOut->Seek(liTemp, STREAM_SEEK_END, &libNewPos);      // 将流指针指向结束位置，从而获取流的大小 
    if (*iDstLen < (int)libNewPos.LowPart)                     // 用户分配的缓冲区不足
    {
        *iDstLen = libNewPos.LowPart;
        bRet = false;
    }
    else
    {
        pStreamOut->Seek(liTemp, STREAM_SEEK_SET, NULL);                   // 将流指针指向开始位置
        pStreamOut->Read(pbDst, libNewPos.LowPart, &ulRealSize);           // 将转换后的JPG图片拷贝给用户
        *iDstLen = ulRealSize;
        bRet = true;
    }


    // 释放内存
    if (pstmp != NULL)
    {
        pstmp->Release();
        pstmp = NULL;
    }
    if (pStreamOut != NULL)
    {
        pStreamOut->Release();
        pStreamOut = NULL;
    }

    GdiplusShutdown(gdiplusToken);
    gdiplusToken = NULL;

    return bRet;
}

int Tool_GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    ImageCodecInfo* pImageCodecInfo = NULL;

    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }
    free(pImageCodecInfo);
    return -1;  // Failure
}

void Tool_ExcuteShellCMD(char* pChCommand)
{
    if (NULL == pChCommand)
    {
        return;
    }
    ShellExecute(NULL, "open", "C:\\WINDOWS\\system32\\cmd.exe", pChCommand, "", SW_HIDE);
}

bool Tool_OverlayStringToImg(unsigned char** pImgsrc, long srcSize,
    unsigned char** pImgDest, long& DestSize,
    const wchar_t* DestString, int FontSize,
    int x, int y, int colorR, int colorG, int colorB,
    int compressQuality)
{
    if (!pImgsrc || !pImgDest || srcSize <= 0 || DestSize <= 0)
    {
        //WriteLog("传入参数为非法值");
        return false;
    }
    if (wcslen(DestString) <= 0 || x < 0 || y < 0)
    {
        //WriteLog("字符串长度为0");
        return false;
    }

    //构造图像	
    IStream *pSrcStream = NULL;
    IStream *pDestStream = NULL;
    CreateStreamOnHGlobal(NULL, TRUE, &pSrcStream);
    CreateStreamOnHGlobal(NULL, TRUE, &pDestStream);
    if (!pSrcStream || !pDestStream)
    {
        //WriteLog("流创建失败.");
        return false;
    }
    LARGE_INTEGER liTemp = { 0 };
    pSrcStream->Seek(liTemp, STREAM_SEEK_SET, NULL);
    pSrcStream->Write(*pImgsrc, srcSize, NULL);
    Bitmap bmp(pSrcStream);
    int iImgWith = bmp.GetWidth();
    int iImgHeight = bmp.GetHeight();

    Graphics grp(&bmp);

    SolidBrush brush(Color(colorR, colorG, colorB));
    FontFamily fontFamily(L"宋体");
    //Gdiplus::Font font(&fontFamily, (REAL)FontSize);
    Gdiplus::Font font(&fontFamily, (REAL)FontSize, FontStyleRegular, UnitPixel);

    RectF layoutRect(x, y, iImgWith - x, 0);
    RectF FinalRect;
    INT codePointsFitted = 0;
    INT linesFitted = 0;
    int strLenth = wcslen(DestString);
    grp.MeasureString(DestString, strLenth, &font, layoutRect, NULL, &FinalRect, &codePointsFitted, &linesFitted);
    grp.DrawString(DestString, -1, &font, FinalRect, NULL, &brush);
    Gdiplus::Status iState = grp.GetLastStatus();
    if (iState == Ok)
    {
        //WriteLog("字符叠加成功");
    }
    else
    {
        //char chLog[260] = { 0 };
        //sprintf(chLog, "字符叠加失败， 错误码为%d", iState);
        //WriteLog(chLog);
    }

    pSrcStream->Seek(liTemp, STREAM_SEEK_SET, NULL);
    pDestStream->Seek(liTemp, STREAM_SEEK_SET, NULL);

    // 将位图按照JPG的格式保存到输出流中
    CLSID jpgClsid;
    Tool_GetEncoderClsid(L"image/jpeg", &jpgClsid);
    int iQuality = compressQuality;
    EncoderParameters encoderParameters;
    encoderParameters.Count = 1;
    encoderParameters.Parameter[0].Guid = EncoderQuality;
    encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
    encoderParameters.Parameter[0].NumberOfValues = 1;
    encoderParameters.Parameter[0].Value = &iQuality;
    bmp.Save(pDestStream, &jpgClsid, &encoderParameters);

    ULARGE_INTEGER uiSize;
    pDestStream->Seek(liTemp, STREAM_SEEK_CUR, &uiSize);
    long iFinalSize = (long)uiSize.QuadPart;
    if (iFinalSize <= DestSize)
    {
        pDestStream->Seek(liTemp, STREAM_SEEK_SET, NULL);
        pDestStream->Read(*pImgDest, iFinalSize, NULL);
        DestSize = iFinalSize;
    }
    else
    {
        DestSize = 0;
        if (pSrcStream)
        {
            pSrcStream->Release();
            pSrcStream = NULL;
        }
        if (pDestStream)
        {
            pDestStream->Release();
            pDestStream = NULL;
        }
        //WriteLog("传入空间不足，字符叠加失败");
        return false;
    }

    if (pSrcStream)
    {
        pSrcStream->Release();
        pSrcStream = NULL;
    }
    if (pDestStream)
    {
        pDestStream->Release();
        pDestStream = NULL;
    }
    return true;
}

bool GetDataFromAppenedInfo(char *pszAppendInfo, std::string strItemName, char *pszRstBuf, int *piRstBufLen)
{
    if (pszAppendInfo == NULL || piRstBufLen == NULL || *piRstBufLen <= 0)
    {
        return false;
    }

    // <RoadNumber value="0" chnname="车道" />
    // <StreetName value="" chnname="路口名称" />
    std::string strAppendInfo = pszAppendInfo;
    size_t siStart = strAppendInfo.find(strItemName);
    if (siStart == std::string::npos)
    {
        return false;
    }
    siStart = strAppendInfo.find("\"", siStart + 1);
    if (siStart == std::string::npos)
    {
        return false;
    }
    size_t siEnd = strAppendInfo.find("\"", siStart + 1);
    if (siEnd == std::string::npos)
    {
        return false;
    }

    std::string strRst = strAppendInfo.substr(siStart + 1, siEnd - siStart - 1);
    if (*piRstBufLen < (int)strRst.length())
    {
        *piRstBufLen = (int)strRst.length();
        return false;
    }

    strncpy_s(pszRstBuf, *piRstBufLen, strRst.c_str(), (int)strRst.length());
    *piRstBufLen = (int)strRst.length();
    return true;
}

void ExcuteCMD(char* pChCommand)
{
#ifdef WIN32

    if (NULL == pChCommand)
    {
        return;
    }
    ShellExecute(NULL, "open", "C:\\WINDOWS\\system32\\cmd.exe", pChCommand, "", SW_HIDE);

#endif // WIN32
}

std::wstring Img_string2wstring(std::string strSrc)
{
    std::wstring wstrDst;
    int iWstrLen = MultiByteToWideChar(CP_ACP, 0, strSrc.c_str(), strSrc.size(), NULL, 0);
    wchar_t* pwcharBuf = new wchar_t[iWstrLen + sizeof(wchar_t)];   // 多一个结束符
    if (pwcharBuf == NULL || iWstrLen <= 0)
    {
        return L"";
    }
    memset(pwcharBuf, 0, iWstrLen*sizeof(wchar_t)+sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, strSrc.c_str(), strSrc.size(), pwcharBuf, iWstrLen);
    pwcharBuf[iWstrLen] = L'\0';
    wstrDst.append(pwcharBuf);
    delete[] pwcharBuf;
    pwcharBuf = NULL;
    return wstrDst;
}

bool DeleteDirectory(char* strDirName)
{
    CFileFind tempFind;

    char strTempFileFind[MAX_PATH];

    MY_SPRINTF(strTempFileFind, sizeof(strTempFileFind), "%s//*.*", strDirName);

    BOOL IsFinded = tempFind.FindFile(strTempFileFind);

    while (IsFinded)
    {
        IsFinded = tempFind.FindNextFile();

        if (!tempFind.IsDots())
        {
            char strFoundFileName[MAX_PATH];

            //strcpy(strFoundFileName, tempFind.GetFileName().GetBuffer(MAX_PATH));
            strcpy_s(strFoundFileName, tempFind.GetFileName().GetBuffer(MAX_PATH));

            if (tempFind.IsDirectory())
            {
                char strTempDir[MAX_PATH];

                MY_SPRINTF(strTempDir, sizeof(strTempDir), "%s//%s", strDirName, strFoundFileName);

                DeleteDirectory(strTempDir);
            }
            else
            {
                char strTempFileName[MAX_PATH];

                MY_SPRINTF(strTempFileName, sizeof(strTempFileName), "%s//%s", strDirName, strFoundFileName);

                DeleteFile(strTempFileName);
            }
        }
    }

    tempFind.Close();

    if (!RemoveDirectory(strDirName))
    {
        return FALSE;
    }

    return TRUE;
}

int CirclelaryDelete(char* folderPath, int iBackUpDays)
{
    printf("进入环覆盖线程主函数,开始查找制定目录下的文件夹");
    char myPath[MAX_PATH] = { 0 };
    //sprintf(myPath, "%s\\*", folderPath);
    MY_SPRINTF(myPath,sizeof(myPath),  "%s\\*", folderPath);

    CTime tmCurrentTime = CTime::GetCurrentTime();
    CTime tmLastMonthTime = tmCurrentTime - CTimeSpan(iBackUpDays, 0, 0, 0);
    int Last_Year = tmLastMonthTime.GetYear();
    int Last_Month = tmLastMonthTime.GetMonth();
    int Last_Day = tmLastMonthTime.GetDay();
    //cout<<Last_Year<<"-"<<Last_Month<<"-"<<Last_Day<<endl;

    CFileFind myFileFind;
    BOOL bFinded = myFileFind.FindFile(myPath);
    char DirectoryName[MAX_PATH] = { 0 };
    while (bFinded)
    {
        bFinded = myFileFind.FindNextFileA();
        if (!myFileFind.IsDots())
        {
            MY_SPRINTF(DirectoryName, sizeof(DirectoryName), "%s", myFileFind.GetFileName().GetBuffer());
            if (myFileFind.IsDirectory())
            {
                int iYear, iMonth, iDay;
                iYear = iMonth = iDay = 0;
                //sscanf(DirectoryName,"%d-%d-%d",&iYear, &iMonth, &iDay);
                sscanf_s(DirectoryName, "%d-%d-%d", &iYear, &iMonth, &iDay);
                if (iYear == 0 && iMonth ==0 && iDay == 0)
                {
                    continue;
                }
                if (iYear < Last_Year)
                {
                    MY_SPRINTF(DirectoryName,sizeof(DirectoryName), "%s\\%s", folderPath, myFileFind.GetFileName().GetBuffer());
                    printf("delete the DirectoryB :%s\n", DirectoryName);
                    DeleteDirectory(DirectoryName);

                    char chLog[MAX_PATH] = { 0 };
                    MY_SPRINTF(chLog,sizeof(chLog), "年份小于当前年份，删除文件夹%s", DirectoryName);
                    printf(chLog);
                }
                else if (iYear == Last_Year)
                {
                    if (iMonth < Last_Month)
                    {
                        MY_SPRINTF(DirectoryName, sizeof(DirectoryName), "%s\\%s", folderPath, myFileFind.GetFileName().GetBuffer());
                        printf("delete the DirectoryB :%s\n", DirectoryName);
                        DeleteDirectory(DirectoryName);

                        char chLog[MAX_PATH] = { 0 };
                        MY_SPRINTF(chLog,sizeof(chLog), "月份小于上一月，删除文件夹%s", DirectoryName);
                        printf(chLog);
                    }
                    else if (iMonth == Last_Month)
                    {
                        if (iDay < Last_Day)
                        {
                            MY_SPRINTF(DirectoryName, sizeof(DirectoryName), "%s\\%s", folderPath, myFileFind.GetFileName().GetBuffer());
                            printf("delete the DirectoryB :%s\n", DirectoryName);
                            DeleteDirectory(DirectoryName);

                            char chLog[MAX_PATH] = { 0 };
                            MY_SPRINTF(chLog, sizeof(chLog), "日号小于指定天数，删除文件夹%s", DirectoryName);
                            printf(chLog);
                        }
                    }
                }
            }
        }
    }
    myFileFind.Close();
    printf("查询结束，退出环覆盖线程主函数..");
    return 0;
}

bool Tool_SaveFileToDisk(char* chImgPath, void* pImgData, DWORD dwImgSize)
{
    printf("begin SaveImgToDisk");
    if (NULL == pImgData)
    {
        printf("end1 SaveImgToDisk");
        return false;
    }
    bool bRet = false;
    size_t iWritedSpecialSize = 0;
    std::string tempFile(chImgPath);
    size_t iPosition = tempFile.rfind("\\");
    std::string tempDir = tempFile.substr(0, iPosition + 1);
    if (MakeSureDirectoryPathExists(tempDir.c_str()))
    {
        FILE* fp = NULL;
        //fp = fopen(chImgPath, "wb+");
        fopen_s(&fp, chImgPath, "wb+");
        if (fp)
        {
            //iWritedSpecialSize = fwrite(pImgData, dwImgSize , 1, fp);
            iWritedSpecialSize = fwrite(pImgData, 1, dwImgSize, fp);
            fclose(fp);
            fp = NULL;
            bRet = true;
        }
        if (iWritedSpecialSize == dwImgSize)
        {
            char chLogBuff[MAX_PATH] = { 0 };
            //sprintf_s(chLogBuff, "%s save success", chImgPath);
            sprintf_s(chLogBuff, "%s save success", chImgPath);
            printf(chLogBuff);
        }
    }
    else
    {
        char chLogBuff[MAX_PATH] = { 0 };
        //sprintf_s(chLogBuff, "%s save failed", chImgPath);
        sprintf_s(chLogBuff, "%s save failed", chImgPath);
        printf(chLogBuff);
        bRet = false;
    }
    printf("end SaveImgToDisk");
    return bRet;
}

#ifdef WIN32
std::string GetSoftVersion(const char* exepath)
{
    std::string strVersionInfo = "";
    if (!exepath)
        return strVersionInfo;
    if (_access(exepath, 0) != 0)
        return strVersionInfo;
    UINT infoSize = GetFileVersionInfoSize(exepath, 0);
    if (infoSize != 0) {
        strVersionInfo.resize(infoSize, 0);
        char *pBuf = NULL;
        pBuf = new char[infoSize];
        VS_FIXEDFILEINFO *pVsInfo;
        if (GetFileVersionInfo(exepath, 0, infoSize, pBuf)) {
            if (VerQueryValue(pBuf, "\\", (void **)&pVsInfo, &infoSize))
            {
                //sprintf(pBuf, "%d.%d.%d.%d", HIWORD(pVsInfo->dwFileVersionMS), LOWORD(pVsInfo->dwFileVersionMS), HIWORD(pVsInfo->dwFileVersionLS), LOWORD(pVsInfo->dwFileVersionLS));
                sprintf_s(pBuf, infoSize,"%d.%d.%d.%d", HIWORD(pVsInfo->dwFileVersionMS), LOWORD(pVsInfo->dwFileVersionMS), HIWORD(pVsInfo->dwFileVersionLS), LOWORD(pVsInfo->dwFileVersionLS));
                strVersionInfo = pBuf;
            }
        }
        delete[] pBuf;
    }
    return strVersionInfo;
}

#endif

