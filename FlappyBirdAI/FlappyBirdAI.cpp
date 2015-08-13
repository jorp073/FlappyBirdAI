// FlappyBirdAI.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "fsm/base.h"
#include "util/CScreenCapturer.h"
#include "observers/CCanvasObserver.h"
#include "observers/CGameStateObserver.h"
#include "observers/CObjectObserver.h"
#include "observers/CBirdHeightObserver.h"
#include "util/CPerformanceCounter.h"
#include "output/COutputWindow.h"

int _tmain(int argc, _TCHAR* argv[])
{
    FLAGS_log_dir = "../logs/";
    google::InitGoogleLogging(argv[0]);

    InitPreciseTickCount();

    COutputWindow::GetInstance()->Init();
    CGameStateObserver::GetInstance()->Init();
    CCanvasObserver::GetInstance()->Init();

    DLOG(INFO) << "start main loop";

    while (true)
    {
        auto key = cv::waitKey(1);
        if (27 == key) break;

        if (CCanvasObserver::GetInstance()->Update())
        {
            if (CCanvasObserver::GetInstance()->StateMachine()->IsInState("Found"))
            {
                if (CGameStateObserver::GetInstance()->Update())
                {
                    if (CGameStateObserver::GetInstance()->StateMachine()->IsInState("Play"))
                    {
                        if (CBirdHeightObserver::GetInstance()->Update(1))
                        {
                        }
                    }
                }
            }
        }
        COutputWindow::GetInstance()->Update();
    };
    DLOG(INFO) << "exit main loop";

    CScreenCapturer::GetInstance()->Release();
    CCanvasObserver::GetInstance()->Release();
    COutputWindow::GetInstance()->Release();

    google::ShutdownGoogleLogging();
    return 0;
}

