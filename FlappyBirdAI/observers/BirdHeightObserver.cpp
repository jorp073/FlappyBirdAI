#include "stdafx.h"
#include "BirdHeightObserver.h"
#include "../output/OutputWindow.h"
#include "../model/HeightTimeModel.h"
#include "../util/MouseController.h"
#include "../model/CollisionForecaster.h"
#include "../recorder/Recorder.h"
#include "../model/JumpRangeModel.h"
#include "../model/ClickDelayModel.h"
#include "BirdRectObserver.h"
#include "PipeObserver.h"
#include "../util/PerformanceCounter.h"
#include "../util/TimeUtil.h"


INIT_SINGLEINSTANCE(CBirdHeightObserver);


CBirdHeightObserver::CBirdHeightObserver()
    : m_pHeightData(new CHeightTimeModel())
    , m_pJumpRangeData(new CJumpRangeModel())
    , m_pClickDelay(new CClickDelayModel())
{
    CCollisionTimeForecaster::GetInstance()->SetModel(m_pHeightData);

    auto pushdata_callback = [=](float fBottomOffset)
    {
        m_pClickDelay->OnGetBottomData(fBottomOffset);
    };

    m_pJumpRangeData->SetPushDataCallback(pushdata_callback);
}


CBirdHeightObserver::~CBirdHeightObserver()
{
    delete m_pHeightData;
    delete m_pJumpRangeData;
    delete m_pClickDelay;
}


bool CBirdHeightObserver::Update(double dTickCount)
{
    auto pPipeObserver = CPipeObserver::GetInstance();
    auto pBirdRectObserver = CBirdRectObserver::GetInstance();

    auto fBirdHeight = pBirdRectObserver->GetBirdHeight();
    auto fPipeHeight = pPipeObserver->GetPipeHeight();
    DLOG(INFO) << "bird height: " << fBirdHeight << ", pipe height: " << fPipeHeight
        << ", pipe right: " << pPipeObserver->GetPipeRight();

    auto pOutputWindow = COutputWindow::GetInstance();

    m_pHeightData->Append(fBirdHeight, fPipeHeight, dTickCount);
    pOutputWindow->SetPipeHeight(fPipeHeight);
    m_pJumpRangeData->OnBirdHeightChanged(fBirdHeight);

    /// forecast collision bottom remain time
    auto pCollisionTimeForecaster = CCollisionTimeForecaster::GetInstance();

    const auto dCollisionTime = pCollisionTimeForecaster->GetCollisionBottomTime();
    const auto dCollisionTickcount = dCollisionTime + m_pHeightData->GetFirstTickCount();
    const auto fClickDelay = m_pClickDelay->GetClickDelay();
    auto dRemainClickTime = dCollisionTickcount - GetPreciseTickCount() - fClickDelay;

    bool bNeedJumpNow = WILL_NOT_CRASH_TIME != dCollisionTime;
    bool bDisplayRemainTime = bNeedJumpNow;

    DLOG(INFO) << "ai collision time: " << dCollisionTime << " needjumpnow: " << bNeedJumpNow;

    /// caculate remain click time
    if (bNeedJumpNow && EMERGENCY_JUMP_TIME != dCollisionTime)
    {
        DLOG(INFO) << "ai bRemainClickTime:" << dRemainClickTime;

        bNeedJumpNow = dRemainClickTime <= 17;
    }

    /// forecast collision-corner
    if (bNeedJumpNow)
    {
        CRASH_FORECAST_PARAM param;
        param.fAverageBirdLeft = pBirdRectObserver->GetAverageBirdLeft();
        param.fAverageBirdRectHeight = pBirdRectObserver->GetAverageBirdRectHeight();
        param.dPipeSpeed = pPipeObserver->GetPipeSpeed();
        param.fPipeRight = pPipeObserver->GetPipeRight();

        bNeedJumpNow = pCollisionTimeForecaster->IsWillCollisionPipeCorner(param);

        DLOG_IF(INFO, !bNeedJumpNow) << "ai collision bottom, but will not collision corner";
    }

    if (bNeedJumpNow)
    {
        // wait for click time
        if (EMERGENCY_JUMP_TIME != dCollisionTime)
        {
            auto fnIsTimeUp = [=]()
            {
                auto bRemainClickTime = dCollisionTickcount - GetPreciseTickCount() - fClickDelay;
                DLOG(INFO) << "ai waitforclick bRemainClickTime=" << bRemainClickTime;

                return bRemainClickTime < 0;
            };

            TimeUtil::WaitForEveryMS(fnIsTimeUp);
        }

        // click now
        CMouseController::GetInstance()->ClickInCanvas();
    }

    /// record and display
    if (bNeedJumpNow)
    {
        double dRemainCollisionTime = dCollisionTickcount - GetPreciseTickCount();
        m_pJumpRangeData->TryPushData(fPipeHeight);
        m_pClickDelay->OnClick(dRemainCollisionTime, dCollisionTime);
    }

    double dHeight = 0;
    auto heightdata = m_pHeightData->GetBirdHeightData();
    if (heightdata.size() > 0)
    {
        double a, b, c;
        pCollisionTimeForecaster->GetABC(a, b, c);
        dHeight = m_pHeightData->GetBirdHeightData().back();
        CRecorder::GetInstance()->RecordData(a, b, c, dHeight);

        pCollisionTimeForecaster->GenParabolaDots(
            PARABOLA_GRAPH_H, PARABOLA_GRAPH_W,
            a, b, c);

        float fRemainTime = bDisplayRemainTime ? (float)dRemainClickTime : 0;
        pOutputWindow->DrawParabola(
            pCollisionTimeForecaster->GetParabolaDots(),
            fRemainTime,
            dHeight,
            bNeedJumpNow);

        pOutputWindow->DrawJumpRange(m_pJumpRangeData);
        pOutputWindow->DrawClickDelay(m_pClickDelay);
    }


    if (bNeedJumpNow)
    {
        m_pHeightData->ResetData();
        pCollisionTimeForecaster->ResetData();
    }

    return true;
}


void CBirdHeightObserver::ResetData()
{
    m_pHeightData->ResetData();
    m_pClickDelay->ResetData();
}