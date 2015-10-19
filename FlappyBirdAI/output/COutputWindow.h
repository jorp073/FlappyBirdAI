#pragma once
#include <string>
#include "opencv2/opencv.hpp"
#include "../util/SingleInstance.h"
#include "../model/define.h"


class CRecorder;
class CJumpRangeModel;
class CClickDelayModel;


class COutputWindow
{
public:
    bool Init();
    void SetCanvasStateText();
    void SetGameStateText();
    void SetBirdRects(const std::vector<cv::Rect>& rects);
    void SetPipeRects(const std::vector<cv::Rect>& rects);
    void SetPipeHeight(float height) { m_fPipeHeight = height; };

    void DrawParabola(
        std::vector<PARABOLA_POINT>& points,
        float fRemainCrashTime,
        double dHeight,
        bool bClick);

    void DrawClickDelay(CClickDelayModel* pModel);

    void DrawJumpRange(CJumpRangeModel* pModel);

    void Update();


private:
    void TopMostWindow(const std::string& strWindowName);

    void CVDrawText(cv::Mat mat, const std::string& strText, int iHeight);

    void TickCountFPS();

    std::string m_strCanvasStateText;
    std::string m_strGameStateText;
    std::vector<cv::Rect> m_rectBirds, m_rectPipes;
    int m_iFPS;
    std::string m_strFPSText;
    int m_dwTickCount;
    float m_fPipeHeight;

    cv::Mat m_matParabola;

    CRecorder* m_pRecorder;

    DEFINE_SINGLEINSTANCE(COutputWindow);
};