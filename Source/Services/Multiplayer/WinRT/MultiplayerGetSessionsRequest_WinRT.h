// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/multiplayer.h"
#include "MultiplayerSessionVisibility_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN
/// <summary>
/// Defines the search filters for retrieveing a collection of discoverable sessions. Used in MultiplayerService::GetSessionsAsync().
/// </summary>

public ref class MultiplayerGetSessionsRequest sealed
{
public:
    /// <summary>
    /// Constructs a new MultiplayerGetSessionsRequest object.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration id associated with the sessions.</param>
    /// <param name="maxItems">The maximum number of sessions to return.</param>
    MultiplayerGetSessionsRequest(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// The service configuration id that the sessions are part of.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ServiceConfigurationId, service_configuration_id);

    /// <summary>
    /// The maximum number of sessions to return.
    /// </summary>
    DEFINE_PROP_GET_OBJ(MaxItems, max_items, uint32);

    /// <summary>
    /// Indicates if private sessions are included in the results.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(IncludePrivateSessions, include_private_sessions, bool);

    /// <summary>
    /// Indicates if sessions that the user hasn't accepted are included in the results.  You must specify xboxUserIdFilter if this value is set to true.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(IncludeReservations, include_reservations, bool);

    /// <summary>
    /// Indicates if inactive sessions are included in the results.  You must specify xboxUserIdFilter if this value is set to true.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(IncludeInactiveSessions, include_inactive_sessions, bool);

    /// <summary>
    /// Optional. Filter results to just sessions that contain this Xbox User ID.
    /// You must specify one of XboxUserIdFilter, XboxUserIdsFilter, or KeywordFilter. 
    /// </summary>
    DEFINE_PROP_GETSET_STR_OBJ(XboxUserIdFilter, xbox_user_id_filter);

    /// <summary>
    /// Optional. Filter results to just sessions that contains these Xbox User IDs.
    /// You must specify one of XboxUserIdFilter, XboxUserIdsFilter, or KeywordFilter. 
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ XboxUserIdsFilter { 
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get(); 
        void set(_In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds);
        }

    /// <summary>
    /// Optional. Filter results to just sessions with this keyword.
    /// You must specify one of XboxUserIdFilter, XboxUserIdsFilter, or KeywordFilter. 
    /// </summary>
    DEFINE_PROP_GETSET_STR_OBJ(KeywordFilter, keyword_filter);

    /// <summary>
    /// Optional. The name of the template for the multiplayer session to filter on.
    /// </summary>
    DEFINE_PROP_GETSET_STR_OBJ(SessionTemplateNameFilter, session_template_name_filter);

    /// <summary>
    /// Filter results to just sessions with the specified visibility.
    /// </summary>
    DEFINE_PROP_GETSET_ENUM_OBJ(VisibilityFilter, visibility_filter, MultiplayerSessionVisibility, xbox::services::multiplayer::multiplayer_session_visibility);

    /// <summary>
    /// Filter results to just sessions with this major version, or less, of the contract. Set to 0 to ignore the contract version.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(ContractVersionFilter, contract_version_filter, uint32);

internal:
    MultiplayerGetSessionsRequest(
        _In_ xbox::services::multiplayer::multiplayer_get_sessions_request cppObj
        );

    xbox::services::multiplayer::multiplayer_get_sessions_request GetCppObj();
private:
    xbox::services::multiplayer::multiplayer_get_sessions_request m_cppObj;
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ m_xboxUserIds;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END