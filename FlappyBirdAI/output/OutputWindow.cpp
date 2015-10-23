#include "stdafx.h"
#include "OutputWindow.h"
#include "../observers/CanvasObserver.h"
#include "../observers/GameStateObserver.h"
#include "../fsm/base.h"
#include "../recorder/Recorder.h"
#include "../model/JumpRangeModel.h"
#include "../model/ClickDelayModel.h"

#define JUMP_NOW_FADEOUT_TIME 0.8


INIT_SINGLEINSTANCE(COutputWindow);


COutputWindow::COutputWindow()
    : m_iFPS(0)
    , m_fPipeHeight(1)
    , m_pRecorder(CRecorder::GetInstance())
    , m_cClickTextColor(0)
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

    cv::namedWindow(WINDOW_NAME_JUMPRANGE);
    TopMostWindow(WINDOW_NAME_JUMPRANGE);

    cv::namedWindow(WINDOW_NAME_CLICKDELAY);
    TopMostWindow(WINDOW_NAME_CLICKDELAY);

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
    float fRemainCollisionTime,
    double dHeight,
    bool bClick)
{
    if (CGameStateObserver::GetInstance()->StateMachine()->IsInState("PlayBack")) return;

    m_matParabola = cv::Mat(PARABOLA_GRAPH_H, PARABOLA_GRAPH_W, CV_8UC3, cv::Scalar(0, 0, 0));
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

    std::stringstream ssHeight;
    ssHeight << "Bird Height: " << dHeight;
    CVDrawText(m_matParabola, ssHeight.str(), 15);

    if (0 != fRemainCollisionTime)
    {
        std::stringstream ssForecast;
        ssForecast << "Will Collision In: " << fRemainCollisionTime;
        CVDrawText(m_matParabola, ssForecast.str(), 35);
    }

    if (bClick) m_cClickTextColor = 255;
    if (m_cClickTextColor > 0)
    {
        auto color = m_cClickTextColor == 255 ? 255 : m_cClickTextColor/2;
        CVDrawText(m_matParabola, "Jump Now!", 55, color);
    }

    cv::imshow(WINDOW_NAME_PARABOLA, m_matParabola);

    // record parabola
    m_pRecorder->RecordParabola(m_matParabola);
}


void COutputWindow::DrawJumpRange(CJumpRangeModel* pModel)
{
    cv::Mat mat(JUMPRANGE_GRAPH_H, JUMPRANGE_GRAPH_W, CV_8UC1, cv::Scalar(0));

    auto lData = pModel->GetRangeData();
    int x = 0, iTop, iBottom;
    for (auto & data : lData)
    {
        if (x >= JUMPRANGE_GRAPH_W-1) break;

        iTop = (int)((1 - data.fTop/PIPE_VERTICAL_DISTANCE) * JUMPRANGE_GRAPH_H + 0.5f);
        if (iTop > JUMPRANGE_GRAPH_H-1) iTop = JUMPRANGE_GRAPH_H-1;
        if (iTop < 0) iTop = 0;

        iBottom = (int)((1 - data.fBottom/PIPE_VERTICAL_DISTANCE) * JUMPRANGE_GRAPH_H + 0.5f);
        if (iBottom > JUMPRANGE_GRAPH_H-1) iBottom = JUMPRANGE_GRAPH_H-1;
        if (iBottom < 0) iBottom = 0;

        cv::line(mat, cv::Point(x, iBottom), cv::Point(x, iTop), cv::Scalar(192));

        x++;
    }

    // draw best bottom offset line
    int fBestBottomOffset = (int)((1 - pModel->GetBestBottomOffset()/PIPE_VERTICAL_DISTANCE) * JUMPRANGE_GRAPH_H + 0.5f);
    cv::line(mat, cv::Point(0, fBestBottomOffset), cv::Point(JUMPRANGE_GRAPH_W-1, fBestBottomOffset), cv::Scalar(255));

    // draw center line
    cv::line(
        mat,
        cv::Point(0, JUMPRANGE_GRAPH_H/2),
        cv::Point(JUMPRANGE_GRAPH_W-1, JUMPRANGE_GRAPH_H/2),
        cv::Scalar(64));

    CVDrawText(mat, "Best Bottom Offset:", 15);
    std::stringstream ss;
    ss << pModel->GetBestBottomOffset();
    CVDrawText(mat, ss.str(), 35);

    cv::imshow(WINDOW_NAME_JUMPRANGE, mat);
}


void COutputWindow::DrawClickDelay(CClickDelayModel* pModel)
{
    auto lRemainTime = pModel->GetRemainCollisionTimeList();
    auto lBottomOffset = pModel->GetBottomOffsetList();

#define CLICKDELAY_WINDOW_W 200
#define CLICKDELAY_WINDOW_H 200
#define BASEX (CLICKDELAY_WINDOW_W/4)
#define BASEY (CLICKDELAY_WINDOW_H*3/4)
#define XRATIO 2
#define YRATIO 800

    cv::Mat mat(CLICKDELAY_WINDOW_H, CLICKDELAY_WINDOW_W, CV_8UC1, cv::Scalar(0));
    cv::line(mat, cv::Point(0, BASEY), cv::Point(CLICKDELAY_WINDOW_W, BASEY), cv::Scalar(127));
    cv::line(mat, cv::Point(BASEX, 0), cv::Point(BASEX, CLICKDELAY_WINDOW_H), cv::Scalar(127));

    const auto PX = [](double x)
    {
        return (int)(x*XRATIO + BASEX + 0.5f);
    };

    const auto PY = [](float y)
    {
        return (int)(y*YRATIO + BASEY + 0.5f);
    };

    int x, y;
    const size_t size = lRemainTime.size();
    for (size_t count = 0; count < size; count++)
    {
        x = PX(lRemainTime[count]);
        y = PY(lBottomOffset[count]);

        if (x > CLICKDELAY_WINDOW_W-1) x = CLICKDELAY_WINDOW_W-1;
        else if (x < 0) x = 0;

        if (y > CLICKDELAY_WINDOW_H-1) y = CLICKDELAY_WINDOW_H-1;
        else if (y < 0) y = 0;

        mat.at<unsigned char>(y, x) = 255;

        count++;
        if (count == size)
        {
            // draw circle for the latest data
            cv::circle(mat, cv::Point(x, y), 5, cv::Scalar(192), 1);
        }
    }

    // draw line
    if (size > 1)
    {
        double k, b;
        pModel->GetKB(k, b);

        auto x1 = -CLICKDELAY_WINDOW_W;
        auto y1 = (float)(k*x1+b);
        auto x2 = CLICKDELAY_WINDOW_W;
        auto y2 = (float)(k*x2+b);

        cv::line(mat, cv::Point(PX(x1), PY(y1)), cv::Point(PX(x2), PY(y2)), cv::Scalar(255));
    }

    // draw text
    std::stringstream ssDelay;
    ssDelay << "Click Delay: " << pModel->GetClickDelay();
    CVDrawText(mat, ssDelay.str(), 15);

    auto bestdelay = pModel->GetBestDelayTime();
    if (0 != bestdelay)
    {
        std::stringstream ssBest;
        ssBest << "Best: " << bestdelay;
        CVDrawText(mat, ssBest.str(), 35);
    }

    cv::imshow(WINDOW_NAME_CLICKDELAY, mat);

    // record ClickDelay graph for playback
    CRecorder::GetInstance()->RecordClickDelay(mat);
}


void COutputWindow::Update(double dt)
{
    if (CGameStateObserver::GetInstance()->StateMachine()->IsInState("PlayBack")) return;

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

    /// draw pipe height and right cornor
    int height = (int)((1-m_fPipeHeight) * CANVAS_NOGROUND_HEIGHT + 0.5f);
    height = height < 0 ? 0 : height;
    height = height > mat.rows-1 ? mat.rows-1 : height;

    int right = (int)(m_fPipeRight * CANVAS_SCALETO_WIDTH + 0.5f);
    height = height < 0 ? 0 : height;
    height = height > mat.cols-1 ? mat.cols-1 : height;

    cv::line(mat, cv::Point(0, height), cv::Point(right, height), cWhite);
    cv::line(mat, cv::Point(right, mat.rows-1), cv::Point(right, height), cWhite);

    cv::imshow(WINDOW_NAME_CANVAS, mat);
    
    // record canvas
    m_pRecorder->RecordCanvas(mat);

    m_strCanvasStateText.clear();
    m_strGameStateText.clear();

    // clear data
    m_rectPipes.clear();
    m_rectBirds.clear();
    m_fPipeHeight = 0;
    m_fPipeRight = 1;

    // fadeout Jump Now text
    unsigned char delta = 255*dt/1000/JUMP_NOW_FADEOUT_TIME;
    m_cClickTextColor = m_cClickTextColor <= delta ? 0 : m_cClickTextColor - delta;
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


void COutputWindow::CVDrawText(cv::Mat mat, const std::string& strText, int iHeight, unsigned char cColor /*= 255*/)
{
    if (0 == strText.size()) return;
    cv::putText(mat, strText, cv::Point(10, iHeight), CV_FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar(cColor,cColor,cColor));
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
