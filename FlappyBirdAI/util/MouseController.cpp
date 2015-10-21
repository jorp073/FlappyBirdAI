#include "stdafx.h"
#include "MouseController.h"
#include "../observers/CanvasObserver.h"


INIT_SINGLEINSTANCE(CMouseController);

#define START_CONTROL_DELAYTIME 2


CMouseController::CMouseController()
    : m_bControlling(false)
{
    m_ptCursorPos.x = 0;
    m_ptCursorPos.y = 0;
    m_dwStayBeginTime = GetTickCount();
}


bool CMouseController::_IsInCanvas()
{
    POINT point;
    ::GetCursorPos(&point);

    auto rect = CCanvasObserver::GetInstance()->GetCanvasRect();
    return rect.contains(cv::Point(point.x, point.y));
}


bool CMouseController::_IsEnableControl()
{
    if (!_IsInCanvas())
    {
        m_bControlling = false;
        m_dwStayBeginTime = GetTickCount();
        return false;
    }

    POINT pt;
    ::GetCursorPos(&pt);
    bool bPosChanged = pt.x != m_ptCursorPos.x || pt.y != m_ptCursorPos.y;
    m_ptCursorPos = pt;

    if (m_bControlling)
    {
        if (bPosChanged)
        {
            m_bControlling = false;
            m_dwStayBeginTime = GetTickCount();
            return false;
        }
    }
    else
    {
        if (GetTickCount() - m_dwStayBeginTime > START_CONTROL_DELAYTIME*1000)
        {
            m_bControlling = true;
        }
    }

    return m_bControlling;
}


void CMouseController::ClickInCanvas()
{
    if (_IsEnableControl())
    {
        DLOG(INFO) << "ai Click";
        ::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        ::mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    }
}


void CMouseController::_MoveTo(int x, int y)
{
    if (_IsEnableControl())
    {
        auto rect = CCanvasObserver::GetInstance()->GetCanvasRect();
        m_ptCursorPos.x = rect.tl().x + x;
        m_ptCursorPos.y = rect.tl().y + y;
        ::SetCursorPos(m_ptCursorPos.x, m_ptCursorPos.y);
    }
}


void CMouseController::Click_LeftButton()
{
    _MoveTo(111, 412);
    ClickInCanvas();
}
