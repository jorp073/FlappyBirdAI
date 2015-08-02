#pragma once

#include <windows.h>
#include <wingdi.h>
#include "opencv2/opencv.hpp"

class CScreenCapturer
{
public:
    static CScreenCapturer* GetInstance()
    {
        if(NULL == m_pInstance)
            m_pInstance = new CScreenCapturer();
        return static_cast<CScreenCapturer*>(m_pInstance);
    }

    ~CScreenCapturer();

    static void Release()
    {
        if(NULL != m_pInstance)
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
    }

    bool Capture(RECT rect);

    RECT getScreenRect();

    cv::Mat& GetMat() { return m_Mat; };

    cv::Mat& GetGrayMat() { return m_GrayMat; };

protected:
    CScreenCapturer();
    static CScreenCapturer * m_pInstance;

private:
    HDC m_hSrcDC, m_hMemDC;
    HBITMAP m_hBitmap , m_hOldBitmap;
    BYTE *m_pBuffer;
    int m_image_nchannels;
    int m_iLastWidth, m_iLastHeight;

    cv::Mat m_GrayMat, m_Mat;
};