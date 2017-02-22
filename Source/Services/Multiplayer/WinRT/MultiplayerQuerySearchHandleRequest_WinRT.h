// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Sets the search handle based on the configuration of this request.
/// </summary>
public ref class MultiplayerQuerySearchHandleRequest sealed
{
public:
    /// <summary> Creates a MultiplayerGetSearchHandleRequest object.
    /// <param name="serviceConfigurationId">The scid within which to query for search handles.</param>
    /// <param name="sessionTemplateName">The name of the template to query for search handles.</param>
    /// </summary>
    MultiplayerQuerySearchHandleRequest(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ sessionTemplateName
        );

    /// <summary>
    /// The service configuration id that the sessions part of.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ServiceConfigurationId, service_configuration_id);

    /// <summary>
    /// The name of the template that the sessions part of.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SessionTemplateName, session_template_name);

    /// <summary>
    /// The attribute to sort the search handles by.
    /// Valid values are "Timestamp desc", "Timestamp asc" or any Numbers search attribute followed by 'asc or 'desc' (ex: 'Numbers/gamerank asc')
    /// </summary>
    DEFINE_PROP_GETSET_STR_OBJ(OrderBy, order_by);

    /// <summary>
    /// The order to sort the search handles by.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(OrderAscending, order_ascending, bool);
    
    /// <summary>
    /// The filter to search for.
    /// </summary>
    DEFINE_PROP_GETSET_STR_OBJ(SearchFilter, search_filter);

    /// <summary>
    /// The social group to get the search handles for.
    /// </summary>
    DEFINE_PROP_GETSET_STR_OBJ(SocialGroup, social_group);

internal:
    MultiplayerQuerySearchHandleRequest(
        _In_ xbox::services::multiplayer::multiplayer_query_search_handle_request cppObj
    );

    const xbox::services::multiplayer::multiplayer_query_search_handle_request& GetCppObj() const;

private:
    xbox::services::multiplayer::multiplayer_query_search_handle_request m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END