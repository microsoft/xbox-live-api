// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "SocialRelationshipChangeSubscription_WinRT.h"

using namespace Microsoft::Xbox::Services::RealTimeActivity;
NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN

SocialRelationshipChangeSubscription::SocialRelationshipChangeSubscription(
    _In_ std::shared_ptr<xbox::services::social::social_relationship_change_subscription> cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

RealTimeActivitySubscriptionState
SocialRelationshipChangeSubscription::State::get()
{
    return static_cast<RealTimeActivitySubscriptionState>(m_cppObj->state());
}

std::shared_ptr<xbox::services::social::social_relationship_change_subscription>
SocialRelationshipChangeSubscription::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END