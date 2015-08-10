#include "stdafx.h"
#include "CGameState.h"
#include "../observers/CGameStateObserver.h"
#include "../observers/CCanvasObserver.h"
#include "../output/OutputWindow.h"
#include "../util/CPerformanceCounter.h"

using namespace GameState;

DEFINE_COUNTER(CPlay_Update);

const double CBase::MIN_MATCH_VALUE = 0.001;

/////////////////////// CBase

double CBase::_GetMatchResult(cv::Mat mat, cv::Mat templ, OUT cv::Point& matchLoc)
{
    int result_cols =  mat.cols - templ.cols + 1;
    int result_rows = mat.rows - templ.rows + 1;
    auto result = cv::Mat(result_rows, result_cols, CV_32FC1);

    /// match
    auto match_method = CV_TM_SQDIFF_NORMED; // notice minVal
    matchTemplate(mat, templ, result, match_method );
    // imshow("match", result);

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
    return finalVal;
}


bool CBase::isMatchResultIncrease(CGameStateObserver* observer, double result)
{
    if (result > m_fLastMatchResult)
    {
        observer->StateMachine()->ChangeState(new CUnknown);
        return true;
    }
    else
    {
        m_fLastMatchResult = result;
        return false;
    }
}


double CBase::MatchTitle(CGameStateObserver* observer)
{
    auto mat = CCanvasObserver::GetInstance()->GetCanvasMat();
    auto templ = observer->GetTemplateMat("title");
    cv::Point pt;
    return _GetMatchResult(mat, templ, pt);
}


double CBase::MatchGetReady(CGameStateObserver* observer)
{
    auto mat = CCanvasObserver::GetInstance()->GetCanvasMat();
    auto templ = observer->GetTemplateMat("getready");
    cv::Point pt;
    return _GetMatchResult(mat, templ, pt);
}


double CBase::MatchGameOver(CGameStateObserver* observer)
{
    auto mat = CCanvasObserver::GetInstance()->GetCanvasMat();
    auto templ = observer->GetTemplateMat("gameover");
    cv::Point pt;
    return _GetMatchResult(mat, templ, pt);
}


/////////////////////// CUnknown


bool CUnknown::Update(CGameStateObserver* observer)
{
    typedef double (CBase::*MATCH_FUNC)(CGameStateObserver*);
    MATCH_FUNC matchfunc[] = {
        &CBase::MatchTitle,
        &CBase::MatchGetReady,
        &CBase::MatchGameOver,
    };
    int funccount = sizeof(matchfunc)/sizeof(MATCH_FUNC);

    /// change to non-play state
    for (int i=0; i<funccount; i++)
    {
        auto result = (this->*matchfunc[i])(observer);
        DLOG(INFO) << "match id: " << i << ", result: " << result;
        if (result < MIN_MATCH_VALUE)
        {
            /// change state
            CBase* state = NULL;
            switch(i)
            {
            case 0:
                state = new CTitle();
                break;
            case 1:
                state = new CGetReady();
                break;
            case 2:
                state = new CGameOver();
                break;
            default:
                assert(false);
            }
            observer->StateMachine()->ChangeState(state);
            return true;
        }
    }

    // change to play state
    observer->StateMachine()->ChangeState(new CPlay());
    return true;
}


//////////////////////// Title

bool CTitle::Update(CGameStateObserver* observer)
{
    auto result = MatchTitle(observer);
    isMatchResultIncrease(observer, result);
    return true;
}


//////////////////////// Get Ready

bool CGetReady::Update(CGameStateObserver* observer)
{
    auto result = MatchGetReady(observer);
    isMatchResultIncrease(observer, result);
    return true;
}


//////////////////////// Game Over

bool CGameOver::Update(CGameStateObserver* observer)
{
    auto result = MatchGameOver(observer);
    isMatchResultIncrease(observer, result);
    return true;
}


//////////////////////// Play

bool CPlay::Update(CGameStateObserver* observer)
{
    COUNTER_HELPER(CPlay_Update);

    /// Binary gray image
    auto canvasmat = CCanvasObserver::GetInstance()->GetCanvasMat();
    auto mat = cv::Mat();
    cv::threshold(canvasmat, mat, 0, 255, 0);

    /// Dilate and Erode, to get less contours
    int dilation_size = 1;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
        cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
        cv::Point( dilation_size, dilation_size ) );
    cv::dilate(mat, mat, element);
    cv::erode(mat, mat, element);
    //cv::imshow("bin", mat);

    /// Find all contours
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    findContours(mat, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    DLOG(INFO) << "findContours count: " << contours.size();

    /// Find bird contour, and get rect
    std::vector<cv::Rect> rectBirds;
    for (size_t i = 0; i < contours.size(); i++)
    {
        cv::Rect rect;
        if (!getBirdRect(contours[i], rect)) continue;
        rectBirds.push_back(rect);
    }

    COutputWindow::GetInstance()->SetBirdRects(rectBirds);

    switch(rectBirds.size())
    {
    case 0:
        // not found any find
        observer->StateMachine()->ChangeState(new CUnknown());
        return true;
    case 1:
        // find singleton bird
        break;
    default:
        // find more than one bird
        // TODO get nearest bird
        observer->StateMachine()->ChangeState(new CUnknown());
        break;
    }

    // try to find bird pos

    return true;
}

bool CPlay::getBirdRect(const std::vector<cv::Point>& contour, OUT cv::Rect& rect)
{
    auto rectBound = cv::boundingRect(cv::Mat(contour));

    // check rect center x ratio on canvas
    float cx = (rectBound.tl().x + rectBound.br().x)/2.0f;
    float cxratio = cx/CANVAS_SCALETO_WIDTH;
    DLOG(INFO) << "cxratio = " << cxratio;
    if (fabs(cxratio - BIRDX_RATIO) > BIRDX_RATIO_OFFSET) return false;

    // check contour area
    double area = fabs(contourArea(contour));
    DLOG(INFO) << "area= " << area;
    if (fabs(area - BIRD_CONTOUR_AREA) > BIRD_CONTROU_AREA_OFFSET) return false;

    rect = rectBound;
    return true;
}

