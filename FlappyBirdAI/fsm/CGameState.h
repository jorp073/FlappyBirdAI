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
        bool IsInGetReadyState(CGameStateObserver* observer);
        bool IsInGameOverState(CGameStateObserver* observer);
        bool IsInPlayState(CGameStateObserver* observer);

    private:
        double _GetMatchResult(cv::Mat mat, cv::Mat templ, OUT cv::Point& matchLoc);
    };


    class CUnknown : public CBase
    {
    public:
        CUnknown() : CBase("Unknown") {};

        virtual bool Update(CGameStateObserver* observer);
    };


    class CTitle : public CBase
    {
    public:
        CTitle() : CBase("Title") {};

        virtual bool Update(CGameStateObserver* observer);
    };


    class CGetReady : public CBase
    {
    public:
        CGetReady() : CBase("GetReady") {};

        virtual bool Update(CGameStateObserver* observer);
    };


    class CGameOver : public CBase
    {
    public:
        CGameOver() : CBase("GameOver") {};

        virtual bool Update(CGameStateObserver* observer);
    };


    class CPlay : public CBase
    {
    public:
        CPlay() : CBase("Play") {};

        virtual bool Update(CGameStateObserver* observer);

    private:
        bool isCanvasPosMoved();
    };
};
