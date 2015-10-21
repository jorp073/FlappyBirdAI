#include "stdafx.h"
#include "CClickDelayModel.h"
#include "CCrashTimeForecaster.h"
#include "../util/MathUtil.h"


#define MIN_DATA_COUNT 50
#define MAX_DATA_COUNT 100


CClickDelayModel::CClickDelayModel()
    : m_fRemainCrashTime(0)
    , m_fBestDelayTime(0)
    , m_fClickDelay(DEFAULT_CLICK_DELAYTIME)
{
    ResetData();
}


void CClickDelayModel::ResetData()
{
    m_k = 0;
    m_b = 0;

    // forget last data, last data maybe wrong because of bird's crash
    if (m_lRemainCrashTime.size() > 1)
    {
        m_lRemainCrashTime.pop_back();
        m_lBottomOffset.pop_back();
    }
}


void CClickDelayModel::OnGetBottomData(float fBottomOffset)
{
    DLOG(INFO) << "ai CClickDelayModel::OnGetBottomData " << fBottomOffset;

    _PushData(m_fRemainCrashTime, fBottomOffset);
}


void CClickDelayModel::OnClick(float fRemainCrashTime)
{
    DLOG(INFO) << "ai CClickDelayModel::OnClick " << fRemainCrashTime;
    m_fRemainCrashTime = fRemainCrashTime;
}


void CClickDelayModel::_PushData(double dRemainCrashTime, float fBottomOffset)
{
    // do not push data of emergency jump and first invalid data
    if (0 == dRemainCrashTime) return;

    m_lRemainCrashTime.push_back(dRemainCrashTime);
    m_lBottomOffset.push_back(fBottomOffset);

    // fit parabola
    if (m_lRemainCrashTime.size() > 1)
    {
        MathUtil::StraightLineFit(m_lRemainCrashTime, m_lBottomOffset, m_k, m_b);
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
    if (m_lRemainCrashTime.size() > MAX_DATA_COUNT)
    {
        std::vector<double> tmp1;
        for (int i=0; i<MIN_DATA_COUNT; i++)
        {
            tmp1.push_back(m_lRemainCrashTime.back());
            m_lRemainCrashTime.pop_back();
        }

        std::vector<float> tmp2;
        for (int i=0; i<MIN_DATA_COUNT; i++)
        {
            tmp2.push_back(m_lBottomOffset.back());
            m_lBottomOffset.pop_back();
        }

        m_lRemainCrashTime.clear();
        m_lBottomOffset.clear();

        for (int i=0; i<MIN_DATA_COUNT; i++)
        {
            m_lRemainCrashTime.push_back(tmp1.back());
            tmp1.pop_back();
        }

        for (int i=0; i<MIN_DATA_COUNT; i++)
        {
            m_lBottomOffset.push_back(tmp2.back());
            tmp2.pop_back();
        }
    }
}
