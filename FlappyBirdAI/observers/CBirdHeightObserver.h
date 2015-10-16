#pragma once

#include "../util/SingleInstance.h"

class CHeightTimeModel;
class CMouseClicker;
class CJumpRangeModel;


class CBirdHeightObserver
{
public:
    bool Init();
    bool Update(double dTickCount);

    void ResetData();

private:

    CHeightTimeModel*       m_pHeightData;
    CJumpRangeModel*        m_pJumpRangeData;
    CMouseClicker*          m_pMouseClicker;


    DEFINE_SINGLEINSTANCE(CBirdHeightObserver);
};

