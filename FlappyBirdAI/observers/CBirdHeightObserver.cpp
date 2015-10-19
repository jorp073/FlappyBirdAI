#include "stdafx.h"
#include "CBirdHeightObserver.h"
#include "CObjectObserver.h"
#include "../output/COutputWindow.h"
#include "../model/CHeightTimeModel.h"
#include "../util/CMouseClicker.h"
#include "../model/CCrashTimeForecaster.h"
#include "../recorder/CRecorder.h"
#include "../model/CJumpRangeModel.h"
#include "../model/CClickDelayModel.h"


INIT_SINGLEINSTANCE(CBirdHeightObserver);


CBirdHeightObserver::CBirdHeightObserver()
    : m_pHeightData(new CHeightTimeModel())
    , m_pJumpRangeData(new CJumpRangeModel())
    , m_pMouseClicker(new CMouseClicker())
    , m_pClickDelay(new CClickDelayModel())
{
    CCrashTimeForecaster::GetInstance()->SetModel(m_pHeightData);

    auto pushdata_callback = [=](float fBottomOffset)
    {
        m_pClickDelay->OnGetBottomData(fBottomOffset);
    };

    m_pJumpRangeData->SetPushDataCallback(pushdata_callback);
}


CBirdHeightObserver::~CBirdHeightObserver()
{
    delete m_pHeightData;
    delete m_pMouseClicker;
}


bool CBirdHeightObserver::Update(double dTickCount)
{
    auto fBirdHeight = CObjectObserver::GetInstance()->GetBirdHeight();
    auto fPipeHeight = CObjectObserver::GetInstance()->GetPipeHeight();
    DLOG(INFO) << "bird height: " << fBirdHeight << ", pipe height: " << fPipeHeight;

    auto pOutputWindow = COutputWindow::GetInstance();

    m_pHeightData->Append(fBirdHeight, fPipeHeight, dTickCount);
    pOutputWindow->SetPipeHeight(fPipeHeight);

    ///
    m_pJumpRangeData->OnBirdHeightChanged(fBirdHeight);

    ///
    auto pCrashTimeForecaster = CCrashTimeForecaster::GetInstance();
    pCrashTimeForecaster->Update(m_pClickDelay->GetClickDelay());
    bool bNeedJumpNow = pCrashTimeForecaster->IsNeedJumpNow();
    if (bNeedJumpNow)
    {
        m_pMouseClicker->TryClick();
        m_pJumpRangeData->TryPushData(fPipeHeight);
        m_pClickDelay->OnClick(pCrashTimeForecaster->GetRemainCrashTime());
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

    pOutputWindow->DrawParabola(
        pCrashTimeForecaster->GetParabolaDots(),
        pCrashTimeForecaster->GetRemainCrashTime(),
        dHeight,
        pCrashTimeForecaster->IsNeedJumpNow());

    pOutputWindow->DrawJumpRange(m_pJumpRangeData);
    pOutputWindow->DrawClickDelay(m_pClickDelay);

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
    m_pClickDelay->ResetData();
}