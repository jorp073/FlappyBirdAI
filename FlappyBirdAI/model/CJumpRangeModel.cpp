#include "stdafx.h"
#include "CJumpRangeModel.h"
#include "CCrashTimeForecaster.h"


#define MAX_TOPBOTTOM_RECORD_COUNT 150
#define CHECK_DESCEND_COUNT 4


CJumpRangeModel::CJumpRangeModel()
    : m_fPipeHeight(DEFAULT_PIPE_HEIGHT)
    , m_fAverageRange(0)
    , m_iTotalDataCount(0)
    , m_fBestBottomOffset(PIPE_VERTICAL_DISTANCE/2)
{
    ResetData();
}


void CJumpRangeModel::ResetData()
{
    m_fBottom = 1;
    m_fTop = 0;
    m_bDataValid = false;
    m_iId = 0;
}


void CJumpRangeModel::OnBirdHeightChanged(float fBirdHeight)
{
    if (m_bDataValid) return;

    m_iId++;
    m_lBirdHeight.push_back(fBirdHeight);

    if (fBirdHeight < m_fBottom)
    {
        m_fBottom = fBirdHeight;
        m_iBottomId = m_iId;
    }

    if (fBirdHeight > m_fTop)
    {
        m_fTop = fBirdHeight;
        m_iTopId = m_iId;
    }

    m_bDataValid = _IsDataValid();
}


void CJumpRangeModel::OnClick(float fNewPipeHeight)
{
    if (m_bDataValid)  _PushData();

    ResetData();
    m_fPipeHeight = fNewPipeHeight;
}


void CJumpRangeModel::_PushData()
{
    JUMP_RANGE data;
    data.fBottom = m_fBottom - m_fPipeHeight;
    data.fTop = m_fTop - m_fPipeHeight;
    data.fRange = data.fTop - data.fBottom;

    m_lData.push_back(data);

    if (m_lData.size() > MAX_TOPBOTTOM_RECORD_COUNT) m_lData.pop_front();

    auto count = m_iTotalDataCount + 1;
    m_fAverageRange = (float)(m_iTotalDataCount) / count * m_fAverageRange + data.fRange / count;
    m_iTotalDataCount = count;

    m_fBestBottomOffset = (PIPE_VERTICAL_DISTANCE - m_fAverageRange)/2;
    CCrashTimeForecaster::GetInstance()->SetBestJumpOffsetY(m_fBestBottomOffset);


    DLOG(INFO) << "ai jump avr range:" << m_fAverageRange <<  " range: " << data.fRange << " bottom: " << data.fBottom << " top: " << data.fTop;
}


bool CJumpRangeModel::_IsDataValid()
{
    auto count = m_lBirdHeight.size();

    if (count <= CHECK_DESCEND_COUNT) return false;

    for (int i=0; i<CHECK_DESCEND_COUNT; i++)
    {
        if (m_lBirdHeight[count-1-i] >= m_lBirdHeight[count-2-i])
        {
            return false;
        }
    }

    // must get bottom height first
    if (m_iTopId <= m_iBottomId) return false;

    m_lBirdHeight.clear();
    return true;
}
