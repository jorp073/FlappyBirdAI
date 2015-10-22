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

    auto fRemainCollisionTime = pCollisionTimeForecaster->GetCollisionBottomRemainTime();
    bool bNeedJumpNow = fRemainCollisionTime < m_pClickDelay->GetClickDelay();

    DLOG(INFO) << "ai remain collision time: " << fRemainCollisionTime << " needjumpnow: " << bNeedJumpNow;

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

    /// make bird jump
    if (bNeedJumpNow)
    {
        CMouseController::GetInstance()->ClickInCanvas();
        m_pJumpRangeData->TryPushData(fPipeHeight);
        m_pClickDelay->OnClick(fRemainCollisionTime);
    }

    /// record and display
    double dHeight = 0;
    auto heightdata = m_pHeightData->GetBirdHeightData();
    if (heightdata.size() > 0)
    {
        double a, b, c;
        pCollisionTimeForecaster->GetABC(a, b, c);
        dHeight = m_pHeightData->GetBirdHeightData().back();
        CRecorder::GetInstance()->RecordData(a, b, c, dHeight);

        pCollisionTimeForecaster->GenParabolaDots(PARABOLA_GRAPH_H, PARABOLA_GRAPH_W, a, b, c);
        pOutputWindow->DrawParabola(
            pCollisionTimeForecaster->GetParabolaDots(),
            fRemainCollisionTime,
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