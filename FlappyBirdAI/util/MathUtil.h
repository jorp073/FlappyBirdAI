#pragma once
#include <vector>

// fit parabola by x[], y[]
// y = ax^2 + bx + c
void ParabolaFit(
    const std::vector<float>& x,
    const std::vector<float>& y,
    /*OUT*/ double& a,
    /*OUT*/ double& b,
    /*OUT*/ double& c);
