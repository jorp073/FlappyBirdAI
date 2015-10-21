#pragma once

#include "../model/define.h"
#include "opencv2/opencv.hpp"
#include "../util/SingleInstance.h"


template <class> class TStateMachine;

class CGameStateObserver
{
public:
    bool Init();
    bool _LoadTemplate(std::string filepath, std::string tname);

    bool Update(double dt);

    TStateMachine<CGameStateObserver>* StateMachine() { return m_pStateMachine; };

    const cv::Mat & GetTemplateMat(std::string str) { return m_vecTemplate[str]; };


private:
    // an instance of the state machine class
    TStateMachine<CGameStateObserver>*  m_pStateMachine;

    std::map<std::string, cv::Mat> m_vecTemplate;

    DEFINE_SINGLEINSTANCE(CGameStateObserver);
};

