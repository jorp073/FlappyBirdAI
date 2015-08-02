#pragma once

#include "../model/define.h"
#include "opencv2/opencv.hpp"


template <class> class TStateMachine;

class CCanvasObserver
{
public:
    CCanvasObserver();

    bool Update();

    void SetMat(cv::Mat mat);
    const cv::Mat & GetCanvasMat() const { return m_matCanvas; };
    const cv::Mat & GetGrayCanvasMat() const { return m_matGrayCanvas; };

    TStateMachine<CCanvasObserver>* StateMachine() { return m_pStateMachine; };

private:
    void _RemoveCanvasBorder();
    
    // an instance of the state machine class
    TStateMachine<CCanvasObserver>*  m_pStateMachine;

    // canvas Mat
    cv::Mat m_matCanvas, m_matGrayCanvas;
};

