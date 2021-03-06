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
    void SetCanvasStateText();
    void SetGameStateText();
    void SetBirdRects(const std::vector<cv::Rect>& rects);
    void SetPipeRects(const std::vector<cv::Rect>& rects);
    void SetPipeHeight(float height) { m_fPipeHeight = height; };
    void SetPipeRight(float right) { m_fPipeRight = right; };

    void DrawParabola(
        std::vector<PARABOLA_POINT>& points,
        float fRemainCollisionTime,
        double dHeight,
        bool bClick);

    void DrawClickDelay(CClickDelayModel* pModel);

    void DrawJumpRange(CJumpRangeModel* pModel);

    void Update(double dt);


private:
    void TopMostWindow(const std::string& strWindowName);

    void CVDrawText(cv::Mat mat, const std::string& strText, int iHeight, unsigned char cColor = 255);

    std::string m_strCanvasStateText;
    std::string m_strGameStateText;
    std::vector<cv::Rect> m_rectBirds, m_rectPipes;

    float m_fPipeHeight, m_fPipeRight;

    cv::Mat m_matParabola;

    CRecorder* m_pRecorder;

    unsigned char m_cClickTextColor;

    DEFINE_SINGLEINSTANCE(COutputWindow);
};