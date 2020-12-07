// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

uint32_t multiplayer_member::member_id() const
{
    return m_internalMember.MemberId;
}

string_t multiplayer_member::initial_team() const
{
    return Utils::StringTFromUtf8(m_internalMember.InitialTeam);
}

string_t multiplayer_member::xbox_user_id() const
{
    return Utils::StringTFromUint64(m_internalMember.Xuid);
}

string_t multiplayer_member::debug_gamertag() const
{
    return Utils::StringTFromUtf8(m_internalMember.DebugGamertag);
}

bool multiplayer_member::is_local() const
{
    return m_internalMember.IsLocal;
}

bool multiplayer_member::is_in_lobby() const
{
    return m_internalMember.IsInLobby;
}

bool multiplayer_member::is_in_game() const
{
    return m_internalMember.IsInGame;
}

multiplayer_session_member_status multiplayer_member::status() const
{
    return static_cast<multiplayer_session_member_status>(m_internalMember.Status);
}

string_t multiplayer_member::connection_address() const
{
    return Utils::StringTFromUtf8(m_internalMember.ConnectionAddress);
}

web::json::value multiplayer_member::properties() const
{
    return Utils::ParseJson(m_internalMember.PropertiesJson);
}

bool multiplayer_member::is_member_on_same_device(
    _In_ std::shared_ptr<multiplayer_member> member
) const
{
    return XblMultiplayerManagerMemberAreMembersOnSameDevice(&m_internalMember, &member->m_internalMember);
}

string_t multiplayer_lobby_session::correlation_id() const
{
    XblGuid correlationId{};
    XblMultiplayerManagerLobbySessionCorrelationId(&correlationId);
    return Utils::StringTFromUtf8(correlationId.value);
}

multiplayer_session_reference multiplayer_lobby_session::session_reference() const
{
    XblMultiplayerSessionReference sessionReference{};
    XblMultiplayerManagerLobbySessionSessionReference(&sessionReference);
    return multiplayer_session_reference(sessionReference);
}

std::vector<std::shared_ptr<multiplayer_member>> multiplayer_lobby_session::local_members() const
{
    size_t localMemberCount = XblMultiplayerManagerLobbySessionLocalMembersCount();
    std::vector<XblMultiplayerManagerMember> localMembers(localMemberCount);
    XblMultiplayerManagerLobbySessionLocalMembers(localMemberCount, localMembers.data());

    return Utils::Transform<std::shared_ptr<multiplayer_member>>(localMembers, [](const XblMultiplayerManagerMember& member)
    {
        return std::make_shared<multiplayer_member>(member);
    });
}

std::vector<std::shared_ptr<multiplayer_member>> multiplayer_lobby_session::members() const
{
    size_t memberCount = XblMultiplayerManagerLobbySessionMembersCount();
    std::vector<XblMultiplayerManagerMember> members(memberCount);
    XblMultiplayerManagerLobbySessionMembers(memberCount, members.data());

    return Utils::Transform<std::shared_ptr<multiplayer_member>>(members, [](const XblMultiplayerManagerMember& member)
    {
        return std::make_shared<multiplayer_member>(member);
    });
}

std::shared_ptr<multiplayer_member> multiplayer_lobby_session::host() const
{
    XblMultiplayerManagerMember host;
    XblMultiplayerManagerLobbySessionHost(&host);
    return std::make_shared<multiplayer_member>(host);
}

web::json::value multiplayer_lobby_session::properties() const
{
    return Utils::ParseJson(XblMultiplayerManagerLobbySessionPropertiesJson());
}

const std::shared_ptr<multiplayer_session_constants> multiplayer_lobby_session::session_constants() const
{
    if (m_sessionConstants == nullptr)
    {
        m_sessionConstants = std::make_shared<multiplayer_session_constants>(true);
    }
    return m_sessionConstants;
}

xbox_live_result<void> multiplayer_lobby_session::add_local_user(
    _In_ xbox_live_user_t user
)
{
    return Utils::ConvertHr(XblMultiplayerManagerLobbySessionAddLocalUser(user));
}

xbox_live_result<void> multiplayer_lobby_session::remove_local_user(
    _In_ xbox_live_user_t user
)
{
    return Utils::ConvertHr(XblMultiplayerManagerLobbySessionRemoveLocalUser(user));
}

xbox_live_result<void> multiplayer_lobby_session::set_local_member_properties(
    _In_ xbox_live_user_t user,
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson,
    _In_opt_ context_t context
)
{
    return Utils::ConvertHr(XblMultiplayerManagerLobbySessionSetLocalMemberProperties(
        user,
        Utils::StringFromStringT(name).data(),
        Utils::StringFromStringT(valueJson.serialize()).data(),
        reinterpret_cast<void*>(context)
    ));
}

xbox_live_result<void> multiplayer_lobby_session::delete_local_member_properties(
    _In_ xbox_live_user_t user,
    _In_ const string_t& name,
    _In_opt_ context_t context
)
{
    return Utils::ConvertHr(XblMultiplayerManagerLobbySessionDeleteLocalMemberProperties(
        user,
        Utils::StringFromStringT(name).data(),
        reinterpret_cast<void*>(context)
    ));
}

xbox_live_result<void> multiplayer_lobby_session::set_local_member_connection_address(
    _In_ xbox_live_user_t user,
    _In_ const string_t& connectionAddress,
    _In_opt_ context_t context
)
{
    return Utils::ConvertHr(XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(
        user,
        Utils::StringFromStringT(connectionAddress).data(),
        reinterpret_cast<void*>(context)
    ));
}

bool multiplayer_lobby_session::is_host(
    _In_ const string_t& xboxUserId
)
{
    return XblMultiplayerManagerLobbySessionIsHost(Utils::Uint64FromStringT(xboxUserId));
}

xbox_live_result<void> multiplayer_lobby_session::set_properties(
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson,
    _In_opt_ context_t context
)
{
    return Utils::ConvertHr(XblMultiplayerManagerLobbySessionSetProperties(
        Utils::StringFromStringT(name).data(),
        Utils::StringFromStringT(valueJson.serialize()).data(),
        reinterpret_cast<void*>(context)
    ));
}

xbox_live_result<void> multiplayer_lobby_session::set_synchronized_properties(
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson,
    _In_opt_ context_t context
)
{
    return Utils::ConvertHr(XblMultiplayerManagerLobbySessionSetSynchronizedProperties(
        Utils::StringFromStringT(name).data(),
        Utils::StringFromStringT(valueJson.serialize()).data(),
        reinterpret_cast<void*>(context)
    ));
}

xbox_live_result<void> multiplayer_lobby_session::set_synchronized_host(
    _In_ std::shared_ptr<multiplayer_member> gameHost,
    _In_opt_ context_t context
)
{
    return Utils::ConvertHr(XblMultiplayerManagerLobbySessionSetSynchronizedHost(
        gameHost->m_internalMember.DeviceToken,
        reinterpret_cast<void*>(context)
    ));
}

#if HC_PLATFORM_IS_MICROSOFT
xbox_live_result<void> multiplayer_lobby_session::invite_friends(
    _In_ xbox_live_user_t user,
    _In_ const string_t& contextStringId,
    _In_ const string_t& customActivationContext
)
{
    return Utils::ConvertHr(XblMultiplayerManagerLobbySessionInviteFriends(
        user,
        Utils::StringFromStringT(contextStringId).data(),
        Utils::StringFromStringT(customActivationContext).data()
    ));
}
#endif

xbox_live_result<void> multiplayer_lobby_session::invite_users(
    _In_ xbox_live_user_t user,
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const string_t& contextStringId,
    _In_ const string_t& customActivationContext
)
{
    auto xuids = Utils::XuidVectorFromXuidStringVector(xboxUserIds);

    return Utils::ConvertHr(XblMultiplayerManagerLobbySessionInviteUsers(
        user,
        xuids.data(),
        xuids.size(),
        Utils::StringFromStringT(contextStringId).data(),
        Utils::StringFromStringT(customActivationContext).data()
    ));
}

string_t multiplayer_game_session::correlation_id() const
{
    return Utils::StringTFromUtf8(XblMultiplayerManagerGameSessionCorrelationId());
}

multiplayer_session_reference multiplayer_game_session::session_reference() const
{
    return multiplayer_session_reference(*XblMultiplayerManagerGameSessionSessionReference());
}

std::vector<std::shared_ptr<multiplayer_member>> multiplayer_game_session::members() const
{
    size_t memberCount = XblMultiplayerManagerGameSessionMembersCount();
    std::vector<XblMultiplayerManagerMember> members(memberCount);
    XblMultiplayerManagerGameSessionMembers(memberCount, members.data());

    return Utils::Transform<std::shared_ptr<multiplayer_member>>(members, [](const XblMultiplayerManagerMember& member)
    {
        return std::make_shared<multiplayer_member>(member);
    });
}

std::shared_ptr<multiplayer_member> multiplayer_game_session::host() const
{
    XblMultiplayerManagerMember host{};
    XblMultiplayerManagerGameSessionHost(&host);
    return std::make_shared<multiplayer_member>(host);
}

web::json::value multiplayer_game_session::properties() const
{
    return Utils::ParseJson(XblMultiplayerManagerGameSessionPropertiesJson());
}

const std::shared_ptr<xbox::services::multiplayer::multiplayer_session_constants> multiplayer_game_session::session_constants() const
{
    if (m_sessionConstants == nullptr)
    {
        m_sessionConstants = std::make_shared<xbox::services::multiplayer::multiplayer_session_constants>(false);
    }
    return m_sessionConstants;
}

bool multiplayer_game_session::is_host(
    _In_ const string_t& xboxUserId
)
{
    return XblMultiplayerManagerGameSessionIsHost(Utils::Uint64FromStringT(xboxUserId));
}

xbox_live_result<void> multiplayer_game_session::set_properties(
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson,
    _In_opt_ context_t context
)
{
    return Utils::ConvertHr(XblMultiplayerManagerGameSessionSetProperties(
        Utils::StringFromStringT(name).data(),
        Utils::StringFromStringT(valueJson.serialize()).data(),
        reinterpret_cast<void*>(context)
    ));
}

xbox_live_result<void> multiplayer_game_session::set_synchronized_properties(
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson,
    _In_opt_ context_t context
)
{
    return Utils::ConvertHr(XblMultiplayerManagerGameSessionSetSynchronizedProperties(
        Utils::StringFromStringT(name).data(),
        Utils::StringFromStringT(valueJson.serialize()).data(),
        reinterpret_cast<void*>(context)
    ));
}

xbox_live_result<void> multiplayer_game_session::set_synchronized_host(
    _In_ std::shared_ptr<multiplayer_member> gameHost,
    _In_opt_ context_t context
)
{
    return Utils::ConvertHr(XblMultiplayerManagerGameSessionSetSynchronizedHost(
        gameHost->m_internalMember.DeviceToken,
        reinterpret_cast<void*>(context)
    ));
}

multiplayer_event_args::multiplayer_event_args(XblMultiplayerEventArgsHandle argsHandle)
    : m_argsHandle(argsHandle) 
{
}

multiplayer_event_args::~multiplayer_event_args()
{
}

string_t multiplayer_event_args::GetXuid() const
{
    uint64_t xuid;
    XblMultiplayerEventArgsXuid(m_argsHandle, &xuid);
    return Utils::StringTFromUint64(xuid);
}

std::vector<std::shared_ptr<multiplayer_member>> multiplayer_event_args::GetMembers() const
{
    size_t memberCount;
    XblMultiplayerEventArgsMembersCount(m_argsHandle, &memberCount);
    std::vector<XblMultiplayerManagerMember> members(memberCount);
    XblMultiplayerEventArgsMembers(m_argsHandle, memberCount, members.data());

    return Utils::Transform<std::shared_ptr<multiplayer_member>>(members, [](const XblMultiplayerManagerMember& member)
    {
        return std::make_shared<multiplayer_member>(member);
    });
}

std::shared_ptr<multiplayer_member> multiplayer_event_args::GetMember() const
{
    XblMultiplayerManagerMember member{};
    XblMultiplayerEventArgsMember(m_argsHandle, &member);
    return std::make_shared<multiplayer_member>(member);
}

web::json::value multiplayer_event_args::GetPropertiesJson() const
{
    const char* json = nullptr;
    XblMultiplayerEventArgsPropertiesJson(m_argsHandle, &json);
    return Utils::ParseJson(json);
}


xbox::services::multiplayer::manager::match_status find_match_completed_event_args::match_status() const
{
    XblMultiplayerMatchStatus status;
    XblMultiplayerEventArgsFindMatchCompleted(m_argsHandle, &status, nullptr);
    return static_cast<xbox::services::multiplayer::manager::match_status>(status);
}

multiplayer_measurement_failure find_match_completed_event_args::initialization_failure_cause() const
{
    XblMultiplayerMeasurementFailure cause;
    XblMultiplayerEventArgsFindMatchCompleted(m_argsHandle, nullptr, &cause);
    return static_cast<xbox::services::multiplayer::multiplayer_measurement_failure>(cause);
}

std::map<string_t, string_t> perform_qos_measurements_event_args::connection_address_to_device_tokens() const
{
    XblMultiplayerPerformQoSMeasurementsArgs args{};
    XblMultiplayerEventArgsPerformQoSMeasurements(m_argsHandle, &args);

    std::map<string_t, string_t> out;
    for (size_t i = 0; i < args.remoteClientsSize; ++i)
    {
        out.insert(std::make_pair(Utils::StringTFromUtf8(args.remoteClients[i].connectionAddress), Utils::StringTFromUtf8(args.remoteClients[i].deviceToken.Value)));
    }
    return out;
}

multiplayer_event::multiplayer_event(_In_ const XblMultiplayerEvent* internalEvent) 
    : m_internalEvent(internalEvent) 
{
}

std::error_code multiplayer_event::err() const
{
    return std::make_error_code(static_cast<xbox::services::xbox_live_error_code>(m_internalEvent->Result));
}

std::string multiplayer_event::err_message() const
{
    if (m_internalEvent->ErrorMessage)
    {
        return m_internalEvent->ErrorMessage;
    }
    return std::string();
}

context_t multiplayer_event::context()
{
    return reinterpret_cast<context_t>(m_internalEvent->Context);
}

multiplayer_event_type multiplayer_event::event_type() const
{
    return static_cast<multiplayer_event_type>(m_internalEvent->EventType);
}

std::shared_ptr<multiplayer_event_args> multiplayer_event::event_args()
{
    if (m_eventArgs == nullptr)
    {
        switch (m_internalEvent->EventType)
        {
        case XblMultiplayerEventType::UserAdded:
        {
            m_eventArgs = std::make_shared<user_added_event_args>(m_internalEvent->EventArgsHandle);
            break;
        }
        case XblMultiplayerEventType::UserRemoved:
        {
            m_eventArgs = std::make_shared<user_removed_event_args>(m_internalEvent->EventArgsHandle);
            break;
        }
        case XblMultiplayerEventType::MemberJoined:
        {
            m_eventArgs = std::make_shared<member_joined_event_args>(m_internalEvent->EventArgsHandle);
            break;
        }
        case XblMultiplayerEventType::MemberLeft:
        {
            m_eventArgs = std::make_shared<member_left_event_args>(m_internalEvent->EventArgsHandle);
            break;
        }
        case XblMultiplayerEventType::MemberPropertyChanged:
        {
            m_eventArgs = std::make_shared<member_property_changed_event_args>(m_internalEvent->EventArgsHandle);
            break;
        }
        case XblMultiplayerEventType::SessionPropertyChanged:
        {
            m_eventArgs = std::make_shared<session_property_changed_event_args>(m_internalEvent->EventArgsHandle);
            break;
        }
        case XblMultiplayerEventType::HostChanged:
        {
            m_eventArgs = std::make_shared<host_changed_event_args>(m_internalEvent->EventArgsHandle);
            break;
        }
        case XblMultiplayerEventType::PerformQosMeasurements:
        {
            m_eventArgs = std::make_shared<perform_qos_measurements_event_args>(m_internalEvent->EventArgsHandle);
            break;
        }
        case XblMultiplayerEventType::FindMatchCompleted:
        {
            m_eventArgs = std::make_shared<find_match_completed_event_args>(m_internalEvent->EventArgsHandle);
            break;
        }
        case XblMultiplayerEventType::JoinLobbyCompleted:
        {
            m_eventArgs = std::make_shared<join_lobby_completed_event_args>(m_internalEvent->EventArgsHandle);
            break;
        }
        default: break;
        }
    }
    return m_eventArgs;
}

multiplayer_session_type multiplayer_event::session_type() const
{
    return static_cast<multiplayer_session_type>(m_internalEvent->SessionType);
}

std::shared_ptr<multiplayer_manager> multiplayer_manager::get_singleton_instance()
{
    static std::shared_ptr<multiplayer_manager> instance = std::shared_ptr<multiplayer_manager>(new multiplayer_manager());
    return instance;
}

void multiplayer_manager::initialize(
    _In_ const string_t& lobbySessionTemplateName
)
{
    XblMultiplayerManagerInitialize(Utils::StringFromStringT(lobbySessionTemplateName).data(), nullptr);
}

std::vector<multiplayer_event> multiplayer_manager::do_work()
{
    const XblMultiplayerEvent* eventPtr;
    size_t eventCount;
    XblMultiplayerManagerDoWork(&eventPtr, &eventCount);

    return Utils::Transform<multiplayer_event>(eventPtr, eventCount, [](const XblMultiplayerEvent& in)
    {
        return multiplayer_event(&in);
    });
}

std::shared_ptr<multiplayer_lobby_session> multiplayer_manager::lobby_session() const
{
    if (m_lobbySession == nullptr)
    {
        m_lobbySession = std::make_shared<multiplayer_lobby_session>();
    }
    return m_lobbySession;
}

std::shared_ptr<multiplayer_game_session> multiplayer_manager::game_session() const
{
    if (XblMultiplayerManagerGameSessionActive())
    {
        return std::make_shared<multiplayer_game_session>();
    }
    return nullptr;
}

xbox_live_result<void> multiplayer_manager::join_lobby(
    _In_ const string_t& handleId,
    _In_ xbox_live_user_t user
)
{
    return Utils::ConvertHr(XblMultiplayerManagerJoinLobby(Utils::StringFromStringT(handleId).data(), user));
}

#if (TV_API || UWP_API)
xbox_live_result<void> multiplayer_manager::join_lobby(
    _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
    _In_ xbox_live_user_t user
)
{
    // TODO
    UNREFERENCED_PARAMETER(eventArgs);
    UNREFERENCED_PARAMETER(user);
    return xbox_live_result<void>(xbox_live_error_code::unsupported);
}
#endif

#if TV_API
xbox_live_result<void> multiplayer_manager::join_lobby(
    _In_ const string_t& handleId,
    _In_ std::vector<Windows::Xbox::System::User^> users
)
{
    // TODO
    UNREFERENCED_PARAMETER(handleId);
    UNREFERENCED_PARAMETER(users);
    return xbox_live_result<void>(xbox_live_error_code::unsupported);
}

xbox_live_result<void> multiplayer_manager::join_lobby(
    _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
    _In_ std::vector<Windows::Xbox::System::User^> users
)
{
    // TODO
    UNREFERENCED_PARAMETER(eventArgs);
    UNREFERENCED_PARAMETER(users);
    return xbox_live_result<void>(xbox_live_error_code::unsupported);
}

void multiplayer_manager::invite_party_to_game()
{
    // TODO
}

#endif

xbox_live_result<void> multiplayer_manager::join_game_from_lobby(
    _In_ const string_t& sessionTemplateName
)
{
    return Utils::ConvertHr(XblMultiplayerManagerJoinGameFromLobby(Utils::StringFromStringT(sessionTemplateName).data()));
}

xbox_live_result<void> multiplayer_manager::join_game(
    _In_ const string_t& sessionName,
    _In_ const string_t& sessionTemplateName,
    _In_ const std::vector<string_t>& xboxUserIds
)
{
    auto xuids = Utils::XuidVectorFromXuidStringVector(xboxUserIds);

    return Utils::ConvertHr(XblMultiplayerManagerJoinGame(
        Utils::StringFromStringT(sessionName).data(),
        Utils::StringFromStringT(sessionTemplateName).data(),
        xuids.data(),
        static_cast<uint32_t>(xuids.size())
    ));
}

xbox_live_result<void> multiplayer_manager::leave_game()
{
    return Utils::ConvertHr(XblMultiplayerManagerLeaveGame());
}

xbox_live_result<void> multiplayer_manager::find_match(
    _In_ const string_t& hopperName,
    _In_ const web::json::value& attributes,
    _In_ const std::chrono::seconds& timeout
)
{
    return Utils::ConvertHr(XblMultiplayerManagerFindMatch(
        Utils::StringFromStringT(hopperName).data(),
        Utils::StringFromStringT(attributes.serialize()).data(),
        static_cast<uint32_t>(timeout.count())
    ));
}

void multiplayer_manager::cancel_match()
{
    return XblMultiplayerManagerCancelMatch();
}

xbox::services::multiplayer::manager::match_status multiplayer_manager::match_status() const
{
    return static_cast<xbox::services::multiplayer::manager::match_status>(XblMultiplayerManagerMatchStatus());
}

std::chrono::seconds multiplayer_manager::estimated_match_wait_time() const
{
    return std::chrono::seconds(XblMultiplayerManagerEstimatedMatchWaitTime());
}

bool multiplayer_manager::auto_fill_members_during_matchmaking() const
{
    return XblMultiplayerManagerAutoFillMembersDuringMatchmaking();
}

void multiplayer_manager::set_auto_fill_members_during_matchmaking(
    _In_ bool autoFillMembers
)
{
    XblMultiplayerManagerSetAutoFillMembersDuringMatchmaking(autoFillMembers);
}

void multiplayer_manager::set_quality_of_service_measurements(
    _In_ std::shared_ptr<std::vector<xbox::services::multiplayer::multiplayer_quality_of_service_measurements>> measurements
)
{
    web::json::value measurementsJson;
    for (const auto& measurement : *measurements)
    {
        web::json::value jsonMeasurement;
        jsonMeasurement[_T("latency")] = static_cast<int64_t>(measurement.latency().count());
        jsonMeasurement[_T("bandwidthDown")] = measurement.bandwidth_down_in_kilobits_per_second();
        jsonMeasurement[_T("bandwidthUp")] = measurement.bandwidth_up_in_kilobits_per_second();
        jsonMeasurement[_T("custom")] = measurement.custom_json();

        measurementsJson[measurement.member_device_token()] = jsonMeasurement;
    }
    auto measurementsJsonString = Utils::StringFromStringT(measurementsJson.serialize());
    XblMultiplayerManagerSetQosMeasurements(measurementsJsonString.data());
}

joinability multiplayer_manager::joinability() const
{
    return static_cast<xbox::services::multiplayer::manager::joinability>(XblMultiplayerManagerJoinability());
}

xbox_live_result<void> multiplayer_manager::set_joinability(
    _In_ xbox::services::multiplayer::manager::joinability value,
    _In_opt_ context_t context
)
{
    return Utils::ConvertHr(XblMultiplayerManagerSetJoinability(
        static_cast<XblMultiplayerJoinability>(value),
        reinterpret_cast<void*>(context)
    ));
}


#if defined(XSAPI_CPPWINRT)
#if TV_API
xbox_live_result<void> multiplayer_manager::join_lobby(
    _In_ const string_t& handleId,
    _In_ std::vector<winrt::Windows::Xbox::System::User> users
)
{
    return join_lobby(handleId, convert_user_vector_to_cppcx(users));
}
#endif
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END