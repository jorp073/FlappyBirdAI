// Pipe Observer
// Observe bottom pipe top right corner position, calculate average pipe speed

#pragma once
#include "opencv2/opencv.hpp"
#include "../util/SingleInstance.h"
#include "../util/MathUtil.h"


class CPipeObserver
{
public:
    bool Update(const CV_CONTOURS& rectAllObjContours, int iBirdLeft, double dt);

    float GetPipeHeight() { return m_fPipeHeight; };
    float GetPipeRight() { return m_fPipeRight; };
    double GetPipeSpeed() { return m_AveragePipeSpeed.GetAverageValue(); };

    void ResetData();

private:
    bool _IsPipeRect(const CV_CONTOUR& contour, int iBirdLeft, /*OUT*/ cv::Rect& rect);

    std::vector<cv::Rect> m_lPipeRects;

    float m_fPipeHeight;
    float m_fPipeRight, m_fLastPipeRight;

    MathUtil::TAverageValue<double> m_AveragePipeSpeed;

    DEFINE_SINGLEINSTANCE(CPipeObserver);
};
