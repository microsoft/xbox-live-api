//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#include "pch.h"
#include "MeshPacketStatisticsForPacketType.h"

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {


MeshPacketStatisticsForPacketType::MeshPacketStatisticsForPacketType() :
    m_numberPacketsReceived(0),
    m_numberPacketsSent(0),
    m_largestPacketReceived(0),
    m_largestPacketSent(0),
    m_numberPacketsDropped(0)
{
}

int32 MeshPacketStatisticsForPacketType::NumberPacketsReceived::get() 
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_numberPacketsReceived;
}

int32 MeshPacketStatisticsForPacketType::NumberPacketsSent::get() 
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_numberPacketsSent;
}

int32 MeshPacketStatisticsForPacketType::LargestPacketReceived::get() 
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_largestPacketReceived;
}

int32 MeshPacketStatisticsForPacketType::LargestPacketSent::get() 
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_largestPacketSent;
}

int32 MeshPacketStatisticsForPacketType::NumberPacketsDropped::get() 
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_numberPacketsDropped;
}

int32 MeshPacketStatisticsForPacketType::NumberPacketsSkipped::get() 
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_numberPacketsSkipped;
}

int32 MeshPacketStatisticsForPacketType::NumberPacketsLost::get() 
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_numberPacketsSkipped - m_numberPacketsDropped;
}

void MeshPacketStatisticsForPacketType::SetNumberPacketsReceived( int32 val )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_numberPacketsReceived = val;
}

void MeshPacketStatisticsForPacketType::SetNumberPacketsSent( int32 val )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_numberPacketsSent = val;
}

void MeshPacketStatisticsForPacketType::SetLargestPacketReceived( int32 val )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_largestPacketReceived = val;
}

void MeshPacketStatisticsForPacketType::SetLargestPacketSent( int32 val )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_largestPacketSent = val;
}


void MeshPacketStatisticsForPacketType::SetNumberPacketsDropped( int32 val )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_numberPacketsDropped = val;
}

void MeshPacketStatisticsForPacketType::SetNumberPacketsSkipped( int32 val )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_numberPacketsSkipped = val;
}

void MeshPacketStatisticsForPacketType::ClearAllStatisticsForPacketType()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_numberPacketsReceived = 0;
    m_numberPacketsSent = 0;
    m_largestPacketReceived = 0;
    m_largestPacketSent = 0;
    m_numberPacketsDropped = 0;
}

}}}}
