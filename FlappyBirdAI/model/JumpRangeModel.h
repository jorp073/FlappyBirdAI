#pragma once
#include <list>
#include <vector>
#include <functional>
#include "../util/MathUtil.h"


struct JUMP_RANGE
{
    float fBottom;
    float fTop;
    float fRange;
};


typedef std::function<void(float)> PUSHDATA_CALLBACK_FUNC;


class CJumpRangeModel
{
public:
    CJumpRangeModel();

    void OnBirdHeightChanged(float fBirdHeight);

    void TryPushData(float fNewPipeHeight);

    void ResetData();

    const std::list<JUMP_RANGE>& GetRangeData() { return m_lData; };

    float GetBestBottomOffset() { return m_fBestBottomOffset; };

    void SetPushDataCallback(PUSHDATA_CALLBACK_FUNC func)  { m_fnPushDataCallback = func; };

private:

    void _PushData(const JUMP_RANGE& data);

    bool _IsDataValid();

    float m_fPipeHeight;

    float m_fBottom, m_fTop;

    int m_iId, m_iBottomId, m_iTopId;

    bool m_bDataValid;
    bool m_bEmergencyJump;

    MathUtil::TAverageValue<float> m_fAverageRange;

    float m_fBestBottomOffset;

    std::vector<float> m_lBirdHeight;

    std::list<JUMP_RANGE> m_lData;

    PUSHDATA_CALLBACK_FUNC m_fnPushDataCallback;
};