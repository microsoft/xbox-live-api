// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
