// FlappyBirdAI.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "fsm/base.h"
#include "util/CScreenCapturer.h"
#include "observers/CCanvasObserver.h"
#include "observers/CGameStateObserver.h"
#include "output/OutputWindow.h"


int _tmain(int argc, _TCHAR* argv[])
{
    FLAGS_log_dir = "../logs/";
    google::InitGoogleLogging("FlappyBirdAI.exe");
    LOG(INFO) << "我在InitInstance()";

    COutputWindow::GetInstance()->Init();
    CGameStateObserver::GetInstance()->Init();
    CCanvasObserver::GetInstance()->Init();

    LOG(INFO) << "start main loop";

    while (true)
    {
        cv::waitKey(1);
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


    CScreenCapturer::GetInstance()->Release();
    CCanvasObserver::GetInstance()->Release();
    COutputWindow::GetInstance()->Release();

    google::ShutdownGoogleLogging();
    return 0;
}

