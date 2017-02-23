// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

tournament_reference::tournament_reference()
{
}

tournament_reference::tournament_reference(
    _In_ string_t definitionName,
    _In_ string_t tournamentId,
    _In_ string_t organizer,
    _In_ string_t serviceConfigurationId
    ) :
    m_definitionName(std::move(definitionName)),
    m_tournamentId(std::move(tournamentId)),
    m_organizer(std::move(organizer)),
    m_serviceConfigurationId(std::move(serviceConfigurationId))
{
}

const string_t&
tournament_reference::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

const string_t&
tournament_reference::definition_name() const
{
    return m_definitionName;
}

const string_t&
tournament_reference::tournament_id() const
{
    return m_tournamentId;
}

const string_t&
tournament_reference::organizer() const
{
    return m_organizer;
}

bool
tournament_reference::is_null() const
{
    return m_definitionName.empty() || m_tournamentId.empty() || m_organizer.empty() || m_serviceConfigurationId.empty();
}

xbox_live_result<tournament_reference>
tournament_reference::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null() || json.size() == 0) return xbox_live_result<tournament_reference>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto response = tournament_reference(
        utils::extract_json_string(json, _T("definitionName"), errc, true),
        utils::extract_json_string(json, _T("tournamentId"), errc, true),
        utils::extract_json_string(json, _T("organizer"), errc, true),
        utils::extract_json_string(json, _T("scid"), errc, true)
        );

    return xbox_live_result<tournament_reference>(response, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END