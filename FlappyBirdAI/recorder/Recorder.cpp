#include "stdafx.h"
#include "Recorder.h"
#include "../observers/GameStateObserver.h"
#include "../fsm/GameState.h"
#include <strstream>

#define RECORD_FRAME_MAX_COUNT 240

INIT_SINGLEINSTANCE(CRecorder);


CRecorder::CRecorder()
    : m_bDisplaying(false)
{
    m_FrameData.dHeight = 1; // make sure dHeight>0, to be enable to record
}


void CRecorder::Init()
{
}


bool CRecorder::IsRecordable()
{
    return CGameStateObserver::GetInstance()->StateMachine()->IsInState("Play");
}


void CRecorder::RecordData(double a, double b, double c, double height)
{
    if (IsRecordable())
    {
        m_FrameData.a = a;
        m_FrameData.b = b;
        m_FrameData.c = c;
        m_FrameData.dHeight = height;
    }
}


void CRecorder::RecordCanvas(const cv::Mat& mat)
{
    if (IsRecordable()) m_FrameData.matCanvas = mat;
}


void CRecorder::RecordParabola(const cv::Mat& mat)
{
    if (IsRecordable()) m_FrameData.matParabola = mat;
}


void CRecorder::RecordClickDelay(const cv::Mat& mat)
{
    if (IsRecordable()) m_FrameData.matClickDelay = mat;
}


void CRecorder::PushRecord()
{
    if (IsRecordable())
    {
        m_lFrameData.push_back(m_FrameData);
        if (m_lFrameData.size() > RECORD_FRAME_MAX_COUNT) m_lFrameData.pop_front();

        //memset(&m_FrameData, 0, sizeof(m_FrameData));
    }
}


void CRecorder::ResetData()
{
    m_lFrameData.clear();
    m_FrameData.dHeight = 1;
    m_bDisplaying = false;
}


void CRecorder::OnDisplayPreviousFrame()
{
    if (!_EnterDisplayState()) return;

    if (m_lFrameData.begin() == m_iterFrame) m_iterFrame = m_lFrameData.end();
    m_iterFrame--;
    m_iCurPlayID--;
    if (m_iCurPlayID < 0) m_iCurPlayID = m_lFrameData.size()-1;

    _DisplayFrame(*m_iterFrame);
}


void CRecorder::OnDisplayNextFrame()
{
    if (!_EnterDisplayState()) return;

    m_iterFrame++;
    m_iCurPlayID++;
    if (m_iCurPlayID >= (int)m_lFrameData.size()) m_iCurPlayID = 0;

    if (m_lFrameData.end() == m_iterFrame) m_iterFrame = m_lFrameData.begin();

    _DisplayFrame(*m_iterFrame);
}


bool CRecorder::_EnterDisplayState()
{
    if (m_bDisplaying) return true;
    if (0 == m_lFrameData.size()) return false;

    m_bDisplaying = true;

    CGameStateObserver::GetInstance()->StateMachine()->ChangeState(
        new GameState::CPlayBack());

    m_iterFrame = m_lFrameData.begin();
    m_iCurPlayID = 0;
    return true;
}


void CRecorder::_DisplayFrame(const RECORD_FRAME& frame)
{
    // frame id
    auto canvas = frame.matCanvas.clone();
    std::strstream ss;
    ss << m_iCurPlayID+1 << "/" << m_lFrameData.size() << '\x00';

    cv::putText(canvas, ss.str(),
        cv::Point(canvas.cols - 60, 15), CV_FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar(255,255,255));

    cv::imshow(WINDOW_NAME_CANVAS, canvas);
    cv::imshow(WINDOW_NAME_PARABOLA, frame.matParabola);
    cv::imshow(WINDOW_NAME_CLICKDELAY, frame.matClickDelay);
    
    // dump data
    std::cout << "id:" << ss.str()
        << " a:" << frame.a << " b:" << frame.b << " c:" << frame.c
        << " height:" << frame.dHeight << std::endl;
}
