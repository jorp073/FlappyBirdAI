#pragma once

#include "../model/define.h"
#include "opencv2/opencv.hpp"
#include "../util/SingleInstance.h"


template <class> class TStateMachine;

class CCanvasObserver
{
public:
    bool Init();
    bool Update();

    void SetCanvasMat(cv::Mat graymat);
    const cv::Mat & GetCanvasMat() const { return m_matCanvas; };

    TStateMachine<CCanvasObserver>* StateMachine() { return m_pStateMachine; };

    int GetNoGroundCanvasHeight();


private:
    cv::Rect _GetRectWithoutBorder(cv::Mat graymat);

    // TODO
    void _FilterBackground(cv::Mat graymat);
    
    // an instance of the state machine class
    TStateMachine<CCanvasObserver>*  m_pStateMachine;

    // canvas Mat
    cv::Mat m_matCanvas, m_matGrayBg;

    DEFINE_SINGLEINSTANCE(CCanvasObserver);
};

