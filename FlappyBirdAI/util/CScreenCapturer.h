#pragma once

#include <windows.h>
#include <wingdi.h>
#include "opencv2/opencv.hpp"
#include "SingleInstance.h"

class CScreenCapturer
{
public:
    bool Capture(RECT rect);

    RECT getScreenRect();

    cv::Mat& GetGrayMat() { return m_GrayMat; };


private:
    bool _ChangeSize(int width, int height);

    HDC m_hSrcDC, m_hMemDC;
    HBITMAP m_hBitmap , m_hOldBitmap;
    BYTE *m_pBuffer;
    int m_image_nchannels;
    int m_iLastWidth, m_iLastHeight;

    cv::Mat m_GrayMat, m_Mat;

    DEFINE_SINGLEINSTANCE(CScreenCapturer);
};