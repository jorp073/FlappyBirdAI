#include "stdafx.h"
#include "CCanvasObserver.h"
#include "../fsm/CCanvasObserverState.h"

CCanvasObserver* CCanvasObserver::m_pInstance = NULL;


CCanvasObserver::CCanvasObserver()
{
}


CCanvasObserver::~CCanvasObserver()
{
    if (m_pStateMachine)
        delete m_pStateMachine;
}


bool CCanvasObserver::Init()
{
    m_pStateMachine = new TStateMachine<CCanvasObserver>(this);
    m_pStateMachine->InitState(new CanvasObserverState::CSearch());

    m_matGrayBg = cv::imread("res/graybg.bmp", -1);
    cv::cvtColor(m_matGrayBg, m_matGrayBg, CV_RGBA2GRAY);
    assert(m_matGrayBg.cols == CANVAS_SCALETO_WIDTH);
    return true;
}


bool CCanvasObserver::Update()
{
    return m_pStateMachine->Update();
}


void CCanvasObserver::SetCanvasMat(cv::Mat mat, cv::Mat graymat)
{
    /// remove border
    auto rect = _GetRectWithoutBorder(graymat);
    auto noborder = cv::Mat(mat, rect);

    /// scale to defined size
    m_matCanvas = cv::Mat(CANVAS_SCALETO_HEIGHT, CANVAS_SCALETO_WIDTH, CV_8UC4);
    cv::resize(noborder, m_matCanvas, cv::Size(CANVAS_SCALETO_WIDTH, CANVAS_SCALETO_HEIGHT));
    m_matCanvas = cv::Mat(m_matCanvas, cv::Rect(0, 0, CANVAS_SCALETO_WIDTH, m_matGrayBg.rows));
    
    /// remove background
    cv::cvtColor(m_matCanvas, m_matGrayCanvas, CV_BGR2GRAY);
    cv::Mat bgmask;
    cv::absdiff(m_matGrayCanvas, m_matGrayBg, bgmask);
    m_matGrayCanvas.copyTo(bgmask, bgmask);
    m_matGrayCanvas = bgmask;
}


cv::Rect CCanvasObserver::_GetRectWithoutBorder(cv::Mat graymat)
{
    assert(graymat.channels() == 1);
    int cx = graymat.cols/2;
    int cy = graymat.rows/2;

    // top
    int top = 0;
    while (CANVAS_BORDER_COLOR == graymat.at<uchar>(top, cx)
        && top < cy)
    {
        top++;
    }

    // bottom
    int bottom = graymat.rows-1;
    while (CANVAS_BORDER_COLOR == graymat.at<uchar>(bottom, cx)
        && bottom > 0)
    {
        bottom--;
    }

    // left
    int left = 0;
    while (CANVAS_BORDER_COLOR == graymat.at<uchar>(cy, left)
        && left < cx) 
    {
        left++;
    }

    // right
    int right = graymat.cols-1;
    while (CANVAS_BORDER_COLOR == graymat.at<uchar>(cy, right)
        && right > 0)
    {
        right--;
    }

    return cv::Rect(left, top, right-left, bottom-top);
}