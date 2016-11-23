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
#include "shared_macros.h"
#include "xsapi/multiplayer.h"
#include "Macros_WinRT.h"
#include "user_context.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Represents a reference to a multiplayer session.
/// </summary>
/// <example>
/// The following example shows the format of the JSON object that represents
/// this class:
/// <code>
/// "sessionRef": {
///   "scid": "foo",
///   "templateName" : "bar",
///   "name" : "session-seven"
/// }
/// </code>
/// </example>
public ref class MultiplayerSessionReference sealed
{
public:
    /// <summary>
    /// Constructs the MultiplayerSession object with data about the session.
    /// </summary>
    /// <param name="serviceConfigurationId">A service configuration ID appropriate for the title.</param>
    /// <param name="sessionTemplateName">The name of the template for the session to be based on.</param>
    /// <param name="sessionName">A unique name for the session.</param>
    MultiplayerSessionReference(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ sessionTemplateName,
        _In_ Platform::String^ sessionName
        );

    /// <summary>
    /// The service configuration ID specific to the title.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ServiceConfigurationId, service_configuration_id);

    /// <summary>
    /// The name of the template for the session.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SessionTemplateName, session_template_name);

    /// <summary>
    /// The unique name of the session.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SessionName, session_name);

    /// <summary>
    /// Returns a URI path representation of the session reference.
    /// </summary>
    /// <returns>
    /// A string indicating the URI path.
    /// </returns>
    Platform::String^ ToUriPath();

    /// <summary>
    /// Returns the session reference parsed from the specified URI.
    /// </summary>
    /// <param name="uriPath">A string indicating the parsed URI path.</param>
    /// <returns>
    /// An object representing the multiplayer session reference.
    /// </returns>
    static MultiplayerSessionReference^ MultiplayerSessionReference::ParseFromUriPath(
        _In_ Platform::String^ uriPath
        );

internal:
    MultiplayerSessionReference(
        xbox::services::multiplayer::multiplayer_session_reference cppObj
        );

    const xbox::services::multiplayer::multiplayer_session_reference& GetCppObj() const;
private:
    xbox::services::multiplayer::multiplayer_session_reference m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END