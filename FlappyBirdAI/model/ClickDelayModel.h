#pragma once
#include <vector>


class CClickDelayModel
{
public:
    CClickDelayModel();

    void OnGetBottomData(float fBottomOffset);

    void OnClick(double dRemainCollisionTime, double bCollisionTime);

    void ResetData();

    const std::vector<double>& GetRemainCollisionTimeList() { return m_lRemainCollisionTime; };
    const std::vector<float>& GetBottomOffsetList() { return m_lBottomOffset; };

    float GetBestDelayTime() { return m_fBestDelayTime; };

    void GetKB(double& k, double& b) { k=m_k; b=m_b; };

    float GetClickDelay() { return m_fClickDelay; };

private:

    void _PushData(double dRemainCollisionTime, float fBottomOffset);

    // delete some data when sizeof m_lRemainCollisionTime is too large, to reduce memory usage.
    void _CompactData();

    double m_dRemainCollisionTime;

    float m_fBestDelayTime;
    size_t m_iTotalDataCount;

    float m_fClickDelay;

    // parameter of parabola
    double m_k, m_b;

    std::vector<double> m_lRemainCollisionTime;
    std::vector<float> m_lBottomOffset;
};