#include "stdafx.h"
#include "CMouseClicker.h"
#include "CScreenCapturer.h"



bool CMouseClicker::IsInCanvas()
{
    POINT point;
    ::GetCursorPos(&point);

    auto rect = CScreenCapturer::GetInstance()->getScreenRect();
    return point.x > rect.left && point.x < rect.right && point.y < rect.bottom && point.y > rect.top;
}


void CMouseClicker::TryClick()
{
    if (!IsInCanvas()) return;

    Click();
    //::Sleep(100);
}


void CMouseClicker::Click()
{
    DLOG(INFO) << "ai Click";
    ::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    ::mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}