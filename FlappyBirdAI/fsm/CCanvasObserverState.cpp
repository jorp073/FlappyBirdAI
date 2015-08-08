#include "stdafx.h"
#include "CCanvasObserverState.h"
#include "../util/CScreenCapturer.h"
#include "../controller/CCanvasObserver.h"


using namespace CanvasObserverState;

/////////////////////// Search

bool CSearch::Update(CCanvasObserver* observer)
{
    // printf("CSearch::Update\n");
    auto rect = CScreenCapturer::GetInstance()->getScreenRect();
    if (!CScreenCapturer::GetInstance()->Capture(rect)) return false;

    auto matGray = CScreenCapturer::GetInstance()->GetGrayMat();

    // filter pix of canvas border
    auto matBorder = _FilterCanvasBorder(matGray);

    cv::Rect rectCanvas;
    if (!_GetCanvasBorderRect(matBorder, rectCanvas)) return false;
    // printf("left2: %d\n", rectCanvas.tl().x);

    auto CanvasMat = cv::Mat(CScreenCapturer::GetInstance()->GetMat(), rectCanvas);
    auto GrayMat = cv::Mat(CScreenCapturer::GetInstance()->GetGrayMat(), rectCanvas);
    observer->SetCanvasMat(CanvasMat, GrayMat);
    observer->StateMachine()->ChangeState(
        new CanvasObserverState::CFound(rectCanvas));

    // printf("CSearch::Update end\n");
    return true;
}


cv::Mat CSearch::_FilterCanvasBorder(cv::Mat mat)
{
    cv::Mat table(1, 256, CV_8U);
    uchar* p = table.data;
    for( int i = 0; i <= 255; ++i)
        p[i] = 0;
    p[CANVAS_BORDER_COLOR] = 255;

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
    // printf("findContours count: %d\n", contours.size());

    /// 最大面积的矩形
    int iMaxAreaID = -1;
    float fMaxArea = -1;
    for (int i = 0; i < contours.size(); i++)
    {
        auto area = fabs(contourArea(contours[i]));
        if (area < CANVAS_BORDER_MIN_AREA) continue;
        printf("area: %.0f\n", area);

        if (area > fMaxArea)
        {
            fMaxArea = area;
            iMaxAreaID = i;
        }
    }

    if (-1 == iMaxAreaID) // 没有找到面积足够的矩形
    {
        // printf("not found canvas border\n");
        return false;
    }

    /// 矩形边界框
    auto boundRect = cv::boundingRect(cv::Mat(contours[iMaxAreaID]));

    /// 绘制包围的矩形框
    //cv::Mat drawing = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    //auto color = cv::Scalar(255, 0, 0);
    //cv::rectangle(drawing, boundRect.tl(), boundRect.br(), color, 1, 8, 0);

    rect = boundRect;
    return true;
}


//////////////////////// Found

bool CFound::Update(CCanvasObserver* observer)
{
    // printf("CFound::Update\n");
    RECT rect = {
        m_CanvasRect.tl().x,
        m_CanvasRect.tl().y,
        m_CanvasRect.br().x,
        m_CanvasRect.br().y,
    };

    auto bCaptureRet = CScreenCapturer::GetInstance()->Capture(rect);
    if (!bCaptureRet) return false;


    /// if canvas pos is moved, then switch to Search state
    bool bIsMoved = isCanvasPosMoved();
    if (bIsMoved)
    {
        observer->StateMachine()->ChangeState(
            new CanvasObserverState::CSearch());
        return false;
    }

    observer->SetCanvasMat(CScreenCapturer::GetInstance()->GetMat(),
        CScreenCapturer::GetInstance()->GetGrayMat());
    // printf("CFound::Update end\n");
    return true;
}


bool CFound::isCanvasPosMoved()
{
    auto mat = CScreenCapturer::GetInstance()->GetGrayMat();

    bool ret = CANVAS_BORDER_COLOR != mat.at<uchar>(0,0)
        || CANVAS_BORDER_COLOR != mat.at<uchar>(mat.rows-1, 0)
        || CANVAS_BORDER_COLOR != mat.at<uchar>(0, mat.cols-1)
        || CANVAS_BORDER_COLOR != mat.at<uchar>(mat.rows-1, mat.cols-1);

    return ret;
}