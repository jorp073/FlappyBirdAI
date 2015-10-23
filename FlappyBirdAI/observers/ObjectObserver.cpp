#include "stdafx.h"
#include "../model/define.h"
#include "ObjectObserver.h"
#include "../output/OutputWindow.h"


INIT_SINGLEINSTANCE(CObjectObserver);


CObjectObserver::CObjectObserver()
    : m_matBinary(cv::Mat())
{
}


void CObjectObserver::Init()
{
}


bool CObjectObserver::Update(cv::Mat matGrayCanvas)
{
    /// Binary gray image
    cv::threshold(matGrayCanvas, m_matBinary, 0, 255, 0);

    /// Dilate and Erode, to get less contours
    int dilation_size = 1;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
        cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
        cv::Point( dilation_size, dilation_size ) );
    cv::dilate(m_matBinary, m_matBinary, element);
    cv::erode(m_matBinary, m_matBinary, element);
    //cv::imshow("bin", mat);

    /// Find all contours
    std::vector<cv::Vec4i> hierarchy;
    findContours(m_matBinary, m_rectContours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    DLOG(INFO) << "findContours count: " << m_rectContours.size();

    return true;
}
