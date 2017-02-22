// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
