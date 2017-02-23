// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "MeshPacketStructs.h"

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {

public ref class UserMeshConnectionPropertyBag sealed
{
internal:
    UserMeshConnectionPropertyBag(Platform::String^ xboxUserId);

public:
    bool IsUserAckedReceived();
    void SetUserAckedReceived(bool val);

private:
    Concurrency::critical_section m_stateLock;

    Platform::String^ m_xboxUserid;
    volatile bool m_isUserAckedReceived;
};

}}}}
