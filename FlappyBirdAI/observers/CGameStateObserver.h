#pragma once

#include "../model/define.h"
#include "opencv2/opencv.hpp"


template <class> class TStateMachine;

class CGameStateObserver
{
public:
    static CGameStateObserver* GetInstance()
    {
        if(NULL == m_pInstance)
            m_pInstance = new CGameStateObserver();
        return static_cast<CGameStateObserver*>(m_pInstance);
    }

    ~CGameStateObserver();

    static void Release()
    {
        if(NULL != m_pInstance)
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
    }

    bool Init();
    bool _LoadTemplate(std::string filepath, std::string tname);

    bool Update();

    TStateMachine<CGameStateObserver>* StateMachine() { return m_pStateMachine; };

    const cv::Mat & GetTemplateMat(std::string str) { return m_vecTemplate[str]; };

protected:
    CGameStateObserver();
    static CGameStateObserver * m_pInstance;

private:
    // an instance of the state machine class
    TStateMachine<CGameStateObserver>*  m_pStateMachine;

    std::map<std::string, cv::Mat> m_vecTemplate;
};

