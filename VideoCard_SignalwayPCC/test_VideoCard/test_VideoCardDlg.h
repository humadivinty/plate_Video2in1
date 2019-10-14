
// test_VideoCardDlg.h : ͷ�ļ�
//

#pragma once


// Ctest_VideoCardDlg �Ի���
class Ctest_VideoCardDlg : public CDialogEx
{
// ����
public:
	Ctest_VideoCardDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TEST_VIDEOCARD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
