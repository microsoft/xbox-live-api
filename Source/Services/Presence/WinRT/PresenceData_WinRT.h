// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "xsapi/presence.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN
/// <summary>
/// Represents data supporting Rich Presence features.
/// </summary>

public ref class PresenceData sealed
{
public:
    /// <summary>
    /// Initializes a new instance of the PresenceData class.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) that identifies where the presence strings are defined by Id.</param>
    /// <param name="presenceId">Id of the presence string that should be used.</param>
    PresenceData(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ presenceId
        );

    /// <summary>
    /// Initializes a new instance of the PresenceData class.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) that identifies where the presence strings are defined by Id.</param>
    /// <param name="presenceId">Id of the presence string that should be used.</param>
    /// <param name="presenceTokenIds">Ids of the strings that should be used to replace the tokens in the presence string.</param>
    PresenceData(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ presenceId,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ presenceTokenIds
        );

    /// <summary>
    /// ID of the service configuration containing the presence strings.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ServiceConfigurationId, service_configuration_id);

    /// <summary>
    /// The ID of a presence string that is defined in the service configuration.
    /// For example, PresenceId = "1" could equal "Playing {0} on {1}" in the service configuration.
    /// The service configuration might map token 0 to Maps and token 1 to MapId
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(PresenceId, presence_id);

    /// <summary>
    /// The IDs of the strings to replace the format string tokens found in the presence string.  These strings are also defined in the service configuration.
    /// The ID values in the collection map to the strings associated with the token arguments found in the PresenceId.
    /// For example let's say this vector view contained the values "4" and "1" and PresenceId = "1" equals "Playing {0} on {1}" in the service configuration.
    /// The service configuration might map Token 0 = Maps, where MapId = "4" equals "Hometown".
    /// The service configuration might map Token 1 = Difficulty, where DifficultyId = "1" equals "Casual"
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ PresenceTokenIds { Windows::Foundation::Collections::IVectorView<Platform::String^>^ get(); }

internal:
    PresenceData(_In_ xbox::services::presence::presence_data cppObj);
    const xbox::services::presence::presence_data& GetCppObj() const;

private:
    xbox::services::presence::presence_data m_cppObj;
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ m_presenceTokenIds;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END