#pragma once

#include <windows.h>
#include <wingdi.h>
#include "opencv2/core/core.hpp"

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

    cv::Mat& getMat() { return m_Mat; };

    cv::Mat& getGreyMat() { return m_Grey; };

protected:
    CScreenCapturer();
    static CScreenCapturer * m_pInstance;

private:
    HDC m_hSrcDC, m_hMemDC;
    HBITMAP m_hBitmap , m_hOldBitmap;
    BYTE *m_pBuffer;
    int m_image_nchannels;

    cv::Mat m_Grey, m_Mat;
};