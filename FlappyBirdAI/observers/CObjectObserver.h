// Bird and Pipes Observer
// Observe Bird and Nearest Pipe position

#pragma once
#include <Windows.h>
#include "opencv2/opencv.hpp"
#include "../util/SingleInstance.h"
#include "../util/MathUtil.h"


class CObjectObserver
{
public:
    bool Update(cv::Mat matGrayCanvas);

    const size_t GetBirdRectsCount() const { return m_rectBirds.size(); };

    float GetBirdHeight();
    float GetPipeHeight();

    SIZE GetAverageBirdRect();

    void ResetData();

private:
    // return whether find bird or not
    std::vector<cv::Rect> GetPipeRects(const std::vector<std::vector<cv::Point>> & rectContours);

    bool _IsBirdRect(const std::vector<cv::Point>& contour, OUT cv::Rect& rect);
    bool _IsPipeRect(const std::vector<cv::Point>& contour, OUT cv::Rect& rect);

    cv::Mat m_matBinary;
    std::vector<cv::Rect> m_rectBirds;
    std::vector<std::vector<cv::Point>> m_rectContours;
    float m_fPipeHeight;

    MathUtil::TAverageValue<float> m_fAverageBirdWidth;
    MathUtil::TAverageValue<float> m_fAverageBirdHeight;

    DEFINE_SINGLEINSTANCE(CObjectObserver);
};
