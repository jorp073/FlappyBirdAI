#include "stdafx.h"
#include "CBirdHeightObserver.h"
#include "CObjectObserver.h"
#include "../output/COutputWindow.h"
#include "../model/CHeightTimeModel.h"
#include "../util/CMouseClicker.h"

INIT_SINGLEINSTANCE(CBirdHeightObserver);


CBirdHeightObserver::CBirdHeightObserver()
    : m_HeightData(new CHeightTimeModel())
    , m_MouseClicker(new CMouseClicker())
{
}


CBirdHeightObserver::~CBirdHeightObserver()
{
    delete m_HeightData;
    delete m_MouseClicker;
}


bool CBirdHeightObserver::Update(float dt)
{
    auto fBirdHeight = CObjectObserver::GetInstance()->GetBirdHeight();
    auto fPipeHeight = CObjectObserver::GetInstance()->GetPipeHeight();
    DLOG(INFO) << "bird height: " << fBirdHeight << ", pipe height: " << fPipeHeight;

    m_HeightData->Append(fBirdHeight, fPipeHeight, dt);

    COutputWindow::GetInstance()->SetPipeHeight(fPipeHeight);

    ///
    if (m_HeightData->IsNeedJumpNow())
    {
        m_MouseClicker->TryClick();
        m_HeightData->OnBirdJump();
    }

    return true;
}
