#include "stdafx.h"
#include "CHeightTimeModel.h"
#include "CCrashTimeForecaster.h"
#include "../util/CPerformanceCounter.h"
#include <iostream>
#include "../util/MathUtil.h"

DEFINE_COUNTER(CCrashTimeForecaster_UPDATE);


CCrashTimeForecaster::CCrashTimeForecaster(CHeightTimeModel* pModel)
    : m_pModel(pModel)
    , m_iOutputWindowWidth(PARABOLA_GRAPH_W)
{
}


bool CCrashTimeForecaster::IsBirdDroppingDown()
{
#define DROPDOWN_COUNT 6
    auto heightdata = m_pModel->GetBirdHeightData();

    auto count = heightdata.size();
    if (count < DROPDOWN_COUNT+1) return false;

    for (int i=0; i<DROPDOWN_COUNT; i++)
        if (heightdata[count-2-i] < heightdata[count-1-i]) return false;
    
    return true;
}


void CCrashTimeForecaster::GetABC(double& a, double& b, double& c)
{
    a = m_a;
    b = m_b;
    c = m_c;
}


void CCrashTimeForecaster::Update()
{
    COUNTER_HELPER(CCrashTimeForecaster_UPDATE);
    auto heightdata = m_pModel->GetBirdHeightData();

    m_bIsNeedJumpNow = false;

    m_a = 0;
    m_b = 0;
    m_c = 0;

    // emergency jump when bird under pipe
    if (heightdata.back() <= 0)
    {
        m_bIsNeedJumpNow = true;
        return;
    }

    if (!IsBirdDroppingDown())
    {
        m_bIsNeedJumpNow = false;
        return;
    }

    if (heightdata.size() < 5)
    {
        m_bIsNeedJumpNow = false;
        return;
    }

    auto timedata = m_pModel->GetTimeData();
    auto count = heightdata.size();

    double a, b, c;
    ParabolaFit(timedata, heightdata, a, b, c);
    m_a = a;
    m_b = b;
    m_c = c;

    DLOG(INFO) << "ai a:" << a << " b:" << b << " c:" << c;

    GenParabolaDots(PARABOLA_GRAPH_H, a, b, c, m_iOutputWindowWidth);

    if (a>=0)
    {
        DLOG(WARNING) << "ai a>=0 a=" << a;
        m_iRemainCrashTime = 9999;
        return;
    }

    double delta = b*b - 4*a*c;

    if (delta<0)
    {
        DLOG(WARNING) << "ai delta<=0 delta=" << delta;
        m_iRemainCrashTime = 9999;
        return;
    }

    // bird will crash pipe/ground when ax2+bx+c = BIRD_HEIGHT/2
    // according to log, BIRD_HEIGHT/2 is 0.162745
    c = c - 0.162745;

    double t = (-b - sqrt(delta)) /2/a;

    float remaintime1 = t - timedata.back();
    float remaintime2 = t - timedata.back() - (m_pModel->GetTimeSinceFirstData() - timedata.back()+timedata[0]);

    DLOG(INFO) << "ai remaintime1:" << remaintime1 << " remaintime2:" << remaintime2;

    m_iRemainCrashTime = remaintime2;
    m_bIsNeedJumpNow = m_iRemainCrashTime <= 175;
}


void CCrashTimeForecaster::GenParabolaDots(int h, double a, double b, double c, /*OUT*/ int& w)
{
    m_vParabolaDots.clear();

    auto heightdata = m_pModel->GetBirdHeightData();
    auto timedata = m_pModel->GetTimeData();
    auto count = heightdata.size();

    if (count < 4)
    {
        return;
    }

    /// Vert and hori scale ratio
    float vratio = h+1;
    float hratio = vratio/1000;
    w = hratio * timedata.back() + 1;

    PARABOLA_POINT originPt;
    PARABOLA_POINT fillinPt;
    originPt.type = 0;
    fillinPt.type = 1;

    /// Position of dots
    for (size_t i=0; i<count; i++)
    {
        // captured data (green dots)
        auto time = timedata[i];
        int x = hratio* time + 0.5;
        int y = h - vratio * heightdata[i] + 0.5;
        if (x>= w)
            x = w -1;
        if (y>= h)
            y = h - 1;
        if (x<0)
            x = 0;
        if (y<0)
            y = 0;

        originPt.x = x;
        originPt.y = y;
        m_vParabolaDots.push_back(originPt);

        // fill in new dots (white dots)
        y = h - vratio * (a*time*time+b*time+c) + 0.5;
        if (y>= h)
            y = h - 1;
        if (y<0)
            y = 0;

        fillinPt.x = x;
        fillinPt.y = y;
        m_vParabolaDots.push_back(fillinPt);
    }
}
