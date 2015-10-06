#pragma once

#include <windows.h>

class CMouseClicker
{
public:
    void TryClick();

private:
    bool IsInCanvas();

    void Click();
};