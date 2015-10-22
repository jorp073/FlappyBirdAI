#pragma once

#include <windows.h>
#include "SingleInstance.h"

class CMouseController
{
public:
    bool ClickInCanvas();

    // Click Left Button In Canvas
    bool Click_LeftButton();

private:
    bool _IsEnableControl();

    bool _IsInCanvas();

    // move mouse
    // x, y is pos in canvas, anchor point is at top left of canvas
    void _MoveTo(int x, int y);

    POINT m_ptCursorPos;

    bool m_bControlling;

    DWORD m_dwStayBeginTime;

    DEFINE_SINGLEINSTANCE(CMouseController);
};