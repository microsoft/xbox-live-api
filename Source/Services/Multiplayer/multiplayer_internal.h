// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi-c/multiplayer_c.h"
#include "real_time_activity_subscription.h"

#define MULTIPLAYER_HANDLE_VERSION 1

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

// Forward declarations
struct SendInvitesContext;

bool operator==(const XblMultiplayerSessionReference& lhs, const XblMultiplayerSessionReference& rhs);

struct Serializers
{
    static Result<XblMultiplayerActivityDetails> DeserializeMultiplayerActivityDetails(_In_ const JsonValue& json);

    static Result<XblMultiplayerSessionQueryResult> DeserializeMultiplayerSessionQueryResult(_In_ const JsonValue& json);

    static Result<XblMultiplayerSessionReference> DeserializeSessionReference(_In_ const JsonValue& json);
    static void SerializeSessionReference(const XblMultiplayerSessionReference& sessionReference, _Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator);

    static Result<XblMultiplayerInviteHandle> DeserializeMultiplayerInvite(_In_ const JsonValue& json);

    static XblMultiplayerSessionRestriction MultiplayerSessionRestrictionFromString(_In_ const xsapi_internal_string &value);
    static xsapi_internal_string StringFromMultiplayerSessionRestriction(_In_ XblMultiplayerSessionRestriction joinRestriction);
    static XblMultiplayerSessionStatus MultiplayerSessionStatusFromString(_In_ const xsapi_internal_string& value);
    static XblMultiplayerSessionVisibility MultiplayerSessionVisibilityFromString(_In_ const xsapi_internal_string& value);
    static xsapi_internal_string StringFromMultiplayerSessionVisibility(_In_ XblMultiplayerSessionVisibility sessionVisibility);

    static XblNetworkAddressTranslationSetting MultiplayerNatSettingFromString(_In_ const xsapi_internal_string& value);
    static XblMultiplayerMeasurementFailure MultiplayerMeasurementFailureFromString(_In_ const xsapi_internal_string& value);
    static XblMultiplayerSessionChangeTypes MultiplayerSessionChangeTypesFromStringVector(_In_ const xsapi_internal_vector<xsapi_internal_string>& changeTypeList);
};

/// <summary>
/// Represents a reference to member in a multiplayer session.
/// </summary>
class MultiplayerSessionMember
{
public:
    MultiplayerSessionMember() = default;
    MultiplayerSessionMember(const xsapi_internal_string& memberId);
    MultiplayerSessionMember(const MultiplayerSessionMember& other);

    static Result<XblMultiplayerSessionMember> Deserialize(
        _In_ const JsonValue& json
    );

    static XblMultiplayerSessionMember Construct(
        _In_ bool isCurrentUser,
        _In_ const xsapi_internal_string& memberId,
        _In_ uint64_t xuid,
        _In_opt_z_ const char* customConstantsJson,
        _In_ bool initializeRequested
    );

    static MultiplayerSessionMember* Get(const XblMultiplayerSessionMember* member);
    // Sets the internal member's m_member pointer and updates the fields of member accordingly
    // TODO fix this. If external member objects move around things break
    static void SetExternalMemberPointer(XblMultiplayerSessionMember& member);

    ~MultiplayerSessionMember();

    xsapi_internal_string MemberId() const;
    XblMultiplayerSessionChangeTypes SubscribedChangeTypes() const;

    void StateLock() const;
    void StateUnlock() const;

    // Use MultiplayerSessionMemberReadLockGuard that wraps StateLock/StateUnlock
    const xsapi_internal_vector<uint32_t>& MembersInGroupUnsafe() const;
    const xsapi_internal_vector<const char*>& GroupsUnsafe() const;
    const xsapi_internal_vector<const char*>& EncountersUnSafe() const;
    const JsonValue& CustomPropertiesJsonUnsafe() const;

    void SetSecureDeviceBaseAddress64(_In_ const xsapi_internal_string& deviceBaseAddress);
    void SetRoles(_In_ const xsapi_internal_vector<XblMultiplayerSessionMemberRole>& roles);
    void SetGroups(_In_reads_(groupsCount) const char** groups, _In_ size_t groupsCount);
    void SetEncounters(_In_reads_(encountersCount) const char** encounters, _In_ size_t encountersCount);

    HRESULT SetStatus(_In_ XblMultiplayerSessionMemberStatus status);
    void SetMembersInGroup(_In_ const xsapi_internal_vector<uint32_t>& membersInGroup);

    HRESULT SetCustomPropertyJson(
        _In_ const xsapi_internal_string& name,
        _In_ const JsonValue& valueJson
    );
    void DeleteCustomPropertyJson(_In_ const xsapi_internal_string& name);
    HRESULT SetServerMeasurementsJson(_In_ const xsapi_internal_string& serverMeasurementsJson);
    HRESULT SetQosMeasurementsJson(_In_ const xsapi_internal_string& qosMeasurementsJson);
    void SetRtaConnectionId(_In_ const xsapi_internal_string& rtaConnectionId);
    void SetSessionChangeSubscription(
        _In_ XblMultiplayerSessionChangeTypes changeTypes,
        _In_ const xsapi_internal_string& subscriptionId
    );

    void Serialize(_Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator);

private:
    static xsapi_internal_vector<xsapi_internal_string> GetVectorViewForChangeTypes(_In_ XblMultiplayerSessionChangeTypes changeTypes);

    XblMultiplayerSessionMember* m_member = nullptr;
    xsapi_internal_string m_customConstantsJson;
    xsapi_internal_string m_customPropertiesString;
    JsonDocument m_customPropertiesJson{ rapidjson::Type::kObjectType };
    xsapi_internal_string m_secureDeviceAddressBase64;
    xsapi_internal_vector<XblMultiplayerSessionMemberRole> m_roles;
    JsonDocument m_resultsJson;
    xsapi_internal_string m_teamId;
    xsapi_internal_string m_initialTeam;

    xsapi_internal_vector<const char*> m_groups;
    xsapi_internal_vector<const char*> m_encounters;
    xsapi_internal_vector<uint32_t> m_membersInGroupIds;

    XblMultiplayerSessionChangeTypes m_subscribedChangeTypes{ XblMultiplayerSessionChangeTypes::None };
    xsapi_internal_string m_matchmakingResultServerMeasurementsJson;
    xsapi_internal_string m_serverMeasurementsJson;
    xsapi_internal_string m_qosMeasurementsJson;

    xsapi_internal_string m_subscriptionId;
    xsapi_internal_string m_rtaConnectionId;

    xsapi_internal_string m_memberIdToWrite;
    bool m_newMember{ false };
    bool m_writeConstants{ false };
    bool m_writeIsActive{ false };
    bool m_writeRoleInfo{ false };
    bool m_writeSecureDeviceAddressBase64{ false };
    bool m_writeQoSMeasurementsJson{ false };
    bool m_writeServerMeasurementsJson{ false };
    bool m_writeMembersInGroup{ false };
    bool m_writeGroups{ false };
    bool m_writeEncounters{ false };
    bool m_writeSubscribedChangeTypes{ false };
    bool m_writeResults{ false };
    bool m_writeCustomPropertiesJson{ false };

    // needs to be recursive mutex since CompareMultiplayerSessions will lock both currentMember and olderSessionMember which might be same 
    mutable std::recursive_mutex m_lockMember; 
};

class MultiplayerSessionMemberReadLockGuard
{
public:
    MultiplayerSessionMemberReadLockGuard(_In_opt_ MultiplayerSessionMember* member) :
        m_member(member)
    {
        if (m_member)
        {
            m_member->StateLock();
        }
    }

    ~MultiplayerSessionMemberReadLockGuard()
    {
        if (m_member)
        {
            m_member->StateUnlock();
        }
    }

    const xsapi_internal_vector<uint32_t>& MembersInGroup() const
    {
        return m_member->MembersInGroupUnsafe();
    }

    const xsapi_internal_vector<const char*>& Groups() const
    {
        return m_member->GroupsUnsafe();
    }

    const xsapi_internal_vector<const char*>& Encounters() const
    {
        return m_member->EncountersUnSafe();
    }

    const JsonValue& CustomPropertiesJson() const
    {
        return m_member->CustomPropertiesJsonUnsafe();
    }

private:
    MultiplayerSessionMember* m_member;
};

class MultiplayerQuerySearchHandleRequest
{
public:
    MultiplayerQuerySearchHandleRequest(
        _In_ const xsapi_internal_string& scid,
        _In_ const xsapi_internal_string& sessionTemplateName
        );
    MultiplayerQuerySearchHandleRequest(
        _In_ const xsapi_internal_string& scid,
        _In_ const xsapi_internal_string& sessionTemplateName,
        _In_ const xsapi_internal_string& orderBy,
        _In_ bool orderAscending,
        _In_ const xsapi_internal_string& searchFilter
        );

    const xsapi_internal_string& Scid() const;
    const xsapi_internal_string& SessionTemplateName() const;
    const xsapi_internal_string& OrderBy() const;
    void SetOrderBy(_In_ const xsapi_internal_string& orderBy);
    bool OrderAscending();
    void SetOrderAscending(_In_ bool orderAscending);
    const xsapi_internal_string& SearchFilter() const;
    void SetSearchFilter(_In_ const xsapi_internal_string& searchFilter);
    const xsapi_internal_string& SocialGroup() const;
    void SetSocialGroup(_In_ const xsapi_internal_string& socialGroup);

    void Serialize(_In_ uint64_t socialGroupXuid, _Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator) const;

private:
    xsapi_internal_string m_serviceConfigurationId;
    xsapi_internal_string m_sessionTemplateName;
    xsapi_internal_string m_orderBy;
    bool m_orderAscending{ false };
    xsapi_internal_string m_searchFilter;
    xsapi_internal_string m_socialGroup;
};

class MultiplayerSearchHandleRequest
{
public:
    MultiplayerSearchHandleRequest(
        _In_ XblMultiplayerSessionReference sessionRef
    );

    const XblMultiplayerSessionReference& SessionReference() const;
    const xsapi_internal_vector<XblMultiplayerSessionTag>& Tags() const;
    void SetTags(_In_ xsapi_internal_vector<XblMultiplayerSessionTag>&& value);
    const xsapi_internal_vector<XblMultiplayerSessionNumberAttribute>& NumberAttributes() const;
    void SetNumberAttributes(_In_ xsapi_internal_vector<XblMultiplayerSessionNumberAttribute>&& attributes);
    const xsapi_internal_vector<XblMultiplayerSessionStringAttribute>& StringAttributes() const;
    void SetStringAttributes(_In_ xsapi_internal_vector<XblMultiplayerSessionStringAttribute>&& attributes);

    void Serialize(_Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator) const;

private:
    uint32_t m_version{ MULTIPLAYER_HANDLE_VERSION };
    XblMultiplayerSessionReference m_sessionReference;
    xsapi_internal_vector<XblMultiplayerSessionTag> m_tags;
    xsapi_internal_vector<XblMultiplayerSessionStringAttribute> m_stringAttributes;
    xsapi_internal_vector<XblMultiplayerSessionNumberAttribute> m_numberAttributes;
};

class MultiplayerInviteHandlePostRequest
{
public:
    MultiplayerInviteHandlePostRequest(
        _In_ const XblMultiplayerSessionReference& sessionReference,
        _In_ uint64_t invitedXuid,
        _In_ uint32_t titleId,
        _In_ const String& contextString,
        _In_ const String& customActivationContext
    ) noexcept;

    void SetInvitedXuid(uint64_t invitedXuid) noexcept;
    const JsonValue& Json() const noexcept;

private:
    JsonDocument m_json;
};

class MultiplayerSubscription : public real_time_activity::Subscription
{
public:
    MultiplayerSubscription() noexcept;

    const String& RtaConnectionId() const;

    typedef Function<void(const XblMultiplayerSessionChangeEventArgs& args)> SessionChangedHandler;
    XblFunctionContext AddSessionChangedHandler(
        SessionChangedHandler handler
    ) noexcept;

    size_t RemoveSessionChangedHandler(
        XblFunctionContext token
    ) noexcept;

    typedef Function<void(const String& connectionId)> ConnectionIdChangedHandler;
    XblFunctionContext AddConnectionIdChangedHandler(
        ConnectionIdChangedHandler handler
    ) noexcept;

    size_t RemoveConnectionIdChangedHandler(
        XblFunctionContext token
    ) noexcept;

protected:
    void OnSubscribe(_In_ const JsonValue& data) noexcept override;
    void OnEvent(_In_ const JsonValue& data) noexcept override;

private:
    String m_connectionId;

    XblFunctionContext m_nextToken{ 1 };
    Map<XblFunctionContext, SessionChangedHandler> m_sessionChangedHandlers;
    Map<XblFunctionContext, ConnectionIdChangedHandler> m_connectionIdChangedHandlers;

    std::mutex m_mutexMultiplayerSubscription;
};

class MultiplayerActivityQueryPostRequest
{
public:
    MultiplayerActivityQueryPostRequest();
    MultiplayerActivityQueryPostRequest(_In_ const xsapi_internal_string& scid, _In_ const xsapi_internal_vector<uint64_t>& xuids);
    MultiplayerActivityQueryPostRequest(_In_ const xsapi_internal_string& scid, _In_ const xsapi_internal_string& socialGroup, _In_ uint64_t socialGroupXuid);

    const xsapi_internal_string& Scid() const;
    const xsapi_internal_vector<uint64_t>& Xuids() const;
    const xsapi_internal_string& SocialGroup() const;
    uint64_t SocialGroupXuid() const;

    void Serialize(_Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator);
private:
    xsapi_internal_string m_scid;
    xsapi_internal_vector<uint64_t> m_xuids;
    uint64_t m_socialGroupXuid{ 0 };
    xsapi_internal_string m_socialGroup;
};

class MultiplayerActivityHandlePostRequest
{
public:
    MultiplayerActivityHandlePostRequest(_In_ XblMultiplayerSessionReference sessionReference);

    const XblMultiplayerSessionReference& SessionReference() const;

    void Serialize(_Out_ JsonValue&, _In_ JsonDocument::AllocatorType& allocator) const;
private:
    XblMultiplayerSessionReference m_sessionReference;
};

class MultiplayerTransferHandlePostRequest
{
public:
    MultiplayerTransferHandlePostRequest(
        _In_ XblMultiplayerSessionReference targetSessionReference,
        _In_ XblMultiplayerSessionReference originSessionReference
    );

    const XblMultiplayerSessionReference& OriginSessionReference() const;
    const XblMultiplayerSessionReference& TargetSessionReference() const;

    void Serialize(_Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator) const;
private:
    XblMultiplayerSessionReference m_originSessionReference;
    XblMultiplayerSessionReference m_targetSessionReference;
};

struct SessionQuery : public XblMultiplayerSessionQuery
{
public:
    SessionQuery(const XblMultiplayerSessionQuery* other) noexcept;
    SessionQuery(const SessionQuery& other) noexcept;
    SessionQuery& operator=(SessionQuery other) noexcept = delete;
    ~SessionQuery() noexcept = default;

    String PathAndQuery() const noexcept;
    JsonDocument RequestBody() const noexcept;

private:
    Vector<uint64_t> m_xuidFilters;
    String m_keywordFilter;
};

class MultiplayerService : public std::enable_shared_from_this<MultiplayerService>
{
public:
    MultiplayerService(
        _In_ User&& user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::AppConfig> appConfig,
        _In_ std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> rtaService
    ) noexcept;

    ~MultiplayerService() noexcept;

    HRESULT WriteSession(
        _In_ std::shared_ptr<XblMultiplayerSession> multiplayerSession,
        _In_ XblMultiplayerSessionWriteMode writeMode,
        _In_ AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>> async
    ) noexcept;

    HRESULT WriteSessionByHandle(
        _In_ std::shared_ptr<XblMultiplayerSession> multiplayerSession,
        _In_ XblMultiplayerSessionWriteMode writeMode,
        _In_ const String& handleId,
        _In_ AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>> async
    ) noexcept;

    HRESULT GetCurrentSession(
        _In_ XblMultiplayerSessionReference sessionReference,
        _In_ AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>> async
    ) const noexcept;

    HRESULT GetCurrentSessionByHandle(
        _In_ const String& handleId,
        _In_ AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>> async
    ) const noexcept;

    HRESULT GetSessions(
        _In_ const SessionQuery& getSessionsRequest,
        _In_ AsyncContext<Result<Vector<XblMultiplayerSessionQueryResult>>> async
    ) const noexcept;

    HRESULT SetActivity(
        _In_ const XblMultiplayerSessionReference& sessionReference,
        _In_ AsyncContext<Result<void>> async
    ) const noexcept;

    HRESULT SetTransferHandle(
        _In_ const XblMultiplayerSessionReference& targetSessionReference,
        _In_ const XblMultiplayerSessionReference& originSessionReference,
        _In_ AsyncContext<Result<String>> async
    ) const noexcept;

    HRESULT CreateSearchHandle(
        _In_ MultiplayerSearchHandleRequest searchHandleRequest,
        _In_ AsyncContext<Result<std::shared_ptr<XblMultiplayerSearchHandleDetails>>> async
    ) const noexcept;

    HRESULT ClearActivity(
        _In_ const String& scid,
        _In_ AsyncContext<Result<void>> callback
    ) const noexcept;

    HRESULT DeleteSearchHandle(
        _In_ const String& handleId,
        _In_ AsyncContext<Result<void>> async
    ) const noexcept;

    HRESULT SendInvites(
        _In_ XblMultiplayerSessionReference sessionReference,
        _In_ const Vector<uint64_t>& xboxUserIds,
        _In_ uint32_t titleId,
        _In_ const String& contextStringId,
        _In_ const String& customActivationContext,
        _In_ AsyncContext<Result<Vector<String>>> async
    ) const noexcept;

    HRESULT GetActivitiesForSocialGroup(
        _In_ const String& scid,
        _In_ uint64_t socialGroupOwnerXuid,
        _In_ const String& socialGroup,
        _In_ AsyncContext<Result<Vector<XblMultiplayerActivityDetails>>> async
    ) const noexcept;

    HRESULT GetActivitiesForUsers(
        _In_ const String& scid,
        _In_ const Vector<uint64_t>& xuids,
        _In_ AsyncContext<Result<Vector<XblMultiplayerActivityDetails>>> async
    ) const noexcept;

    HRESULT GetSearchHandles(
        _In_ const MultiplayerQuerySearchHandleRequest& searchHandleRequest,
        _In_ AsyncContext<Result<Vector<std::shared_ptr<XblMultiplayerSearchHandleDetails>>>> async
    ) const noexcept;

    HRESULT EnableMultiplayerSubscriptions() noexcept;
    HRESULT DisableMultiplayerSubscriptions() noexcept;
    bool SubscriptionsEnabled() noexcept;

    XblFunctionContext AddMultiplayerSessionChangedHandler(
        _In_ MultiplayerSubscription::SessionChangedHandler handler
    ) noexcept;

    void RemoveMultiplayerSessionChangedHandler(
        _In_ XblFunctionContext token
    ) noexcept;

    typedef Function<void()> SubscriptionLostHandler;
    XblFunctionContext AddMultiplayerSubscriptionLostHandler(
        _In_ SubscriptionLostHandler handler
    ) noexcept;

    void RemoveMultiplayerSubscriptionLostHandler(
        _In_ XblFunctionContext token
    ) noexcept;

    XblFunctionContext AddMultiplayerConnectionIdChangedHandler(
        _In_ MultiplayerSubscription::ConnectionIdChangedHandler handler
    ) noexcept;

    void RemoveMultiplayerConnectionIdChangedHandler(
        _In_ XblFunctionContext token
    ) noexcept;

private:
    // Sets the RTA connection Id on a session if subscriptions are enabled
    HRESULT SetRtaConnectionId(
        _In_ std::shared_ptr<XblMultiplayerSession> session,
        _In_ AsyncContext<Result<void>> async
    ) noexcept;

    HRESULT SubscribeToRta(std::unique_lock<std::mutex> lock) noexcept;
    HRESULT UnsubscribeFromRta() noexcept;

    HRESULT WriteSessionUsingSubpath(
        _In_ std::shared_ptr<XblMultiplayerSession> session,
        _In_ XblMultiplayerSessionWriteMode mode,
        _In_ const String& subpathAndQuery,
        _In_ AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>> async
    ) noexcept;

    static String MultiplayerSessionDirectoryCreateOrUpdateSubpath(
        _In_ const String& serviceConfigurationId,
        _In_ const String& sessionTemplateName,
        _In_ const String& sessionName
    ) noexcept;

    static String MultiplayerSessionDirectoryCreateOrUpdateByHandleSubpath(
        _In_ const String& handleId
    ) noexcept;

    User m_user;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::AppConfig> m_appConfig;
    std::shared_ptr<real_time_activity::RealTimeActivityManager> m_rtaManager;

    // RTA state
    std::shared_ptr<MultiplayerSubscription> m_subscription;

    XblFunctionContext m_rtaConnectionStateChangedToken{ 0 };
    XblFunctionContext m_nextClientToken{ 1 };
    Map<XblFunctionContext, SubscriptionLostHandler> m_subscriptionLostHandlers;
    Map<XblFunctionContext, MultiplayerSubscription::ConnectionIdChangedHandler> m_connectionIdChangedHandlers;

    List<std::pair<std::shared_ptr<XblMultiplayerSession>, AsyncContext<Result<void>>>> m_sessionsAwaitingConnectionId;

    // Since MPSA RTA subscription can be used both to get session changed events and to enable
    // automatic session member removal, we allow titles to force enable the RTA subscription
    bool m_forceEnableRtaSubscription{ false };

    std::mutex m_mutexMultiplayerService;
};

class RoleTypes
{
public:
    RoleTypes() = default;
    RoleTypes(const RoleTypes& other) noexcept;
    RoleTypes& operator=(RoleTypes other) noexcept;
    ~RoleTypes() noexcept;

    static Result<RoleTypes> Deserialize(const JsonValue& json) noexcept;
    JsonValue Serialize(JsonDocument::AllocatorType& allocator) const noexcept;

    const Vector<XblMultiplayerRoleType>& Values() const noexcept;

    HRESULT SetRoleSettings(
        String&& roleTypeName,
        String&& roleName,
        uint32_t* maxCount,
        uint32_t* targetCount
    ) noexcept;

    XblMultiplayerRole* GetRole(
        String&& roleType,
        String&& roleName
    ) const noexcept;
private:
    Vector<XblMultiplayerRoleType> m_values{};
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END

struct XblMultiplayerSession : public xbox::services::RefCounter, public std::enable_shared_from_this<XblMultiplayerSession>
{
public:
    XblMultiplayerSession(
        _In_ uint64_t xuid,
        _In_opt_ const XblMultiplayerSessionReference* sessionReference = nullptr,
        _In_opt_ const XblMultiplayerSessionInitArgs* initArgs = nullptr
    );

    XblMultiplayerSession(
        _In_ uint64_t xboxUserId,
        _In_ XblMultiplayerSessionReference sessionReference,
        _In_ const xsapi_internal_string& eTag,
        _In_ const xsapi_internal_string& responseDate,
        _In_ const JsonValue& json
    );

    XblMultiplayerSession(const XblMultiplayerSession& other);
    XblMultiplayerSession& operator=(const XblMultiplayerSession& rhs) = delete;

    ~XblMultiplayerSession();

    bool operator==(const XblMultiplayerSession& rhs) const;

    void Initialize();

    time_t TimeOfSession() const;
    const xsapi_internal_string ETag() const;
    const XblMultiplayerSessionInfo& SessionInfo() const; // only written during Deserialize so safe to return ref
    const XblMultiplayerSessionInitializationInfo& InitializationInfo() const; // only written during Deserialize so safe to return ref
    const XblMultiplayerSessionReference& SessionReference() const; // only written during ctor so safe to return ref
    const xsapi_internal_vector<XblDeviceToken>& HostCandidates() const; // only written during Deserialize so safe to return ref

    void StateLock() const;
    void StateUnlock() const;

    // Use XblMultiplayerSessionReadLockGuard that wraps StateLock/StateUnlock
    const XblMultiplayerSessionConstants& SessionConstantsUnsafe() const;
    const XblMultiplayerSessionProperties& SessionPropertiesUnsafe() const;
    const xbox::services::multiplayer::RoleTypes& RoleTypesUnsafe() const;
    const xsapi_internal_vector<XblMultiplayerSessionMember>& MembersUnsafe() const;
    const xsapi_internal_vector<const char*>& ServerConnectionStringCandidatesUnsafe() const;
    const xsapi_internal_vector<uint32_t>& TurnCollectionUnsafe() const;
    const xsapi_internal_vector<const char*>& KeywordsUnsafe() const;
    const XblMultiplayerSessionMember* CurrentUserUnsafe() const;
    xbox::services::multiplayer::MultiplayerSessionMember* CurrentUserInternalUnsafe() const;
    const XblMultiplayerSessionMember* GetMemberUnsafe(uint32_t memberId) const;
    const xsapi_internal_string& RawServersJsonUnsafe() const;
    const xsapi_internal_string& ETagUnsafe() const;

    uint32_t MembersAccepted() const;
    const xsapi_internal_string RawServersJson() const;
    std::shared_ptr<const XblMultiplayerMatchmakingServer> MatchmakingServer() const; // only written during Deserialize so safe to return ref
    XblWriteSessionStatus WriteStatus() const;

    HRESULT DeserializationError() const;

    HRESULT SetServersJson(
        _In_ const xsapi_internal_string& serversJson
    );
    HRESULT AddMemberReservation(
        _In_ uint64_t xuid,
        _In_opt_z_ const char* memberCustomConstantsJson = nullptr,
        _In_ bool initializeRequested = false
    );
    HRESULT Join(
        _In_opt_z_ const char* memberCustomConstantsJson = nullptr,
        _In_ bool initializeRequested = true,
        _In_ bool joinWithActiveStatus = true
    );
    void SetVisibility(
        _In_ XblMultiplayerSessionVisibility visibility
    );
    void SetMaxMembersInSession(
        _In_ uint32_t maxMembersInSession
    );
    HRESULT SetMutableRoleSettings(
        _In_ String&& roleTypeName,
        _In_ String&& roleName,
        _In_opt_ uint32_t * maxCount,
        _In_opt_ uint32_t * targetCount
    );
    HRESULT SetTimeouts(
        _In_ uint64_t memberReservedTimeout,
        _In_ uint64_t memberInactiveTimeout,
        _In_ uint64_t memberReadyTimeout,
        _In_ uint64_t sessionEmptyTimeout
    );
    HRESULT SetQosConnectivityMetrics(
        _In_ bool enableLatencyMetric,
        _In_ bool enableBandwidthDownMetric,
        _In_ bool enableBandwidthUpMetric,
        _In_ bool enableCustomMetric
    );
    HRESULT SetMemberInitialization(
        _In_ const XblMultiplayerMemberInitialization& memberInitialization
    );
    HRESULT SetPeerToPeerRequirements(
        _In_ const XblMultiplayerPeerToPeerRequirements& requirements
    );
    HRESULT SetPeerToHostRequirements(
        _In_ const XblMultiplayerPeerToHostRequirements& requirements
    );
    HRESULT SetMeasurementServerAddresses(
        _In_ const xsapi_internal_string& measurementServerAddresses
    );
    HRESULT SetSessionCapabilities(
        _In_ const XblMultiplayerSessionCapabilities& capabilities
    );
    HRESULT SetCloudComputePackageJson(
        _In_ const xsapi_internal_string& sessionCloudComputePackageConstantsJson
    );
    void SetInitializationStatus(
        _In_ bool initializationSucceeded
    );
    void SetHostDeviceToken(
        _In_ const XblDeviceToken hostDeviceToken
    );
    void SetHostDeviceToken(
        _In_ const xsapi_internal_string& deviceToken
    );
    void SetMatchmakingServerConnectionPath(
        _In_ const xsapi_internal_string& serverConnectionPath
    );
    void SetClosed(
        _In_ bool closed
    );
    void SetLocked(
        _In_ bool locked
    );
    void SetAllocateCloudCompute(
        _In_ bool allocateCloudCompute
    );
    void SetMatchmakingResubmit(
        _In_ bool matchResubmit
    );
    HRESULT SetServerConnectionStringCandidates(
        _In_reads_(serverConnectionStringCandidatesCount) const char** serverConnectionStringCandidates,
        _In_ size_t serverConnectionStringCandidatesCount
    );
    HRESULT SetSessionChangeSubscription(
        _In_ XblMultiplayerSessionChangeTypes changeTypes
    );
    HRESULT Leave();
    HRESULT SetCurrentUserStatus(
        _In_ XblMultiplayerSessionMemberStatus status
    );
    HRESULT SetCurrentUserSecureDeviceAddressBase64(
        _In_ const xsapi_internal_string& value
    );
    HRESULT SetCurrentUserRoleInfo(
        _In_ const xsapi_internal_vector<XblMultiplayerSessionMemberRole>& roles
    );
    HRESULT SetCurrentUserMembersInGroup(
        _In_ const xsapi_internal_vector<uint32_t>& membersInGroup
    );
    HRESULT SetCurrentUserGroups(
        _In_reads_(groupsCount) const char** groups,
        _In_ size_t groupsCount
    );
    HRESULT SetCurrentUserEncounters(
        _In_reads_(encountersCount) const char** encounters,
        _In_ size_t encountersCount
    );
    HRESULT SetCurrentUserServerMeasurementsJson(
        _In_ const xsapi_internal_string& serverMeasurementsJson
    );
    HRESULT SetCurrentUserQosMeasurementsJson(
        _In_ const xsapi_internal_string& qosMeasurementsJson
    );
    HRESULT SetCurrentUserMemberCustomPropertyJson(
        _In_ const xsapi_internal_string& name,
        _In_ const JsonValue& valueJson = JsonValue()
    );
    HRESULT DeleteCurrentUserMemberCustomPropertyJson(
        _In_ const xsapi_internal_string& name
    );
    HRESULT SetMatchmakingTargetSessionConstantsJson(
        _In_ const xsapi_internal_string& matchmakingTargetSessionConstantsJson
    );
    HRESULT SetSessionCustomPropertyJson(
        _In_ const xsapi_internal_string& name,
        _In_ const JsonValue& valueJson = JsonValue()
    );
    HRESULT DeleteSessionCustomPropertyJson(
        _In_ const xsapi_internal_string& name
    );
    HRESULT SetKeywords(
        _In_ const char** keywords,
        _In_ size_t keywordsCount
    );
    void SetJoinRestriction(
        _In_ XblMultiplayerSessionRestriction joinRestriction
    );
    void SetReadRestriction(
        _In_ XblMultiplayerSessionRestriction readRestriction
    );
    HRESULT SetTurnCollection(
        _In_ const xsapi_internal_vector<uint32_t>& turnCollection
    );

    XblMultiplayerSessionChangeTypes CompareMultiplayerSessions(
        _In_ std::shared_ptr<XblMultiplayerSession> other
    );
    static XblWriteSessionStatus ConvertHttpStatusToWriteSessionStatus(
        _In_ int32_t httpStatusCode
    );

    void SetWriteSessionStatus(
        int32_t httpStatusCode
    );

    static xsapi_internal_string ConvertMultiplayerHostSelectionMetricToString(_In_ XblMultiplayerMetrics multiplayMetric);
    static XblMultiplayerMetrics ConvertStringToMultiplayerHostSelectionMetric(_In_ const xsapi_internal_string& value);
    static XblMultiplayerInitializationStage ConvertStringToMultiplayerInitializationStage(_In_ const xsapi_internal_string& value);
    static XblMatchmakingStatus ConvertStringToMatchmakingStatus(_In_ const xsapi_internal_string& value);
    static xsapi_internal_string ConvertMatchmakingStatusToString(_In_ XblMatchmakingStatus matchmakingStatus);

    static bool IsHost(
        _In_ const xsapi_internal_string& memberDeviceToken,
        _In_ const std::shared_ptr<XblMultiplayerSession>& session
    );

    static bool IsPlayerInSession(
        _In_ uint64_t xboxUserId,
        _In_ const std::shared_ptr<XblMultiplayerSession>& session
    );

    static const XblMultiplayerSessionMember* GetPlayerInSession(
        _In_ uint64_t xboxUserId,
        _In_ std::shared_ptr<XblMultiplayerSession> session
    );
    static const XblMultiplayerSessionMember* HostMember(_In_ std::shared_ptr<XblMultiplayerSession> session);
    static bool HasSessionPropertyChanged(
        _In_ const std::shared_ptr<XblMultiplayerSession>& session1,
        _In_ const std::shared_ptr<XblMultiplayerSession>& session2,
        _In_ const xsapi_internal_string& propertyName
    );

    static bool DoSessionsMatch(_In_ std::shared_ptr<XblMultiplayerSession> lhs, _In_ std::shared_ptr<XblMultiplayerSession> rhs);

    void Serialize(_Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator);

private:
    // Deserialization helpers
    HRESULT Deserialize(_In_ const JsonValue& json);
    HRESULT DeserializeMembers(_In_ const JsonValue& json);
    HRESULT DeserializeMatchmakingServer(_In_ const JsonValue& json);
    HRESULT DeserializeSessionProperties(_In_ const JsonValue& json);
    HRESULT DeserializeSessionConstants(_In_ const JsonValue& json);

    // Serialization helpers
    void SerializeSessionProperties(_Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator);
    void SerializeSessionConstants(_Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator);

    // Fields not part of MPSD document
    uint64_t m_xuid{ 0 };
    xsapi_internal_string m_eTag;
    time_t m_sessionRetrievedTime;

    // MPSD document fields
    XblMultiplayerSessionInfo m_info{};
    XblMultiplayerSessionInitializationInfo m_initialization{};
    XblMultiplayerSessionReference m_sessionReference{};
    xsapi_internal_vector<XblDeviceToken> m_hostCandidates;

    // Constants
    XblMultiplayerSessionConstants m_sessionConstants{};
    xsapi_internal_vector<uint64_t> m_initiatorXuids;
    XblMultiplayerMemberInitialization m_memberInitialization{};
    xsapi_internal_string m_constantsCustomJson;
    xsapi_internal_string m_constantsCloudComputePackageJson;
    xsapi_internal_string m_constantsMeasurementServerAddressesJson;

    // Properties
    XblMultiplayerSessionProperties m_sessionProperties{};
    xsapi_internal_vector<const char*> m_keywords;
    xsapi_internal_vector<uint32_t> m_sessionOwnerIndices;
    xsapi_internal_vector<uint32_t> m_turnCollection;
    xsapi_internal_vector<const char*> m_serverConnectionStringCandidates;
    xsapi_internal_string m_matchmakingServerConnectionString;
    xsapi_internal_string m_matchmakingTargetSessionConstantsJson;
    xsapi_internal_string m_sessionCustomPropertiesJson;

    // Roles
    xbox::services::multiplayer::RoleTypes m_roleTypes;

    // Member info
    xsapi_internal_vector<XblMultiplayerSessionMember> m_members;
    XblMultiplayerSessionMember* m_memberCurrentUser{ nullptr };
    uint32_t m_membersAccepted{ 0 };

    // Servers info
    xsapi_internal_string m_serversJson;
    std::shared_ptr<XblMultiplayerMatchmakingServer> m_matchmakingServer;
    xsapi_internal_string m_matchmakingStatusDetails;
    xsapi_internal_string m_lastTeamResultTeam;

    XblWriteSessionStatus m_writeSessionStatus{};
    std::atomic<bool> m_joiningSession;
    bool m_newSession{ false };
    HRESULT m_deserializationError;
    xsapi_internal_string m_sessionSubscriptionGuid;

    bool m_writePropertiesKeywords{ false };
    bool m_writePropertiesTurns{ false };
    bool m_writeInitializationStatus{ false };
    bool m_initializationSucceeded{ false };
    bool m_writeHostDeviceToken{ false };
    bool m_writeMatchmakingServerConnectionPath{ false };
    bool m_writeMatchmakingResubmit{ false };
    bool m_writeServerConnectionStringCandidates{ false };
    bool m_leaveSession{ false };
    bool m_writeClosed{ false };
    bool m_writeLocked{ false };
    bool m_writeAllocateCloudCompute{ false };
    bool m_writeRoleTypes{ false };
    bool m_writeTimeouts{ false };
    bool m_writeQosConnectivityMetrics{ false };
    bool m_writeMemberInitialization{ false };
    bool m_writePeerToPeerRequirements{ false };
    bool m_writePeerToHostRequirements{ false };
    bool m_writeMeasurementServerAddresses{ false };
    bool m_writeJoinRestriction{ false };
    bool m_writeReadRestriction{ false };
    bool m_writeServersJson{ false };
    bool m_writeMatchmakingTargetSessionConstants{ false };
    bool m_writeSessionCustomPropertiesJson{ false };
    bool m_writeConstants{ false };

    mutable std::recursive_mutex m_lockSession;
    uint32_t m_memberRequestIndex{ 0 };

    // RefCounter override
    std::shared_ptr<xbox::services::RefCounter> GetSharedThis() override;
};

class XblMultiplayerSessionReadLockGuard
{
public:
    XblMultiplayerSessionReadLockGuard(_In_opt_ std::shared_ptr<XblMultiplayerSession> session) :
        m_session(session)
    {
        if (m_session)
        {
            m_session->StateLock();
        }
    }

    ~XblMultiplayerSessionReadLockGuard()
    {
        if (m_session)
        {
            m_session->StateUnlock();
        }
    }

    const XblMultiplayerSessionConstants& SessionConstants() const
    {
        return m_session->SessionConstantsUnsafe();
    }

    const XblMultiplayerSessionProperties& SessionProperties() const
    {
        return m_session->SessionPropertiesUnsafe();
    }

    const xbox::services::multiplayer::RoleTypes& RoleTypes() const
    {
        return m_session->RoleTypesUnsafe();
    }

    const xsapi_internal_vector<XblMultiplayerSessionMember>& Members() const
    {
        return m_session->MembersUnsafe();
    }

    const xsapi_internal_vector<const char*>& ServerConnectionStringCandidates() const
    {
        return m_session->ServerConnectionStringCandidatesUnsafe();
    }

    const xsapi_internal_vector<uint32_t>& TurnCollection() const
    {
        return m_session->TurnCollectionUnsafe();
    }

    const xsapi_internal_vector<const char*>& Keywords() const
    {
        return m_session->KeywordsUnsafe();
    }
    const XblMultiplayerSessionMember* CurrentUser() const
    {
        return m_session->CurrentUserUnsafe();
    }

    xbox::services::multiplayer::MultiplayerSessionMember* CurrentUserInternal() const
    {
        return m_session->CurrentUserInternalUnsafe();
    }

    const XblMultiplayerSessionMember* GetMember(uint32_t memberId) const
    {
        return m_session->GetMemberUnsafe(memberId);
    }

private:
    std::shared_ptr<XblMultiplayerSession> m_session;
};

struct XblMultiplayerSearchHandleDetails : public xbox::services::RefCounter, public std::enable_shared_from_this<XblMultiplayerSearchHandleDetails>
{
public:
    XblMultiplayerSearchHandleDetails() = default;

    static xbox::services::Result<std::shared_ptr<XblMultiplayerSearchHandleDetails>> Deserialize(_In_ const JsonValue& json);

    const XblMultiplayerSessionReference& SessionReference() const;
    const xsapi_internal_string& HandleId() const;
    const xsapi_internal_vector<uint64_t>& SessionOwnerXuids() const;
    const xsapi_internal_vector<XblMultiplayerSessionTag>& Tags() const;
    const xsapi_internal_vector<XblMultiplayerSessionNumberAttribute>& NumberAttributes() const;
    const xsapi_internal_vector<XblMultiplayerSessionStringAttribute>& StringAttributes() const;
    const xbox::services::multiplayer::RoleTypes& RoleTypes() const;
    XblMultiplayerSessionVisibility Visibility() const;
    XblMultiplayerSessionRestriction JoinRestriction() const;
    bool Closed() const;
    size_t MaxMembersCount() const;
    size_t MembersCount() const;
    const xsapi_internal_string& CustomSessionPropertiesJson() const;
    const xbox::services::datetime& HandleCreationTime() const;

private:
    XblMultiplayerSearchHandleDetails(const XblMultiplayerSearchHandleDetails&) = delete;
    XblMultiplayerSearchHandleDetails& operator=(XblMultiplayerSearchHandleDetails) = delete;

    // RefCounter
    std::shared_ptr<RefCounter> GetSharedThis() override;

    XblMultiplayerSessionReference m_sessionReference{};
    xsapi_internal_string m_handleId;
    xsapi_internal_vector<XblMultiplayerSessionTag> m_tags;
    xsapi_internal_vector<uint64_t> m_sessionOwners;
    bool m_closed{ false };
    xsapi_internal_vector<XblMultiplayerSessionStringAttribute> m_stringAttributes;
    xsapi_internal_vector<XblMultiplayerSessionNumberAttribute> m_numberAttributes;
    xbox::services::multiplayer::RoleTypes m_roleTypes;
    XblMultiplayerSessionVisibility m_visibility{ XblMultiplayerSessionVisibility::Unknown };
    XblMultiplayerSessionRestriction m_joinRestriction{ XblMultiplayerSessionRestriction::Unknown };
    size_t m_maxMembersCount{ 0 };
    size_t m_membersCount{ 0 };
    xbox::services::datetime m_handleCreationTime;
    xsapi_internal_string m_customSessionPropertiesJson;
};
