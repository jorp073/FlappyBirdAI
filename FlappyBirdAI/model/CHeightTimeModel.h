#pragma once
#include <list>

class CHeightTimeModel
{
public:
    CHeightTimeModel();

    void Append(float fBirdHeightOnGround, float fPipeHeight, float dt);

    // todo check bird start jump
    void OnBirdJump();

private:

    void ResetData();

    void OnPipeHeightChange(float height);

    void PushBirdHeightOnPipe(float height);

    std::list<float> m_lBirdHeightOnGround;
    std::list<float> m_lBirdHeightOnPipe;
    std::list<float> m_lTime;

    float m_fCurTime;
    float m_fPipeHeight;
    float m_fMaxBirdHeightOnPipe;
};