// FlappyBirdAI.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "controller/CCanvasObserver.h"
#include "util/CScreenCapturer.h"


int _tmain(int argc, _TCHAR* argv[])
{
    auto pCanvasObserver = new CCanvasObserver();

    while (true)
    {
        if (!pCanvasObserver->Update())
            break;
    };


    CScreenCapturer::GetInstance()->Release();
	return 0;
}

