#include "stdafx.h"
#include "PerformanceCounter.h"

static double QUADPART = 0;

void InitPreciseTickCount()
{
    LARGE_INTEGER freqc;
    QueryPerformanceFrequency(&freqc);
    QUADPART = 1000.0/freqc.QuadPart;
    DLOG(INFO) << "QUADPART: " << QUADPART;
}


LONGLONG GetPreciseTickCount() {
    LARGE_INTEGER llStart;
    QueryPerformanceCounter(&llStart);
    return (LONGLONG)(llStart.QuadPart * QUADPART);
}


void LogPerformance(const char* name, LONGLONG tickcount)
{
    DLOG(INFO) << "PerformaceCounter: " << name << ": " << (unsigned int)tickcount;
}
