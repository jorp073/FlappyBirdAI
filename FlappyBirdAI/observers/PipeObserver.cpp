#include "stdafx.h"
#include "../model/define.h"
#include "PipeObserver.h"
#include "../output/OutputWindow.h"


INIT_SINGLEINSTANCE(CPipeObserver);


CPipeObserver::CPipeObserver()
    : m_AveragePipeSpeed(0)
{
    ResetData();
}


void CPipeObserver::ResetData()
{
    m_fPipeHeight = DEFAULT_PIPE_HEIGHT;
    m_fPipeRight = 1;
    m_fLastPipeRight = m_fPipeRight;

    // don't ClearCount()
    // or pipe right will move left when there's no pipe at the beginning on 2nd round.
    m_AveragePipeSpeed.Reset(); 
    DLOG(INFO) << "ai m_AveragePipeSpeed.Reset() " << m_AveragePipeSpeed.GetAverageValue();
}


bool CPipeObserver::Update(const CV_CONTOURS& rectAllObjContours, int iBirdLeft, double dt)
{
    /// pipe rects
    m_lPipeRects.clear();

    for (size_t i = 0; i < rectAllObjContours.size(); i++)
    {
        cv::Rect rect;
        if (_IsPipeRect(rectAllObjContours[i], iBirdLeft, rect))
        {
            m_lPipeRects.push_back(rect);
        }
    }

    COutputWindow::GetInstance()->SetPipeRects(m_lPipeRects);

    /// pipe height and right

    DLOG(INFO) << "ai rectPipes.size()=" << m_lPipeRects.size();

    float heightInCanvas = (float)CANVAS_NOGROUND_HEIGHT;
    switch (m_lPipeRects.size())
    {
    case 0: // found on pipes
        // return pipe height of last frame

        // forecast pipe right by average pipe speed
        m_fPipeRight += (float)(m_AveragePipeSpeed.GetAverageValue() * dt);

        break;

    case 2: // multi result, sort by x pos
        {
            auto compare = [&](cv::Rect a, cv::Rect b) { return a.tl().x < b.tl().x; };
            std::sort(m_lPipeRects.begin(), m_lPipeRects.end(), compare);
        }

    case 1: // singleton
        heightInCanvas = (float)m_lPipeRects[0].tl().y;
        m_fPipeHeight = 1 - heightInCanvas/CANVAS_NOGROUND_HEIGHT;
        m_fPipeRight = (float)(m_lPipeRects[0].br().x) / CANVAS_SCALETO_WIDTH;

        // pipe speed
        if (m_fPipeRight <= m_fLastPipeRight
            && dt>1e-4
            && m_fPipeRight<=(float)(CANVAS_SCALETO_WIDTH-2)/CANVAS_SCALETO_WIDTH)
        {
            auto speed = (m_fPipeRight - m_fLastPipeRight)/dt;

            auto average = m_AveragePipeSpeed.GetAverageValue();
            if (speed < 0
                && average < 0
                && speed < 10 * average)
            {
                // bird rect width changes every frame,
                // passed pipe may become current pipe again,
                // so current speed may be about 100 times of average time
                DLOG(WARNING) << "current pipe speed is too large, because passed pipe becomes current again";
            }
            else
            {
                // update average pipe speed
                m_AveragePipeSpeed.Append(speed);

                DLOG(INFO) << "ai pipe speed: " << speed
                    << " average:" << m_AveragePipeSpeed.GetAverageValue();
            }
        }
    default:
        break;
    }

    m_fLastPipeRight = m_fPipeRight;
    DLOG(INFO) << "ai pipe right to show: " << m_fPipeRight;
    COutputWindow::GetInstance()->SetPipeRight(m_fPipeRight);

    return true;
}


bool CPipeObserver::_IsPipeRect(const CV_CONTOUR& contour, int iBirdLeft, /*OUT*/ cv::Rect& rect)
{
    static const int PIPE_WIDTH_MAX = CANVAS_SCALETO_WIDTH * 37 / 191;
    static const int PIPE_WIDTH_MIN = 10;
    static const int PIPE_BOTTOM = CANVAS_NOGROUND_HEIGHT-1;
    static const int PIPE_BOTTOM_OFFSET = 2;

    auto rectBound = cv::boundingRect(cv::Mat(contour));

    DLOG(INFO) << "rectBound: " << rectBound.tl().x << ", " << rectBound.tl().y
        << ", " << rectBound.br().x << ", " << rectBound.br().y;

    // check width
    int width = rectBound.br().x - rectBound.tl().x;
    if (width > PIPE_WIDTH_MAX || width < PIPE_WIDTH_MIN) return false;

    // check bottom
    if (abs(rectBound.br().y - PIPE_BOTTOM) > PIPE_BOTTOM_OFFSET) return false;

    // check in front of bird
    if (rectBound.br().x < iBirdLeft) return false;

    DLOG(INFO) << "is pipe";
    rect = rectBound;
    return true;
}
