#include "stdafx.h"
#include "HeightTimeModel.h"
#include "../util/PerformanceCounter.h"


CHeightTimeModel::CHeightTimeModel()
    : m_fPipeHeight(0)
    , m_dFirstTickCount(0)
{
}


void CHeightTimeModel::Append(float fBirdHeightOnGround, float fPipeHeight, double dTickcount)
{
    if (0 == m_lTime.size())
    {
        m_dFirstTickCount = dTickcount;
    }

    double dCurTime = dTickcount - m_dFirstTickCount;

    DLOG(INFO) << "ai CHeightTimeModel::Append " << fBirdHeightOnGround << "," << fPipeHeight << "," << dCurTime;

    if (fabs(fPipeHeight - m_fPipeHeight) > 0.00001f)
    {
        OnPipeHeightChange(fPipeHeight);
    }

    m_lTime.push_back(dCurTime);

    m_lBirdHeightOnGround.push_back(fBirdHeightOnGround);
    PushBirdHeightOnPipe(fBirdHeightOnGround - m_fPipeHeight);
}


void CHeightTimeModel::ResetData()
{
    m_lBirdHeightOnGround.clear();
    m_lBirdHeightOnPipe.clear();
    m_lTime.clear();
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
    m_lBirdHeightOnPipe.push_back(height);
}

