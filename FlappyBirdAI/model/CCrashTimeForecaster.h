#pragma once
//#include <vector>
//#include <functional>

#include <Windows.h>
#include "define.h"
#include "../util/SingleInstance.h"


class CHeightTimeModel;


class CCrashTimeForecaster
{
public:
    void SetModel(CHeightTimeModel* pModel);

    void Update();

    bool IsNeedJumpNow() { return m_bIsNeedJumpNow; };

    float GetRemainCrashTime() { return m_iRemainCrashTime; };

    void GetABC(double& a, double& b, double& c);

    // 获取抛物线点集
    std::vector<PARABOLA_POINT>& GetParabolaDots() { return m_vParabolaDots; };

    void ResetData();

    void SetBestJumpOffsetY(float fOffsetY);

private:
    // return whether bird is dropping down
    bool IsBirdDroppingDown();

    // h: output window height
    // w: output window width
    void GenParabolaDots(int h, int w, double a, double b, double c);

    CHeightTimeModel* m_pModel;
    std::vector<PARABOLA_POINT> m_vParabolaDots;
    float m_iRemainCrashTime;
    bool m_bIsNeedJumpNow;
    double m_a, m_b, m_c;
    bool m_bIsDroppingDown;

    // height offset of jump place from ground/bottom pipe
    // the height offset that bird worst possibly contacts bottom and top pipe
    float m_fBestJumpOffsetY;


    DEFINE_SINGLEINSTANCE(CCrashTimeForecaster);
};
