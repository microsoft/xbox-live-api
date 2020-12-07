// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "public_utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_BEGIN
match_ticket_details_response::match_ticket_details_response() :
    m_buffer(),
    m_matchTicketDetailsResponse()
{
}

match_ticket_details_response::match_ticket_details_response(std::shared_ptr<char> buffer) :
    m_buffer(buffer),
    m_matchTicketDetailsResponse(*reinterpret_cast<XblMatchTicketDetailsResponse*>(m_buffer.get()))
{
}

ticket_status
match_ticket_details_response::match_status() const
{
    return static_cast<ticket_status>(m_matchTicketDetailsResponse.matchStatus);
}

std::chrono::seconds
match_ticket_details_response::estimated_wait_time() const
{
    return std::chrono::seconds(m_matchTicketDetailsResponse.estimatedWaitTime);
}

preserve_session_mode
match_ticket_details_response::preserve_session() const
{
    return static_cast<preserve_session_mode>(m_matchTicketDetailsResponse.preserveSession);
}

xbox::services::multiplayer::multiplayer_session_reference
match_ticket_details_response::ticket_session() const
{
    return xbox::services::multiplayer::multiplayer_session_reference(
        Utils::StringTFromUtf8(m_matchTicketDetailsResponse.ticketSession.Scid),
        Utils::StringTFromUtf8(m_matchTicketDetailsResponse.ticketSession.SessionTemplateName),
        Utils::StringTFromUtf8(m_matchTicketDetailsResponse.ticketSession.SessionName));
}

xbox::services::multiplayer::multiplayer_session_reference
match_ticket_details_response::target_session() const
{
    return xbox::services::multiplayer::multiplayer_session_reference (
        Utils::StringTFromUtf8(m_matchTicketDetailsResponse.targetSession.Scid),
        Utils::StringTFromUtf8(m_matchTicketDetailsResponse.targetSession.SessionTemplateName),
        Utils::StringTFromUtf8(m_matchTicketDetailsResponse.targetSession.SessionName));
}

web::json::value
match_ticket_details_response::ticket_attributes() const
{
    return web::json::value(Utils::StringTFromUtf8(m_matchTicketDetailsResponse.ticketAttributes));
}

ticket_status match_ticket_details_response::convert_string_to_ticket_status(
    _In_ const string_t& value
)
{
    ticket_status ticketStatus = ticket_status::unknown;
    if (!value.empty())
    {
        if (Utils::Stricmp(value, _T("expired")) == 0)
        {
            ticketStatus = ticket_status::expired;
        }
        else if (Utils::Stricmp(value, _T("searching")) == 0)
        {
            ticketStatus = ticket_status::searching;
        }
        else if (Utils::Stricmp(value, _T("found")) == 0)
        {
            ticketStatus = ticket_status::found;
        }
        else if (Utils::Stricmp(value, _T("canceled")) == 0)
        {
            ticketStatus = ticket_status::canceled;
        }
    }

    return ticketStatus;
}

preserve_session_mode match_ticket_details_response::convert_string_to_preserve_session_mode(
    _In_ const string_t& value
)
{
    preserve_session_mode preserve_session_mode = preserve_session_mode::unknown;
    if (!value.empty())
    {
        if (Utils::Stricmp(value, _T("always")) == 0)
        {
            preserve_session_mode = preserve_session_mode::always;
        }
        else if (Utils::Stricmp(value, _T("never")) == 0)
        {
            preserve_session_mode = preserve_session_mode::never;
        }
    }
    return preserve_session_mode;
}


hopper_statistics_response::hopper_statistics_response() :
    m_buffer(),
    m_hopperStatisticsResponse()
{
}

hopper_statistics_response::hopper_statistics_response(std::shared_ptr<char> buffer) :
    m_buffer(buffer),
    m_hopperStatisticsResponse(*reinterpret_cast<XblHopperStatisticsResponse*>(m_buffer.get()))
{
}

/// <summary>
/// Name of the hopper in which a match was requested.
/// </summary>
string_t
hopper_statistics_response::hopper_name() const
{
    return Utils::StringTFromUtf8(m_hopperStatisticsResponse.hopperName);
}

/// <summary>
/// Estimated wait time for a match request to be matched with other players.
/// </summary>
std::chrono::seconds hopper_statistics_response::estimated_wait_time() const
{
    return std::chrono::seconds(m_hopperStatisticsResponse.estimatedWaitTime);
}

/// <summary>
/// The number of players in the hopper waiting to be matched.
/// </summary>
uint32_t hopper_statistics_response::players_waiting_to_match() const
{
    return m_hopperStatisticsResponse.playersWaitingToMatch;
}

create_match_ticket_response::create_match_ticket_response():
    m_createMatchTicketResponse{  }
{
}

create_match_ticket_response::create_match_ticket_response(
    XblCreateMatchTicketResponse response
) :
    m_createMatchTicketResponse(response)
{
}

string_t
create_match_ticket_response::match_ticket_id() const
{
    return Utils::StringTFromUtf8(m_createMatchTicketResponse.matchTicketId);
}

std::chrono::seconds
create_match_ticket_response::estimated_wait_time() const
{
    return std::chrono::seconds(m_createMatchTicketResponse.estimatedWaitTime);
}

matchmaking_service::matchmaking_service(_In_ XblContextHandle contextHandle)
{
    XblContextDuplicateHandle(contextHandle, &m_xblContext);
}

matchmaking_service::matchmaking_service(const matchmaking_service& other) 
{
    XblContextDuplicateHandle(other.m_xblContext, &m_xblContext);
}

matchmaking_service& matchmaking_service::operator=(matchmaking_service other) 
{
    std::swap(m_xblContext, other.m_xblContext);
    return *this;
}

matchmaking_service::~matchmaking_service() 
{
    XblContextCloseHandle(m_xblContext);
}

pplx::task<xbox_live_result<create_match_ticket_response>> matchmaking_service::create_match_ticket(
    _In_ const xbox::services::multiplayer::multiplayer_session_reference& ticketSessionReference,
    _In_ const string_t& matchmakingServiceConfigurationId,
    _In_ const string_t& hopperName,
    _In_ const std::chrono::seconds& ticketTimeout,
    _In_ preserve_session_mode preserveSession,
    _In_ const web::json::value& ticketAttributesJson
) 
{
    auto xblContext = m_xblContext;

    auto asyncWrapper = new AsyncWrapper<create_match_ticket_response>(
        [](XAsyncBlock* async, create_match_ticket_response& result) 
    {
        XblCreateMatchTicketResponse resultResponse;
        auto hr = XblMatchmakingCreateMatchTicketResult(async, &resultResponse);
        if (SUCCEEDED(hr)) 
        {
            result = create_match_ticket_response(resultResponse);
        }
        return hr;
    });

    XblMultiplayerSessionReference _ticketSessionReference;
    Utils::Utf8FromCharT(ticketSessionReference.service_configuration_id().data(), _ticketSessionReference.Scid, sizeof(_ticketSessionReference.Scid));
    Utils::Utf8FromCharT(ticketSessionReference.session_template_name().data(), _ticketSessionReference.SessionTemplateName, sizeof(_ticketSessionReference.SessionTemplateName));
    Utils::Utf8FromCharT(ticketSessionReference.session_name().data(), _ticketSessionReference.SessionName, sizeof(_ticketSessionReference.SessionName));

    auto hr = XblMatchmakingCreateMatchTicketAsync(
        xblContext,
        _ticketSessionReference,
        Utils::StringFromStringT(matchmakingServiceConfigurationId).c_str(),
        Utils::StringFromStringT(hopperName).c_str(),
        ticketTimeout.count(),
        static_cast<XblPreserveSessionMode>(preserveSession),
        Utils::StringFromStringT(ticketAttributesJson.serialize()).c_str(),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<void>> matchmaking_service::delete_match_ticket(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& hopperName,
    _In_ const string_t& ticketId
) 
{
    auto xblContext = m_xblContext;

    auto asyncWrapper = new AsyncWrapper<void>();
    auto hr = XblMatchmakingDeleteMatchTicketAsync(
        xblContext,
        Utils::StringFromStringT(serviceConfigurationId).c_str(),
        Utils::StringFromStringT(hopperName).c_str(),
        Utils::StringFromStringT(ticketId).c_str(),
        &asyncWrapper->async);

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<match_ticket_details_response>> matchmaking_service::get_match_ticket_details(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& hopperName,
    _In_ const string_t& ticketId
) 
{
    auto xblContext = m_xblContext;

    auto asyncWrapper = new AsyncWrapper<match_ticket_details_response>(
        [](XAsyncBlock* async, match_ticket_details_response& result)
    {
        size_t bufferSize;
        auto hr = XblMatchmakingGetMatchTicketDetailsResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
            XblMatchTicketDetailsResponse* resultPtr;
            hr = XblMatchmakingGetMatchTicketDetailsResult(async, bufferSize, buffer.get(), &resultPtr, nullptr);

            if (SUCCEEDED(hr)) 
            {
                result = match_ticket_details_response(buffer);
            }
        }
        return hr;
    });

    auto hr = XblMatchmakingGetMatchTicketDetailsAsync(
        xblContext,
        Utils::StringFromStringT(serviceConfigurationId).c_str(),
        Utils::StringFromStringT(hopperName).c_str(),
        Utils::StringFromStringT(ticketId).c_str(),
        &asyncWrapper->async);

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<hopper_statistics_response>>  matchmaking_service::get_hopper_statistics(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& hopperName
) 
{
    auto xblContext = m_xblContext;

    auto asyncWrapper = new AsyncWrapper<hopper_statistics_response>(
        [](XAsyncBlock* async, hopper_statistics_response& result)
    {
        size_t bufferSize;
        auto hr = XblMatchmakingGetHopperStatisticsResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
            XblHopperStatisticsResponse* resultPtr;
            hr = XblMatchmakingGetHopperStatisticsResult(async, bufferSize, buffer.get(), &resultPtr, nullptr);

            if (SUCCEEDED(hr)) 
            {
                result = hopper_statistics_response(buffer);
            }
        }
        return hr;
    });

    auto hr = XblMatchmakingGetHopperStatisticsAsync(
        xblContext,
        Utils::StringFromStringT(serviceConfigurationId).c_str(),
        Utils::StringFromStringT(hopperName).c_str(),
        &asyncWrapper->async);

    return asyncWrapper->Task(hr);
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_END