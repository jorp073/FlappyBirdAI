#pragma once

#include "../model/define.h"
#include "opencv2/core/core.hpp"


template <class> class TStateMachine;

class CCanvasObserver
{
public:
    CCanvasObserver();

    bool Update();

    void setCanvasPos(const POS point) { m_ptCanvasPos = point; };
    const POS & getCanvasPos() const { return m_ptCanvasPos; };

    TStateMachine<CCanvasObserver>* StateMachine() { return m_pStateMachine; };

private:
    // an instance of the state machine class
    TStateMachine<CCanvasObserver>*  m_pStateMachine;

    // canvas pos
    POS m_ptCanvasPos;
};

