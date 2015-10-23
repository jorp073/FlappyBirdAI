#include "stdafx.h"
#include "TimeUtil.h"
#include <Windows.h>


void TimeUtil::WaitForEveryMS(const IS_TIME_UP_FUNC& fnIsTimeUp)
{
    if (TIMERR_NOERROR == ::timeBeginPeriod(1))
    {
        while (true)
        {
            if (!fnIsTimeUp())
            {
                ::Sleep(1);
            }
            else
            {
                ::timeEndPeriod(1);
                break;
            }
        }
    }
    else
    {
        DLOG(ERROR) << "timeBeginPeriod failed";
    }
}