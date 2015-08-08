#pragma once
#include <string>
#include "opencv2/opencv.hpp"

class COutputWindow
{
public:
    static COutputWindow* GetInstance()
    {
        if(NULL == m_pInstance)
            m_pInstance = new COutputWindow();
        return static_cast<COutputWindow*>(m_pInstance);
    }

    ~COutputWindow();

    static void Release()
    {
        if(NULL != m_pInstance)
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
    }

    bool Init();
    void SetCanvasStateText();
    void SetGameStateText();
    void SetBirdRects(const std::vector<cv::Rect> rects);

    void Update();

protected:
    COutputWindow();
    static COutputWindow * m_pInstance;

private:
    void TopMostWindow(const std::string& strWindowName);

    void DrawText(cv::Mat mat, const std::string& strText, int iHeight);

    void TickCountFPS();

    std::string m_strCanvasStateText;
    std::string m_strGameStateText;
    std::vector<cv::Rect> m_rectBirds;
    int m_iFPS;
    std::string m_strFPSText;
    int m_dwTickCount;
};