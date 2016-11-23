//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "xsapi/xbox_live_context_settings.h"
#include "XboxSocialRelationshipResult_WinRT.h"
#include "SocialRelationship_WinRT.h"
#include "SocialGroupConstants_WinRT.h"
#include "SocialRelationshipChangeSubscription_WinRT.h"
#include "SocialRelationshipChangeEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN

class SocialRelationshipSubscriptionEventBind : public std::enable_shared_from_this<SocialRelationshipSubscriptionEventBind>
{
public:
    SocialRelationshipSubscriptionEventBind(
        _In_ Platform::WeakReference setting,
        _In_ xbox::services::social::social_service& cppObj
        );

    void AddSocialRelationshipSubscriptionEvent();

    void SocialRelationshipChangeRouter(_In_ const xbox::services::social::social_relationship_change_event_args& args);

    void RemoveSocialChangeSubcriptionEvent(_In_ xbox::services::social::social_service& cppObj);

private:
    function_context m_functionContext;
    Platform::WeakReference m_setting;
    xbox::services::social::social_service m_cppObj;
};

/// <summary>
/// Defines access methods that return social relationship information between the user and one or more other Xbox Live users.
/// </summary>
public ref class SocialService sealed
{
public:
    /// <summary>
    /// Registers for social relationship change notifications.  Event handlers will receive SocialRelationshipChangeEventArgs^.
    /// </summary>
    event Windows::Foundation::EventHandler<SocialRelationshipChangeEventArgs^>^ SocialRelationshipChanged;

    /// <summary>
    /// Returns a XboxSocialRelationshipResult containing a the list of people that the user is connected to.
    /// Defaults to filtering to PersonView.All
    /// Defaults to startIndex and maxItems of 0 to return entire list if possible
    /// </summary>
    /// <returns>An XboxSocialRelationshipResult object.</returns>
    /// <remarks>Calls V1 GET /users/{ownerId}/people?view={view}&amp;startIndex={startIndex}&amp;maxItems={maxItems}</remarks>
    Windows::Foundation::IAsyncOperation<XboxSocialRelationshipResult^>^ GetSocialRelationshipsAsync();

    /// <summary>
    /// Returns a XboxSocialRelationshipResult containing a the list of people that the user is connected to.
    /// </summary>
    /// <param name="socialRelationshipFilter">Controls how the list is filtered</param>
    /// <returns>An XboxSocialRelationshipResult object.</returns>
    /// <remarks>Calls V1 GET /users/{ownerId}/people?view={view}&amp;startIndex={startIndex}&amp;maxItems={maxItems}</remarks>
    [Windows::Foundation::Metadata::DefaultOverload]
    Windows::Foundation::IAsyncOperation<XboxSocialRelationshipResult^>^ GetSocialRelationshipsAsync(
        _In_ SocialRelationship socialRelationshipFilter
        );

    Windows::Foundation::IAsyncOperation<XboxSocialRelationshipResult^>^ GetSocialRelationshipsAsync(
        _In_ Platform::String^ xboxUserId
        );

    /// <summary>
    /// Returns a XboxSocialRelationshipResult containing a the list of people that the user is connected to.
    /// </summary>
    /// <param name="socialRelationshipFilter">Controls how the list is filtered</param>
    /// <param name="startIndex">Controls the starting index to return</param>
    /// <param name="maxItems">Controls the number of XboxSocialRelationship objects to get.  0 will return as many as possible</param>
    /// <returns>An XboxSocialRelationshipResult object.</returns>
    /// <remarks>Calls V1 GET /users/{ownerId}/people?view={view}&amp;startIndex={startIndex}&amp;maxItems={maxItems}</remarks>
    Windows::Foundation::IAsyncOperation<XboxSocialRelationshipResult^>^ GetSocialRelationshipsAsync(
        _In_ SocialRelationship socialRelationshipFilter,
        _In_ uint32 startIndex,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Subscribes to social relationship change notifications via the SocialRelationshipChanged event
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the person of the subscription</param>
    /// <returns>SocialRelationshipChangeSubscription containing the initial value of the Xbox User Id
    /// Register for social relationship changes via the SocialRelationshipChanged event</returns>
    SocialRelationshipChangeSubscription^ SubscribeToSocialRelationshipChange(
        _In_ Platform::String^ xboxUserId
        );

    /// <summary>
    /// Unsubscribes a previously created social relationship change subscription
    /// </summary>
    /// <param name="subscription">The subscription object to unsubscribe</param>
    void UnsubscribeFromSocialRelationshipChange(
        _In_ SocialRelationshipChangeSubscription^ subscription
        );

internal:
    SocialService(
        _In_ xbox::services::social::social_service cppObj
        );

    void RaiseSocialRelationshipChange(_In_ SocialRelationshipChangeEventArgs^ args);
private:
    ~SocialService();

    xbox::services::social::social_service m_cppObj;
    std::shared_ptr<SocialRelationshipSubscriptionEventBind> m_socialRelationshipSubscriptionEventBind;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END
