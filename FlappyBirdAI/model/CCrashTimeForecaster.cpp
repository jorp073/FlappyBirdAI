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
    , m_bIsDroppingDown(false)
{
}


void CCrashTimeForecaster::ResetData()
{
    m_bIsDroppingDown = false;
}


bool CCrashTimeForecaster::IsBirdDroppingDown()
{
#define DROPDOWN_COUNT 3
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

    /// emergency jump when bird under pipe
    if (heightdata.back() <= 0)
    {
        DLOG(INFO) << "ai need emergency jump";
        m_bIsNeedJumpNow = true;
        m_iRemainCrashTime = 0;
        return;
    }

    /// get is dropping
    if (!m_bIsDroppingDown)
    {
        m_bIsDroppingDown = IsBirdDroppingDown();
        if (!m_bIsDroppingDown)
        {
            m_bIsNeedJumpNow = false;
        }
    }

    if (heightdata.size() < 5)
    {
        DLOG(INFO) << "ai dots count < 5";
        m_bIsNeedJumpNow = false;
        m_iRemainCrashTime = 0;
        return;
    }

    /// fit parabola
    auto timedata = m_pModel->GetTimeData();
    auto count = heightdata.size();

    double a, b, c;
    ParabolaFit(timedata, heightdata, a, b, c);
    m_a = a;
    m_b = b;
    m_c = c;

    DLOG(INFO) << "ai a:" << a << " b:" << b << " c:" << c;

    GenParabolaDots(PARABOLA_GRAPH_H, a, b, c, m_iOutputWindowWidth);

    if (m_bIsDroppingDown)
    {
        if (a>=0)
        {
            DLOG(INFO) << "ai a>=0 a=" << a;
            m_iRemainCrashTime = 9999;
        }
        else
        {
            // bird will crash pipe/ground when ax2+bx+c = BIRD_HEIGHT/2
            // according to log, BIRD_HEIGHT/2 is about 0.16
            c = c - 0.0426;

            double delta = b*b - 4*a*c;

            if (delta<0)
            {
                DLOG(INFO) << "ai delta<=0 delta=" << delta;
                // when bird is near ground, and pipe appears, bird is under pipe
                m_iRemainCrashTime = 0;
            }
            else
            {
                double t = (-b - sqrt(delta)) /2/a;

                double remaintime1 = t - timedata.back();
                double remaintime2 = t - timedata.back() - (m_pModel->GetTimeSinceFirstData() - timedata.back()+timedata[0]);

                DLOG(INFO) << "ai remaintime1:" << remaintime1 << " remaintime2:" << remaintime2;
                DLOG(INFO) << "ai f(t):" << a*t*t+b*t+c;

                m_iRemainCrashTime = (float)remaintime2;
            }
        }
    }
    else
    {
        m_iRemainCrashTime = 9999;
    }

    m_bIsNeedJumpNow = m_iRemainCrashTime <= 41;
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
    float vratio = (float)(h+1);
    float hratio = vratio/1000;
    w = (int)(hratio * timedata.back() + 1);

    PARABOLA_POINT originPt;
    PARABOLA_POINT fillinPt;
    originPt.type = 0;
    fillinPt.type = 1;

    /// Position of dots
    for (size_t i=0; i<count; i++)
    {
        // captured data (green dots)
        auto time = timedata[i];
        int x = (int)(hratio* time + 0.5f);
        int y = (int)(h - vratio * heightdata[i] + 0.5f);
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
        y = (int)(h - vratio * (a*time*time+b*time+c) + 0.5);
        if (y>= h)
            y = h - 1;
        if (y<0)
            y = 0;

        fillinPt.x = x;
        fillinPt.y = y;
        m_vParabolaDots.push_back(fillinPt);
    }
}
