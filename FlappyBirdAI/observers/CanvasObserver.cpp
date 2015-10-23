#include "stdafx.h"
#include "CanvasObserver.h"
#include "../fsm/CanvasObserverState.h"
#include "../util/PerformanceCounter.h"
#include "../output/OutputWindow.h"


DEFINE_COUNTER(CCanvasObserver_SetCanvasMat);

INIT_SINGLEINSTANCE(CCanvasObserver);


CCanvasObserver::CCanvasObserver()
{
}


CCanvasObserver::~CCanvasObserver()
{
    if (m_pStateMachine)
        delete m_pStateMachine;
}


void CCanvasObserver::Init()
{
    m_pStateMachine = new TStateMachine<CCanvasObserver>(this);
    m_pStateMachine->InitState(new CanvasObserverState::CSearch());

    m_matGrayBg = cv::imread("res/graybg.bmp", -1);
    cv::cvtColor(m_matGrayBg, m_matGrayBg, CV_RGB2GRAY);
    assert(m_matGrayBg.cols == CANVAS_SCALETO_WIDTH);

    m_matCanvas = cv::Mat(CANVAS_SCALETO_HEIGHT, CANVAS_SCALETO_WIDTH, CV_8UC1);
}


bool CCanvasObserver::Update(double dt)
{
    if (m_pStateMachine->Update(dt))
    {
        COutputWindow::GetInstance()->SetCanvasStateText();
        return true;
    }
    return false;
}


void CCanvasObserver::SetCanvasMat(cv::Mat graymat)
{
    COUNTER_HELPER(CCanvasObserver_SetCanvasMat);

    /// remove border
    auto rect = _GetRectWithoutBorder(graymat);
    auto noborder = cv::Mat(graymat, rect);

    /// scale to defined size
    cv::resize(noborder, m_matCanvas, cv::Size(CANVAS_SCALETO_WIDTH, CANVAS_SCALETO_HEIGHT));

    /// remove ground
    auto noground = cv::Mat(m_matCanvas, cv::Rect(0, 0, CANVAS_SCALETO_WIDTH, m_matGrayBg.rows));

    /// remove background
    cv::Mat bgmask;
    cv::absdiff(noground, m_matGrayBg, bgmask);
    noground.copyTo(m_matCanvas, bgmask);
}


bool CCanvasObserver::IsBorderColor(uchar color)
{
    return color >= CANVAS_BORDER_COLOR_BEGIN && color <= CANVAS_BORDER_COLOR_END;
}


bool CCanvasObserver::IsCanvasPosMoved(const cv::Mat& mat)
{
    bool ret = !CCanvasObserver::IsBorderColor(mat.at<uchar>(0,0))
        || !CCanvasObserver::IsBorderColor(mat.at<uchar>(mat.rows-1, 0))
        || !CCanvasObserver::IsBorderColor(mat.at<uchar>(0, mat.cols-1))
        || !CCanvasObserver::IsBorderColor(mat.at<uchar>(mat.rows-1, mat.cols-1));

    return ret;
}


cv::Rect CCanvasObserver::_GetRectWithoutBorder(cv::Mat graymat)
{
    assert(graymat.channels() == 1);
    int cx = graymat.cols/2;
    int cy = graymat.rows/2;

    // top
    int top = 0;
    while (IsBorderColor(graymat.at<uchar>(top, cx))
        && top < cy)
    {
        top++;
    }

    // bottom
    int bottom = graymat.rows-1;
    while (IsBorderColor(graymat.at<uchar>(bottom, cx))
        && bottom > 0)
    {
        bottom--;
    }

    // left
    int left = 0;
    while (IsBorderColor(graymat.at<uchar>(cy, left))
        && left < cx) 
    {
        left++;
    }

    // right
    int right = graymat.cols-1;
    while (IsBorderColor(graymat.at<uchar>(cy, right))
        && right > 0)
    {
        right--;
    }

    return cv::Rect(left, top, right-left, bottom-top);
}