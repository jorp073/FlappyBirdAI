#pragma once

#include "../util/SingleInstance.h"

class CHeightTimeModel;
class CMouseClicker;
class CCrashTimeForecaster;


class CBirdHeightObserver
{
public:
    bool Init();
    bool Update(float dt);

    void ResetData();

private:

    CHeightTimeModel*       m_pHeightData;
    CMouseClicker*          m_pMouseClicker;
    CCrashTimeForecaster*   m_pCrashTimeForecaster;


    DEFINE_SINGLEINSTANCE(CBirdHeightObserver);
};

