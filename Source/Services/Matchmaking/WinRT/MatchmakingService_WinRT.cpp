// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "matchmakingService_winrt.h"
#include "Utils_WinRT.h"

using namespace Concurrency;
using namespace Platform::Collections;
using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services::matchmaking;
using namespace xbox::services::multiplayer;
using namespace Microsoft::Xbox::Services::Multiplayer;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_BEGIN

const wchar_t c_matchmakingContractVersionHeaderValue[] = L"103";

MatchmakingService::MatchmakingService(
    _In_ xbox::services::matchmaking::matchmaking_service cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

IAsyncOperation<CreateMatchTicketResponse^>^
MatchmakingService::CreateMatchTicketAsync(
    _In_ MultiplayerSessionReference^ ticketSessionReference,
    _In_ String^ matchmakingServiceConfigurationId,
    _In_ String^ hopperName,
    _In_ TimeSpan ticketTimeout,
    _In_ PreserveSessionMode preserveSession,
    _In_opt_ String^ ticketAttributesJson,
    _In_ Platform::Boolean isSymmetric
    )
{
    web::json::value ticketAttributes;

    if (!ticketAttributesJson->IsEmpty())
    {
        ticketAttributes = web::json::value::parse(STRING_T_FROM_PLATFORM_STRING(ticketAttributesJson));
    }

    auto task = m_cppObj.create_match_ticket(
        ticketSessionReference == nullptr ? multiplayer_session_reference() : ticketSessionReference->GetCppObj(),
        STRING_T_FROM_PLATFORM_STRING(matchmakingServiceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(hopperName),
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(ticketTimeout),
        static_cast<preserve_session_mode>(preserveSession),
        ticketAttributes,
        isSymmetric
        )
    .then([](xbox_live_result<create_match_ticket_response> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new CreateMatchTicketResponse(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncAction^
MatchmakingService::DeleteMatchTicketAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ hopperName,
    _In_ Platform::String^ ticketId
    )
{
    auto task = m_cppObj.delete_match_ticket(
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(hopperName),
        STRING_T_FROM_PLATFORM_STRING(ticketId)
    ).then([](xbox_live_result<void> cppResult)
    {
        THROW_IF_ERR(cppResult);
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<MatchTicketDetailsResponse^>^
MatchmakingService::GetMatchTicketDetailsAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ hopperName,
    _In_ Platform::String^ ticketId
    )
{
    auto task = m_cppObj.get_match_ticket_details(
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(hopperName),
        STRING_T_FROM_PLATFORM_STRING(ticketId)
        )
    .then([](xbox_live_result<match_ticket_details_response> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new MatchTicketDetailsResponse(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<HopperStatisticsResponse^>^
MatchmakingService::GetHopperStatisticsAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ hopperName
    )
{
    auto task = m_cppObj.get_hopper_statistics(
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(hopperName)
        )
    .then([](xbox_live_result<hopper_statistics_response> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new HopperStatisticsResponse(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_END
