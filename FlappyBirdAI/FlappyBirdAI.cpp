// FlappyBirdAI.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "fsm/base.h"
#include "util/CScreenCapturer.h"
#include "controller/CCanvasObserver.h"
#include "controller/CGameStateObserver.h"

int _tmain(int argc, _TCHAR* argv[])
{
    auto pGameStateObserver = new CGameStateObserver();
    pGameStateObserver->Init();

    cv::namedWindow("GrayCanvas");

    while (true)
    {
        cv::waitKey(1);
        CCanvasObserver::GetInstance()->Update();
        if (!CCanvasObserver::GetInstance()->StateMachine()->IsInState("Found")) continue;

        printf("Observer found\n");
        pGameStateObserver->Update();
        //if (!pGameStateObserver->StateMachine()->IsInState("Play")) continue;

        cv::imshow("GrayCanvas", CCanvasObserver::GetInstance()->GetGrayCanvasMat());

        cv::waitKey(1);
        Sleep(1);
    };


    CScreenCapturer::GetInstance()->Release();
    return 0;
}

