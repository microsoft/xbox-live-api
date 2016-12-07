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
