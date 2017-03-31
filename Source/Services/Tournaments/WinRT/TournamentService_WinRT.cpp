// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "TournamentService_WinRT.h"
#include "Utils_WinRT.h"
#include "TeamChangeEventArgs_WinRT.h"
#include "TournamentChangeEventArgs_WinRT.h"
#include "XboxLiveContextSettings_WinRT.h"

using namespace Platform;
using namespace concurrency;
using namespace xbox::services;
using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

TeamEventBind::TeamEventBind(
    _In_ Platform::WeakReference setting,
    _In_ xbox::services::tournaments::tournament_service& cppObj
    ) :
    m_setting(setting),
    m_cppObj(cppObj)
{
}

void
TeamEventBind::RemoveTeamEvent(
    _In_ xbox::services::tournaments::tournament_service& cppObj
    )
{
    cppObj.remove_team_changed_handler(m_functionContext);
}

void TeamEventBind::TeamChangeRouter(
    _In_ const xbox::services::tournaments::team_change_event_args& teamChangeEventArgs
    )
{
    TournamentService^ setting = m_setting.Resolve<TournamentService>();
    if (setting != nullptr)
    {
        if (m_cppObj._Xbox_live_context_settings()->use_core_dispatcher_for_event_routing() && Xbox::Services::XboxLiveContextSettings::Dispatcher != nullptr)
        {
            Xbox::Services::XboxLiveContextSettings::Dispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([setting, teamChangeEventArgs]()
            {
                setting->RaiseTeamChange(ref new TeamChangeEventArgs(teamChangeEventArgs));
            }));
        }
        else
        {
            setting->RaiseTeamChange(ref new TeamChangeEventArgs(teamChangeEventArgs));
        }
    }
}

void TeamEventBind::AddTeamEvent()
{
    std::weak_ptr<TeamEventBind> thisWeakPtr = shared_from_this();
    m_functionContext = m_cppObj.add_team_changed_handler([thisWeakPtr](_In_ const xbox::services::tournaments::team_change_event_args& teamChangeEventArgs)
    {
        std::shared_ptr<TeamEventBind> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->TeamChangeRouter(teamChangeEventArgs);
        }
    });
}

void TournamentService::RaiseTeamChange(_In_ TeamChangeEventArgs^ args)
{
    TeamChanged(this, args);
}

TournamentService::TournamentService(
    _In_ xbox::services::tournaments::tournament_service cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

const xbox::services::tournaments::tournament_service&
TournamentService::GetCppObj() const
{
    return m_cppObj;
}

Windows::Foundation::IAsyncOperation<TournamentRequestResult^>^  
TournamentService::GetTournamentsAsync(
    _In_ TournamentRequest^ request
    )
{
    auto task = m_cppObj.get_tournaments(request->GetCppObj())
    .then([](xbox_live_result<tournament_request_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new TournamentRequestResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<Tournament^>^ 
TournamentService::GetTournamentDetailsAsync(
    _In_ Platform::String^ organizerId,
    _In_ Platform::String^ tournamentId
    )
{
    auto task = m_cppObj.get_tournament_details(
        STRING_T_FROM_PLATFORM_STRING(organizerId),
        STRING_T_FROM_PLATFORM_STRING(tournamentId)
        )
    .then([](xbox_live_result<tournament> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new Tournament(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<TeamRequestResult^>^
TournamentService::GetTeamsAsync(
    _In_ TeamRequest^ request
    )
{
    auto task = m_cppObj.get_teams(request->GetCppObj())
    .then([](xbox_live_result<team_request_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new TeamRequestResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<TeamInfo^>^
TournamentService::GetTeamDetailsAsync(
    _In_ Platform::String^ organizerId,
    _In_ Platform::String^ tournamentId,
    _In_ Platform::String^ teamId
    )
{
    auto task = m_cppObj.get_team_details(
        STRING_T_FROM_PLATFORM_STRING(organizerId),
        STRING_T_FROM_PLATFORM_STRING(tournamentId),
        STRING_T_FROM_PLATFORM_STRING(teamId)
    )
    .then([](xbox_live_result<team_info> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new TeamInfo(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

TeamChangeSubscription^
TournamentService::SubscribeToTeamChange( 
    _In_ Platform::String^ organizerId,
    _In_ Platform::String^ tournamentId,
    _In_ Platform::String^ teamId
    )
{
    std::shared_ptr<team_change_subscription> subscription;
    auto subscriptionResult = m_cppObj.subscribe_to_team_change(
        STRING_T_FROM_PLATFORM_STRING(organizerId),
        STRING_T_FROM_PLATFORM_STRING(tournamentId),
        STRING_T_FROM_PLATFORM_STRING(teamId)
        );

    THROW_IF_ERR(subscriptionResult);

    subscription = subscriptionResult.payload();

    return ref new TeamChangeSubscription(subscription);
}

void
TournamentService::UnsubscribeFromTeamChange(
    _In_ TeamChangeSubscription^ subscription
)
{
    THROW_INVALIDARGUMENT_IF_NULL(subscription);

    auto unsubscribeResult = m_cppObj.unsubscribe_from_team_change(
        subscription->GetCppObj()
        );

    THROW_IF_ERR(unsubscribeResult);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END