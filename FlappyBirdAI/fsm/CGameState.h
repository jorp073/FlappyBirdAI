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
        CBase(const std::string name) : TState(name), m_fLastMatchResult(9999) {};
        double MatchTitle(CGameStateObserver* observer);
        double MatchGetReady(CGameStateObserver* observer);
        double MatchGameOver(CGameStateObserver* observer);

    protected:
        bool isMatchResultIncrease(CGameStateObserver* observer, double result);

        static const double MIN_MATCH_VALUE;
        double m_fLastMatchResult;

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

        virtual void Enter(CGameStateObserver* observer);
        virtual bool Update(CGameStateObserver* observer);
    };


    class CPlay : public CBase
    {
    public:
        CPlay() : CBase("Play") {};

        virtual bool Update(CGameStateObserver* observer);
    };
};
