// FlappyBirdAI.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "fsm/GameState.h"
#include "util/ScreenCapturer.h"
#include "observers/CanvasObserver.h"
#include "observers/GameStateObserver.h"
#include "observers/ObjectObserver.h"
#include "observers/BirdHeightObserver.h"
#include "observers/PipeObserver.h"
#include "util/PerformanceCounter.h"
#include "util/MouseController.h"
#include "output/OutputWindow.h"
#include "recorder/Recorder.h"
#include "model/FPSCounter.h"
#include <direct.h>


void OnPressKey(int key, /*OUT*/ bool& bExitMainLoop)
{
    bExitMainLoop = false;

    switch (key)
    {
    case VK_ESCAPE:
        if (CGameStateObserver::GetInstance()->StateMachine()->IsInState("PlayBack"))
        {
            // exit PlayBack state, back to play game state
            CGameStateObserver::GetInstance()->StateMachine()->ChangeState(new GameState::CUnknown());
        }
        else
        {
            bExitMainLoop = true;
        }
        break;

    case CV_KEY_LEFT:
        CRecorder::GetInstance()->OnDisplayPreviousFrame();
        break;

    case CV_KEY_RIGHT:
        CRecorder::GetInstance()->OnDisplayNextFrame();
        break;

    default:
        if (-1 != key) std::cout << "press key:" << key << std::endl;
    };
}


void Init(_TCHAR* szModulePath)
{
    FLAGS_log_dir = "logs/";
    ::_mkdir(FLAGS_log_dir.c_str());
    google::InitGoogleLogging(szModulePath);

    InitPreciseTickCount();

    COutputWindow::GetInstance()->Init();
    CGameStateObserver::GetInstance()->Init();
    CCanvasObserver::GetInstance()->Init();
    CFPSCounter::GetInstance()->Init();
}


void Release()
{
    CScreenCapturer::GetInstance()->Release();
    CCanvasObserver::GetInstance()->Release();
    COutputWindow::GetInstance()->Release();

    google::ShutdownGoogleLogging();
}


// main function
int _tmain(int argc, _TCHAR* argv[])
{
    Init(argv[0]);

    DLOG(INFO) << "Start main loop";

    auto dTime = GetPreciseTickCount();

    while (true)
    {
        /// get delta time
        auto dTickCount = GetPreciseTickCount();
        double dt = dTickCount - dTime;
        dTime = dTickCount;

        /// parse key press
        auto key = cv::waitKey(1);
        bool bExitMainLoop;
        OnPressKey(key, bExitMainLoop);
        if (bExitMainLoop) break;

        /// observe
        if (CCanvasObserver::GetInstance()->Update(dt))
        {
            CFPSCounter::GetInstance()->OnCaptureScreen();

            if (CCanvasObserver::GetInstance()->StateMachine()->IsInState("Found"))
            {
                if (CGameStateObserver::GetInstance()->Update(dt))
                {
                    if (CGameStateObserver::GetInstance()->StateMachine()->IsInState("Play"))
                    {
                        // skip frame if pipe right doesn't change
                        if (!CPipeObserver::GetInstance()->IsPipeRightChanged())
                        {
                            CFPSCounter::GetInstance()->OnSkipFrame();
                            continue;
                        }

                        CBirdHeightObserver::GetInstance()->Update(dTickCount);
                    }
                }
            }
        }
        COutputWindow::GetInstance()->Update(dt);
        CRecorder::GetInstance()->PushRecord();
    }; // while (true)

    DLOG(INFO) << "Exit main loop";

    Release();
    return 0;
}

