#include "stdafx.h"
#include "CGameState.h"
#include "../controller/CGameStateObserver.h"
#include "../controller/CCanvasObserver.h"


using namespace GameState;

/////////////////////// CBase

double CBase::_GetMatchResult(cv::Mat mat, cv::Mat templ, OUT cv::Point matchLoc)
{
    int result_cols =  mat.cols - templ.cols + 1;
    int result_rows = mat.rows - templ.rows + 1;
    auto result = cv::Mat(result_rows, result_cols, CV_32FC1);

    /// match
    auto match_method = CV_TM_SQDIFF_NORMED; // notice minVal
    matchTemplate(mat, templ, result, match_method );
    imshow("result1", result);

    /// locate match position
    double minVal, maxVal, finalVal;
    cv::Point minLoc, maxLoc;

    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

    if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
    {
        matchLoc = minLoc;
        finalVal = minVal;
    }
    else
    {
        matchLoc = maxLoc; 
        finalVal = maxVal;
    }
    printf("matchLoc (%d, %d) = %f\n", matchLoc.x, matchLoc.y, finalVal);
    return finalVal;
}


bool CBase::IsInTitleState(CGameStateObserver* observer)
{
    auto mat = CCanvasObserver::GetInstance()->GetGrayCanvasMat();
    auto templ = observer->GetTemplateMat("title");
    cv::Point pt;
    auto value = _GetMatchResult(mat, templ, pt);
    // title.png value:
    // title state:     0
    // get ready state: 0.1607
    // play state:      0.17
    // game over state: 0.0326
    return value < 0.0001;
}


/////////////////////// CUnknown


bool CUnknown::Update(CGameStateObserver* observer)
{
    printf("CUnknown::Update()\n");
    if (IsInTitleState(observer))
        return true;
    // 
    return true;
}


//////////////////////// Found

bool CTitle::Update(CGameStateObserver* observer)
{
    return true;
}
