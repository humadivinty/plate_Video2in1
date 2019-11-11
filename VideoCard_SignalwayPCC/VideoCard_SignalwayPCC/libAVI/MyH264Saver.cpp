#include "stdafx.h"
#include "MyH264Saver.h"
#include "utilityTool/ToolFunction.h"
//#include <QDebug>


MyH264Saver::MyH264Saver():
m_bExit(false),
m_iSaveH264Flag(0),
m_iTimeFlag(0),
M_iStopTimeFlag(TIME_FLAG_UNDEFINE),
m_iTmpTime(0),
m_lastvideoidx(-1),
m_bFirstSave(false),
//m_pLocker(NULL),
m_hThreadSaveH264(NULL)
{
    InitializeCriticalSection(&m_Locker);
	InitializeCriticalSection(&m_DataListLocker);

    m_hThreadSaveH264 = CreateThread(NULL, 0, H264DataProceesor, this, 0, NULL);
}


MyH264Saver::~MyH264Saver()
{
    SetIfExit(true);
    SetSaveFlag(false);
    Tool_SafeCloseThread(m_hThreadSaveH264);

    DeleteCriticalSection(&m_Locker);
	DeleteCriticalSection(&m_DataListLocker);
}

bool MyH264Saver::addDataStruct(CustH264Struct* pDataStruct)
{
    if (pDataStruct == NULL)
    {
        return false;
    }
	//char buf[256] = { 0 };
	//sprintf_s(buf, "%ld\n", pDataStruct->m_llFrameTime);
	//OutputDebugString(buf);

    std::shared_ptr<CustH264Struct> pData = std::shared_ptr<CustH264Struct>(pDataStruct);
	EnterCriticalSection(&m_DataListLocker);
    if (m_lDataStructList.size() > 250)
    {
        m_lDataStructList.pop_front();
    }
    m_lDataStructList.push_back(pData);
	LeaveCriticalSection(&m_DataListLocker);

    return true;
}

bool MyH264Saver::StartSaveH264(INT64 beginTimeStamp, const char* pchFilePath)
{
    //qDebug()<< "StartSaveH264 :"<< "beginTimeStamp "<< beginTimeStamp;
    SetSaveFlag(SAVING_FLAG_SAVING);
    SetStopTimeFlag(TIME_FLAG_UNDEFINE);
    SetIfFirstSave(true);
    SetTimeFlag(beginTimeStamp);
    SetSavePath(pchFilePath, strlen(pchFilePath));
	m_lastvideoidx = -1;
    return true;
}

bool MyH264Saver::StopSaveH264(int TimeFlag)
{
    //SetSaveFlag(SAVING_FLAG_SHUT_DOWN);
    SetStopTimeFlag(TimeFlag);
    return true;
}

DWORD WINAPI MyH264Saver::H264DataProceesor(LPVOID lpThreadParameter)
{
    if (NULL == lpThreadParameter)
    {
        return 0;
    }
    MyH264Saver* pSaver = (MyH264Saver*)lpThreadParameter;
    return pSaver->processH264Data();
}

DWORD MyH264Saver::processH264Data()
{
    int  iSaveFlag = 0;
    int iVideoWidth = -1;
    int iVideoHeight = -1;
    while (!GetIfExit())
    {
        //Sleep(50);
		EnterCriticalSection(&m_DataListLocker);
        if (m_lDataStructList.size() <= 0)
        {
			LeaveCriticalSection(&m_DataListLocker);
            Sleep(50);
            continue;
        }
		LeaveCriticalSection(&m_DataListLocker);

		char buf[256] = { 0 };

        if( GetStopTimeFlag() != TIME_FLAG_UNDEFINE
                && GetStopTimeFlag() < GetTickCount() )
        {
            //qDebug()<< "GetStopTimeFlag "  <<GetStopTimeFlag() << "< "<< "GetTickCount" <<GetTickCount();
            iSaveFlag = SAVING_FLAG_SHUT_DOWN;
            SetStopTimeFlag(TIME_FLAG_UNDEFINE);
        }
        else
        {
            iSaveFlag = GetSaveFlag();
        }

        std::shared_ptr<CustH264Struct > pData = nullptr;
        switch (iSaveFlag)
        {
        case SAVING_FLAG_NOT_SAVE:
            Sleep(10);
            break;
        case SAVING_FLAG_SAVING:
            EnterCriticalSection(&m_DataListLocker);
            for (auto r = m_lDataStructList.begin(); r != m_lDataStructList.end(); )
            {
                if ((*r)->m_llFrameTime < GetTimeFlag())
                {
                    //r = m_lDataStructList.erase(r);
                    r++;
                }
                else
                {
                    if ( (*r)->m_llFrameTime > m_iTmpTime
                            || ( (*r)->m_llFrameTime == m_iTmpTime && ( (*r)->index == m_lastvideoidx + 1 || ((*r)->index == 0 && m_lastvideoidx == 400))))
                    {
                        pData = (*r);
                        m_iTmpTime = (*r)->m_llFrameTime;
                        m_lastvideoidx = (*r)->index;
                        break;
                    }
                    r++;
                }
            }
            LeaveCriticalSection(&m_DataListLocker);

            //pData = m_lDataStructList.front();

            if (pData == nullptr)
            {
                break;
            }

//            sprintf_s(buf, "%ld, ", pData->m_llFrameTime);
//            OutputDebugStringA(buf);
//            sprintf_s(buf, "index = %d\n",  pData->index);
//            OutputDebugStringA(buf);

            //EnterCriticalSection(&m_DataListLocker);
            //m_lDataStructList.pop_front();
            //LeaveCriticalSection(&m_DataListLocker);
            iVideoWidth = pData->m_iWidth;
            iVideoHeight = pData->m_iHeight;
            if (GetIfFirstSave())
            {
                if (!m_264AviLib.IsNULL())
                {
                    m_264AviLib.close();
                }
                if (iVideoWidth > 0
                    && iVideoHeight > 0)
                {
                    m_264AviLib.setAviInfo((char*)GetSavePath(), iVideoWidth, iVideoHeight, 25, "H264");
                    SetIfFirstSave(false);
                }
            }
            if (!m_264AviLib.IsNULL()
                && pData->m_llFrameTime > GetTimeFlag()
                && 0== m_264AviLib.writeFrame((char*)pData->m_pbH264FrameData, pData->m_iDataSize, pData->m_isIFrame))
            {
                //SetIfFirstSave(false);
            }
            break;
        case SAVING_FLAG_SHUT_DOWN:
            if (!m_264AviLib.IsNULL())
            {
				m_iTmpTime = 0;
                m_264AviLib.close();
                SetSaveFlag(SAVING_FLAG_NOT_SAVE);
            }
            break;
        default:
            break;
        }       

    }
    return 0;
}

void MyH264Saver::SetIfExit(bool bValue)
{
    EnterCriticalSection(&m_Locker);
    m_bExit = bValue;
    LeaveCriticalSection(&m_Locker);
}

bool MyH264Saver::GetIfExit()
{
    bool bValue = false;
    EnterCriticalSection(&m_Locker);
    bValue = m_bExit;
    LeaveCriticalSection(&m_Locker);
    return bValue;
}

void MyH264Saver::SetSaveFlag(int iValue)
{
    EnterCriticalSection(&m_Locker);
    m_iSaveH264Flag = iValue;
    LeaveCriticalSection(&m_Locker);
}

int MyH264Saver::GetSaveFlag()
{
    int iValue = false;
    EnterCriticalSection(&m_Locker);
    iValue = m_iSaveH264Flag;
    LeaveCriticalSection(&m_Locker);
    return iValue;
}

void MyH264Saver::SetTimeFlag(INT64 iValue)
{
    EnterCriticalSection(&m_Locker);
    m_iTimeFlag = iValue;
	m_iTmpTime = iValue;
    LeaveCriticalSection(&m_Locker);
}

INT64 MyH264Saver::GetTimeFlag()
{
    INT64 iValue = false;
    EnterCriticalSection(&m_Locker);
    iValue = m_iTimeFlag;
    LeaveCriticalSection(&m_Locker);
    return iValue;
}

void MyH264Saver::SetStopTimeFlag(INT64 iValue)
{
    EnterCriticalSection(&m_Locker);
    M_iStopTimeFlag = iValue;
    LeaveCriticalSection(&m_Locker);
}

INT64 MyH264Saver::GetStopTimeFlag()
{
    INT64 iValue = false;
    EnterCriticalSection(&m_Locker);
    iValue = M_iStopTimeFlag;
    LeaveCriticalSection(&m_Locker);
    return iValue;
}

void MyH264Saver::SetIfFirstSave(bool bValue)
{
    EnterCriticalSection(&m_Locker);
    m_bFirstSave = bValue;
    LeaveCriticalSection(&m_Locker);
}

bool MyH264Saver::GetIfFirstSave()
{
    bool bValue = false;
    EnterCriticalSection(&m_Locker);
    bValue = m_bFirstSave;
    LeaveCriticalSection(&m_Locker);
    return bValue;
}

void MyH264Saver::SetSavePath(const char* filePath, size_t bufLength)
{
    EnterCriticalSection(&m_Locker);
    if (bufLength < sizeof(m_chFilePath))
    {
        memcpy(m_chFilePath, filePath, bufLength);
        m_chFilePath[bufLength] = '\0';
    }
    LeaveCriticalSection(&m_Locker);
}

const char* MyH264Saver::GetSavePath()
{
    const char* pValue = NULL;
    EnterCriticalSection(&m_Locker);
    pValue = m_chFilePath;
    LeaveCriticalSection(&m_Locker);
    return pValue;
}
