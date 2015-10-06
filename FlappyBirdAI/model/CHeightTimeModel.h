#pragma once
#include <vector>

class CHeightTimeModel
{
public:
    CHeightTimeModel();

    void Append(float fBirdHeightOnGround, float fPipeHeight, float dt);

    void OnBirdJump();

    bool IsNeedJumpNow();
private:

    void ResetData();

    void OnPipeHeightChange(float height);

    void PushBirdHeightOnPipe(float height);

    // return is bird dropping down now
    bool IsBirdDroppingDown();

    // get remain crash time in ms
    float GetRemainCrashTime();


    std::vector<float> m_lBirdHeightOnGround;
    std::vector<float> m_lBirdHeightOnPipe;
    std::vector<float> m_lTime;

    float m_fCurTime;
    float m_fPipeHeight;
    float m_fMaxBirdHeightOnPipe;
};