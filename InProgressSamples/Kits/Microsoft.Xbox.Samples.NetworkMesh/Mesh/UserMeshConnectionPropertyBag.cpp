// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UserMeshConnectionPropertyBag.h"

using namespace Microsoft::Xbox::Samples::NetworkMesh;
using namespace Windows::Foundation;

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {

UserMeshConnectionPropertyBag::UserMeshConnectionPropertyBag(Platform::String^ xboxUserId) :
    m_xboxUserid(xboxUserId),
    m_isUserAckedReceived(false)
{

}

bool UserMeshConnectionPropertyBag::IsUserAckedReceived()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_isUserAckedReceived;
}
void UserMeshConnectionPropertyBag::SetUserAckedReceived(bool val)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_isUserAckedReceived = val;
}

}}}}