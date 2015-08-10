// FlappyBirdAI.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "fsm/base.h"
#include "util/CScreenCapturer.h"
#include "observers/CCanvasObserver.h"
#include "observers/CGameStateObserver.h"
#include "output/OutputWindow.h"
#include "util/CPerformanceCounter.h"


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

        CCanvasObserver::GetInstance()->Update();
        COutputWindow::GetInstance()->SetCanvasStateText();
        if (CCanvasObserver::GetInstance()->StateMachine()->IsInState("Found"))
        {
            CGameStateObserver::GetInstance()->Update();
            COutputWindow::GetInstance()->SetGameStateText();
            if (CGameStateObserver::GetInstance()->StateMachine()->IsInState("Play"))
            {
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

