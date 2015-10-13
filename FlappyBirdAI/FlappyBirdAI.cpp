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
#include "recorder/CRecorder.h"


int _tmain(int argc, _TCHAR* argv[])
{
    FLAGS_log_dir = "../logs/";
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
        float dt = llTickCount - llTime;
        llTime = llTickCount;

        /// parse key press
        auto key = cv::waitKey(1);
        bool bBreak = false;
        switch (key)
        {
        case VK_ESCAPE:
            bBreak = true;
            break;
        case 2424832: // CV LEFT KEY
            CRecorder::GetInstance()->OnDisplayPreviousFrame();
            bBreak = false;
            break;
        case 2555904: // CV RIGHT KEY
            CRecorder::GetInstance()->OnDisplayNextFrame();
            bBreak = false;
            break;
        default:
            if (-1 != key) std::cout << "press key:" << key << std::endl;
        };
        if (bBreak) break;

        /// observe
        if (CCanvasObserver::GetInstance()->Update())
        {
            if (CCanvasObserver::GetInstance()->StateMachine()->IsInState("Found"))
            {
                if (CGameStateObserver::GetInstance()->Update())
                {
                    if (CGameStateObserver::GetInstance()->StateMachine()->IsInState("Play"))
                    {
                        if (CBirdHeightObserver::GetInstance()->Update(dt))
                        {
                        }
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

