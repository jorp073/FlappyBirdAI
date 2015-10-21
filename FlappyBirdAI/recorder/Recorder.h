#pragma once
#include "opencv2/opencv.hpp"
#include "../util/SingleInstance.h"
#include <list>

struct RECORD_FRAME
{
    double a, b, c;
    double dHeight;
    cv::Mat matCanvas;
    cv::Mat matParabola;
    cv::Mat matClickDelay;
};


class CRecorder
{
public:
    void RecordData(double a, double b, double c, double height);
    void RecordCanvas(const cv::Mat& mat);
    void RecordParabola(const cv::Mat& mat);
    void RecordClickDelay(const cv::Mat& mat);
    void PushRecord();

    void OnDisplayPreviousFrame();
    void OnDisplayNextFrame();

    void ResetData();

private:
    bool IsRecordable();

    bool _EnterDisplayState();

    void _DisplayFrame(const RECORD_FRAME& frame);

    std::list<RECORD_FRAME> m_lFrameData;
    RECORD_FRAME m_FrameData;
    std::list<RECORD_FRAME>::iterator m_iterFrame;
    int m_iCurPlayID;

    bool m_bDisplaying;

    DEFINE_SINGLEINSTANCE(CRecorder);
};
