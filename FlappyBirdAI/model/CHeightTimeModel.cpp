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
    if (fabs(fPipeHeight - m_fPipeHeight) > 0.0001f)
    {
        OnPipeHeightChange(fPipeHeight);
    }

    dt += m_fCurTime;
    m_lTime.push_back(dt);

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
