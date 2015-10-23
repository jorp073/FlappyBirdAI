#pragma once
#include <functional>

namespace TimeUtil
{

typedef std::function<bool()> IS_TIME_UP_FUNC;

void WaitForEveryMS(const IS_TIME_UP_FUNC& fnIsTimeUp);


}; // namespace TimeUtil