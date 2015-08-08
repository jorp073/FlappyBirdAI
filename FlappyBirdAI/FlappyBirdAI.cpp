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
    COutputWindow::GetInstance()->Init();
    CGameStateObserver::GetInstance()->Init();
    CCanvasObserver::GetInstance()->Init();

    printf("start main loop\n");
    while (true)
    {
        cv::waitKey(1);
        CCanvasObserver::GetInstance()->Update();
        COutputWindow::GetInstance()->SetCanvasStateText();
        if (!CCanvasObserver::GetInstance()->StateMachine()->IsInState("Found")) continue;

        CGameStateObserver::GetInstance()->Update();
        COutputWindow::GetInstance()->SetGameStateText();
        COutputWindow::GetInstance()->Update();
        if (!CGameStateObserver::GetInstance()->StateMachine()->IsInState("Play")) continue;
        cv::waitKey(1);
    };


    CScreenCapturer::GetInstance()->Release();
    CCanvasObserver::GetInstance()->Release();
    COutputWindow::GetInstance()->Release();
    return 0;
}

