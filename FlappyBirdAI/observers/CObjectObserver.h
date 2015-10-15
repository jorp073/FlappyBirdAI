// Bird and Pipes Observer
// Observe Bird and Nearest Pipe position

#pragma once
#include <Windows.h>
#include "opencv2/opencv.hpp"
#include "../util/SingleInstance.h"


class CObjectObserver
{
public:
    bool Update(cv::Mat matGrayCanvas);

    const size_t GetBirdRectsCount() const { return m_rectBirds.size(); };

    float GetBirdHeight();
    float GetPipeHeight();

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

    DEFINE_SINGLEINSTANCE(CObjectObserver);
};
