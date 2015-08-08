#pragma once

#include "../model/define.h"
#include "opencv2/opencv.hpp"


template <class> class TStateMachine;

class CCanvasObserver
{
public:
    static CCanvasObserver* GetInstance()
    {
        if(NULL == m_pInstance)
            m_pInstance = new CCanvasObserver();
        return static_cast<CCanvasObserver*>(m_pInstance);
    }

    ~CCanvasObserver();

    static void Release()
    {
        if(NULL != m_pInstance)
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
    }

    bool Init();
    bool Update();

    void SetCanvasMat(cv::Mat graymat);
    const cv::Mat & GetCanvasMat() const { return m_matCanvas; };

    TStateMachine<CCanvasObserver>* StateMachine() { return m_pStateMachine; };

protected:
    CCanvasObserver();
    static CCanvasObserver * m_pInstance;

private:
    cv::Rect _GetRectWithoutBorder(cv::Mat graymat);

    // TODO
    void _FilterBackground(cv::Mat graymat);
    
    // an instance of the state machine class
    TStateMachine<CCanvasObserver>*  m_pStateMachine;

    // canvas Mat
    cv::Mat m_matCanvas, m_matGrayBg;
};

