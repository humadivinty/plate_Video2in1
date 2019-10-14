
// test_VideoCardDlg.h : 头文件
//

#pragma once


// Ctest_VideoCardDlg 对话框
class Ctest_VideoCardDlg : public CDialogEx
{
// 构造
public:
	Ctest_VideoCardDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TEST_VIDEOCARD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonInit();
    afx_msg void OnBnClickedButtonDeinit();
    afx_msg void OnBnClickedButtonStartdisplay();
    afx_msg void OnBnClickedButtonStopdisplay();
    afx_msg void OnBnClickedButtonGetimage();
    afx_msg void OnBnClickedButtonGetimagefile();
    afx_msg void OnBnClickedButtonTvpdisplay();
    afx_msg void OnBnClickedButtonTvpclear();
    afx_msg void OnBnClickedButtonSynctime();
    afx_msg void OnBnClickedButtonShowtime();
    afx_msg void OnBnClickedButtonGetstatus();
    afx_msg void OnBnClickedButtonGetstatusmsg();
    afx_msg void OnBnClickedButtonGetdevinfo();
    afx_msg void OnBnClickedButtonGetversion();


    bool SaveFileToDisk(char* chImgPath, void* pImgData, DWORD dwImgSize);
private:
    int m_iIndex;
    BYTE* m_pImgBuffer;
public:
    afx_msg void OnBnClickedButtonVcTvpdisplay();
    afx_msg void OnBnClickedButtonVcGethwversion();
};
