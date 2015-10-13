#include "stdafx.h"
#include "MathUtil.h"
#include <math.h>

void ParabolaFit(
    const std::vector<float>& x,
    const std::vector<float>& y,
    /*OUT*/ double& a,
    /*OUT*/ double& b,
    /*OUT*/ double& c)
{
    double sum_x4=0, sum_x3=0, sum_x2=0, sum_x=0, sum_n=0, sum_y=0, sum_yx=0, sum_yx2=0;

    auto count = x.size();
    for (int i=0;i<count;i++)
    {
        sum_n += 1;
        sum_x += x[i];
        sum_x2 += pow(x[i],2.0);
        sum_x3 += pow(x[i],3.0);
        sum_x4 += pow(x[i],4.0);
        sum_y += y[i];
        sum_yx += y[i]*x[i];
        sum_yx2 += y[i]*x[i]*x[i];
    }

    double a1,a2,a3,b1,b2,b3,c1,c2,c3;
    a1 = sum_x3/sum_x4;
    b1 = sum_x2/sum_x4;
    c1 = sum_yx2/sum_x4;
    a2 = sum_x2-sum_x3*a1;
    b2 = sum_x-sum_x2*a1;
    c2 = sum_yx-sum_yx2*a1;
    a3 = sum_x-sum_x3*b1;
    b3 = sum_n-sum_x2*b1;
    c3 = sum_y-sum_yx2*b1;

    double d1,d2,d3,e1,e2,e3;
    d2 = b2/a2;
    e2 = c2/a2;
    d1 = b1-a1*d2;
    e1 = c1-a1*e2;
    d3 = b3-a3*d2;
    e3 = c3-a3*e2;
    c = e3/d3;
    a = e1-d1*c;
    b = e2-d2*c;
}