#include "stdafx.h"
#include "../model/define.h"
#include "CObjectObserver.h"
#include "../output/COutputWindow.h"
#include "CCanvasObserver.h"


INIT_SINGLEINSTANCE(CObjectObserver);


CObjectObserver::CObjectObserver()
    : m_matBinary(cv::Mat())
{
}


bool CObjectObserver::Update(cv::Mat matGrayCanvas)
{
    /// Binary gray image
    cv::threshold(matGrayCanvas, m_matBinary, 0, 255, 0);

    /// Dilate and Erode, to get less contours
    int dilation_size = 1;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
        cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
        cv::Point( dilation_size, dilation_size ) );
    cv::dilate(m_matBinary, m_matBinary, element);
    cv::erode(m_matBinary, m_matBinary, element);
    //cv::imshow("bin", mat);

    /// Find all contours
    std::vector<cv::Vec4i> hierarchy;
    findContours(m_matBinary, m_rectContours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    DLOG(INFO) << "findContours count: " << m_rectContours.size();

    /// Find bird contour, and get rect
    m_rectBirds =  GetBirdRects(m_rectContours);

    COutputWindow::GetInstance()->SetBirdRects(m_rectBirds);

    return true;
}


std::vector<cv::Rect> CObjectObserver::GetBirdRects(const std::vector<std::vector<cv::Point>> & rectContours)
{
    std::vector<cv::Rect> rects;
    for (size_t i = 0; i < rectContours.size(); i++)
    {
        cv::Rect rect;
        if (_IsBirdRect(rectContours[i], rect))
        {
            rects.push_back(rect);
        }
    }

    return rects;
}





float CObjectObserver::GetBirdHeight()
{
    CHECK(1 == m_rectBirds.size()) << "m_rectBirds must has singleton value!";

    const int iNoGroundCanvasHeight = CCanvasObserver::GetInstance()->GetNoGroundCanvasHeight();

    float h = (m_rectBirds[0].tl().y + m_rectBirds[0].br().y) / 2.0f;
    return 1 - h/iNoGroundCanvasHeight;
}


float CObjectObserver::GetPipeHeight()
{
    const int iNoGroundCanvasHeight = CCanvasObserver::GetInstance()->GetNoGroundCanvasHeight();

    auto rectPipes =  GetPipeRects(m_rectContours);
    COutputWindow::GetInstance()->SetPipeRects(rectPipes);

    DLOG(INFO) << "rectPipes.size()=" << rectPipes.size();

    float heightInCanvas = (float)iNoGroundCanvasHeight;
    switch (rectPipes.size())
    {
    case 0: // not found pipes
        break;

    case 2: // multi result, sort by x pos
        {
            auto compare = [&](cv::Rect a, cv::Rect b) { return a.tl().x < b.tl().x; };
            std::sort(rectPipes.begin(), rectPipes.end(), compare);
        }

    case 1: // singleton
        heightInCanvas = (float)rectPipes[0].tl().y;

    default:
        break;
    }

    return 1 - heightInCanvas/iNoGroundCanvasHeight;
}


std::vector<cv::Rect> CObjectObserver::GetPipeRects(const std::vector<std::vector<cv::Point>> & rectContours)
{
    std::vector<cv::Rect> rects;
    for (size_t i = 0; i < rectContours.size(); i++)
    {
        cv::Rect rect;
        if (_IsPipeRect(rectContours[i], rect))
        {
            rects.push_back(rect);
        }
    }

    return rects;
}


bool CObjectObserver::_IsBirdRect(const std::vector<cv::Point>& contour, OUT cv::Rect& rect)
{
    auto rectBound = cv::boundingRect(cv::Mat(contour));

    // check rect center x ratio on canvas
    float cx = (rectBound.tl().x + rectBound.br().x)/2.0f;
    float cxratio = cx/CANVAS_SCALETO_WIDTH;
    DLOG(INFO) << "cxratio = " << cxratio;
    if (fabs(cxratio - BIRDX_RATIO) > BIRDX_RATIO_OFFSET) return false;

    // check contour area
    double area = fabs(contourArea(contour));
    DLOG(INFO) << "area= " << area;
    if (fabs(area - BIRD_CONTOUR_AREA) > BIRD_CONTROU_AREA_OFFSET) return false;

    rect = rectBound;
    return true;
}


bool CObjectObserver::_IsPipeRect(const std::vector<cv::Point>& contour, OUT cv::Rect& rect)
{
    static const int PIPE_WIDTH_MAX = CANVAS_SCALETO_WIDTH * 37 / 191;
    static const int PIPE_WIDTH_MIN = 10;
    static const int PIPE_BOTTOM = m_matBinary.rows-1;
    static const int PIPE_BOTTOM_OFFSET = 2;

    CHECK(1 == m_rectBirds.size()) << "m_rectBirds must has singleton value!";

    auto rectBound = cv::boundingRect(cv::Mat(contour));

    DLOG(INFO) << "rectBound: " << rectBound.tl().x << ", " << rectBound.tl().y
        << ", " << rectBound.br().x << ", " << rectBound.br().y;

    // check width
    int width = rectBound.br().x - rectBound.tl().x;
    if (width > PIPE_WIDTH_MAX || width < PIPE_WIDTH_MIN) return false;

    // check bottom
    if (abs(rectBound.br().y - PIPE_BOTTOM) > PIPE_BOTTOM_OFFSET) return false;

    // check in front of bird
    if (rectBound.br().x < m_rectBirds[0].tl().x) return false;

    DLOG(INFO) << "is pipe";
    rect = rectBound;
    return true;
}
