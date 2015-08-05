#include "stdafx.h"
#include "CGameState.h"
#include "../controller/CGameStateObserver.h"
#include "../controller/CCanvasObserver.h"


using namespace GameState;

/////////////////////// CBase

double CBase::_GetMatchResult(cv::Mat mat, cv::Mat templ, OUT cv::Point& matchLoc)
{
    int result_cols =  mat.cols - templ.cols + 1;
    int result_rows = mat.rows - templ.rows + 1;
    auto result = cv::Mat(result_rows, result_cols, CV_32FC1);

    /// match
    auto match_method = CV_TM_SQDIFF_NORMED; // notice minVal
    matchTemplate(mat, templ, result, match_method );
    // imshow("match", result);

    /// locate match position
    double minVal, maxVal, finalVal;
    cv::Point minLoc, maxLoc;

    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

    if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
    {
        matchLoc = minLoc;
        finalVal = minVal;
    }
    else
    {
        matchLoc = maxLoc; 
        finalVal = maxVal;
    }
    return finalVal;
}


bool CBase::IsInTitleState(CGameStateObserver* observer)
{
    auto mat = CCanvasObserver::GetInstance()->GetGrayCanvasMat();
    auto templ = observer->GetTemplateMat("title");
    cv::Point pt;
    auto value = _GetMatchResult(mat, templ, pt);

    // title state:     0.0 - 0.57
    // get ready state: 1.0
    // play state:      1.0
    // game over state: 1.0
    // printf("IsInTitleState value= %f\n", value);
    return value < 0.7;
}


bool CBase::IsInGetReadyState(CGameStateObserver* observer)
{
    auto mat = CCanvasObserver::GetInstance()->GetGrayCanvasMat();
    auto templ = observer->GetTemplateMat("getready");
    cv::Point pt;
    auto value = _GetMatchResult(mat, templ, pt);

    // title state:     1.0
    // get ready state: 0.0 - 0.53
    // play state:      1.0
    // game over state: 1.0
    // printf("IsInGetReadyState value= %f\n", value);
    return value < 0.6;
}


bool CBase::IsInGameOverState(CGameStateObserver* observer)
{
    auto mat = CCanvasObserver::GetInstance()->GetGrayCanvasMat();
    auto templ = observer->GetTemplateMat("gameover");
    cv::Point pt;
    auto value = _GetMatchResult(mat, templ, pt);

    // title state:     1.0
    // get ready state: 1.0
    // play state:      1.0
    // game over state: 0.0 - 0.13
    // printf("IsInGameOverState value= %f\n", value);
    return value < 0.5;
}


bool CBase::IsInPlayState(CGameStateObserver* observer)
{
    auto mat = CCanvasObserver::GetInstance()->GetGrayCanvasMat();


    
    auto templ1 = observer->GetTemplateMat("play1");
    auto templ2 = observer->GetTemplateMat("play2");
    auto templ3 = observer->GetTemplateMat("play3");
    auto templ4 = observer->GetTemplateMat("play4");
    auto templ5 = observer->GetTemplateMat("play5");
    cv::Point p1, p2, p3, p4, p5;
    auto value1 = _GetMatchResult(mat, templ1, p1);
    auto value2 = _GetMatchResult(mat, templ2, p2);
    auto value3 = _GetMatchResult(mat, templ3, p3);
    auto value4 = _GetMatchResult(mat, templ4, p4);
    auto value5 = _GetMatchResult(mat, templ5, p5);

    auto fp = p1;
    if (value2 < value1)
    {
        double t;
        t=value1; value1=value2; value2=t;
        fp = p2;
    }
    if (value3 < value1)
    {
        double t;
        t=value1; value1=value3; value3=t;
        fp = p3;
    }
    if (value4 < value1)
    {
        double t;
        t=value1; value1=value4; value4=t;
        fp = p4;
    }
    if (value5 < value1)
    {
        double t;
        t=value1; value1=value5; value5=t;
        fp = p5;
    }
    // play state:      0.11
    printf("IsInPlayState value=%f, %d,%d\n", value1, fp.x, fp.y);
    return 47 < fp.x && fp.x < 55;
}


/////////////////////// CUnknown


bool CUnknown::Update(CGameStateObserver* observer)
{
    // printf("CUnknown::Update()\n");
    if(IsInGetReadyState(observer))
    {
        observer->StateMachine()->ChangeState(new CGetReady());
    }

    else if(IsInGameOverState(observer))
    {
        observer->StateMachine()->ChangeState(new CGameOver());
    }

    else if (IsInTitleState(observer))
    {
        observer->StateMachine()->ChangeState(new CTitle());
    }

    else if(IsInPlayState(observer))
    {
        observer->StateMachine()->ChangeState(new CPlay());
    }

    return true;
}


//////////////////////// Title

bool CTitle::Update(CGameStateObserver* observer)
{
    if (!IsInTitleState(observer))
    {
        observer->StateMachine()->ChangeState(new CUnknown());
    }
    return true;
}


//////////////////////// Get Ready

bool CGetReady::Update(CGameStateObserver* observer)
{
    if (!IsInGetReadyState(observer))
    {
        observer->StateMachine()->ChangeState(new CUnknown());
    }
    return true;
}


//////////////////////// Game Over

bool CGameOver::Update(CGameStateObserver* observer)
{
    if (!IsInGameOverState(observer))
    {
        observer->StateMachine()->ChangeState(new CUnknown());
    }
    return true;
}


//////////////////////// Play

bool CPlay::Update(CGameStateObserver* observer)
{
    if (!IsInPlayState(observer))
    {
        observer->StateMachine()->ChangeState(new CUnknown());
        imshow("exit play", CCanvasObserver::GetInstance()->GetGrayCanvasMat());
    }
    return true;
}

