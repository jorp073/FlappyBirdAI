#include "stdafx.h"
#include "COutputWindow.h"
#include "../observers/CCanvasObserver.h"
#include "../observers/CGameStateObserver.h"
#include "../fsm/base.h"
#include "../recorder/CRecorder.h"


INIT_SINGLEINSTANCE(COutputWindow);


COutputWindow::COutputWindow()
    : m_iFPS(0)
    , m_fPipeHeight(1)
    , m_pRecorder(CRecorder::GetInstance())
{
}


COutputWindow::~COutputWindow()
{
}


bool COutputWindow::Init()
{
    cv::namedWindow(WINDOW_NAME_CANVAS);
    TopMostWindow(WINDOW_NAME_CANVAS);

    cv::namedWindow(WINDOW_NAME_PARABOLA);
    TopMostWindow(WINDOW_NAME_PARABOLA);

    m_dwTickCount = ::GetTickCount();
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


void COutputWindow::SetBirdRects(const std::vector<cv::Rect>& rects)
{
    m_rectBirds = rects;
}


void COutputWindow::SetPipeRects(const std::vector<cv::Rect>& rects)
{
    m_rectPipes = rects;
}


void COutputWindow::DrawParabola(
    std::vector<PARABOLA_POINT>& points,
    int iOutputWindowWidth,
    float fRemainCrashTime,
    double dHeight,
    bool bClick)
{
    if ("PlayBack" == CGameStateObserver::GetInstance()->StateMachine()->CurrentState()->GetName()) return;

    iOutputWindowWidth = iOutputWindowWidth < PARABOLA_GRAPH_W ? PARABOLA_GRAPH_W : iOutputWindowWidth;

    m_matParabola = cv::Mat(PARABOLA_GRAPH_H, iOutputWindowWidth, CV_8UC3, cv::Scalar(0, 0, 0));
    for (auto& point : points)
    {
        if (0 == point.type)
        {
            // collected original data
            cv::circle(m_matParabola, cv::Point(point.x, point.y), 7, cv::Scalar(0, 255, 0));
        }
    }

    for (auto& point : points)
    {
        if (1 == point.type)
        {
            // fit parabola
            cv::circle(m_matParabola, cv::Point(point.x, point.y), 2, cv::Scalar(0, 0, 255), -1);
        }
    }

    std::stringstream ssForecast;
    ssForecast << "Crash Forecast: " << fRemainCrashTime;
    CVDrawText(m_matParabola, ssForecast.str(), 15);

    std::stringstream ssHeight;
    ssHeight << "Bird Height: " << dHeight;
    CVDrawText(m_matParabola, ssHeight.str(), 35);

    if (bClick)
    {
        CVDrawText(m_matParabola, "Jump Now!", 55);
    }


    cv::imshow(WINDOW_NAME_PARABOLA, m_matParabola);

    // record parabola
    m_pRecorder->RecordParabola(m_matParabola);
}


void COutputWindow::Update()
{
    if ("PlayBack" == CGameStateObserver::GetInstance()->StateMachine()->CurrentState()->GetName()) return;

    auto mat = CCanvasObserver::GetInstance()->GetCanvasMat();
    if (NULL == mat.data) return;
    mat = mat.clone();

    /// count fps
    TickCountFPS();

    /// draw text
    CVDrawText(mat, m_strCanvasStateText, 15);
    CVDrawText(mat, m_strGameStateText, 35);
    CVDrawText(mat, m_strFPSText, mat.rows - 10);

    const auto cWhite = cv::Scalar(255, 255, 255);
    const auto cGray = cv::Scalar(127, 127, 127);
    /// draw birds rect
    for (auto rect : m_rectBirds)
    {
        cv::rectangle(mat, rect.tl(), rect.br(), cWhite, 1, 8, 0);
    }

    /// draw pipe rect
    for (auto rect : m_rectPipes)
    {
        cv::rectangle(mat, rect.tl(), rect.br(), cGray, CV_FILLED, 8, 0);
    }

    /// draw pipe height line
    const int iNoGroundCanvasHeight = CCanvasObserver::GetInstance()->GetNoGroundCanvasHeight();
    int height = (int)((1-m_fPipeHeight) * iNoGroundCanvasHeight + 0.5f);
    height = height < 0 ? 0 : height;
    height = height > mat.rows-1 ? mat.rows-1 : height;
    cv::line(mat, cv::Point(0, height), cv::Point(mat.cols-1, height), cWhite);

    cv::imshow(WINDOW_NAME_CANVAS, mat);
    
    // record canvas
    m_pRecorder->RecordCanvas(mat);

    m_strCanvasStateText.clear();
    m_strGameStateText.clear();

    // clear data
    m_rectPipes.clear();
    m_rectBirds.clear();
    m_fPipeHeight = 1;
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


void COutputWindow::CVDrawText(cv::Mat mat, const std::string& strText, int iHeight)
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
