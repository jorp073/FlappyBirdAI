#include "stdafx.h"
#include "HeightTimeModel.h"
#include "CollisionForecaster.h"
#include "../util/PerformanceCounter.h"
#include <iostream>
#include "../util/MathUtil.h"

INIT_SINGLEINSTANCE(CCollisionTimeForecaster);

DEFINE_COUNTER(CCollisionTimeForecaster_UPDATE);


CCollisionTimeForecaster::CCollisionTimeForecaster()
    : m_bIsDroppingDown(false)
    , m_fBestJumpOffsetY(0)
    , m_pModel(NULL)
{
}


void CCollisionTimeForecaster::SetModel(CHeightTimeModel* pModel)
{
    m_pModel = pModel;
}


void CCollisionTimeForecaster::ResetData()
{
    m_bIsDroppingDown = false;
}


bool CCollisionTimeForecaster::_IsBirdDroppingDown()
{
#define DROPDOWN_COUNT 3
    auto heightdata = m_pModel->GetBirdHeightData();

    auto count = heightdata.size();
    if (count < DROPDOWN_COUNT+1) return false;

    for (int i=0; i<DROPDOWN_COUNT; i++)
        if (heightdata[count-2-i] < heightdata[count-1-i]) return false;

    return true;
}


void CCollisionTimeForecaster::GetABC(double& a, double& b, double& c)
{
    a = m_a;
    b = m_b;
    c = m_c;
}


double CCollisionTimeForecaster::GetCollisionBottomTime()
{
    COUNTER_HELPER(CCollisionTimeForecaster_UPDATE);
    auto heightdata = m_pModel->GetBirdHeightData();

    bool bAlmostCollisionBottom = false;

    m_a = 0;
    m_b = 0;
    m_c = 0;

    /// emergency jump when bird under pipe
    if (heightdata.back() <= 0)
    {
        DLOG(INFO) << "ai heightdata.back() <= 0, need emergency jump";
        bAlmostCollisionBottom = true;
        return EMERGENCY_JUMP_TIME;
    }

    /// get is dropping
    if (!m_bIsDroppingDown)
    {
        m_bIsDroppingDown = _IsBirdDroppingDown();
        if (!m_bIsDroppingDown)
        {
            bAlmostCollisionBottom = false;
        }
    }

    if (heightdata.size() < MIN_HEIGHT_DATA_TO_JUMP)
    {
        DLOG(INFO) << "ai dots count: " << heightdata.size() << " < " << MIN_HEIGHT_DATA_TO_JUMP;
        return WILL_NOT_CRASH_TIME;
    }

    /// fit parabola
    auto timedata = m_pModel->GetTimeData();
    auto count = heightdata.size();

    double a, b, c;
    MathUtil::ParabolaFit(timedata, heightdata, a, b, c);
    m_a = a;
    m_b = b;
    m_c = c;

    DLOG(INFO) << "ai a:" << a << " b:" << b << " c:" << c;

    if (!m_bIsDroppingDown) return WILL_NOT_CRASH_TIME;


    if (a>=0)
    {
        DLOG(INFO) << "ai a>=0 a=" << a;
        return WILL_NOT_CRASH_TIME;
    }
    else
    {
        c -= m_fBestJumpOffsetY;

        double delta = b*b - 4*a*c;

        if (delta<0)
        {
            DLOG(INFO) << "ai emergency jump, pipeheight changed, delta<=0 delta=" << delta;
            // when bird is near ground, and pipe appears, bird is under pipe
            return EMERGENCY_JUMP_TIME;
        }
        else
        {
            double t = (-b - sqrt(delta)) /2/a;

            DLOG(INFO) << "ai f(t):" << a*t*t+b*t+c;

            return t;
        }
    }
}


bool CCollisionTimeForecaster::IsWillCollisionPipeCorner(const CRASH_FORECAST_PARAM& data)
{
    DLOG(INFO) << "ai IsWillCollisionPipeCorner dPipeSpeed: " << data.dPipeSpeed;
    // pipe speed may be zero
    if (0 == data.dPipeSpeed) return true;

    // bird corner: bottom left corner of bird
    // pipe corner: top right corner of the bottom pipe
    // x distance of bird corner and pipe corner
    float dx = data.fPipeRight - data.fAverageBirdLeft;
    DLOG(INFO) << "ai IsWillCollisionPipeCorner dx: " << dx;

    if (dx <=0) return false;

    // when bird corner and pipe corner are vertical aligned
    // vertical overlapped pixes
    double vertpix;
    {
        double remaintime = -dx / data.dPipeSpeed;
        double time = remaintime + m_pModel->GetTimeData().back();
        double birdheight = m_a * time * time + m_b * time + m_c;
        double birdbottom = birdheight - data.fAverageBirdRectHeight/2;
        vertpix = -birdbottom * CANVAS_NOGROUND_HEIGHT;

        DLOG(INFO) << "ai IsWillCollisionPipeCorner remaintime: " << remaintime
            << " time: " << time << " birdheight: " << birdheight
            << " birdbottom: " << birdbottom;
        DLOG(INFO) << "ai vert_overlap_pix: " << vertpix;
    }

    // when bird corner and pipe corner are horizontal aligned
    // horizontal overlapped pixes
    double horipix = 0;
    {
        double birdheight = data.fAverageBirdRectHeight/2;
        double c = m_c - birdheight;
        double delta = m_b*m_b - 4*m_a*c;

        DLOG(INFO) << "delta:" << delta;

        if (delta < 0 || m_a >= 0) return true;

        double time = (-m_b - sqrt(delta))/2/m_a;
        double remaintime = time - m_pModel->GetTimeData().back();
        double dx2 = -remaintime * data.dPipeSpeed;
        
        horipix = (dx - dx2) * CANVAS_SCALETO_WIDTH;
        DLOG(INFO) << "ai hori_overlap_pix: " << horipix;
    }

    return vertpix > 0.5 && horipix > 0.5;
}


void CCollisionTimeForecaster::SetBestJumpOffsetY(float fOffsetY)
{
    DLOG(INFO) << "ai CCollisionTimeForecaster::SetBestJumpOffsetY " << fOffsetY;
    m_fBestJumpOffsetY = fOffsetY;
}


void CCollisionTimeForecaster::GenParabolaDots(int h, int w, double a, double b, double c)
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
