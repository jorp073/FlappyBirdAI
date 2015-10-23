#pragma once
#include <vector>


class CHeightTimeModel
{
public:
    CHeightTimeModel();

    void Append(float fBirdHeightOnGround, float fPipeHeight, double dTickcount);

    void ResetData();

    const std::vector<float>& GetBirdHeightData() { return m_lBirdHeightOnPipe; };

    const std::vector<double>& GetTimeData() { return m_lTime; };

    double GetFirstTickCount() { return m_dFirstTickCount; };

private:

    void OnPipeHeightChange(float height);

    void PushBirdHeightOnPipe(float height);

    std::vector<float> m_lBirdHeightOnGround;
    std::vector<float> m_lBirdHeightOnPipe;
    std::vector<double> m_lTime;

    float m_fPipeHeight;
    double m_dFirstTickCount;
};