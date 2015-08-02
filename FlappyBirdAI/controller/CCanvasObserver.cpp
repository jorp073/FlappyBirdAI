#include "stdafx.h"
#include "CCanvasObserver.h"
#include "../fsm/CCanvasObserverState.h"


CCanvasObserver::CCanvasObserver()
{
    m_pStateMachine = new TStateMachine<CCanvasObserver>(this);
    m_pStateMachine->InitState(new CanvasObserverState::CSearch());
}


bool CCanvasObserver::Update()
{
    return m_pStateMachine->Update();
}


void CCanvasObserver::SetMat(cv::Mat mat)
{
    m_matCanvas = mat.clone();
    cv::cvtColor(m_matCanvas, m_matGrayCanvas, CV_BGR2GRAY);

    _RemoveCanvasBorder();
}


void CCanvasObserver::_RemoveCanvasBorder()
{
    int cx = m_matGrayCanvas.cols/2;
    int cy = m_matGrayCanvas.rows/2;

    // top
    int top = 0;
    while (CANVAS_BORDER_COLOR == m_matGrayCanvas.at<uchar>(top, cx)
        && top < cy)
    {
        top++;
    }

    // bottom
    int bottom = m_matGrayCanvas.rows-1;
    while (CANVAS_BORDER_COLOR == m_matGrayCanvas.at<uchar>(bottom, cx)
        && bottom > 0)
    {
        bottom--;
    }

    // left
    int left = 0;
    while (CANVAS_BORDER_COLOR == m_matGrayCanvas.at<uchar>(cy, left)
        && left < cx) 
    {
        left++;
    }

    // right
    int right = m_matGrayCanvas.cols-1;
    while (CANVAS_BORDER_COLOR == m_matGrayCanvas.at<uchar>(cy, right)
        && right > 0)
    {
        right--;
    }

    auto rect = cv::Rect(left, top, right-left, bottom-top);
    m_matGrayCanvas = cv::Mat(m_matGrayCanvas, rect);
    m_matCanvas = cv::Mat(m_matCanvas, rect);
}