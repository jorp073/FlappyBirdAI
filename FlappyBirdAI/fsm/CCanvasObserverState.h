#pragma once

#include "base.h"
#include "opencv2/opencv.hpp"
#include <Windows.h>

class CCanvasObserver;

namespace CanvasObserverState
{
    class CSearch : public TState<CCanvasObserver>
    {
    public:
        CSearch() : TState("Search") {};

        virtual bool Update(CCanvasObserver* observer);

    private:
        cv::Mat _FilterCanvasBorder(cv::Mat mat);

        bool _GetCanvasBorderRect(cv::Mat mat, OUT cv::Rect& rect);
    };


    class CFound : public TState<CCanvasObserver>
    {
    public:
        CFound(cv::Rect rect)
            : TState("Found")
            , m_CanvasRect(rect)
        {};

        virtual bool Update(CCanvasObserver* observer);

    private:
        bool isCanvasPosMoved();

        cv::Rect m_CanvasRect;
    };
};
