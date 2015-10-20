#pragma once

#include "../util/SingleInstance.h"

class CHeightTimeModel;
class CJumpRangeModel;
class CClickDelayModel;


class CBirdHeightObserver
{
public:
    bool Init();
    bool Update(double dTickCount);

    void ResetData();

private:

    CHeightTimeModel*       m_pHeightData;
    CJumpRangeModel*        m_pJumpRangeData;
    CClickDelayModel*       m_pClickDelay;


    DEFINE_SINGLEINSTANCE(CBirdHeightObserver);
};

