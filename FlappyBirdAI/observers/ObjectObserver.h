// Objects Observer
// Observe all objects contours

#pragma once
#include "opencv2/opencv.hpp"
#include "../util/SingleInstance.h"


class CObjectObserver
{
public:
    bool Update(cv::Mat matGrayCanvas);

    const CV_CONTOURS& GetAllObjContours() { return m_rectContours; };

private:

    cv::Mat m_matBinary;
    CV_CONTOURS m_rectContours;

    DEFINE_SINGLEINSTANCE(CObjectObserver);
};
