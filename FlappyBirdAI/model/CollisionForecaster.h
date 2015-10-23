#pragma once
//#include <vector>
//#include <functional>

#include <Windows.h>
#include "define.h"
#include "../util/SingleInstance.h"

struct CRASH_FORECAST_PARAM
{
    float fAverageBirdLeft;
    float fAverageBirdRectHeight;
    double dPipeSpeed;
    float fPipeRight;
};


class CHeightTimeModel;


class CCollisionTimeForecaster
{
public:
    void SetModel(CHeightTimeModel* pModel);

    double GetCollisionBottomTime();
    
    bool IsWillCollisionPipeCorner(const CRASH_FORECAST_PARAM& data);

    void GetABC(double& a, double& b, double& c);

    // 获取抛物线点集
    std::vector<PARABOLA_POINT>& GetParabolaDots() { return m_vParabolaDots; };

    void ResetData();

    void SetBestJumpOffsetY(float fOffsetY);

    // generate parabola graph for output window
    // h, w: output window height and width
    void GenParabolaDots(int h, int w, double a, double b, double c);

private:
    // return whether bird is dropping down
    bool _IsBirdDroppingDown();

    CHeightTimeModel* m_pModel;
    std::vector<PARABOLA_POINT> m_vParabolaDots;
    double m_a, m_b, m_c;
    bool m_bIsDroppingDown;

    // height offset of jump place from ground/bottom pipe
    // the height offset that bird worst possibly contacts bottom and top pipe
    float m_fBestJumpOffsetY;


    DEFINE_SINGLEINSTANCE(CCollisionTimeForecaster);
};
