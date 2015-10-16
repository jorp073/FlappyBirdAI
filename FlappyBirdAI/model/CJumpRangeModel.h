#pragma once
#include <list>
#include <vector>


struct JUMP_RANGE
{
    float fBottom;
    float fTop;
    float fRange;
};


class CJumpRangeModel
{
public:
    CJumpRangeModel();

    void OnBirdHeightChanged(float fBirdHeight);

    void OnClick(float fNewPipeHeight);

    void ResetData();

    const std::list<JUMP_RANGE>& GetRangeData() { return m_lData; };

    float GetBestBottomOffset() { return m_fBestBottomOffset; };

private:

    void _PushData();

    bool _IsDataValid();

    float m_fPipeHeight;

    float m_fBottom, m_fTop;

    int m_iId, m_iBottomId, m_iTopId;

    bool m_bDataValid;

    float m_fAverageRange, m_fBestBottomOffset;
    size_t m_iTotalDataCount;

    std::vector<float> m_lBirdHeight;

    std::list<JUMP_RANGE> m_lData;
};