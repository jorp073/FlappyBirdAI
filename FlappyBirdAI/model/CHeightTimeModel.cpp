#include "stdafx.h"
#include "CHeightTimeModel.h"


CHeightTimeModel::CHeightTimeModel()
    : m_fCurTime(0)
    , m_fPipeHeight(0)
    , m_fMaxBirdHeightOnPipe(0)
{
}


void CHeightTimeModel::Append(float fBirdHeightOnGround, float fPipeHeight, float dt)
{
    DLOG(INFO) << "ai CHeightTimeModel::Append " << fBirdHeightOnGround << "," << fPipeHeight << "," << dt;

    if (fabs(fPipeHeight - m_fPipeHeight) > 0.0001f)
    {
        OnPipeHeightChange(fPipeHeight);
    }

    m_fCurTime += dt;
    m_lTime.push_back(m_fCurTime);

    m_lBirdHeightOnGround.push_back(fBirdHeightOnGround);
    PushBirdHeightOnPipe(fBirdHeightOnGround - m_fPipeHeight);
}


void CHeightTimeModel::OnBirdJump()
{
    ResetData();
}



void CHeightTimeModel::ResetData()
{
    m_lBirdHeightOnGround.clear();
    m_lBirdHeightOnPipe.clear();
    m_lTime.clear();
    m_fCurTime = 0;
    m_fMaxBirdHeightOnPipe = 0;
}


void CHeightTimeModel::OnPipeHeightChange(float height)
{
    m_lBirdHeightOnPipe.clear();
    m_fPipeHeight = height;

    for (auto h : m_lBirdHeightOnGround)
    {
        PushBirdHeightOnPipe(h - height);
    }
}


void CHeightTimeModel::PushBirdHeightOnPipe(float height)
{
    if (height > m_fMaxBirdHeightOnPipe) m_fMaxBirdHeightOnPipe = height;

    m_lBirdHeightOnPipe.push_back(height);
}


bool CHeightTimeModel::IsNeedJumpNow()
{
    if (!IsBirdDroppingDown()) return false;

    float time = GetRemainCrashTime();

    return  time < 0.30f * 1000;
}


bool CHeightTimeModel::IsBirdDroppingDown()
{
    auto count = m_lBirdHeightOnPipe.size();
    if (count < 2) return false;

    return m_lBirdHeightOnPipe[count-2] > m_lBirdHeightOnPipe[count-1];
}


float CHeightTimeModel::GetRemainCrashTime()
{
    auto height = m_lBirdHeightOnPipe.back();

    auto count = m_lBirdHeightOnPipe.size();
    auto dh = m_lBirdHeightOnPipe[count-2] - m_lBirdHeightOnPipe[count-1];
    auto dt = m_lTime[count-1] - m_lTime[count-2];
    auto speed = dh / dt;
    DLOG(INFO) << "ai dh:" << dh << " dt:" << dt << " speed:" << speed;
    CHECK(dh >= 0) << "dh must > 0!";
    CHECK(dt >= 0) << "dt must > 0!";

    float remaintime = height/speed;
    DLOG(INFO) << "ai height:" << height << " remaincrashtime:" << remaintime;

    return remaintime;
}