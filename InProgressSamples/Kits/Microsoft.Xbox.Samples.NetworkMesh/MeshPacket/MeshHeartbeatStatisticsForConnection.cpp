//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#include "pch.h"
#include "MeshHeartbeatStatisticsForConnection.h"

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {


MeshHeartbeatStatisticsForConnection::MeshHeartbeatStatisticsForConnection()
{
    m_lastHeartbeatReceived.Duration = 0;
    m_lastHeartbeatSent.Duration = 0;
}

TimeSpan MeshHeartbeatStatisticsForConnection::LastHeartbeatReceived::get() 
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_lastHeartbeatReceived;
}

TimeSpan MeshHeartbeatStatisticsForConnection::LastHeartbeatSent::get() 
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_lastHeartbeatSent;
}

void MeshHeartbeatStatisticsForConnection::SetLastHeartbeatReceived( TimeSpan val )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_lastHeartbeatReceived = val;
}

void MeshHeartbeatStatisticsForConnection::SetLastHeartbeatSent( TimeSpan val )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_lastHeartbeatSent = val;
}

}}}}
