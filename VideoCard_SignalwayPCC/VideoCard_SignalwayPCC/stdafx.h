// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
//#include <windows.h>
#include <afx.h>



// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include <Dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include<ctime>
#include<atltime.h>

#define DLL_VERSION "2019101501"
#define INI_FILE_NAME "\\VideoCard_SignalwayPCC.ini"
#define DLL_LOG_NAME "VideoCard_SignalwayPCC_log.txt"

#ifdef WINXP
#define MY_SPRINTF sprintf
#else
#define MY_SPRINTF sprintf_s
#endif

#define  CAM_COUNT 10

#define BIN_IMG_SIZE 280
#define COMPRESS_PLATE_IMG_SIZE 5*1024
#define COMPRESS_BIG_IMG_SIZE 100*1024

#define VIDEO_IMG_FORMAT_BMP 0
#define VIDEO_IMG_FORMAT_JPG 1

#define DLL_NAME "\\XLW_Plate.ini"