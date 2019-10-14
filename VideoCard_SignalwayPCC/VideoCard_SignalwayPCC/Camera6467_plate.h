#pragma once
#include "BaseCamera.h"
class Camera6467_plate :    public BaseCamera
{
public:
    Camera6467_plate();
    Camera6467_plate(const char* chIP, HWND  hWnd, int Msg);
    virtual ~Camera6467_plate();

    void SetDisConnectCallback(void* funcDisc, void* pUser);
    void SetReConnectCallback(void* funcReco, void* pUser);
    bool GetOneJpegImg(CameraIMG &destImg);
    void SendConnetStateMsg(bool isConnect);

    //获取发送状态回调的时间间隔
    int GetTimeInterval();
    void SetConnectStatus_Callback(void* func, void* pUser, int TimeInterval);

    bool GetResultComplete();
    void SetResultComplete(bool bfinish);

    CameraResult* GetOneResult();

    void SetVideoPlayHandle(HWND handle, int Width, int Height);
    bool StartPlayVideo();
    void StopPlayVideo();
    void SetExitStatusVideo();
    bool GetOneImgFromVideo(int format, PBYTE dataBuffer, int* bufferLenght);
private:
    void AnalysisAppendInfo(CameraResult* CamResult);
    void ReadConfig();
private:

    int m_iTimeInvl;
    int m_iCompressBigImgSize;
    int m_iCompressSamllImgSize;
    int m_iVideoWidth;
    int m_iVideoHeight;

    DWORD m_dwLastCarID;

    bool m_bResultComplete;
    bool m_bJpegComplete;
    bool m_bSaveToBuffer;
    bool m_bOverlay;
    bool m_bCompress;

    PBYTE m_pTempBin;
    PBYTE m_pTempBig1;
    PBYTE m_pVideoImgBuffer;

    CameraResult* m_CameraResult;
    CameraResult* m_BufferResult;

    CameraIMG m_CIMG_StreamJPEG;

    CRITICAL_SECTION m_csResult;
    CameraIMG m_Bin_IMG_Temp;
    CameraIMG m_Small_IMG_Temp;

    void* g_pUser;
    void* g_func_ReconnectCallback;
    void* g_ConnectStatusCallback;
    void* g_func_DisconnectCallback;
    void* m_hVideo;

    HWND m_hVideoWindow;
private:

    virtual int RecordInfoBegin(DWORD dwCarID);

    virtual int RecordInfoEnd(DWORD dwCarID);

    virtual int RecordInfoPlate(DWORD dwCarID,
        LPCSTR pcPlateNo,
        LPCSTR pcAppendInfo,
        DWORD dwRecordType,
        DWORD64 dw64TimeMS);

    virtual int RecordInfoBigImage(DWORD dwCarID,
        WORD  wImgType,
        WORD  wWidth,
        WORD  wHeight,
        PBYTE pbPicData,
        DWORD dwImgDataLen,
        DWORD dwRecordType,
        DWORD64 dw64TimeMS);

    virtual int RecordInfoSmallImage(DWORD dwCarID,
        WORD wWidth,
        WORD wHeight,
        PBYTE pbPicData,
        DWORD dwImgDataLen,
        DWORD dwRecordType,
        DWORD64 dw64TimeMS);

    virtual int RecordInfoBinaryImage(DWORD dwCarID,
        WORD wWidth,
        WORD wHeight,
        PBYTE pbPicData,
        DWORD dwImgDataLen,
        DWORD dwRecordType,
        DWORD64 dw64TimeMS);

    virtual int DeviceJPEGStream(
        PBYTE pbImageData,
        DWORD dwImageDataLen,
        DWORD dwImageType,
        LPCSTR szImageExtInfo);

    virtual void ThreadFunc_CheckStatus();
};

