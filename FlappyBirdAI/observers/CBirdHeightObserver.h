#pragma once

#include "../util/SingleInstance.h"
#include "../model/CHeightTimeModel.h"

class CBirdHeightObserver
{
public:
    bool Init();
    bool Update(float dt);

private:

    DEFINE_SINGLEINSTANCE(CBirdHeightObserver);
    CHeightTimeModel m_HeightData;
};

