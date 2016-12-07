//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#pragma once
#include "MeshPacketStructs.h"

using namespace Windows::Foundation;

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {

public ref class MeshHeartbeatStatisticsForConnection sealed
{
public:
    MeshHeartbeatStatisticsForConnection();

    property TimeSpan LastHeartbeatReceived { TimeSpan get(); }
    property TimeSpan LastHeartbeatSent { TimeSpan get(); }

internal:
    void SetLastHeartbeatReceived(TimeSpan val);
    void SetLastHeartbeatSent(TimeSpan val);

private:
    Concurrency::critical_section m_stateLock;

    TimeSpan m_lastHeartbeatReceived;
    TimeSpan m_lastHeartbeatSent;
};

}}}}
