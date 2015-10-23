#include "stdafx.h"
#include "FPSCounter.h"

INIT_SINGLEINSTANCE(CFPSCounter);


CFPSCounter::CFPSCounter()
    : m_iValidFPS(0)
    , m_iCaptureFPS(0)
    , m_iCapturedCount(0)
    , m_iSkippedFrameCount(0)
{
}


CFPSCounter::~CFPSCounter()
{
}


void CFPSCounter::Init()
{
    m_dwTickCount = ::GetTickCount();
}


void CFPSCounter::OnCaptureScreen()
{
    m_iCapturedCount++;
    auto tickcount = GetTickCount();
    if (tickcount - m_dwTickCount > 1000)
    {
        m_iCaptureFPS = m_iCapturedCount;
        m_iValidFPS = m_iCapturedCount - m_iSkippedFrameCount;

        m_iCapturedCount = 0;
        m_iSkippedFrameCount = 0;
        m_dwTickCount = tickcount;
    }
}


void CFPSCounter::OnSkipFrame()
{
    m_iSkippedFrameCount++;
}
