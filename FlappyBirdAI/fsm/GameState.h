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
    CBase(const std::string name)
        : TState(name)
        , m_fLastMatchResult(9999)
        , m_bAutoClick(false)
    {
    };


    double MatchTitle(CGameStateObserver* observer);
    double MatchGetReady(CGameStateObserver* observer);
    double MatchGameOver(CGameStateObserver* observer);

    virtual bool Update(CGameStateObserver* observer, double dt);

protected:
    bool isMatchResultIncrease(CGameStateObserver* observer, double result);

    void StartAutoClick(double remaintime);

    virtual bool OnAutoClick() { return false; };

    static const double MIN_MATCH_VALUE;
    double m_fLastMatchResult;
    bool m_bAutoClick;
    double m_dAutoClickRemainTime;

private:
    double _GetMatchResult(cv::Mat mat, cv::Mat templ, OUT cv::Point& matchLoc);
};


class CUnknown : public CBase
{
public:
    CUnknown() : CBase("Unknown") {};

    virtual bool Update(CGameStateObserver* observer, double dt);
};


class CTitle : public CBase
{
public:
    CTitle() : CBase("Title") { StartAutoClick(200); };

    virtual bool Update(CGameStateObserver* observer, double dt);

    virtual bool OnAutoClick();
};


class CWaitForGetReady: public CBase
{
public:
    CWaitForGetReady() : CBase("Wait4GetReady") {};

    virtual bool Update(CGameStateObserver* observer, double dt);
};


class CGetReady : public CBase
{
public:
    CGetReady() : CBase("GetReady") { StartAutoClick(200); };

    virtual bool Update(CGameStateObserver* observer, double dt);
    virtual void Exit(CGameStateObserver* observer);

    virtual bool OnAutoClick();
};


class CGameOver : public CBase
{
public:
    CGameOver() : CBase("GameOver") { StartAutoClick(300); };

    virtual void Enter(CGameStateObserver* observer);
    virtual bool Update(CGameStateObserver* observer, double dt);

    virtual bool OnAutoClick();
};


class CPlay : public CBase
{
public:
    CPlay() : CBase("Play") {};

    virtual bool Update(CGameStateObserver* observer, double dt);
};


class CPlayBack : public CBase
{
public:
    CPlayBack() : CBase("PlayBack") {};

    virtual bool Update(CGameStateObserver* observer, double dt);
};

}; // namespace GameState
