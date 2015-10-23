#include "stdafx.h"
#include "GameState.h"
#include "../observers/GameStateObserver.h"
#include "../observers/CanvasObserver.h"
#include "../observers/ObjectObserver.h"
#include "../observers/BirdHeightObserver.h"
#include "../observers/BirdRectObserver.h"
#include "../observers/PipeObserver.h"
#include "../util/PerformanceCounter.h"
#include "../recorder/Recorder.h"
#include "../util/MouseController.h"

using namespace GameState;

DEFINE_COUNTER(CPlay_Update);

const double CBase::MIN_MATCH_VALUE = 0.04;

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


bool CBase::isMatchResultIncrease(CGameStateObserver* observer, double result)
{
    if (result > m_fLastMatchResult)
    {
        observer->StateMachine()->ChangeState(new CUnknown);
        return true;
    }
    else
    {
        m_fLastMatchResult = result;
        return false;
    }
}


double CBase::MatchTitle(CGameStateObserver* observer)
{
    auto mat = CCanvasObserver::GetInstance()->GetCanvasMat();
    auto templ = observer->GetTemplateMat("title");
    cv::Point pt;
    return _GetMatchResult(mat, templ, pt);
}


double CBase::MatchGetReady(CGameStateObserver* observer)
{
    auto mat = CCanvasObserver::GetInstance()->GetCanvasMat();
    auto templ = observer->GetTemplateMat("getready");
    cv::Point pt;
    return _GetMatchResult(mat, templ, pt);
}


double CBase::MatchGameOver(CGameStateObserver* observer)
{
    auto mat = CCanvasObserver::GetInstance()->GetCanvasMat();
    auto templ = observer->GetTemplateMat("gameover");
    cv::Point pt;
    return _GetMatchResult(mat, templ, pt);
}


bool CBase::Update(CGameStateObserver* observer, double dt)
{
    if (m_bAutoClick)
    {
        m_dAutoClickRemainTime -= dt;
        if (m_dAutoClickRemainTime <=0)
        {
            bool ret = OnAutoClick();
            if (ret) m_bAutoClick = false;

            return ret;
        }
    }

    return false;
};


void CBase::StartAutoClick(double remaintime)
{
    m_bAutoClick = true;
    m_dAutoClickRemainTime = remaintime;
};


/////////////////////// CUnknown


bool CUnknown::Update(CGameStateObserver* observer, double dt)
{
    typedef double (CBase::*MATCH_FUNC)(CGameStateObserver*);
    MATCH_FUNC matchfunc[] = {
        &CBase::MatchTitle,
        &CBase::MatchGetReady,
        &CBase::MatchGameOver,
    };
    int funccount = sizeof(matchfunc)/sizeof(MATCH_FUNC);

    /// change to non-play state
    for (int i=0; i<funccount; i++)
    {
        auto result = (this->*matchfunc[i])(observer);
        DLOG(INFO) << "match id: " << i << ", result: " << result;
        if (result < MIN_MATCH_VALUE)
        {
            /// change state
            CBase* state = NULL;
            switch(i)
            {
            case 0:
                state = new CTitle();
                break;
            case 1:
                state = new CGetReady();
                break;
            case 2:
                state = new CGameOver();
                break;
            default:
                assert(false);
            }
            observer->StateMachine()->ChangeState(state);
            return true;
        }
    }

    // change to play state
    observer->StateMachine()->ChangeState(new CPlay());
    
    return observer->StateMachine()->Update(dt);
}


//////////////////////// Title

bool CTitle::Update(CGameStateObserver* observer, double dt)
{
    auto result = MatchTitle(observer);
    if (!isMatchResultIncrease(observer, result))
    {
        if (CBase::Update(observer, dt))
        {
            observer->StateMachine()->ChangeState(new CWaitForGetReady());
            return true;
        }
    }

    return true;
}


bool CTitle::OnAutoClick()
{
    return CMouseController::GetInstance()->Click_LeftButton();
}


//////////////////////// Wait For Get Ready

bool CWaitForGetReady::Update(CGameStateObserver* observer, double dt)
{
    auto result = MatchGetReady(observer);
    if (result < MIN_MATCH_VALUE)
    {
        ::Sleep(300);
        observer->StateMachine()->ChangeState(new CGetReady());
    }
    return true;
}


//////////////////////// Get Ready

bool CGetReady::Update(CGameStateObserver* observer, double dt)
{
    auto result = MatchGetReady(observer);
    if (!isMatchResultIncrease(observer, result))
    {
        CBase::Update(observer, dt);
    }

    return true;
}


bool CGetReady::OnAutoClick()
{
    return CMouseController::GetInstance()->ClickInCanvas();
}


void CGetReady::Exit(CGameStateObserver* observer)
{
    // clear play back data
    CRecorder::GetInstance()->ResetData();
    CPipeObserver::GetInstance()->ResetData();
}


//////////////////////// Game Over

void CGameOver::Enter(CGameStateObserver* observer)
{
    CBirdHeightObserver::GetInstance()->ResetData();
}


bool CGameOver::Update(CGameStateObserver* observer, double dt)
{
    auto result = MatchGameOver(observer);
    if (!isMatchResultIncrease(observer, result))
    {
        CBase::Update(observer, dt);
    }

    return true;
}


bool CGameOver::OnAutoClick()
{
    return CMouseController::GetInstance()->Click_LeftButton();
}


//////////////////////// Play

bool CPlay::Update(CGameStateObserver* observer, double dt)
{
    COUNTER_HELPER(CPlay_Update);

    auto canvasmat = CCanvasObserver::GetInstance()->GetCanvasMat();
    bool ret = CObjectObserver::GetInstance()->Update(canvasmat);
    if (!ret) return false;

    auto contours = CObjectObserver::GetInstance()->GetAllObjContours();

    auto pBirdRectObserver = CBirdRectObserver::GetInstance();
    pBirdRectObserver->Update(contours);

    auto nBirdCount = pBirdRectObserver->GetBirdRectsCount();

    switch(nBirdCount)
    {
    case 0:
        // not found any find
        DLOG(INFO) << "not found any bird";
        observer->StateMachine()->ChangeState(new CUnknown());
        return false;
    case 1:
        // find singleton bird
        CPipeObserver::GetInstance()->Update(contours, pBirdRectObserver->GetBirdLeft(), dt);
        break;
    default:
        // find more than one bird
        // TODO get nearest bird
        DLOG(INFO) << "find more than one bird";
        observer->StateMachine()->ChangeState(new CUnknown());
        return false;
        break;
    }

    return true;
}


//////////////////////// PlayBack

bool CPlayBack::Update(CGameStateObserver* observer, double dt)
{
    return true;
}