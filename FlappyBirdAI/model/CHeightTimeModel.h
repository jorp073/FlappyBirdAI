#pragma once
#include <vector>


class CHeightTimeModel
{
public:
    CHeightTimeModel();

    void Append(float fBirdHeightOnGround, float fPipeHeight, float dt);

    void ResetData();

    const std::vector<float>& GetBirdHeightData() { return m_lBirdHeightOnPipe; };

    const std::vector<float>& GetTimeData() { return m_lTime; };

    double GetTimeSinceFirstData();

private:

    void OnPipeHeightChange(float height);

    void PushBirdHeightOnPipe(float height);

    std::vector<float> m_lBirdHeightOnGround;
    std::vector<float> m_lBirdHeightOnPipe;
    std::vector<float> m_lTime;

    float m_fCurTime;
    float m_fPipeHeight;
    double m_dFirstDataTickCount;
};