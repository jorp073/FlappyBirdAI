#include "stdafx.h"
#include "CPerformanceCounter.h"

static double QUADPART = 0;

void InitPreciseTickCount()
{
    LARGE_INTEGER freqc;
    QueryPerformanceFrequency(&freqc);
    QUADPART = 1000.0/freqc.QuadPart;
    DLOG(INFO) << "QUADPART: " << QUADPART;
}


double GetPreciseTickCount() {
    LARGE_INTEGER llStart;
    QueryPerformanceCounter(&llStart);
    return (double)(llStart.QuadPart * QUADPART);
}


void LogPerformance(const char* name, double tickcount)
{
    DLOG(INFO) << "PerformaceCounter: " << name << ": " << (double)tickcount;
}
