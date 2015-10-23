#pragma once
#include "../util/SingleInstance.h"


class CFPSCounter
{
public:

    void Init();

    void OnCaptureScreen();

    void OnSkipFrame();

    int GetCaptureFPS() { return m_iCaptureFPS; };
    int GetValidFPS() { return m_iValidFPS; };

private:
    int m_iValidFPS, m_iCaptureFPS;
    int m_iCapturedCount, m_iSkippedFrameCount;
    int m_dwTickCount;


    DEFINE_SINGLEINSTANCE(CFPSCounter);
};