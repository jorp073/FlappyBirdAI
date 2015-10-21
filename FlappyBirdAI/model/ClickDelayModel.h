#pragma once
#include <vector>


class CClickDelayModel
{
public:
    CClickDelayModel();

    void OnGetBottomData(float fBottomOffset);

    void OnClick(float fRemainCrashTime);

    void ResetData();

    const std::vector<double>& GetRemainCrashTimeList() { return m_lRemainCrashTime; };
    const std::vector<float>& GetBottomOffsetList() { return m_lBottomOffset; };

    float GetBestDelayTime() { return m_fBestDelayTime; };

    void GetKB(double& k, double& b) { k=m_k; b=m_b; };

    float GetClickDelay() { return m_fClickDelay; };

private:

    void _PushData(double dRemainCrashTime, float fBottomOffset);

    // delete some data when sizeof m_lRemainCrashTime is too large, to reduce memory usage.
    void _CompactData();

    float m_fRemainCrashTime;

    float m_fBestDelayTime;
    size_t m_iTotalDataCount;

    float m_fClickDelay;

    // parameter of parabola
    double m_k, m_b;

    std::vector<double> m_lRemainCrashTime;
    std::vector<float> m_lBottomOffset;
};