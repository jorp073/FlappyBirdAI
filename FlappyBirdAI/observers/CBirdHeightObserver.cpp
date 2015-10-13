#include "stdafx.h"
#include "CBirdHeightObserver.h"
#include "CObjectObserver.h"
#include "../output/COutputWindow.h"
#include "../model/CHeightTimeModel.h"
#include "../util/CMouseClicker.h"
#include "../model/CCrashTimeForecaster.h"
#include "../recorder/CRecorder.h"


INIT_SINGLEINSTANCE(CBirdHeightObserver);


CBirdHeightObserver::CBirdHeightObserver()
    : m_pHeightData(new CHeightTimeModel())
    , m_pMouseClicker(new CMouseClicker())
    , m_pCrashTimeForecaster(new CCrashTimeForecaster(m_pHeightData))
{
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
    m_pCrashTimeForecaster->Update();
    if (m_pCrashTimeForecaster->IsNeedJumpNow())
    {
        m_pMouseClicker->TryClick();
        m_pHeightData->ResetData();
        m_pCrashTimeForecaster->ResetData();
    }

    COutputWindow::GetInstance()->DrawParabola(
        m_pCrashTimeForecaster->GetParabolaDots(),
        m_pCrashTimeForecaster->GetOutputWindowWidth(),
        m_pCrashTimeForecaster->GetRemainCrashTime(),
        m_pCrashTimeForecaster->IsNeedJumpNow());

    // record data
    auto heightdata = m_pHeightData->GetBirdHeightData();
    if (heightdata.size() > 0)
    {
        double a, b, c;
        m_pCrashTimeForecaster->GetABC(a, b, c);
        auto dHeight = m_pHeightData->GetBirdHeightData().back();
        CRecorder::GetInstance()->RecordData(a, b, c, dHeight);
    }

    return true;
}


void CBirdHeightObserver::ResetData()
{
    m_pHeightData->ResetData();
}