#pragma once

#include "../model/define.h"
#include "opencv2/opencv.hpp"
#include "../util/SingleInstance.h"


template <class> class TStateMachine;

class CCanvasObserver
{
public:
    static bool IsBorderColor(uchar color);

    bool Update(double dt);

    void SetCanvasMat(cv::Mat graymat);
    const cv::Mat & GetCanvasMat() const { return m_matCanvas; };

    TStateMachine<CCanvasObserver>* StateMachine() { return m_pStateMachine; };

    bool IsCanvasPosMoved(const cv::Mat& mat);

    void SetCanvasRect(const cv::Rect& rectCanvas) { m_CanvasRect = rectCanvas; };

    const cv::Rect& GetCanvasRect() { return m_CanvasRect; };

private:

    cv::Rect _GetRectWithoutBorder(cv::Mat graymat);
    
    // an instance of the state machine class
    TStateMachine<CCanvasObserver>*  m_pStateMachine;

    // canvas Mat
    cv::Mat m_matCanvas, m_matGrayBg;

    cv::Rect m_CanvasRect;

    DEFINE_SINGLEINSTANCE(CCanvasObserver);
};

