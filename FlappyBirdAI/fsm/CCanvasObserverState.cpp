#include "stdafx.h"
#include "CCanvasObserverState.h"
#include "../util/CScreenCapturer.h"
#include "../controller/CCanvasObserver.h"
#include "opencv2/highgui/highgui.hpp"


using namespace CanvasObserverState;

/////////////////////// Search

bool CSearch::Update(CCanvasObserver* observer)
{
    auto rect = CScreenCapturer::GetInstance()->getScreenRect();
    if (!CScreenCapturer::GetInstance()->Capture(rect)) return false;

    bool bFound = SearchCanvas(CScreenCapturer::GetInstance()->getMat());
    if (bFound)
    {
        observer->StateMachine()->ChangeState(new CanvasObserverState::CFound());
    }
    return bFound;
}


bool CSearch::SearchCanvas(cv::Mat mat)
{
    cv::imshow("screen", mat);
    while (true)
        cv::waitKey(10);
    return true;
}

//////////////////////// Found

bool CFound::Update(CCanvasObserver* observer)
{
    return true;
}
