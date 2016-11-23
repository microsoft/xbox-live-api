//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "SocialService_WinRT.h"
#include "Utils_WinRT.h"
#include "XboxLiveContextSettings_WinRT.h"

using namespace pplx;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace xbox::services::social;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN

SocialService::SocialService(
    _In_ social_service cppObj
    ) : 
    m_cppObj(cppObj)
{
    m_socialRelationshipSubscriptionEventBind = std::make_shared<SocialRelationshipSubscriptionEventBind>(Platform::WeakReference(this), m_cppObj);
    m_socialRelationshipSubscriptionEventBind->AddSocialRelationshipSubscriptionEvent();
}

SocialService::~SocialService()
{
    m_socialRelationshipSubscriptionEventBind->RemoveSocialChangeSubcriptionEvent(m_cppObj);
}

IAsyncOperation<XboxSocialRelationshipResult^>^
SocialService::GetSocialRelationshipsAsync()
{
    return GetSocialRelationshipsAsync(SocialRelationship::All, 0, 0);
}

IAsyncOperation<XboxSocialRelationshipResult^>^
SocialService::GetSocialRelationshipsAsync(
    _In_ SocialRelationship socialRelationshipFilter
    )
{
    return GetSocialRelationshipsAsync(socialRelationshipFilter, 0, 0);
}

Windows::Foundation::IAsyncOperation<XboxSocialRelationshipResult^>^
SocialService::GetSocialRelationshipsAsync(
    _In_ Platform::String^ xboxUserId
    )
{
    auto task = m_cppObj.get_social_relationships(STRING_T_FROM_PLATFORM_STRING(xboxUserId))
    .then([](xbox::services::xbox_live_result<xbox_social_relationship_result> cppSocialRelationshipResult)
    {
        THROW_IF_ERR(cppSocialRelationshipResult);
        return ref new XboxSocialRelationshipResult(cppSocialRelationshipResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<XboxSocialRelationshipResult^>^
SocialService::GetSocialRelationshipsAsync(
    _In_ SocialRelationship socialRelationshipFilter,
    _In_ uint32 startIndex,
    _In_ uint32 maxItems
    )
{
    xbox_social_relationship_filter cppSocialRelationshipFilter = static_cast<xbox_social_relationship_filter>(socialRelationshipFilter);
    auto task = m_cppObj.get_social_relationships(cppSocialRelationshipFilter, startIndex, maxItems)
    .then([](xbox::services::xbox_live_result<xbox_social_relationship_result> cppSocialRelationshipResult)
    {
        THROW_IF_ERR(cppSocialRelationshipResult);
        return ref new XboxSocialRelationshipResult(cppSocialRelationshipResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

SocialRelationshipChangeSubscription^
SocialService::SubscribeToSocialRelationshipChange(
    _In_ Platform::String^ xboxUserId
    )
{
    std::shared_ptr<social_relationship_change_subscription> subscription;
    auto subscriptionResult = m_cppObj.subscribe_to_social_relationship_change(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId)
        );

    THROW_IF_ERR(subscriptionResult);

    subscription = subscriptionResult.payload();

    return ref new SocialRelationshipChangeSubscription(subscription);
}

void
SocialService::UnsubscribeFromSocialRelationshipChange(
    _In_ SocialRelationshipChangeSubscription^ subscription
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(subscription);

    auto unsubscribeResult = m_cppObj.unsubscribe_from_social_relationship_change(
        subscription->GetCppObj()
        );

    THROW_IF_ERR(unsubscribeResult);
}

void SocialService::RaiseSocialRelationshipChange (
    _In_ SocialRelationshipChangeEventArgs^ args
    )
{
    SocialRelationshipChanged(this, args);
}

SocialRelationshipSubscriptionEventBind::SocialRelationshipSubscriptionEventBind(
    _In_ Platform::WeakReference setting,
    _In_ xbox::services::social::social_service& cppObj
    ) :
    m_setting(setting),
    m_cppObj(cppObj)
{
}

void
SocialRelationshipSubscriptionEventBind::SocialRelationshipChangeRouter(
    _In_ const xbox::services::social::social_relationship_change_event_args& args
    )
{
    SocialService^ socialService = m_setting.Resolve<SocialService>();
    if (socialService != nullptr)
    {
        if (m_cppObj._Xbox_live_context_settings()->use_core_dispatcher_for_event_routing() && Xbox::Services::XboxLiveContextSettings::Dispatcher != nullptr)
        {
            Xbox::Services::XboxLiveContextSettings::Dispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([socialService, args]()
            {
                socialService->RaiseSocialRelationshipChange(ref new SocialRelationshipChangeEventArgs(args));
            }));
        }
        else
        {
            socialService->RaiseSocialRelationshipChange(ref new SocialRelationshipChangeEventArgs(args));
        }
    }
}

void
SocialRelationshipSubscriptionEventBind::RemoveSocialChangeSubcriptionEvent(
    _In_ xbox::services::social::social_service& cppObj
    )
{
    cppObj.remove_social_relationship_changed_handler(m_functionContext);
}

void SocialRelationshipSubscriptionEventBind::AddSocialRelationshipSubscriptionEvent()
{
    std::weak_ptr<SocialRelationshipSubscriptionEventBind> thisWeakPtr = shared_from_this();
    m_functionContext = m_cppObj.add_social_relationship_changed_handler([thisWeakPtr](_In_ const xbox::services::social::social_relationship_change_event_args& args)
    {
        std::shared_ptr<SocialRelationshipSubscriptionEventBind> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->SocialRelationshipChangeRouter(args);
        }
    });
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END