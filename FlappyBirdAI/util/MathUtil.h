#pragma once
#include <vector>

namespace MathUtil
{

// fit parabola by x[], y[]
// y = ax^2 + bx + c
void ParabolaFit(
    const std::vector<double>& x,
    const std::vector<float>& y,
    /*OUT*/ double& a,
    /*OUT*/ double& b,
    /*OUT*/ double& c);


// fit straight line by x[], y[]
// y = kx + b
void StraightLineFit(
    const std::vector<double>& x,
    const std::vector<float>& y,
    /*OUT*/ double& k,
    /*OUT*/ double& b);


template <typename T>
class TAverageValue
{
public:
    TAverageValue(T defData) : m_nCount(0) { m_AverageValue = defData; };

    void Append(T data)
    {
        float newcount = (float)(m_nCount + 1);
        m_AverageValue = m_nCount/newcount * m_AverageValue + data/newcount;
        m_nCount++;
    };

    inline T GetAverageValue() { return m_AverageValue; };

private:
    size_t m_nCount;
    T m_AverageValue;
};

}; // namespace MathUtil