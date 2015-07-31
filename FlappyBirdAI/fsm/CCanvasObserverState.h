#pragma once

#include "base.h"
#include "opencv2/core/core.hpp"

class CCanvasObserver;

namespace CanvasObserverState
{
    class CSearch : public TState<CCanvasObserver>
    {
    public:
        CSearch() : TState("Search") {};

        virtual bool Update(CCanvasObserver* observer);

        bool SearchCanvas(cv::Mat mat);
    };


    class CFound : public TState<CCanvasObserver>
    {
    public:
        CFound() : TState("Found") {};

        virtual bool Update(CCanvasObserver* observer);
    };
};
