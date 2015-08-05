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
    printf("CUnknown::init() end\n");
    _LoadTemplate("res/gamestate_title.png",    "title");
    _LoadTemplate("res/gamestate_getready.png", "getready");
    _LoadTemplate("res/gamestate_gameover.png", "gameover");
    _LoadTemplate("res/gamestate_play1.png",    "play1");
    _LoadTemplate("res/gamestate_play2.png",    "play2");
    _LoadTemplate("res/gamestate_play3.png",    "play3");
    _LoadTemplate("res/gamestate_play4.png",    "play4");
    _LoadTemplate("res/gamestate_play5.png",    "play5");
    return true;
}


bool CGameStateObserver::_LoadTemplate(std::string filepath, std::string tname)
{
    cv::Mat mat1 = cv::imread(filepath, CV_8UC1);
    assert(mat1.cols > 0);
    if (CV_8UC1 != mat1.type())
    {
        cv::cvtColor(mat1, mat1, CV_RGB2GRAY);
    }
    assert(mat1.type() == CV_8UC1);

    m_vecTemplate.insert(std::make_pair(tname, mat1));
    // imshow("temp1", mat1);
    return true;
}
