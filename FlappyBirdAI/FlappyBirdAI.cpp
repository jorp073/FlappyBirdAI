// FlappyBirdAI.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "fsm/CGameState.h"
#include "util/CScreenCapturer.h"
#include "observers/CCanvasObserver.h"
#include "observers/CGameStateObserver.h"
#include "observers/CObjectObserver.h"
#include "observers/CBirdHeightObserver.h"
#include "util/CPerformanceCounter.h"
#include "output/COutputWindow.h"
#include "recorder/CRecorder.h"
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

    auto llTime = GetPreciseTickCount();

    while (true)
    {
        /// get delta time
        auto llTickCount = GetPreciseTickCount();
        float dt = (float)(llTickCount - llTime);
        llTime = llTickCount;

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
        if (CCanvasObserver::GetInstance()->Update())
        {
            if (CCanvasObserver::GetInstance()->StateMachine()->IsInState("Found"))
            {
                if (CGameStateObserver::GetInstance()->Update())
                {
                    if (CGameStateObserver::GetInstance()->StateMachine()->IsInState("Play"))
                    {
                        CBirdHeightObserver::GetInstance()->Update(dt);
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

