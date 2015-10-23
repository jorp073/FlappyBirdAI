#include "stdafx.h"
#include "ClickDelayModel.h"
#include "CollisionForecaster.h"
#include "../util/MathUtil.h"

#define INVALID_REMAIN_COLLISION_TIME -1

#define MIN_DATA_COUNT 50
#define MAX_DATA_COUNT 100


CClickDelayModel::CClickDelayModel()
    : m_dRemainCollisionTime(INVALID_REMAIN_COLLISION_TIME)
    , m_fBestDelayTime(0)
    , m_fClickDelay(DEFAULT_CLICK_DELAYTIME)
{
    ResetData();
}


void CClickDelayModel::ResetData()
{
    m_k = 0;
    m_b = 0;

    // forget last data, last data maybe wrong because of bird's collision
    if (m_lRemainCollisionTime.size() > 1)
    {
        m_lRemainCollisionTime.pop_back();
        m_lBottomOffset.pop_back();
    }
}


void CClickDelayModel::OnGetBottomData(float fBottomOffset)
{
    _PushData(m_dRemainCollisionTime, fBottomOffset);
}


void CClickDelayModel::OnClick(double dRemainCollisionTime, double bCollisionTime)
{
    if (EMERGENCY_JUMP_TIME == bCollisionTime || WILL_NOT_CRASH_TIME == bCollisionTime)
    {
        m_dRemainCollisionTime = INVALID_REMAIN_COLLISION_TIME;
    }
    else
    {
        m_dRemainCollisionTime = dRemainCollisionTime;
    }
}


void CClickDelayModel::_PushData(double dRemainCollisionTime, float fBottomOffset)
{
    // do not push data of emergency jump and first invalid data
    if (INVALID_REMAIN_COLLISION_TIME == dRemainCollisionTime) return;

    DLOG(INFO) << "CClickDelayModel::_PushData " << dRemainCollisionTime << ", " << fBottomOffset;

    m_lRemainCollisionTime.push_back(dRemainCollisionTime);
    m_lBottomOffset.push_back(fBottomOffset);

    // fit parabola
    if (m_lRemainCollisionTime.size() > 1)
    {
        MathUtil::StraightLineFit(m_lRemainCollisionTime, m_lBottomOffset, m_k, m_b);
    }

    // try get best click delay time
    m_fBestDelayTime = 0;
    if (m_k > 1e-5 || m_k < -1e-5)
    {
        m_fBestDelayTime = (float)(-m_b/m_k);

        const float STEP = 1;
        if (fabsf(m_fClickDelay - m_fBestDelayTime) <= STEP)
        {
            m_fClickDelay = m_fBestDelayTime;
        }
        else if (m_fClickDelay < m_fBestDelayTime)
        {
            m_fClickDelay += STEP;
        }
        else
        {
            m_fClickDelay -= STEP;
        }
    }

    _CompactData();
}


void CClickDelayModel::_CompactData()
{
    if (m_lRemainCollisionTime.size() > MAX_DATA_COUNT)
    {
        std::vector<double> tmp1;
        for (int i=0; i<MIN_DATA_COUNT; i++)
        {
            tmp1.push_back(m_lRemainCollisionTime.back());
            m_lRemainCollisionTime.pop_back();
        }

        std::vector<float> tmp2;
        for (int i=0; i<MIN_DATA_COUNT; i++)
        {
            tmp2.push_back(m_lBottomOffset.back());
            m_lBottomOffset.pop_back();
        }

        m_lRemainCollisionTime.clear();
        m_lBottomOffset.clear();

        for (int i=0; i<MIN_DATA_COUNT; i++)
        {
            m_lRemainCollisionTime.push_back(tmp1.back());
            tmp1.pop_back();
        }

        for (int i=0; i<MIN_DATA_COUNT; i++)
        {
            m_lBottomOffset.push_back(tmp2.back());
            tmp2.pop_back();
        }
    }
}
