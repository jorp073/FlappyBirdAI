#pragma once
#include <string>
#include "opencv2/opencv.hpp"
#include "../util/SingleInstance.h"
#include "../model/define.h"


class CRecorder;


class COutputWindow
{
public:
    bool Init();
    void SetCanvasStateText();
    void SetGameStateText();
    void SetBirdRects(const std::vector<cv::Rect> rects);
    void SetPipeHeight(float height) { m_fPipeHeight = (1-height)*CANVAS_SCALETO_HEIGHT; };

    void DrawParabola(
        std::vector<PARABOLA_POINT>& points,
        int iOutputWindowWidth,
        float fRemainCrashTime,
        double dHeight,
        bool bClick);

    void Update();


private:
    void TopMostWindow(const std::string& strWindowName);

    void CVDrawText(cv::Mat mat, const std::string& strText, int iHeight);

    void TickCountFPS();

    std::string m_strCanvasStateText;
    std::string m_strGameStateText;
    std::vector<cv::Rect> m_rectBirds;
    int m_iFPS;
    std::string m_strFPSText;
    int m_dwTickCount;
    float m_fPipeHeight;

    cv::Mat m_matParabola;

    CRecorder* m_pRecorder;

    DEFINE_SINGLEINSTANCE(COutputWindow);
};