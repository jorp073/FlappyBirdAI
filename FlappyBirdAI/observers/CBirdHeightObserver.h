#pragma once

#include "../util/SingleInstance.h"

class CHeightTimeModel;
class CMouseClicker;

class CBirdHeightObserver
{
public:
    bool Init();
    bool Update(float dt);


private:
    CHeightTimeModel* m_HeightData;
    CMouseClicker* m_MouseClicker;


    DEFINE_SINGLEINSTANCE(CBirdHeightObserver);
};

