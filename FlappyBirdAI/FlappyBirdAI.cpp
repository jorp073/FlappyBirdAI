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
    CCanvasObserver::GetInstance()->Init();

    cv::namedWindow("GrayCanvas");
    printf("start main loop\n");

    while (true)
    {
        cv::waitKey(1);
        CCanvasObserver::GetInstance()->Update();
        if (!CCanvasObserver::GetInstance()->StateMachine()->IsInState("Found")) continue;

        // printf("Observer found\n");
        pGameStateObserver->Update();
        //if (!pGameStateObserver->StateMachine()->IsInState("Play")) continue;

        auto mat = CCanvasObserver::GetInstance()->GetCanvasMat();
        cv::imshow("GrayCanvas", CCanvasObserver::GetInstance()->GetGrayCanvasMat());
        //cv::imshow("Canvas", mat);
        //std::vector<cv::Mat> sbgr(mat.channels());
        //cv::split(mat, sbgr);
        //cv::imshow("b", sbgr[0]);
        //cv::imshow("g", sbgr[1]);
        //cv::imshow("r", sbgr[2]);


        cv::waitKey(1);
        Sleep(1);
    };


    CScreenCapturer::GetInstance()->Release();
    return 0;
}

