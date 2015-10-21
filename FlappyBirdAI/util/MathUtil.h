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

}; // namespace MathUtil