#include "stdafx.h"
#include "CCanvasObserverState.h"
#include "../util/CScreenCapturer.h"
#include "../observers/CCanvasObserver.h"
#include "../util/CPerformanceCounter.h"

using namespace CanvasObserverState;

DEFINE_COUNTER(CSearch_Update);
DEFINE_COUNTER(CFound_Update);

/////////////////////// Search

bool CSearch::Update(CCanvasObserver* observer)
{
    auto rect = CScreenCapturer::GetInstance()->getScreenRect();
    if (!CScreenCapturer::GetInstance()->Capture(rect)) return false;

    COUNTER_HELPER(CSearch_Update);
    auto matGray = CScreenCapturer::GetInstance()->GetGrayMat();

    // filter pix of canvas border
    auto matBorder = _FilterCanvasBorder(matGray);

    cv::Rect rectCanvas;
    if (!_GetCanvasBorderRect(matBorder, rectCanvas)) return false;

    auto GrayMat = cv::Mat(CScreenCapturer::GetInstance()->GetGrayMat(), rectCanvas);
    observer->SetCanvasMat(GrayMat);
    observer->SetCanvasRect(rectCanvas);
    observer->StateMachine()->ChangeState(new CanvasObserverState::CFound());

    return true;
}


cv::Mat CSearch::_FilterCanvasBorder(cv::Mat mat)
{
    static cv::Mat table(1, 256, CV_8U);
    static bool bInitialized = false;

    if (!bInitialized)
    {
        bInitialized = true;

        uchar* p = table.data;
        for (int i = 0; i < CANVAS_BORDER_COLOR_BEGIN; ++i) p[i] = 0;
        for (int i = CANVAS_BORDER_COLOR_BEGIN; i <= CANVAS_BORDER_COLOR_END; ++i) p[i] = 255;
        for (int i = CANVAS_BORDER_COLOR_END+1; i <= 255; ++i) p[i] = 0;
    }

    auto ret = mat.clone();
    LUT(mat, table, ret);
    return ret;
}


bool CSearch::_GetCanvasBorderRect(cv::Mat mat, OUT cv::Rect& rect)
{
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    /// 寻找轮廓
    findContours(mat, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    /// 最大面积的矩形
    int iMaxAreaID = -1;
    double dMaxArea = -1;
    for (size_t i = 0; i < contours.size(); i++)
    {
        auto area = fabs(contourArea(contours[i]));
        if (area < CANVAS_BORDER_MIN_AREA) continue;
        DLOG(INFO) << "canvas area: " << area;

        if (area > dMaxArea)
        {
            dMaxArea = area;
            iMaxAreaID = i;
        }
    }

    if (-1 == iMaxAreaID) // 没有找到面积足够的矩形
    {
        return false;
    }

    // fix crash when click OK on game over state
    // game over frame may change color in border color range
    // so only find canvas in specific area
    if (fabsf(202929 - (float)dMaxArea) > 1e-5)
    {
        return false;
    }

    /// 矩形边界框
    auto boundRect = cv::boundingRect(cv::Mat(contours[iMaxAreaID]));
    rect = boundRect;
    return true;
}


//////////////////////// Found

bool CFound::Update(CCanvasObserver* observer)
{
    COUNTER_HELPER(CFound_Update);

    auto rectCanvas = observer->GetCanvasRect();
    RECT rect = {
        rectCanvas.tl().x,
        rectCanvas.tl().y,
        rectCanvas.br().x,
        rectCanvas.br().y,
    };

    auto bCaptureRet = CScreenCapturer::GetInstance()->Capture(rect);
    if (!bCaptureRet) return false;


    /// if canvas pos is moved, then switch to Search state
    bool bIsMoved = observer->IsCanvasPosMoved(CScreenCapturer::GetInstance()->GetGrayMat());
    if (bIsMoved)
    {
        observer->StateMachine()->ChangeState(
            new CanvasObserverState::CSearch());
        return false;
    }

    observer->SetCanvasMat(CScreenCapturer::GetInstance()->GetGrayMat());
    return true;
}


