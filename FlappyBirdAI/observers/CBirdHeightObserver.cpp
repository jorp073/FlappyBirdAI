#include "stdafx.h"
#include "CBirdHeightObserver.h"
#include "CObjectObserver.h"
#include "../output/COutputWindow.h"
#include "../model/CHeightTimeModel.h"
#include "../util/CMouseClicker.h"
#include "../model/CCrashTimeForecaster.h"
#include "../recorder/CRecorder.h"
#include "../model/CJumpRangeModel.h"


INIT_SINGLEINSTANCE(CBirdHeightObserver);


CBirdHeightObserver::CBirdHeightObserver()
    : m_pHeightData(new CHeightTimeModel())
    , m_pJumpRangeData(new CJumpRangeModel())
    , m_pMouseClicker(new CMouseClicker())
{
    CCrashTimeForecaster::GetInstance()->SetModel(m_pHeightData);
}


CBirdHeightObserver::~CBirdHeightObserver()
{
    delete m_pHeightData;
    delete m_pMouseClicker;
}


bool CBirdHeightObserver::Update(float dt)
{
    auto fBirdHeight = CObjectObserver::GetInstance()->GetBirdHeight();
    auto fPipeHeight = CObjectObserver::GetInstance()->GetPipeHeight();
    DLOG(INFO) << "bird height: " << fBirdHeight << ", pipe height: " << fPipeHeight;

    m_pHeightData->Append(fBirdHeight, fPipeHeight, dt);
    COutputWindow::GetInstance()->SetPipeHeight(fPipeHeight);

    ///
    m_pJumpRangeData->OnBirdHeightChanged(fBirdHeight);

    ///
    auto pCrashTimeForecaster = CCrashTimeForecaster::GetInstance();
    pCrashTimeForecaster->Update();
    bool bNeedJumpNow = pCrashTimeForecaster->IsNeedJumpNow();
    if (bNeedJumpNow)
    {
        m_pMouseClicker->TryClick();
        m_pJumpRangeData->OnClick(fPipeHeight);
    }

    double dHeight = 0;

    // record data
    auto heightdata = m_pHeightData->GetBirdHeightData();
    if (heightdata.size() > 0)
    {
        double a, b, c;
        CCrashTimeForecaster::GetInstance()->GetABC(a, b, c);
        dHeight = m_pHeightData->GetBirdHeightData().back();
        CRecorder::GetInstance()->RecordData(a, b, c, dHeight);
    }

    COutputWindow::GetInstance()->DrawParabola(
        pCrashTimeForecaster->GetParabolaDots(),
        pCrashTimeForecaster->GetOutputWindowWidth(),
        pCrashTimeForecaster->GetRemainCrashTime(),
        dHeight,
        pCrashTimeForecaster->IsNeedJumpNow());

    if (bNeedJumpNow)
    {
        m_pHeightData->ResetData();
        pCrashTimeForecaster->ResetData();
    }

    return true;
}


void CBirdHeightObserver::ResetData()
{
    m_pHeightData->ResetData();
}