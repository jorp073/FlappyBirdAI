// Bird Observer
// Observe Bird rect

#pragma once
#include "opencv2/opencv.hpp"
#include "../util/SingleInstance.h"
#include "../util/MathUtil.h"


class CBirdRectObserver
{
public:
    size_t GetBirdRectsCount() const { return m_rectBirds.size(); };

    void Update(const CV_CONTOURS& rectAllObjContours);

    float GetBirdHeight();
    int GetBirdLeft();

    cv::Size2f GetAverageBirdSize();

private:

    bool _IsBirdRect(const CV_CONTOUR& contour, /*OUT*/ cv::Rect& rect);

    std::vector<cv::Rect> m_rectBirds;

    MathUtil::TAverageValue<float> m_fAverageBirdWidth;
    MathUtil::TAverageValue<float> m_fAverageBirdHeight;

    DEFINE_SINGLEINSTANCE(CBirdRectObserver);
};
