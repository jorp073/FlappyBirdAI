#include "stdafx.h"
#include "COutputWindow.h"
#include "../observers/CCanvasObserver.h"
#include "../observers/CGameStateObserver.h"
#include "../fsm/base.h"

INIT_SINGLEINSTANCE(COutputWindow);


COutputWindow::COutputWindow()
    : m_iFPS(0)
    , m_fPipeHeight(0)
{
}


COutputWindow::~COutputWindow()
{
}


bool COutputWindow::Init()
{
    cv::namedWindow("Main Output");
    TopMostWindow("Main Output");

    m_dwTickCount = GetTickCount();
    return true;
}


void COutputWindow::SetCanvasStateText()
{
    m_strCanvasStateText = "Canvas: " +
        CCanvasObserver::GetInstance()->StateMachine()->CurrentState()->GetName();
}


void COutputWindow::SetGameStateText()
{
    m_strGameStateText = "GameState: " +
        CGameStateObserver::GetInstance()->StateMachine()->CurrentState()->GetName();
}


void COutputWindow::SetBirdRects(const std::vector<cv::Rect> rects)
{
    m_rectBirds = rects;
}


void COutputWindow::Update()
{
    auto mat = CCanvasObserver::GetInstance()->GetCanvasMat();
    if (NULL == mat.data) return;
    mat = mat.clone();

    /// count fps
    TickCountFPS();

    /// draw text
    DrawText(mat, m_strCanvasStateText, 15);
    DrawText(mat, m_strGameStateText, 35);
    DrawText(mat, m_strFPSText, mat.rows - 10);

    auto color = cv::Scalar(255, 255, 255);
    /// draw birds rect
    for (auto rect : m_rectBirds)
    {
        cv::rectangle(mat, rect.tl(), rect.br(), color, 1, 8, 0);
    }

    /// draw pipe height line
    cv::line(mat, cv::Point(0, m_fPipeHeight), cv::Point(mat.cols-1, m_fPipeHeight), color);

    cv::imshow("Main Output", mat);

    m_strCanvasStateText.clear();
    m_strGameStateText.clear();
}


void COutputWindow::TopMostWindow(const std::string& strWindowName)
{
    HWND hWnd = (HWND)cvGetWindowHandle(strWindowName.c_str());
    if (hWnd) {
        HWND hRawWnd = ::GetParent(hWnd);
        if (hRawWnd != NULL) {
            SetWindowPos(hRawWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE |SWP_NOMOVE);
        }
    }
}


void COutputWindow::DrawText(cv::Mat mat, const std::string& strText, int iHeight)
{
    if (0 == strText.size()) return;
    cv::putText(mat, strText, cv::Point(10, iHeight), CV_FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar(255,255,255));
}


void COutputWindow::TickCountFPS()
{
    m_iFPS++;
    auto tickcount = GetTickCount();
    if (tickcount - m_dwTickCount > 1000)
    {
        m_dwTickCount = tickcount;
        std::stringstream stmFPS;
        stmFPS << m_iFPS;
        m_iFPS = 0;
        m_strFPSText = "FPS: " + stmFPS.str();
    }
}
