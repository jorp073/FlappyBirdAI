#include "stdafx.h"
#include "../model/define.h"
#include "BirdRectObserver.h"
#include "../output/OutputWindow.h"


INIT_SINGLEINSTANCE(CBirdRectObserver);


CBirdRectObserver::CBirdRectObserver()
    : m_fAverageBirdWidth(-1)
    , m_fAverageBirdHeight(-1)
{
}


void CBirdRectObserver::Update(const CV_CONTOURS& rectAllObjContours)
{
    m_rectBirds.clear();
    cv::Rect rect;
    for (auto & contour : rectAllObjContours)
    {
        if (_IsBirdRect(contour, rect))
        {
            m_rectBirds.push_back(rect);
        }
    }

    COutputWindow::GetInstance()->SetBirdRects(m_rectBirds);

    // calculate average bird size
    if (1 == m_rectBirds.size())
    {
        rect = m_rectBirds[0];
        m_fAverageBirdWidth.Append((float)rect.br().x - rect.tl().x);
        m_fAverageBirdHeight.Append((float)rect.br().y - rect.tl().y);
    }
}


float CBirdRectObserver::GetBirdHeight()
{
    CHECK(1 == m_rectBirds.size()) << "m_rectBirds must has singleton value!";

    auto rect = m_rectBirds[0];
    float h = (rect.tl().y + rect.br().y) / 2.0f;
    return 1 - h/CANVAS_NOGROUND_HEIGHT;
}


int CBirdRectObserver::GetBirdLeft()
{
    CHECK(1 == m_rectBirds.size()) << "m_rectBirds must has singleton value!";

    return m_rectBirds[0].tl().x;
}


cv::Size2f CBirdRectObserver::GetAverageBirdSize()
{
    cv::Size2f size;
    size.width = m_fAverageBirdWidth.GetAverageValue();
    size.height = m_fAverageBirdHeight.GetAverageValue();

    return size;
}


bool CBirdRectObserver::_IsBirdRect(const CV_CONTOUR& contour, /*OUT*/ cv::Rect& rect)
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

