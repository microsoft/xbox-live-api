//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#pragma once
#include "MeshPacketStructs.h"

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {

public ref class MeshPacketStatisticsForPacketType sealed
{
public:
    MeshPacketStatisticsForPacketType();

    property int32 NumberPacketsReceived { int32 get(); }
    property int32 NumberPacketsSent { int32 get(); }
    property int32 LargestPacketReceived { int32 get(); }
    property int32 LargestPacketSent { int32 get(); }
    property int32 NumberPacketsDropped { int32 get(); }
    property int32 NumberPacketsSkipped { int32 get(); }
    property int32 NumberPacketsLost { int32 get(); }

internal:
    void SetNumberPacketsReceived(int32 val);
    void SetNumberPacketsSent(int32 val);
    void SetLargestPacketReceived(int32 val);
    void SetLargestPacketSent(int32 val);
    void SetNumberPacketsDropped(int32 val);
    void SetNumberPacketsSkipped(int32 val);

    void ClearAllStatisticsForPacketType();

private:
    Concurrency::critical_section m_stateLock;

    long m_numberPacketsReceived;
    long m_numberPacketsSent;
    long m_largestPacketReceived;
    long m_largestPacketSent;
    long m_numberPacketsDropped;
    long m_numberPacketsSkipped;
};

}}}}
