#include "stdafx.h"
#include "CGameStateObserver.h"
#include "../fsm/CGameState.h"


CGameStateObserver::CGameStateObserver()
{
    m_pStateMachine = new TStateMachine<CGameStateObserver>(this);
    m_pStateMachine->InitState(new GameState::CUnknown());
}


bool CGameStateObserver::Update()
{
    return m_pStateMachine->Update();
}


bool CGameStateObserver::Init()
{
    printf("CUnknown::init()\n");
    cv::Mat mat1 = cv::imread("res/gamestate_title.png", CV_8UC1);
    assert(mat1.cols > 0);
    if (CV_8UC1 != mat1.type())
    {
        cv::cvtColor(mat1, mat1, CV_RGB2GRAY);
    }
    assert(mat1.type() == CV_8UC1);
    m_vecTemplate.insert(std::make_pair("title", mat1));
    printf("CUnknown::init() end\n");

    imshow("temp1", mat1);
    return true;
}