#include "stdafx.h"
#include "CCanvasObserver.h"
#include "../fsm/CCanvasObserverState.h"


CCanvasObserver::CCanvasObserver()
{
    m_pStateMachine = new TStateMachine<CCanvasObserver>(this);
    m_pStateMachine->InitState(new CanvasObserverState::CSearch());
}


bool CCanvasObserver::Update()
{
    return m_pStateMachine->Update();
}
