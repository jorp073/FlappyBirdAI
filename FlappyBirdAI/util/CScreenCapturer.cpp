#include "stdafx.h"
#include "CScreenCapturer.h"


CScreenCapturer* CScreenCapturer::m_pInstance = NULL;


CScreenCapturer::CScreenCapturer()
    :m_hOldBitmap(NULL)
    , m_image_nchannels(0)
    , m_pBuffer(NULL)
    , m_iLastWidth(0)
    , m_iLastHeight(0)
{
}


CScreenCapturer::~CScreenCapturer()
{
    if(NULL != m_pInstance)
    {
	    if (m_pBuffer)
		    delete[] m_pBuffer;
	    if (m_hMemDC)
		    DeleteDC(m_hMemDC);
	    if (m_hSrcDC && m_hBitmap)
		    SelectObject(m_hSrcDC, m_hBitmap);
	    if (m_hSrcDC)
		    DeleteDC(m_hSrcDC);
	    if (m_hBitmap)
		    DeleteObject(m_hBitmap);
    }
}


bool CScreenCapturer::Capture(RECT rect)
{
    m_hSrcDC = ::GetDC(0);
    if (!m_hSrcDC) return false;

    m_hMemDC = ::CreateCompatibleDC(m_hSrcDC);
    if (!m_hMemDC) return false;

    auto width = rect.right - rect.left;
	auto height = rect.bottom - rect.top;

    m_hBitmap = ::CreateCompatibleBitmap(m_hSrcDC, width, height);
    if (!m_hBitmap) return false;

    m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC, m_hBitmap);
    if (!::BitBlt(m_hMemDC, 0, 0, width, height, m_hSrcDC, rect.left, rect.top, SRCCOPY)) return false;

    m_hBitmap = (HBITMAP)::SelectObject(m_hMemDC, m_hOldBitmap);
    BITMAP bmp;
	::GetObject(m_hBitmap, sizeof(BITMAP), &bmp);

    if (!m_Mat.data) {
        if (1 == bmp.bmBitsPixel)
		{
			//("目标窗体最小化，请切换到前面!\n");
			return false;
		}
	}

    // if change width or height, then realloc buff memory
    if (m_iLastWidth != width || m_iLastHeight != height)
    {
        printf("CScreenCapturer::Capture changesize w:%d, h:%d\n", width, height);
        m_iLastWidth = width;
        m_iLastHeight = height;
        if (m_pBuffer)
        {
            delete[] m_pBuffer;
            m_pBuffer = NULL;
        }

		m_image_nchannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
		m_Mat = cv::Mat(cv::Size(width, height), CV_8UC(m_image_nchannels));
        m_pBuffer = new BYTE[width * height * m_image_nchannels];
		if (!m_pBuffer)
        {
			return false;
        }
    }

	::GetBitmapBits(m_hBitmap, width * height * m_image_nchannels, m_pBuffer);
	::memcpy(m_Mat.data, m_pBuffer, width * height * m_image_nchannels);

	cv::cvtColor(m_Mat, m_GrayMat, CV_BGR2GRAY);

	::SelectObject(m_hMemDC, m_hOldBitmap);
	::DeleteObject(m_hOldBitmap);

    return true;
}


RECT CScreenCapturer::getScreenRect()
{
    RECT rect;
    int width = ::GetSystemMetrics(SM_CXSCREEN);
    int height = ::GetSystemMetrics(SM_CYSCREEN);
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    return rect;
}