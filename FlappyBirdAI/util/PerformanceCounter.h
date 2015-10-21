//PerformanceCounter.h
#pragma once

void InitPreciseTickCount();
double GetPreciseTickCount();
void LogPerformance(const char* name, double tickcount);

template<class T>
class CPerformanceCounter
{
public:
    ~CPerformanceCounter()
    {
    }


    static CPerformanceCounter & getInstance(const char* name)
    {
        static CPerformanceCounter instance(name);
        return instance ;
    }


    void BeginCounter()
    {
        m_tmpCounter = GetPreciseTickCount();
    }


    void EndCounter()
    {
        LogPerformance(m_szName, GetPreciseTickCount() - m_tmpCounter);
    }


private:
    CPerformanceCounter(const char* name)
        :m_szName(name)
    {
    }


    const char* m_szName ;
    double m_tmpCounter ;

};


template <class T>
class CPerformanceCounterHelper
{
public:
    CPerformanceCounterHelper(const char *counter_name)
        :m_szName(counter_name)
    {
        CPerformanceCounter<T>::getInstance(m_szName).BeginCounter();
    }


    ~CPerformanceCounterHelper()
    {
        CPerformanceCounter<T>::getInstance(m_szName).EndCounter();
    }


private:
    const char* m_szName;
};


#define DEFINE_COUNTER(x) struct x;

#ifdef ENABLE_PERFORMANCE_COUNTER
#define BEGIN_COUNTER(counter_name) CPerformanceCounter<counter_name>::getInstance(#counter_name).BeginCounter();
#define END_COUNTER(counter_name) CPerformanceCounter<counter_name>::getInstance(#counter_name).EndCounter();
#define COUNTER_HELPER(counter_name) CPerformanceCounterHelper<counter_name> counter_##counter_name(#counter_name);
#else
#define BEGIN_COUNTER(counter_name)
#define END_COUNTER(counter_name)
#define COUNTER_HELPER(counter_name)
#endif
