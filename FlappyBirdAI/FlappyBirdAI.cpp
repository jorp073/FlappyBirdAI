// FlappyBirdAI.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "fsm/GameState.h"
#include "util/ScreenCapturer.h"
#include "observers/CanvasObserver.h"
#include "observers/GameStateObserver.h"
#include "observers/ObjectObserver.h"
#include "observers/BirdHeightObserver.h"
#include "util/PerformanceCounter.h"
#include "util/MouseController.h"
#include "output/OutputWindow.h"
#include "recorder/Recorder.h"
#include <direct.h>


int _tmain(int argc, _TCHAR* argv[])
{
    FLAGS_log_dir = "logs/";
    ::_mkdir(FLAGS_log_dir.c_str());
    google::InitGoogleLogging(argv[0]);

    InitPreciseTickCount();

    COutputWindow::GetInstance()->Init();
    CGameStateObserver::GetInstance()->Init();
    CCanvasObserver::GetInstance()->Init();

    DLOG(INFO) << "start main loop";

    auto dTime = GetPreciseTickCount();

    while (true)
    {
        /// get delta time
        auto dTickCount = GetPreciseTickCount();
        double dt = dTickCount - dTime;
        dTime = dTickCount;

        /// parse key press
        auto key = cv::waitKey(1);
        if (VK_ESCAPE == key)
        {
            if (CGameStateObserver::GetInstance()->StateMachine()->IsInState("PlayBack"))
            {
                // exit PlayBack, back to auto play
                CGameStateObserver::GetInstance()->StateMachine()->ChangeState(new GameState::CUnknown());
            }
            else
            {
                break;
            }
        }

        switch (key)
        {
        case 2424832: // CV LEFT KEY
            CRecorder::GetInstance()->OnDisplayPreviousFrame();
            break;
        case 2555904: // CV RIGHT KEY
            CRecorder::GetInstance()->OnDisplayNextFrame();
            break;
        default:
            if (-1 != key) std::cout << "press key:" << key << std::endl;
        };

        /// observe
        if (CCanvasObserver::GetInstance()->Update(dt))
        {
            if (CCanvasObserver::GetInstance()->StateMachine()->IsInState("Found"))
            {
                if (CGameStateObserver::GetInstance()->Update(dt))
                {
                    auto state = CGameStateObserver::GetInstance()->StateMachine()->CurrentState()->GetName();
                    if ("Play" == state)
                    {
                        CBirdHeightObserver::GetInstance()->Update(dTickCount);
                    }
                    else if ("Title" == state)
                    {
                        ::Sleep(200);
                        CMouseController::GetInstance()->Click_LeftButton();
                    }
                    else if ("GetReady" == state)
                    {
                        ::Sleep(200);
                        CMouseController::GetInstance()->ClickInCanvas();
                    }
                    else if ("GameOver" == state)
                    {
                        ::Sleep(300);
                        CMouseController::GetInstance()->Click_LeftButton();
                    }
                }
            }
        }
        COutputWindow::GetInstance()->Update();
        CRecorder::GetInstance()->PushRecord();
    };
    DLOG(INFO) << "exit main loop";

    CScreenCapturer::GetInstance()->Release();
    CCanvasObserver::GetInstance()->Release();
    COutputWindow::GetInstance()->Release();

    google::ShutdownGoogleLogging();
    return 0;
}

