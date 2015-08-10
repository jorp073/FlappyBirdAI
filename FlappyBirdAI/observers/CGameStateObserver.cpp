#include "stdafx.h"
#include "CGameStateObserver.h"
#include "../fsm/CGameState.h"

INIT_SINGLEINSTANCE(CGameStateObserver);


CGameStateObserver::CGameStateObserver()
{
}


CGameStateObserver::~CGameStateObserver()
{
    if (m_pStateMachine)
        delete m_pStateMachine;

}


bool CGameStateObserver::Update()
{
    return m_pStateMachine->Update();
}


bool CGameStateObserver::Init()
{
    m_pStateMachine = new TStateMachine<CGameStateObserver>(this);
    m_pStateMachine->InitState(new GameState::CUnknown());
    _LoadTemplate("res/gamestate_title.png",    "title");
    _LoadTemplate("res/gamestate_getready.png", "getready");
    _LoadTemplate("res/gamestate_gameover.png", "gameover");
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
