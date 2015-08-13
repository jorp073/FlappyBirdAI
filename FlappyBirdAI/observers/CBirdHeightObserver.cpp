#include "stdafx.h"
#include "CBirdHeightObserver.h"
#include "CObjectObserver.h"
#include "../output/COutputWindow.h"

INIT_SINGLEINSTANCE(CBirdHeightObserver);


CBirdHeightObserver::CBirdHeightObserver()
{
}


CBirdHeightObserver::~CBirdHeightObserver()
{

}


bool CBirdHeightObserver::Update(float dt)
{
    auto fBirdHeight = CObjectObserver::GetInstance()->GetBirdHeight();
    auto fPipeHeight = CObjectObserver::GetInstance()->GetPipeHeight();
    DLOG(INFO) << "bird height: " << fBirdHeight << ", pipe height: " << fPipeHeight;

    m_HeightData.Append(fBirdHeight, fPipeHeight, dt);

    COutputWindow::GetInstance()->SetPipeHeight(fPipeHeight);

    return true;
}
