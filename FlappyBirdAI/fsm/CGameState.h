#pragma once

#include "base.h"
#include "opencv2/opencv.hpp"
#include <Windows.h>

class CGameStateObserver;

namespace GameState
{
    class CBase : public TState<CGameStateObserver>
    {
    public:
        CBase(const std::string name) : TState(name) {};
        bool IsInTitleState(CGameStateObserver* observer);

    private:
        double _GetMatchResult(cv::Mat mat, cv::Mat templ, OUT cv::Point matchLoc);
    };


    class CUnknown : public CBase
    {
    public:
        CUnknown() : CBase("None") {};

        virtual bool Update(CGameStateObserver* observer);
    };


    class CTitle : public CBase
    {
    public:
        CTitle() : CBase("Title") {};

        virtual bool Update(CGameStateObserver* observer);
    };


    class CPlay : public CBase
    {
    public:
        CPlay(cv::Rect rect)
            : CBase("Play")
            , m_CanvasRect(rect)
        {};

        virtual bool Update(CGameStateObserver* observer);

    private:
        bool isCanvasPosMoved();

        cv::Rect m_CanvasRect;
    };
};
