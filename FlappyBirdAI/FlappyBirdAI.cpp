// FlappyBirdAI.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "controller/CCanvasObserver.h"
#include "util/CScreenCapturer.h"


int _tmain(int argc, _TCHAR* argv[])
{
    auto pCanvasObserver = new CCanvasObserver();

    cv::namedWindow("canvas");

    while (true)
    {
        while (!pCanvasObserver->Update())
        {
            cv::waitKey(100);
        }

        printf("Observer found\n");

        cv::imshow("canvas", pCanvasObserver->GetCanvasMat());

        cv::waitKey(10);
    };


    CScreenCapturer::GetInstance()->Release();
	return 0;
}

