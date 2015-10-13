#pragma once
//#include <vector>
//#include <functional>

#include <Windows.h>
#include "define.h"


class CHeightTimeModel;


class CCrashTimeForecaster
{
public:
    CCrashTimeForecaster(CHeightTimeModel* pModel);

    void Update();

    bool IsNeedJumpNow() { return m_bIsNeedJumpNow; };

    float GetRemainCrashTime() { return m_iRemainCrashTime; };

    void GetABC(double& a, double& b, double& c);

    // 获取抛物线点集
    std::vector<PARABOLA_POINT>& GetParabolaDots() { return m_vParabolaDots; };

    int GetOutputWindowWidth() { return m_iOutputWindowWidth; };

    void ResetData();

private:
    // return whether bird is dropping down
    bool IsBirdDroppingDown();

    // h: output window height
    // w: output window width
    void GenParabolaDots(int h, double a, double b, double c, /*OUT*/ int& w);

    CHeightTimeModel* m_pModel;
    std::vector<PARABOLA_POINT> m_vParabolaDots;
    int m_iOutputWindowWidth;
    bool m_bIsNeedJumpNow;
    float m_iRemainCrashTime;
    double m_a, m_b, m_c;
    bool m_bIsDroppingDown;
};
