//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#pragma once

#include "Clock.h"

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {

public ref class ProcessThreadsEventArgs sealed
{
public:

internal:
    ProcessThreadsEventArgs(){}
};

public ref class MeshThread sealed
{
public:
    event Windows::Foundation::EventHandler<ProcessThreadsEventArgs^>^ OnDoWork;

    MeshThread( UINT sendPeriodInMilliseconds, uint32 threadAffinityMask, int priorityClass );
    UINT GetSendPeriod( );
    void SetSendPeriod( UINT sendPeriodInMilliseconds);
    void WakeupThread();

    virtual ~MeshThread();

internal:
    void Shutdown();
    static DWORD WINAPI StaticThreadProc(MeshThread^ networkSendThread);
    DWORD WINAPI ThreadProc();

private:

    uint32 m_threadAffinityMask;
    CRITICAL_SECTION m_threadManagementLock;
    HANDLE m_terminateThreadEvent;
    HANDLE m_threadHandle;
    int m_priorityClass;
    UINT m_sendEveryPeriodInMilliseconds;
    HANDLE m_wakeupEventHandle;
    Clock m_clock;

};

}}}}
