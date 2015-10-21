#include "stdafx.h"
#include "../model/define.h"
#include "CObjectObserver.h"
#include "../output/COutputWindow.h"
#include "CCanvasObserver.h"


INIT_SINGLEINSTANCE(CObjectObserver);


CObjectObserver::CObjectObserver()
    : m_matBinary(cv::Mat())
    , m_fAverageBirdWidth(-1)
    , m_fAverageBirdHeight(-1)
{
    ResetData();
}


void CObjectObserver::ResetData()
{
    m_fPipeHeight = DEFAULT_PIPE_HEIGHT;
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

    /// Find bird rect
    m_rectBirds.clear();
    cv::Rect rect;
    for (auto & contour : m_rectContours)
    {
        if (_IsBirdRect(contour, rect))
        {
            m_rectBirds.push_back(rect);
        }
    }

    COutputWindow::GetInstance()->SetBirdRects(m_rectBirds);

    return true;
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


float CObjectObserver::GetBirdHeight()
{
    CHECK(1 == m_rectBirds.size()) << "m_rectBirds must has singleton value!";

    auto rect = m_rectBirds[0];

    m_fAverageBirdWidth.Append(rect.br().x - rect.tl().x);
    m_fAverageBirdHeight.Append(rect.br().y - rect.tl().y);

    float h = (rect.tl().y + rect.br().y) / 2.0f;
    return 1 - h/CANVAS_NOGROUND_HEIGHT;
}


float CObjectObserver::GetPipeHeight()
{
    auto rectPipes =  GetPipeRects(m_rectContours);
    COutputWindow::GetInstance()->SetPipeRects(rectPipes);

    DLOG(INFO) << "rectPipes.size()=" << rectPipes.size();

    float heightInCanvas = (float)CANVAS_NOGROUND_HEIGHT;
    switch (rectPipes.size())
    {
    case 0: // found on pipes
        // return pipe height of last frame
        break;

    case 2: // multi result, sort by x pos
        {
            auto compare = [&](cv::Rect a, cv::Rect b) { return a.tl().x < b.tl().x; };
            std::sort(rectPipes.begin(), rectPipes.end(), compare);
        }

    case 1: // singleton
        heightInCanvas = (float)rectPipes[0].tl().y;
        m_fPipeHeight = 1 - heightInCanvas/CANVAS_NOGROUND_HEIGHT;

    default:
        break;
    }

    return m_fPipeHeight;
}


SIZE CObjectObserver::GetAverageBirdRect()
{
    SIZE size;
    size.cx = m_fAverageBirdWidth.GetAverageValue();
    size.cy = m_fAverageBirdHeight.GetAverageValue();

    return size;
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
