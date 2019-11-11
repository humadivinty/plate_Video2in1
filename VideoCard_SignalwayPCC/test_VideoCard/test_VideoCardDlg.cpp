
// test_VideoCardDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "test_VideoCard.h"
#include "test_VideoCardDlg.h"
#include "afxdialogex.h"
#include <string>
#include <direct.h> // _getcwd

#include "../VideoCard_SignalwayPCC/VideoCard_SignalwayPCC.h"
#ifdef DEBUG
#pragma comment(lib, "../debug/VideoCard_SignalwayPCC.lib")
#else
#pragma comment(lib, "../release/VideoCard_SignalwayPCC.lib")
#endif

#include <Dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  MAX_IMG_SIZE 10*1024*1024

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Ctest_VideoCardDlg 对话框



Ctest_VideoCardDlg::Ctest_VideoCardDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Ctest_VideoCardDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ctest_VideoCardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Ctest_VideoCardDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_Init, &Ctest_VideoCardDlg::OnBnClickedButtonInit)
    ON_BN_CLICKED(IDC_BUTTON_Deinit, &Ctest_VideoCardDlg::OnBnClickedButtonDeinit)
    ON_BN_CLICKED(IDC_BUTTON_StartDisplay, &Ctest_VideoCardDlg::OnBnClickedButtonStartdisplay)
    ON_BN_CLICKED(IDC_BUTTON_StopDisplay, &Ctest_VideoCardDlg::OnBnClickedButtonStopdisplay)
    ON_BN_CLICKED(IDC_BUTTON_GetImage, &Ctest_VideoCardDlg::OnBnClickedButtonGetimage)
    ON_BN_CLICKED(IDC_BUTTON_GetImageFile, &Ctest_VideoCardDlg::OnBnClickedButtonGetimagefile)
    ON_BN_CLICKED(IDC_BUTTON_TVPDisplay, &Ctest_VideoCardDlg::OnBnClickedButtonTvpdisplay)
    ON_BN_CLICKED(IDC_BUTTON_TVPClear, &Ctest_VideoCardDlg::OnBnClickedButtonTvpclear)
    ON_BN_CLICKED(IDC_BUTTON_SyncTime, &Ctest_VideoCardDlg::OnBnClickedButtonSynctime)
    ON_BN_CLICKED(IDC_BUTTON_ShowTime, &Ctest_VideoCardDlg::OnBnClickedButtonShowtime)
    ON_BN_CLICKED(IDC_BUTTON_GetStatus, &Ctest_VideoCardDlg::OnBnClickedButtonGetstatus)
    ON_BN_CLICKED(IDC_BUTTON_GetStatusMsg, &Ctest_VideoCardDlg::OnBnClickedButtonGetstatusmsg)
    ON_BN_CLICKED(IDC_BUTTON_GetDevInfo, &Ctest_VideoCardDlg::OnBnClickedButtonGetdevinfo)
    ON_BN_CLICKED(IDC_BUTTON_GetVersion, &Ctest_VideoCardDlg::OnBnClickedButtonGetversion)
    ON_BN_CLICKED(IDC_BUTTON_VC_TVPDisplay, &Ctest_VideoCardDlg::OnBnClickedButtonVcTvpdisplay)
    ON_BN_CLICKED(IDC_BUTTON_VC_GetHWVersion, &Ctest_VideoCardDlg::OnBnClickedButtonVcGethwversion)
    ON_BN_CLICKED(IDC_BUTTON_GetVideoFile, &Ctest_VideoCardDlg::OnBnClickedButtonGetvideofile)
END_MESSAGE_MAP()


// Ctest_VideoCardDlg 消息处理程序

BOOL Ctest_VideoCardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//ShowWindow(SW_MINIMIZE);

	// TODO:  在此添加额外的初始化代码
    m_iIndex = 0;
    m_pImgBuffer = NULL;
    ((CComboBox*)GetDlgItem(IDC_COMBO_select))->AddString("BMP");
    ((CComboBox*)GetDlgItem(IDC_COMBO_select))->AddString("JPG");

    ((CComboBox*)GetDlgItem(IDC_COMBO_TimeStle))->AddString("0");
    ((CComboBox*)GetDlgItem(IDC_COMBO_TimeStle))->AddString("1");
    ((CComboBox*)GetDlgItem(IDC_COMBO_TimeStle))->AddString("2");
    ((CComboBox*)GetDlgItem(IDC_COMBO_TimeStle))->AddString("3");

    ((CComboBox*)GetDlgItem(IDC_COMBO_videoFormat))->AddString("0");
    ((CComboBox*)GetDlgItem(IDC_COMBO_videoFormat))->AddString("1");
    ((CComboBox*)GetDlgItem(IDC_COMBO_videoFormat))->AddString("2");
    ((CComboBox*)GetDlgItem(IDC_COMBO_videoFormat))->AddString("3");

    GetDlgItem(IDC_EDIT_Time)->SetWindowText("6");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Ctest_VideoCardDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Ctest_VideoCardDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Ctest_VideoCardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Ctest_VideoCardDlg::OnBnClickedButtonInit()
{
    // TODO:  在此添加控件通知处理程序代码
    CString cstrIP;
    GetDlgItem(IDC_IPADDRESS1)->GetWindowTextA(cstrIP);
    CString strTemp;
    strTemp.Format("%s, 8000, admin, admin", cstrIP);

    int iRet = VC_Init(1, strTemp.GetBuffer());
    m_iIndex = iRet;
    strTemp.ReleaseBuffer();

    char chLog[MAX_PATH] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "VC_Init, 参数 %s,  返回值为 %d, ", strTemp.GetBuffer(), iRet);
    strTemp.ReleaseBuffer();

    MessageBox(chLog);
}


void Ctest_VideoCardDlg::OnBnClickedButtonDeinit()
{
    // TODO:  在此添加控件通知处理程序代码
    int iRet = VC_Deinit(m_iIndex);
    char chLog[MAX_PATH] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "VC_Deinit , nHandle = %d, 返回值为 %d", m_iIndex, iRet);
    MessageBox(chLog);
}


void Ctest_VideoCardDlg::OnBnClickedButtonStartdisplay()
{
    // TODO:  在此添加控件通知处理程序代码
    CWnd   *pWnd = GetDlgItem(IDC_STATIC_Picture); // 取得控件的指针
    HWND hwnd = pWnd->GetSafeHwnd();  // 取得控件的句柄
    int iRet = VC_StartDisplay(m_iIndex, 720, 576, (int)hwnd);
    char chLog[MAX_PATH] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "VC_StartDisplay , nHandle = %d, 返回值为 %d", m_iIndex, iRet);
    MessageBox(chLog);
}


void Ctest_VideoCardDlg::OnBnClickedButtonStopdisplay()
{
    // TODO:  在此添加控件通知处理程序代码
    int iRet = VC_StopDisplay(m_iIndex);
    char chLog[MAX_PATH] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "VC_StartDisplay , nHandle = %d, 返回值为 %d", m_iIndex, iRet);
    MessageBox(chLog);
}


void Ctest_VideoCardDlg::OnBnClickedButtonGetimage()
{
    // TODO:  在此添加控件通知处理程序代码
    char chLog[MAX_PATH] = { 0 };
    if (!m_pImgBuffer)
    {
        m_pImgBuffer = new BYTE[MAX_IMG_SIZE];
    }
    if (m_pImgBuffer)
    {
        TCHAR szFileName[MAX_PATH] = { 0 };
        GetModuleFileName(NULL, szFileName, MAX_PATH);	//取得包括程序名的全路径
        PathRemoveFileSpec(szFileName);				//去掉程序名
        char chPath[MAX_PATH] = { 0 };

        memset(m_pImgBuffer, 0, MAX_IMG_SIZE);
        CString cstrFormat;
        GetDlgItem(IDC_COMBO_select)->GetWindowText(cstrFormat);
        int iBufferLength = MAX_IMG_SIZE, iRet = -2000;
        if (-1 != cstrFormat.Find("BMP"))
        {
            iRet = VC_GetImage(m_iIndex, 0, (char*)m_pImgBuffer, &iBufferLength);
            sprintf_s(chPath, sizeof(chPath), "%s\\ImgBuffer\\VC_GetImage_%ld.bmp", szFileName,GetTickCount());
        }
        else
        {
            iBufferLength = 200 * 1024;
            iRet = VC_GetImage(m_iIndex, 1, (char*)m_pImgBuffer, &iBufferLength);
            sprintf_s(chPath, sizeof(chPath), "%s\\ImgBuffer\\VC_GetImage_%ld.jpg", szFileName, GetTickCount());
        }
        if (iRet == 0)
        {
            SaveFileToDisk(chPath, m_pImgBuffer, iBufferLength);
            //FILE* pFile = NULL;
            //fopen_s(&pFile, chPath, "wb");
            //if (pFile)
            //{
            //    fwrite(m_pImgBuffer, iBufferLength, 1, pFile);
            //    fclose(pFile);
            //    pFile = NULL;
            //}
        }
        sprintf_s(chLog, sizeof(chLog), "VC_GetImage , nHandle = %d, 返回值为 %d", m_iIndex, iRet);
        MessageBox(chLog);
    }
    else
    {
        sprintf_s(chLog, sizeof(chLog), "VC_GetImage , 图片缓存空间申请是失败");
        MessageBox(chLog);
    }
}


void Ctest_VideoCardDlg::OnBnClickedButtonGetimagefile()
{
    // TODO:  在此添加控件通知处理程序代码
    char chLog[MAX_PATH] = { 0 };

    TCHAR szFileName[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szFileName, MAX_PATH);	//取得包括程序名的全路径
    PathRemoveFileSpec(szFileName);				//去掉程序名
    char chPath[MAX_PATH] = { 0 };

    CString cstrFormat;
    GetDlgItem(IDC_COMBO_select)->GetWindowText(cstrFormat);
    int iBufferLength = MAX_IMG_SIZE, iRet = -2000;
        
    if (-1 != cstrFormat.Find("BMP"))
    {
        sprintf_s(chPath, sizeof(chPath), "%s\\ImgBuffer\\VC_GetImageFile_%ld.bmp", szFileName, GetTickCount());
        iRet = VC_GetImageFile(m_iIndex, 0, chPath);
    }
    else
    {
        sprintf_s(chPath, sizeof(chPath), "%s\\ImgBuffer\\VC_GetImageFile_%ld.jpg", szFileName, GetTickCount());
        iRet = VC_GetImageFile(m_iIndex, 1, chPath);
    }

    sprintf_s(chLog, sizeof(chLog), "VC_GetImageFile , nHandle = %d, 返回值为 %d", m_iIndex, iRet);
    MessageBox(chLog);
}


void Ctest_VideoCardDlg::OnBnClickedButtonTvpdisplay()
{
    // TODO:  在此添加控件通知处理程序代码
    CString cstrRow, cstrCol;
    GetDlgItem(IDC_EDIT_Row)->GetWindowText(cstrRow);
    GetDlgItem(IDC_EDIT_Col)->GetWindowText(cstrCol);
    int iRow = atoi(cstrRow.GetBuffer());
    int iCol = atoi(cstrCol.GetBuffer());
    cstrRow.ReleaseBuffer();
    cstrCol.ReleaseBuffer();

    char chText[256] = { 0 };
    sprintf_s(chText, sizeof(chText), "你好hello");
    int iRet = VLPR_TVPDisplay(m_iIndex, iRow, iCol, chText);

    char chLog[256] = {0};
    sprintf_s(chLog, sizeof(chLog), "VLPR_TVPDisplay , nHandle = %d,text = %s 返回值为 %d", m_iIndex, chText, iRet);
    MessageBox(chLog);
}


void Ctest_VideoCardDlg::OnBnClickedButtonTvpclear()
{
    // TODO:  在此添加控件通知处理程序代码
    CString cstrRow, cstrCol, cstrLenght;
    GetDlgItem(IDC_EDIT_Row)->GetWindowText(cstrRow);
    GetDlgItem(IDC_EDIT_Col)->GetWindowText(cstrCol);
    GetDlgItem(IDC_EDIT_Length)->GetWindowText(cstrLenght);
    int iRow = atoi(cstrRow.GetBuffer());
    int iCol = atoi(cstrCol.GetBuffer());
    int iLength = atoi(cstrLenght.GetBuffer());
    cstrRow.ReleaseBuffer();
    cstrCol.ReleaseBuffer();
    cstrLenght.ReleaseBuffer();

    int iRet = VC_TVPClear(m_iIndex, iRow, iCol, iLength);

    char chLog[256] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "VLPR_TVPDisplay , nHandle = %d,length = %d, 返回值为 %d", m_iIndex, iLength, iRet);
    MessageBox(chLog);
}


void Ctest_VideoCardDlg::OnBnClickedButtonSynctime()
{
    // TODO:  在此添加控件通知处理程序代码
    char chText[128] = { 0 };
    CTime CurrentTime = CTime::GetCurrentTime();
    sprintf_s(chText, "%s", CurrentTime.Format("%Y%m%d%H%M%S"));

    int iRet = VC_SyncTime(m_iIndex, chText);

    char chLog[256] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "VC_SyncTime , nHandle = %d,Time = %s, 返回值为 %d", m_iIndex, chText, iRet);
    MessageBox(chLog);
}


void Ctest_VideoCardDlg::OnBnClickedButtonShowtime()
{
    // TODO:  在此添加控件通知处理程序代码
    CString cstrStyle;
    GetDlgItem(IDC_COMBO_TimeStle)->GetWindowText(cstrStyle);
    int iStyle = atoi(cstrStyle.GetBuffer());
    cstrStyle.ReleaseBuffer();

    int iRet = VC_ShowTime(m_iIndex,  iStyle);
    char chLog[256] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "VC_ShowTime , nHandle = %d,style = %d, 返回值为 %d", m_iIndex, iStyle, iRet);
    MessageBox(chLog);
}


void Ctest_VideoCardDlg::OnBnClickedButtonGetstatus()
{
    // TODO:  在此添加控件通知处理程序代码
    int iStatus = -2000;
    int iRet = VC_GetStatus(m_iIndex, &iStatus);

    char chLog[256] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "VC_GetStatus , nHandle = %d,iStatus = %d, 返回值为 %d", m_iIndex, iStatus, iRet);
    MessageBox(chLog);
}


void Ctest_VideoCardDlg::OnBnClickedButtonGetstatusmsg()
{
    // TODO:  在此添加控件通知处理程序代码
    int nStatusCode = 0;
    CString strCode;
    GetDlgItem(IDC_EDIT_statuCode)->GetWindowText(strCode);
    if (strCode.IsEmpty())
    {
        nStatusCode = 0;
    }
    else
    {
        nStatusCode = atoi(strCode.GetBuffer());
        strCode.ReleaseBuffer();
    }

    char chStatusMsg[MAX_PATH] = { 0 };
    int nStatusMsgLen = sizeof(chStatusMsg);
    int iRet = VC_GetStatusMsg(nStatusCode, chStatusMsg, nStatusMsgLen);

    char chLog[MAX_PATH] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "VC_GetStatusMsg , nStatusCode = %d, chStatusMsg = %s, nStatusMsgLen = %d, 返回值为 %d", nStatusCode, chStatusMsg, nStatusMsgLen, iRet);
    MessageBox(chLog);
}


void Ctest_VideoCardDlg::OnBnClickedButtonGetdevinfo()
{
    // TODO:  在此添加控件通知处理程序代码
    char chVersionInfo[MAX_PATH] = { 0 };
    char chDevInfo[MAX_PATH] = {0};
    int iVrLength = sizeof(chVersionInfo);
    int iDevLength = sizeof(chDevInfo);

    int iRet = VC_GetDevInfo(chVersionInfo, iVrLength, chDevInfo, iDevLength);

    char chLog[MAX_PATH] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "VC_GetDevInfo , sCompany = %s,  sDevMode= %s, 返回值为 %d", chVersionInfo, chDevInfo, iRet);
    MessageBox(chLog);
}


void Ctest_VideoCardDlg::OnBnClickedButtonGetversion()
{
    // TODO:  在此添加控件通知处理程序代码
    char chDevVersion[MAX_PATH] = { 0 };
    char chAPIVersion[MAX_PATH] = { 0 };
    int iDevVersionLength = sizeof(chDevVersion);
    int iAPIVersionLength = sizeof(chAPIVersion);

    int iRet = VC_GetVersion(chDevVersion, iDevVersionLength, chAPIVersion, iAPIVersionLength);

    char chLog[MAX_PATH] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "VC_GetVersion , DevVersion = %s,  APIVersion= %s, 返回值为 %d", chDevVersion, chAPIVersion, iRet);
    MessageBox(chLog);
}


bool Ctest_VideoCardDlg::SaveFileToDisk(char* chImgPath, void* pImgData, DWORD dwImgSize)
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


void Ctest_VideoCardDlg::OnBnClickedButtonVcTvpdisplay()
{
    // TODO:  在此添加控件通知处理程序代码
    CString cstrRow, cstrCol;
    GetDlgItem(IDC_EDIT_Row)->GetWindowText(cstrRow);
    GetDlgItem(IDC_EDIT_Col)->GetWindowText(cstrCol);
    int iRow = atoi(cstrRow.GetBuffer());
    int iCol = atoi(cstrCol.GetBuffer());
    cstrRow.ReleaseBuffer();
    cstrCol.ReleaseBuffer();

    char chText[256] = { 0 };
    sprintf_s(chText, sizeof(chText), "你好hello");
    int iRet = VC_TVPDisplay(m_iIndex, iRow, iCol, chText);

    char chLog[256] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "VC_TVPDisplay , nHandle = %d,text = %s 返回值为 %d", m_iIndex, chText, iRet);
    MessageBox(chLog);
}


void Ctest_VideoCardDlg::OnBnClickedButtonVcGethwversion()
{
    // TODO:  在此添加控件通知处理程序代码
    // TODO:  在此添加控件通知处理程序代码
    char chDevVersion[MAX_PATH] = { 0 };
    char chAPIVersion[MAX_PATH] = { 0 };
    int iDevVersionLength = sizeof(chDevVersion);
    int iAPIVersionLength = sizeof(chAPIVersion);

    int iRet = VC_GetHWVersion(m_iIndex, chDevVersion, iDevVersionLength, chAPIVersion, iAPIVersionLength);

    char chLog[MAX_PATH] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "VC_GetHWVersion , sHWVersion = %s,  sAPIVersion= %s, 返回值为 %d", chDevVersion, chAPIVersion, iRet);
    MessageBox(chLog);
}


void Ctest_VideoCardDlg::OnBnClickedButtonGetvideofile()
{
    // TODO:  在此添加控件通知处理程序代码
    CHAR szPath[256] = { 0 };
    //getcwd(szPath,sizeof(szPath));
    _getcwd(szPath, sizeof(szPath));

    char chTemp[256] = {0};
    GetItemText(IDC_EDIT_Time, chTemp, sizeof(chTemp));
    int iTime = atoi(chTemp);

    memset(chTemp, '\0', sizeof(chTemp));
    GetItemText(IDC_COMBO_videoFormat, chTemp, sizeof(chTemp));
    int iFormat = atoi(chTemp);

    char chVideoFileName[256] = {0};
    sprintf_s(chVideoFileName, sizeof(chVideoFileName), "%s\\%lu.avi", szPath, GetTickCount());

    int iRet = VC_GetVideoFile(m_iIndex, iFormat, iTime, chVideoFileName);

    char chLog[MAX_PATH] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "VC_GetVideoFile ,  iFormat= %d, iTime= %d , 返回值= %d, 路径 = %s", 
        iFormat,
        iTime, 
        iRet, 
        chVideoFileName);
    MessageBox(chLog);
}

bool Ctest_VideoCardDlg::GetItemText(int ItemID, char* buffer, size_t bufSize)
{
    CString strTemp;
    GetDlgItem(ItemID)->GetWindowText(strTemp);
    if (strTemp.GetLength() < bufSize)
    {
        //sprintf(buffer, "%s", strTemp.GetBuffer());
        sprintf_s(buffer, bufSize, "%s", strTemp.GetBuffer());
        strTemp.ReleaseBuffer();
        return true;
    }
    return false;
}
