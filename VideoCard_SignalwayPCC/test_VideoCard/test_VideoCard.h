
// test_VideoCard.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Ctest_VideoCardApp: 
// �йش����ʵ�֣������ test_VideoCard.cpp
//

class Ctest_VideoCardApp : public CWinApp
{
public:
	Ctest_VideoCardApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Ctest_VideoCardApp theApp;