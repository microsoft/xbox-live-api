// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"
#include "xsapi-cpp/multiplayer.h"
#include "xsapi-cpp/xbox_live_context.h"
#include "xsapi_utils.h"

#pragma warning (disable : 26444)

using xbox::services::multiplayer::Serializers;

static xsapi_internal_string StringFromUint64Internal(_In_ uint64_t val)
{
    xsapi_internal_stringstream ss;
    ss << val;
    return ss.str();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

#define MPSD_URI "https://sessiondirectory.xboxlive.com"
#define MPSD_RTA_URI MPSD_URI "/connections/"

// JSON Verification helpers for fields that may not be present (and are not required to be)
#define VERIFY_JSON_UINT(json, fieldName, actualValue) \
    { \
        uint64_t expectedValue{}; \
        VERIFY_SUCCEEDED(JsonUtils::ExtractJsonInt(json, fieldName, expectedValue)); \
        VERIFY_ARE_EQUAL_UINT(expectedValue, static_cast<uint64_t>(actualValue)); \
    }

#define VERIFY_JSON_INT(json, fieldName, actualValue) \
    { \
        int64_t expectedValue{}; \
        VERIFY_SUCCEEDED(JsonUtils::ExtractJsonInt(json, fieldName, expectedValue)); \
        VERIFY_ARE_EQUAL_UINT(expectedValue, static_cast<int64_t>(actualValue)); \
    }

#define VERIFY_JSON_STRING(json, fieldName, actualValue) \
    { \
        String expectedValue{}; \
        VERIFY_SUCCEEDED(JsonUtils::ExtractJsonString(json, fieldName, expectedValue)); \
        if (!expectedValue.empty()) { \
            VERIFY_ARE_EQUAL_STR_IGNORE_CASE(expectedValue.data(), actualValue); \
        } \
    }

#define VERIFY_JSON_BOOL(json, fieldName, actualValue) \
    { \
        bool expectedValue{}; \
        VERIFY_SUCCEEDED(JsonUtils::ExtractJsonBool(json, fieldName, expectedValue)); \
        VERIFY_ARE_EQUAL(expectedValue, actualValue); \
    }

#define VERIFY_JSON_TIME(json, fieldName, actualValue) \
    { \
        if (json.HasMember(fieldName)) { \
            VERIFY_IS_TRUE(VerifyTime(actualValue, json[fieldName].GetString())); \
        } else { \
            VERIFY_ARE_EQUAL_UINT(0, actualValue); \
        } \
    }

#define VERIFY_JSON_FIELD(json, fieldName, jsonString) \
    { \
        if (json.HasMember(fieldName)) { \
            VERIFY_IS_TRUE(VerifyJson(json[fieldName], jsonString)); \
        } else { \
            VERIFY_IS_NULL(jsonString); \
        } \
    }

#define VERIFY_JSON_FIELD_NOTNULL(json, fieldName, jsonString) \
    { \
        if (json.HasMember(fieldName)) { \
            VERIFY_IS_TRUE(VerifyJson(json[fieldName], jsonString)); \
        } else { \
            VERIFY_IS_NOT_NULL(jsonString); \
            VERIFY_IS_TRUE(jsonString[0] == 0); \
        } \
    }

#define VERIFY_JSON_INT_STRING(json, fieldName, actualIntValue) \
    { \
        uint64_t expectedValue{}; \
        VERIFY_SUCCEEDED(JsonUtils::ExtractJsonStringToUInt64(json, fieldName, expectedValue)); \
        VERIFY_ARE_EQUAL_UINT(expectedValue, actualIntValue); \
    }

DEFINE_TEST_CLASS(MultiplayerTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(MultiplayerTests)

    static const JsonDocument testJson;
    static const JsonValue& defaultSessionJson;
    static const XblMultiplayerSessionReference defaultSessionReference;

    class MPTestEnv : public TestEnvironment
    {
    public:
        MPTestEnv() noexcept 
        {
            m_baseMock = std::make_shared<HttpMock>("", "");
            m_baseMock->SetMockMatchedCallback(
                [](HttpMock* /*mock*/, xsapi_internal_string uri, xsapi_internal_string /*body*/)
                {
                    LOGS_DEBUG << "Unmocked HttpCall, uri=" << uri;
                    assert(false);
                }
            );

            // MPSD response expect local user's Xuid to be 1234
            m_xboxLiveContext = CreateMockXboxLiveContext(1234);
        }

        XblContextHandle XboxLiveContext() const noexcept
        {
            return m_xboxLiveContext.get();
        }

    private:
        std::shared_ptr<HttpMock> m_baseMock;
        std::shared_ptr<XblContext> m_xboxLiveContext;
    };

    // RAII wrapper for MPSD session
    class MultiplayerSession
    {
    public:
        // No public constructor. Create a session using MultiplayerSession::Create or MultiplayerSession::Get

        MultiplayerSession(const MultiplayerSession& other) noexcept
        {
            VERIFY_SUCCEEDED(XblMultiplayerSessionDuplicateHandle(other.m_handle, &m_handle));
            VERIFY_SUCCEEDED(XblContextDuplicateHandle(other.m_context, &m_context));
        }

        MultiplayerSession& operator=(MultiplayerSession other) noexcept
        {
            std::swap(m_handle, other.m_handle);
            std::swap(m_context, other.m_context);
            return *this;
        }

        ~MultiplayerSession() noexcept
        {
            XblMultiplayerSessionCloseHandle(m_handle);
            XblContextCloseHandle(m_context);
        }

        static MultiplayerSession Create(
            XblContextHandle xboxLiveContext,
            const XblMultiplayerSessionReference* sessionReference = &MultiplayerTests::defaultSessionReference,
            const XblMultiplayerSessionInitArgs* initArgs = nullptr
        ) noexcept
        {
            auto sessionHandle = XblMultiplayerSessionCreateHandle(xboxLiveContext->Xuid(), sessionReference, initArgs);
            return MultiplayerSession{ sessionHandle, xboxLiveContext };
        }

        static MultiplayerSession Get(
            XblContextHandle xboxLiveContext,
            const XblMultiplayerSessionReference& sessionReference = MultiplayerTests::defaultSessionReference,
            const JsonValue& response = MultiplayerTests::defaultSessionJson
        ) noexcept
        {
            Stringstream mockUri;
            mockUri << MPSD_URI;
            mockUri << "/serviceconfigs/" << sessionReference.Scid;
            mockUri << "/sessionTemplates/" << sessionReference.SessionTemplateName;
            mockUri << "/sessions/" << sessionReference.SessionName;

            HttpMock mock{ "GET", mockUri.str() };
            mock.SetResponseBody(response);

            XAsyncBlock async{};
            VERIFY_SUCCEEDED(XblMultiplayerGetSessionAsync(xboxLiveContext, &sessionReference, &async));
            VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

            XblMultiplayerSessionHandle sessionHandle{ nullptr };
            VERIFY_SUCCEEDED(XblMultiplayerGetSessionResult(&async, &sessionHandle));

            MultiplayerSession session{ sessionHandle, xboxLiveContext };
            session.Verify(response);

            return session;
        }

        static MultiplayerSession Get(
            XblContextHandle xboxLiveContext,
            const xsapi_internal_string& handleId,
            const JsonValue& response = MultiplayerTests::defaultSessionJson
        ) noexcept
        {
            Stringstream mockUri;
            mockUri << MPSD_URI << "/handles/" << handleId.data() << "/session";

            HttpMock mock{ "GET", mockUri.str() };
            mock.SetResponseBody(response);

            XblMultiplayerSessionReferenceUri sessionPath{};
            VERIFY_SUCCEEDED(XblMultiplayerSessionReferenceToUriPath(&MultiplayerTests::defaultSessionReference, &sessionPath));
            mock.SetResponseHeaders(HttpHeaders{ {"Content-Location", sessionPath.value } });

            XAsyncBlock async{};
            VERIFY_SUCCEEDED(XblMultiplayerGetSessionByHandleAsync(xboxLiveContext, handleId.data(), &async));
            VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

            XblMultiplayerSessionHandle sessionHandle{ nullptr };
            VERIFY_SUCCEEDED(XblMultiplayerGetSessionByHandleResult(&async, &sessionHandle));

            MultiplayerSession session{ sessionHandle, xboxLiveContext };
            session.Verify(response);

            return session;
        }

        void Write(
            const JsonValue& expectedRequestBody = JsonValue{ rapidjson::kObjectType },
            const JsonValue& responseBody = MultiplayerTests::defaultSessionJson
        ) noexcept
        {
            auto sessionReference{ Reference() };

            Stringstream mockUri;
            mockUri << MPSD_URI;
            mockUri << "/serviceconfigs/" << sessionReference->Scid;
            mockUri << "/sessionTemplates/" << sessionReference->SessionTemplateName;
            mockUri << "/sessions/" << sessionReference->SessionName;

            auto mock = std::make_shared<HttpMock>( "PUT", mockUri.str() );
            mock->SetResponseBody(responseBody);

            bool requestWellFormed{ true };
            mock->SetMockMatchedCallback(
                [&](HttpMock* /*mock*/, xsapi_internal_string /*uri*/, xsapi_internal_string body)
                {
                    requestWellFormed &= VerifyJson(expectedRequestBody, body.data());
                }
            );

            XAsyncBlock async{};
            VERIFY_SUCCEEDED(XblMultiplayerWriteSessionAsync(
                m_context,
                m_handle,
                XblMultiplayerSessionWriteMode::UpdateOrCreateNew,
                &async)
            );

            VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
            VERIFY_IS_TRUE(requestWellFormed);

            XblMultiplayerSessionHandle updatedSessionHandle{ nullptr };
            VERIFY_SUCCEEDED(XblMultiplayerWriteSessionResult(&async, &updatedSessionHandle));

            MultiplayerSession updatedSession{ updatedSessionHandle, m_context };
            updatedSession.Verify(responseBody);

            // Update MultiplayerSession object in place since we are doing synchronous updates anyhow
            *this = updatedSession;
        }

        XblMultiplayerSessionHandle Handle() const noexcept
        {
            return m_handle;
        }

        const XblMultiplayerSessionReference* Reference() const noexcept
        {
            return XblMultiplayerSessionSessionReference(m_handle);
        }

        const XblMultiplayerSessionConstants* Constants() const noexcept
        {
            return XblMultiplayerSessionSessionConstants(m_handle);
        }

        const XblMultiplayerSessionProperties* Properties() const noexcept
        {
            return XblMultiplayerSessionSessionProperties(m_handle);
        }

        std::unordered_map<uint32_t, const XblMultiplayerSessionMember*> Members() const noexcept
        {
            const XblMultiplayerSessionMember* members{ nullptr };
            size_t membersCount{ 0 };
            VERIFY_SUCCEEDED(XblMultiplayerSessionMembers(m_handle, &members, &membersCount));

            std::unordered_map<uint32_t, const XblMultiplayerSessionMember*> map{};
            for (size_t i = 0; i < membersCount; ++i)
            {
                map[members[i].MemberId] = members + i;
            }
            return map;
        }

        std::vector<const XblMultiplayerRoleType*> RoleTypes() const noexcept
        {
            const XblMultiplayerRoleType* roles{ nullptr };
            size_t roleCount{ 0 };
            VERIFY_SUCCEEDED(XblMultiplayerSessionRoleTypes(m_handle, &roles, &roleCount));

            return Utils::Transform<const XblMultiplayerRoleType*>(roles, roleCount, [](const XblMultiplayerRoleType& r) { return &r; });
        }

    private:
        // Semantics are to take over ownership of an existing session handle. Caller should not call close.
        MultiplayerSession(
            XblMultiplayerSessionHandle sessionHandle,
            XblContextHandle xblContextHandle
        ) noexcept
            : m_handle(sessionHandle)
        {
            VERIFY_IS_NOT_NULL(m_handle);
            VERIFY_SUCCEEDED(XblContextDuplicateHandle(xblContextHandle, &m_context));
        }

        // Verify the session matches with an MPSD session document
        void Verify(const JsonValue& json) const noexcept
        {
            VerifySessionInfo(json);
            if (json.HasMember("initializing"))
            {
                VerifyInitializationInfo(json["initializing"]);
            }
            if (json.HasMember("hostCandidates"))
            {
                VerifyHostCandidates(json["hostCandidates"]);
            }
            if (json.HasMember("roleTypes"))
            {
                VerifyRoles(json["roleTypes"]);
            }
            else
            {
                VERIFY_ARE_EQUAL_UINT(0, RoleTypes().size());
            }
            VerifyConstants(json["constants"]);
            VerifyProperties(json["properties"]);
            VerifyServers(json["servers"]);
            VerifyMembers(json["members"]);
        }

        void VerifySessionInfo(const JsonValue& json) const noexcept
        {
            const auto& i{ *XblMultiplayerSessionGetInfo(m_handle) };

            VERIFY_JSON_INT(json, "contractVersion", i.ContractVersion);
            VERIFY_JSON_STRING(json, "branch", i.Branch);
            VERIFY_JSON_INT(json, "changeNumber", i.ChangeNumber);
            VERIFY_JSON_STRING(json, "correlationId", i.CorrelationId);
            VERIFY_JSON_TIME(json, "startTime", i.StartTime);
            VERIFY_JSON_TIME(json, "nextTimer", i.NextTimer);
        }

        void VerifyInitializationInfo(const JsonValue& json) const noexcept
        {
            const auto& i{ *XblMultiplayerSessionGetInitializationInfo(m_handle) };
            switch (i.Stage)
            {
            case XblMultiplayerInitializationStage::Evaluating:
            {
                VERIFY_ARE_EQUAL_STR_IGNORE_CASE(json["stage"].GetString(), "evaluating");
                break;
            }
            case XblMultiplayerInitializationStage::Failed:
            {
                VERIFY_ARE_EQUAL_STR_IGNORE_CASE(json["stage"].GetString(), "failed");
                break;
            }
            case XblMultiplayerInitializationStage::Joining:
            {
                VERIFY_ARE_EQUAL_STR_IGNORE_CASE(json["stage"].GetString(), "joining");
                break;
            }
            case XblMultiplayerInitializationStage::Measuring:
            {
                VERIFY_ARE_EQUAL_STR_IGNORE_CASE(json["stage"].GetString(), "measuring");
                break;
            }
            case XblMultiplayerInitializationStage::None:
            case XblMultiplayerInitializationStage::Unknown:
            default:
            {
                VERIFY_IS_TRUE(!json.HasMember("stage"));
            }
            }

            VERIFY_JSON_TIME(json, "stageStartTime", i.StageStartTime);
            VERIFY_JSON_UINT(json, "episode", i.Episode);
        }

        void VerifyHostCandidates(const JsonValue& json) const noexcept
        {
            const XblDeviceToken* hostCandidates{ nullptr };
            size_t count{ 0 };
            VERIFY_SUCCEEDED(XblMultiplayerSessionHostCandidates(m_handle, &hostCandidates, &count));
            const auto& expectedHostCandidates{ json.GetArray() };
            VERIFY_ARE_EQUAL_UINT(expectedHostCandidates.Size(), count);

            for (uint32_t i = 0; i < count; ++i)
            {
                VERIFY_ARE_EQUAL_STR_IGNORE_CASE(expectedHostCandidates[i].GetString(), hostCandidates[i].Value);
            }
        }

        void VerifyRoles(const JsonValue& json) const noexcept
        {
            auto rolesTypes{ RoleTypes() };

            VERIFY_ARE_EQUAL_UINT(json.MemberCount(), rolesTypes.size());
            for (auto roleType : rolesTypes)
            {
                VERIFY_IS_NOT_NULL(roleType);

                VERIFY_IS_TRUE(json.HasMember(roleType->Name));
                const auto& roleTypeJson{ json[roleType->Name] };
                VERIFY_ARE_EQUAL(roleType->OwnerManaged, roleTypeJson["ownerManaged"].GetBool());

                XblMutableRoleSettings expectedMutableSettings{ XblMutableRoleSettings::None };
                const auto& mutableSettingsArray{ roleTypeJson["mutableRoleSettings"].GetArray() };
                for (auto& setting : mutableSettingsArray)
                {
                    xsapi_internal_string settingString{ setting.GetString() };
                    if (settingString == "max")
                    {
                        expectedMutableSettings |= XblMutableRoleSettings::Max;
                    }
                    else if (settingString == "target")
                    {
                        expectedMutableSettings |= XblMutableRoleSettings::Target;
                    }
                }

                VERIFY_IS_TRUE(roleType->MutableRoleSettings == expectedMutableSettings);

                const auto& rolesJson{ roleTypeJson["roles"] };
                VERIFY_ARE_EQUAL_UINT(rolesJson.MemberCount(), roleType->RoleCount);
                for (size_t i = 0; i < roleType->RoleCount; ++i)
                {
                    auto& role{ roleType->Roles[i] };
                    VERIFY_IS_TRUE(rolesJson.HasMember(role.Name));
                    const auto& roleJson{ rolesJson[role.Name] };

                    VERIFY_ARE_EQUAL(roleType, role.RoleType);

                    std::unordered_set<uint64_t> actualMemberXuids(role.MemberXuids, role.MemberXuids + role.MemberCount);
                    const auto& expectedMemberXuids{ roleJson["memberXuids"].GetArray() };
                    VERIFY_ARE_EQUAL_UINT(expectedMemberXuids.Size(), actualMemberXuids.size());
                    for (auto& xuid : expectedMemberXuids)
                    {
                        VERIFY_IS_TRUE(actualMemberXuids.find(atol(xuid.GetString())) != actualMemberXuids.end());
                    }

                    VERIFY_ARE_EQUAL_UINT(roleJson["target"].GetUint(), role.TargetCount);
                    VERIFY_ARE_EQUAL_UINT(roleJson["max"].GetUint(), role.MaxMemberCount);
                }
            }
        }

        void VerifyConstants(const JsonValue& json) const noexcept
        {
            auto& c{ *Constants() };

            const auto& systemJson = json["system"];

            VERIFY_JSON_UINT(systemJson, "maxMembersCount", c.MaxMembersInSession);
            if (c.Visibility != XblMultiplayerSessionVisibility::Unknown)
            {
                VERIFY_IS_TRUE(multiplayer::Serializers::MultiplayerSessionVisibilityFromString(systemJson["visibility"].GetString()) == c.Visibility);
            }

            if (c.InitiatorXuidsCount)
            {
                std::unordered_set<uint64_t> actualInitiators{ c.InitiatorXuids, c.InitiatorXuids + c.InitiatorXuidsCount };
                const auto& expectedInitiators{ systemJson["initiators"].GetArray() };
                VERIFY_ARE_EQUAL_UINT(expectedInitiators.Size(), actualInitiators.size());
                for (auto& xuid : expectedInitiators)
                {
                    VERIFY_IS_TRUE(actualInitiators.find(atol(xuid.GetString())) != actualInitiators.end());
                }
            }

            VERIFY_JSON_FIELD_NOTNULL(systemJson, "cloudComputePackage", c.SessionCloudComputePackageConstantsJson);
            VERIFY_JSON_UINT(systemJson, "reservedRemovalTimeout", c.MemberReservedTimeout);
            VERIFY_JSON_UINT(systemJson, "inactiveRemovalTimeout", c.MemberInactiveTimeout);
            VERIFY_JSON_UINT(systemJson, "readyRemovalTimeout", c.MemberReadyTimeout);
            VERIFY_JSON_UINT(systemJson, "sessionEmptyTimeout", c.SessionEmptyTimeout);

            if (systemJson.HasMember("metrics"))
            {
                VERIFY_JSON_BOOL(systemJson["metrics"], "latency", c.EnableMetricsLatency);
                VERIFY_JSON_BOOL(systemJson["metrics"], "bandwidthDown", c.EnableMetricsBandwidthDown);
                VERIFY_JSON_BOOL(systemJson["metrics"], "bandwidthUp", c.EnableMetricsBandwidthUp);
                VERIFY_JSON_BOOL(systemJson["metrics"], "custom", c.EnableMetricsCustom);
            }

            if (systemJson.HasMember("memberInitialization"))
            {
                VERIFY_JSON_UINT(systemJson["memberInitialization"], "joinTimeout", c.MemberInitialization->JoinTimeout);
                VERIFY_JSON_UINT(systemJson["memberInitialization"], "measurementTimeout", c.MemberInitialization->MeasurementTimeout);
                VERIFY_JSON_UINT(systemJson["memberInitialization"], "evaluationTimeout", c.MemberInitialization->EvaluationTimeout);
                VERIFY_JSON_BOOL(systemJson["memberInitialization"], "externalEvaluation", c.MemberInitialization->ExternalEvaluation);
                VERIFY_JSON_UINT(systemJson["memberInitialization"], "membersNeededToStart", c.MemberInitialization->MembersNeededToStart);
            }

            if (systemJson.HasMember("peerToPeerRequirements"))
            {
                VERIFY_JSON_UINT(systemJson["peerToPeerRequirements"], "latencyMaximum", c.PeerToPeerRequirements.LatencyMaximum);
                VERIFY_JSON_UINT(systemJson["peerToPeerRequirements"], "bandwidthMinimum", c.PeerToPeerRequirements.BandwidthMinimumInKbps);
            }

            if (systemJson.HasMember("peerToHostRequirements"))
            {
                VERIFY_JSON_UINT(systemJson["peerToHostRequirements"], "latencyMaximum", c.PeerToHostRequirements.LatencyMaximum);
                VERIFY_JSON_UINT(systemJson["peerToHostRequirements"], "bandwidthDownMinimum", c.PeerToHostRequirements.BandwidthDownMinimumInKbps);
                VERIFY_JSON_UINT(systemJson["peerToHostRequirements"], "bandwidthUpMinimum", c.PeerToHostRequirements.BandwidthUpMinimumInKbps);
            }

            VERIFY_JSON_FIELD_NOTNULL(systemJson, "measurementServerAddresses", c.MeasurementServerAddressesJson);

            const auto& capabilitesJson = systemJson["capabilities"];
            VERIFY_JSON_BOOL(capabilitesJson, "connectivity", c.SessionCapabilities.Connectivity);
            VERIFY_JSON_BOOL(capabilitesJson, "suppressPresenceActivityCheck", c.SessionCapabilities.SuppressPresenceActivityCheck);
            VERIFY_JSON_BOOL(capabilitesJson, "gameplay", c.SessionCapabilities.Gameplay);
            VERIFY_JSON_BOOL(capabilitesJson, "large", c.SessionCapabilities.Large);
            VERIFY_JSON_BOOL(capabilitesJson, "connectionRequiredForActiveMembers", c.SessionCapabilities.ConnectionRequiredForActiveMembers);
            VERIFY_JSON_BOOL(capabilitesJson, "userAuthorizationStyle", c.SessionCapabilities.UserAuthorizationStyle);
            VERIFY_JSON_BOOL(capabilitesJson, "crossPlay", c.SessionCapabilities.Crossplay);
            VERIFY_JSON_BOOL(capabilitesJson, "searchable", c.SessionCapabilities.Searchable);
            VERIFY_JSON_BOOL(capabilitesJson, "hasOwners", c.SessionCapabilities.HasOwners);

            VERIFY_JSON_FIELD_NOTNULL(json, "custom", c.CustomJson);
        }

        void VerifyProperties(const JsonValue& json) const noexcept
        {
            auto& p{ *Properties() };
            auto& systemJson{ json["system"] };

            if (p.KeywordCount)
            {
                std::unordered_set<xsapi_internal_string> actualKeywords(p.Keywords, p.Keywords + p.KeywordCount);
                const auto& expectedKeywords = systemJson["keywords"].GetArray();
                VERIFY_ARE_EQUAL_UINT(expectedKeywords.Size(), actualKeywords.size());
                for (auto& keyword : expectedKeywords)
                {
                    VERIFY_IS_TRUE(actualKeywords.find(keyword.GetString()) != actualKeywords.end());
                }
            }

            if (p.JoinRestriction != XblMultiplayerSessionRestriction::Unknown)
            {
                VERIFY_IS_TRUE(p.JoinRestriction == multiplayer::Serializers::MultiplayerSessionRestrictionFromString(systemJson["joinRestriction"].GetString()));
            }

            if (p.ReadRestriction != XblMultiplayerSessionRestriction::Unknown)
            {
                VERIFY_IS_TRUE(p.ReadRestriction == multiplayer::Serializers::MultiplayerSessionRestrictionFromString(systemJson["readRestriction"].GetString()));
            }

            if (p.TurnCollectionCount)
            {
                std::unordered_set<uint32_t> actualTurns(p.TurnCollection, p.TurnCollection + p.TurnCollectionCount);
                const auto& expectedTurns{ systemJson["turn"].GetArray() };
                VERIFY_ARE_EQUAL_UINT(expectedTurns.Size(), actualTurns.size());
                for (auto& turn : expectedTurns)
                {
                    VERIFY_IS_TRUE(actualTurns.find(turn.GetUint()) != actualTurns.end());
                }
            }

            if (systemJson.HasMember("matchmaking"))
            {
                VERIFY_JSON_FIELD(systemJson["matchmaking"], "targetSessionConstants", p.MatchmakingTargetSessionConstantsJson);
                VERIFY_JSON_STRING(systemJson["matchmaking"], "serverConnectionString", p.MatchmakingServerConnectionString);
            }

            if (p.ServerConnectionStringCandidatesCount)
            {
                std::unordered_set<xsapi_internal_string> actualServerCandidates(p.ServerConnectionStringCandidates, p.ServerConnectionStringCandidates + p.ServerConnectionStringCandidatesCount);
                const auto& expectedServerCandidates{ systemJson["serverConnectionStringCandidates"].GetArray() };
                VERIFY_ARE_EQUAL_UINT(expectedServerCandidates.Size(), actualServerCandidates.size());
                for (auto& candidate : expectedServerCandidates)
                {
                    VERIFY_IS_TRUE(actualServerCandidates.find(candidate.GetString()) != actualServerCandidates.end());
                }
            }

            if (p.SessionOwnerMemberIdsCount)
            {
                std::unordered_set<uint32_t> actualSessionOwners(p.SessionOwnerMemberIds, p.SessionOwnerMemberIds + p.SessionOwnerMemberIdsCount);
                const auto& expectedSessionOwners{ systemJson["owners"].GetArray() };
                VERIFY_ARE_EQUAL_UINT(expectedSessionOwners.Size(), actualSessionOwners.size());
                for (auto& ownerId : expectedSessionOwners)
                {
                    VERIFY_IS_TRUE(actualSessionOwners.find(ownerId.GetUint()) != actualSessionOwners.end());
                }
            }

            VERIFY_JSON_STRING(systemJson, "host", p.HostDeviceToken.Value);
            VERIFY_JSON_BOOL(systemJson, "closed", p.Closed);
            VERIFY_JSON_BOOL(systemJson, "locked", p.Locked);
            VERIFY_JSON_BOOL(systemJson, "allocateCloudCompute", p.AllocateCloudCompute);
            VERIFY_JSON_BOOL(systemJson, "matchmakingResubmit", p.MatchmakingResubmit);

            VERIFY_JSON_FIELD_NOTNULL(json, "custom", p.SessionCustomPropertiesJson);
        }

        void VerifyServers(const JsonValue& json) const noexcept
        {
            auto jsonRawServers = XblMultiplayerSessionRawServersJson(m_handle);
            VERIFY_IS_TRUE(VerifyJson(json, jsonRawServers));

            // matchmaking server
            if (json.HasMember("matchmaking"))
            {
                const auto& matchmakingJson{ json["matchmaking"]["properties"]["system"] };
                auto m{ XblMultiplayerSessionMatchmakingServer(m_handle) };

                switch (m->Status)
                {
                case XblMatchmakingStatus::Searching:
                {
                    VERIFY_ARE_EQUAL_STR_IGNORE_CASE("searching", matchmakingJson["status"].GetString());
                    break;
                }
                default: break;
                }

                VERIFY_JSON_STRING(matchmakingJson, "statusDetails", m->StatusDetails);
                VERIFY_JSON_INT(matchmakingJson, "typicalWait", m->TypicalWaitInSeconds);
                VerifyMultiplayerSessionReference(m->TargetSessionRef, matchmakingJson["targetSessionRef"]);
            }
        }

        void VerifyMembers(const JsonValue& json) const noexcept
        {
            auto members{ Members() };
            VERIFY_ARE_EQUAL_UINT(json.MemberCount(), members.size());

            for (auto iter = json.MemberBegin(); iter != json.MemberEnd(); ++iter)
            {
                if (xsapi_internal_string{ iter->name.GetString() } == "me")
                {
                    VERIFY_ARE_EQUAL_UINT(1u, members.size());
                    VerifyMember(*members.begin()->second, iter->value);
                }
                else
                {
                    VerifyMember(*members[atoi(iter->name.GetString())], iter->value);
                }
            }
        }

        void VerifyMember(
            const XblMultiplayerSessionMember& m,
            const JsonValue& expected
        ) const noexcept
        {
            // Member constants
            auto& constantsSystemJson{ expected["constants"]["system"] };

            if (constantsSystemJson.HasMember("xuid"))
            {
                VERIFY_ARE_EQUAL_UINT(m.Xuid, atol(constantsSystemJson["xuid"].GetString()));
            }

            auto currentUser{ XblMultiplayerSessionCurrentUser(m_handle) };
            bool isCurrentUser{ currentUser ? m.Xuid == currentUser->Xuid : false };
            VERIFY_IS_TRUE(m.IsCurrentUser == isCurrentUser);

            VERIFY_JSON_BOOL(constantsSystemJson, "initialize", m.InitializeRequested);
            if (constantsSystemJson.HasMember("matchmakingResult"))
            {
                VERIFY_IS_TRUE(VerifyJson(constantsSystemJson["matchmakingResult"]["serverMeasurements"], m.MatchmakingResultServerMeasurementsJson));
            }

            if (expected["constants"].HasMember("custom"))
            {
                VERIFY_IS_TRUE(VerifyJson(expected["constants"]["custom"], m.CustomConstantsJson));
            }

            // Member properties
            auto& propertiesSystemJson{ expected["properties"]["system"] };

            // Verify subscribed change types only for localUser
            if (isCurrentUser && propertiesSystemJson.HasMember("subscription"))
            {
                Vector<String> expectedChangedTypesVector{};
                VERIFY_SUCCEEDED(JsonUtils::ExtractJsonVector<String>(JsonUtils::JsonStringExtractor, propertiesSystemJson["subscription"]["changeTypes"], expectedChangedTypesVector));
                auto expectedChangedTypes = multiplayer::Serializers::MultiplayerSessionChangeTypesFromStringVector(expectedChangedTypesVector);
                VERIFY_IS_TRUE(expectedChangedTypes == XblMultiplayerSessionSubscribedChangeTypes(m_handle));
            }

            switch (m.Status)
            {
            case XblMultiplayerSessionMemberStatus::Active:
            {
                VERIFY_IS_TRUE(propertiesSystemJson["active"].GetBool());
                break;
            }
            case XblMultiplayerSessionMemberStatus::Ready:
            {
                VERIFY_IS_TRUE(propertiesSystemJson["ready"].GetBool());
                break;
            }
            case XblMultiplayerSessionMemberStatus::Reserved:
            {
                VERIFY_IS_TRUE(expected["reserved"].GetBool());
                break;
            }
            case XblMultiplayerSessionMemberStatus::Inactive:
            {
                VERIFY_IS_TRUE(!expected.HasMember("reserved") || !expected["reserved"].GetBool());
                VERIFY_IS_TRUE(!propertiesSystemJson.HasMember("active") || !propertiesSystemJson["active"].GetBool());
                VERIFY_IS_TRUE(!propertiesSystemJson.HasMember("ready") || !propertiesSystemJson["ready"].GetBool());
                break;
            }
            default:
            {
                assert(false);
            }
            }

            VERIFY_JSON_STRING(propertiesSystemJson, "secureDeviceAddress", m.SecureDeviceBaseAddress64);

            if (propertiesSystemJson.HasMember("initializationGroup"))
            {
                std::unordered_set<uint32_t> actualInitializationGroup(m.MembersInGroupIds, m.MembersInGroupIds + m.MembersInGroupCount);
                const auto& expectedInitializationGroup{ propertiesSystemJson["initializationGroup"].GetArray() };
                VERIFY_ARE_EQUAL_UINT(expectedInitializationGroup.Size(), actualInitializationGroup.size());
                for (auto& id : expectedInitializationGroup)
                {
                    VERIFY_IS_TRUE(actualInitializationGroup.find(id.GetUint()) != actualInitializationGroup.end());
                }
            }

            VERIFY_JSON_FIELD(propertiesSystemJson, "serverMeasurements", m.ServerMeasurementsJson);
            VERIFY_JSON_FIELD(propertiesSystemJson, "measurements", m.QosMeasurementsJson);

            VERIFY_JSON_FIELD(expected["properties"], "custom", m.CustomPropertiesJson);

            // Root
            VERIFY_JSON_STRING(expected, "gamertag", m.Gamertag);
            VERIFY_JSON_STRING(expected, "deviceToken", m.DeviceToken.Value);
            if (expected.HasMember("nat"))
            {
                VERIFY_IS_TRUE(multiplayer::Serializers::MultiplayerNatSettingFromString(expected["nat"].GetString()) == m.Nat);
            }
            VERIFY_JSON_INT_STRING(expected, "activeTitleId", m.ActiveTitleId);
            VERIFY_JSON_TIME(expected, "joinTime", m.JoinTime);
            VERIFY_JSON_BOOL(expected, "turn", m.IsTurnAvailable);
            if (expected.HasMember("initializationFailure"))
            {
                VERIFY_IS_TRUE(multiplayer::Serializers::MultiplayerMeasurementFailureFromString(expected["initializationFailure"].GetString()) == m.InitializationFailureCause);
            }
            VERIFY_JSON_UINT(expected, "initializationEpisode", m.InitializationEpisode);

            // Roles
            if (m.Roles)
            {
                const auto& expectedRoles{ expected["roles"] };
                VERIFY_ARE_EQUAL_UINT(expectedRoles.MemberCount(), m.RolesCount);
                for (size_t i = 0; i < m.RolesCount; ++i)
                {
                    VERIFY_IS_TRUE(expectedRoles.HasMember(m.Roles[i].roleTypeName));
                    VERIFY_ARE_EQUAL_STR(expectedRoles[m.Roles[i].roleTypeName].GetString(), m.Roles[i].roleName);
                }
            }
        }

        XblMultiplayerSessionHandle m_handle{ nullptr };
        XblContextHandle m_context{ nullptr };
    };

    // RAII wrapper around XblMultiplayerSearchHandle
    class MultiplayerSearchDetails
    {
    public:
        // No public constructor. Create MultiplayerSearchDetails for a session using MultiplayerSearchDetails::Create,
        // or query existing using MultiplayerSearchDetails::Query

        MultiplayerSearchDetails(const MultiplayerSearchDetails& other) noexcept
        {
            VERIFY_SUCCEEDED(XblMultiplayerSearchHandleDuplicateHandle(other.m_handle, &m_handle));
        }
        MultiplayerSearchDetails& operator=(MultiplayerSearchDetails rhs) = delete;
        ~MultiplayerSearchDetails() noexcept
        {
            XblMultiplayerSearchHandleCloseHandle(m_handle);
        }

        static MultiplayerSearchDetails Create(
            XblContextHandle xboxLiveContext,
            const XblMultiplayerSessionReference& sessionRef,
            const std::vector<XblMultiplayerSessionTag>& tags = {},
            const std::vector<XblMultiplayerSessionNumberAttribute>& numberAttributes = {},
            const std::vector<XblMultiplayerSessionStringAttribute>& stringAttributes = {},
            const JsonValue& responseJson = testJson["searchHandleJson"]
        ) noexcept
        {
            auto mock = std::make_shared<HttpMock>( "POST", MPSD_URI "/handles" );
            mock->SetResponseBody(responseJson);

            bool requestWellFormed{ true };
            mock->SetMockMatchedCallback(
                [&](HttpMock* mock, xsapi_internal_string uri, xsapi_internal_string requestBody)
                {
                    UNREFERENCED_PARAMETER(mock);
                    UNREFERENCED_PARAMETER(uri);
                    requestWellFormed = VerifyJson(responseJson, requestBody.data());
                });

            XAsyncBlock async{};
            VERIFY_SUCCEEDED(XblMultiplayerCreateSearchHandleAsync(
                xboxLiveContext,
                &sessionRef,
                tags.data(),
                tags.size(),
                numberAttributes.data(),
                numberAttributes.size(),
                stringAttributes.data(),
                stringAttributes.size(),
                &async
            ));

            VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

            XblMultiplayerSearchHandle handle{ nullptr };
            VERIFY_SUCCEEDED(XblMultiplayerCreateSearchHandleResult(&async, &handle));

            MultiplayerSearchDetails result{ handle };
            result.Verify(responseJson);

            return result;
        }

        static std::vector<MultiplayerSearchDetails> Query(
            XblContextHandle xboxLiveContext,
            const char* scid,
            const char* sessionTemplateName,
            const char* orderByAttribute = nullptr,
            bool orderAscending = true,
            const char* searchFilter = nullptr,
            const char* socialGroup = nullptr,
            const JsonValue& expectedRequestBody = testJson["searchHandlesRequestJson"]
        ) noexcept
        {
            const auto& responseBody{ testJson["searchHandlesResponseJson"] };

            auto mock = std::make_shared<HttpMock>( "POST", MPSD_URI "/handles/query" );
            mock->SetResponseBody(responseBody);

            bool requestWellFormed{ true };
            mock->SetMockMatchedCallback(
                [&](HttpMock* mock, xsapi_internal_string uri, xsapi_internal_string body)
                {
                    UNREFERENCED_PARAMETER(mock);
                    UNREFERENCED_PARAMETER(uri);
                    requestWellFormed &= VerifyJson(expectedRequestBody, body.data());
                }
            );

            XAsyncBlock async{};
            VERIFY_SUCCEEDED(XblMultiplayerGetSearchHandlesAsync(
                xboxLiveContext,
                scid,
                sessionTemplateName,
                orderByAttribute,
                orderAscending,
                searchFilter,
                socialGroup,
                &async
            ));

            VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
            VERIFY_IS_TRUE(requestWellFormed);

            size_t resultCount{};
            VERIFY_SUCCEEDED(XblMultiplayerGetSearchHandlesResultCount(&async, &resultCount));
            VERIFY_ARE_EQUAL_UINT(responseBody["results"].GetArray().Size(), resultCount);

            std::vector<XblMultiplayerSearchHandle> resultHandles(resultCount, nullptr);
            VERIFY_SUCCEEDED(XblMultiplayerGetSearchHandlesResult(&async, resultHandles.data(), resultCount));

            std::vector<MultiplayerSearchDetails> results;
            for (uint32_t i = 0; i < results.size(); ++i)
            {
                MultiplayerSearchDetails result{ resultHandles[i] };
                result.Verify(responseBody["results"].GetArray()[i]);
                results.push_back(result);
            }

            return results;
        }

        XblMultiplayerSearchHandle Handle() const noexcept
        {
            return m_handle;
        }
    private:
        // Constructor takes ownership of the handle. Caller shouldn't close the handle
        MultiplayerSearchDetails(XblMultiplayerSearchHandle handle) noexcept
            : m_handle(handle)
        {
            VERIFY_IS_NOT_NULL(m_handle);
        }

        void Verify(const JsonValue& json) const noexcept
        {
            XblMultiplayerSessionReference ref{};
            VERIFY_SUCCEEDED(XblMultiplayerSearchHandleGetSessionReference(m_handle, &ref));
            VerifyMultiplayerSessionReference(ref, json["sessionRef"]);

            const char* id{ nullptr };
            XblMultiplayerSearchHandleGetId(m_handle, &id);
            VERIFY_JSON_STRING(json, "id", id);

            if (json.HasMember("createTime"))
            {
                time_t creationTime{};
                VERIFY_SUCCEEDED(XblMultiplayerSearchHandleGetCreationTime(m_handle, &creationTime));
                VERIFY_IS_TRUE(VerifyTime(creationTime, json["createTime"].GetString()));
            }

            const auto& attrJson{ json["searchAttributes"] };

            const XblMultiplayerSessionTag* tags{ nullptr };
            size_t tagCount{ 0 };
            VERIFY_SUCCEEDED(XblMultiplayerSearchHandleGetTags(m_handle, &tags, &tagCount));
            VERIFY_ARE_EQUAL_UINT(attrJson["tags"].GetArray().Size(), tagCount);
            for (uint32_t i = 0; i < tagCount; ++i)
            {
                VERIFY_ARE_EQUAL_STR(attrJson["tags"].GetArray()[i].GetString(), tags[i].value);
            }

            const XblMultiplayerSessionStringAttribute* strings{ nullptr };
            size_t stringCount{ 0 };
            VERIFY_SUCCEEDED(XblMultiplayerSearchHandleGetStringAttributes(m_handle, &strings, &stringCount));
            VERIFY_ARE_EQUAL_UINT(attrJson["strings"].MemberCount(), stringCount);
            for (size_t i = 0; i < stringCount; ++i)
            {
                VERIFY_IS_TRUE(attrJson["strings"].HasMember(strings[i].name));
                VERIFY_ARE_EQUAL_STR(attrJson["strings"][strings[i].name].GetString(), strings[i].value);
            }

            const XblMultiplayerSessionNumberAttribute* numbers{ nullptr };
            size_t numberCount{ 0 };
            VERIFY_SUCCEEDED(XblMultiplayerSearchHandleGetNumberAttributes(m_handle, &numbers, &numberCount));
            VERIFY_ARE_EQUAL_UINT(attrJson["numbers"].MemberCount(), numberCount);
            for (size_t i = 0; i < numberCount; ++i)
            {
                VERIFY_IS_TRUE(attrJson["numbers"].HasMember(numbers[i].name));
                VERIFY_ARE_EQUAL_DOUBLE(attrJson["numbers"][numbers[i].name].GetDouble(), numbers[i].value);
            }

            const char* customPropertiesJson{ nullptr };
            VERIFY_SUCCEEDED(XblMultiplayerSearchHandleGetCustomSessionPropertiesJson(m_handle, &customPropertiesJson));
            VERIFY_JSON_FIELD(json, "customProperties", customPropertiesJson);

            if (json.HasMember("relatedInfo"))
            {
                const auto& ri{ json["relatedInfo"] };

                XblMultiplayerSessionRestriction joinRestriction{};
                VERIFY_SUCCEEDED(XblMultiplayerSearchHandleGetJoinRestriction(m_handle, &joinRestriction));
                VERIFY_IS_TRUE(multiplayer::Serializers::MultiplayerSessionRestrictionFromString(ri["joinRestriction"].GetString()) == joinRestriction);

                bool closed{};
                VERIFY_SUCCEEDED(XblMultiplayerSearchHandleGetSessionClosed(m_handle, &closed));
                VERIFY_JSON_BOOL(ri, "closed", closed);

                size_t maxMembers{}, memberCount{};
                VERIFY_SUCCEEDED(XblMultiplayerSearchHandleGetMemberCounts(m_handle, &maxMembers, &memberCount));
                VERIFY_JSON_INT(ri, "maxMembersCount", maxMembers);
                VERIFY_JSON_INT(ri, "membersCount", maxMembers);

                XblMultiplayerSessionVisibility visibility{};
                VERIFY_SUCCEEDED(XblMultiplayerSearchHandleGetVisibility(m_handle, &visibility));
                VERIFY_IS_TRUE(multiplayer::Serializers::MultiplayerSessionVisibilityFromString(ri["visibility"].GetString()) == visibility);

                const uint64_t* xuids{ nullptr };
                size_t xuidCount{};
                VERIFY_SUCCEEDED(XblMultiplayerSearchHandleGetSessionOwnerXuids(m_handle, &xuids, &xuidCount));

                const auto& xuidsJson{ ri["sessionOwners"].GetArray() };
                VERIFY_ARE_EQUAL_UINT(xuidsJson.Size(), xuidCount);
                for (uint32_t i = 0; i < xuidCount; ++i)
                {
                    VERIFY_ARE_EQUAL_UINT(atol(xuidsJson[i].GetString()), xuids[i]);
                }
            }

            // TODO we have no public API exposing role types for search handles
        }

        XblMultiplayerSearchHandle m_handle;
    };

    static void VerifyMultiplayerSessionReference(
        const XblMultiplayerSessionReference& sessionReference,
        const JsonValue& json
    )
    {
        VERIFY_ARE_EQUAL_STR(json["scid"].GetString(), sessionReference.Scid);
        VERIFY_ARE_EQUAL_STR(json["templateName"].GetString(), sessionReference.SessionTemplateName);
        VERIFY_ARE_EQUAL_STR(json["name"].GetString(), sessionReference.SessionName);
    }

    static void VerifyMultiplayerSessionQueryResult(
        const std::vector<XblMultiplayerSessionQueryResult>& result,
        const JsonValue& json
    )
    {
        const auto& expectedResults{ json["results"].GetArray() };
        VERIFY_ARE_EQUAL_UINT(expectedResults.Size(), result.size());

        for (uint32_t i = 0; i < result.size(); ++i)
        {
            const auto& a{ result[i] };
            const auto& e{ expectedResults[i] };

            VERIFY_ARE_EQUAL_UINT(atol(e["xuid"].GetString()), a.Xuid);
            VERIFY_IS_TRUE(VerifyTime(a.StartTime, e["startTime"].GetString()));
            
            VerifyMultiplayerSessionReference(a.SessionReference, e["sessionRef"]);
            VERIFY_ARE_EQUAL_UINT(e["accepted"].GetUint(), a.AcceptedMemberCount);
            VERIFY_IS_TRUE(multiplayer::Serializers::MultiplayerSessionStatusFromString(e["status"].GetString()) == a.Status);
            VERIFY_IS_TRUE(multiplayer::Serializers::MultiplayerSessionVisibilityFromString(e["visibility"].GetString()) == a.Visibility);
            VERIFY_IS_TRUE(multiplayer::Serializers::MultiplayerSessionRestrictionFromString(e["joinRestriction"].GetString()) == a.JoinRestriction);
            VERIFY_ARE_EQUAL(e["myTurn"].GetBool(), a.IsMyTurn);
        }
    }

    std::vector<XblMultiplayerSessionQueryResult> QuerySessions(
        XblContextHandle xboxLiveContext,
        const XblMultiplayerSessionQuery* query,
        const JsonValue& responseJson = testJson["defaultQuerySessionsResponse"]
    ) noexcept
    {
        auto mock = std::make_shared<HttpMock>( "", "https://sessiondirectory.xboxlive.com" );
        mock->SetResponseBody(responseJson);

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock* mock, xsapi_internal_string uri, xsapi_internal_string body)
            {
                UNREFERENCED_PARAMETER(mock);
                bool batch{ query->XuidFiltersCount > 1 };

                xsapi_internal_stringstream expectedPath;
                expectedPath << "/serviceconfigs/" << query->Scid << "/sessiontemplates/" << query->SessionTemplateNameFilter;
                if (batch)
                {
                    expectedPath << "/batch";
                }
                else
                {
                    expectedPath << "/sessions";
                }
                requestWellFormed &= expectedPath.str() == HttpMock::GetUriPath(uri);

                auto queryParams{ HttpMock::GetQueryParams(uri) };
                if (query->MaxItems)
                {
                    requestWellFormed &= queryParams["take"].data() == Utils::StringFromUint64(query->MaxItems);
                }
                if (query->IncludePrivateSessions)
                {
                    requestWellFormed &= queryParams["private"] == "true";
                }
                if (query->IncludeReservations)
                {
                    requestWellFormed &= queryParams["reservations"] == "true";
                }
                if (query->IncludeInactiveSessions)
                {
                    requestWellFormed &= queryParams["inactive"] == "true";
                }
                if (query->KeywordFilter)
                {
                    requestWellFormed &= queryParams["keyword"] == query->KeywordFilter;
                }
                if (query->ContractVersionFilter)
                {
                    requestWellFormed &= queryParams["version"] == StringFromUint64Internal(query->ContractVersionFilter);
                }
                switch (query->VisibilityFilter)
                {
                case XblMultiplayerSessionVisibility::Full:
                {
                    requestWellFormed &= queryParams["visibility"] == "full";
                    break;
                }
                case XblMultiplayerSessionVisibility::Open:
                {
                    requestWellFormed &= queryParams["visibility"] == "open";
                    break;
                }
                case XblMultiplayerSessionVisibility::PrivateSession:
                {
                    requestWellFormed &= queryParams["visibility"] == "private";
                    break;
                }
                case XblMultiplayerSessionVisibility::Visible:
                {
                    requestWellFormed &= queryParams["visibility"] == "visibile";
                    break;
                }
                default:
                {
                    break;
                }
                }

                if (batch)
                {
                    requestWellFormed &= !body.empty();

                    JsonDocument bodyJson;
                    bodyJson.Parse(body.data());

                    Vector<uint64_t> xuids{};
                    JsonUtils::ExtractJsonVector(JsonUtils::JsonXuidExtractor, bodyJson, "xuids", xuids, false);

                    requestWellFormed &= xuids.size() == query->XuidFiltersCount;

                    std::unordered_set<uint64_t> unionSet(query->XuidFilters, query->XuidFilters + query->XuidFiltersCount);
                    for (auto xuid : xuids)
                    {
                        unionSet.erase(xuid);
                    }
                    requestWellFormed &= unionSet.empty();
                }
                else
                {
                    requestWellFormed &= body.empty();
                    if (query->XuidFiltersCount == 1)
                    {
                        requestWellFormed &= queryParams["xuid"] == StringFromUint64Internal(query->XuidFilters[0]);
                    }
                }
            }
        );

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMultiplayerQuerySessionsAsync(xboxLiveContext, query, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);

        size_t resultCount{};
        VERIFY_SUCCEEDED(XblMultiplayerQuerySessionsResultCount(&async, &resultCount));

        std::vector<XblMultiplayerSessionQueryResult> results(resultCount);
        VERIFY_SUCCEEDED(XblMultiplayerQuerySessionsResult(&async, resultCount, results.data()));
        VerifyMultiplayerSessionQueryResult(results, responseJson);

        return results;
    }

    DEFINE_TEST_CASE(TestGetSessionAsync)
    {
        TEST_LOG(L"Test starting: TestGetSessionAsync");

        MPTestEnv testEnv{};
        MultiplayerSession::Get(testEnv.XboxLiveContext());
    }

    DEFINE_TEST_CASE(TestGetSessionWithHandleAsync)
    {
        TEST_LOG(L"Test starting: TestGetSessionWithHandleAsync");

        MPTestEnv testEnv{};
        MultiplayerSession::Get(testEnv.XboxLiveContext(), "testHandle");
    }

    DEFINE_TEST_CASE(TestGetLargeSessionAsync)
    {
        TEST_LOG(L"Test starting: TestGetLargeSessionAsync");

        MPTestEnv testEnv{};
        MultiplayerSession::Get(testEnv.XboxLiveContext(), defaultSessionReference, testJson["largeSessionDocument"]);
    }

    DEFINE_TEST_CASE(TestQuerySessions)
    {
        TEST_LOG(L"Test starting: TestQuerySessions");

        MPTestEnv env{};
        std::vector<uint64_t> xuidFilters{ 1 };

        // without keyword filter and without context version filter
        XblMultiplayerSessionQuery query
        {
            "8d050174-412b-4d51-a29b-d55a34edfdb7",
            100,
            true,
            true,
            true,
            xuidFilters.data(),
            xuidFilters.size(),
            nullptr, // No keyword filter
            "integration",
            XblMultiplayerSessionVisibility::Full,
            0 // No contract version filter
        };

        QuerySessions(env.XboxLiveContext(), &query);

        // without xbox user id filter and with all the include settings off
        query = XblMultiplayerSessionQuery
        {
            "8d050174-412b-4d51-a29b-d55a34edfdb7",
            100,
            false, // exclude private sesions
            false, // exclude reservations
            false, // exclude inactive sessions
            nullptr, // no xuid filters
            0,
            "hello",
            "integration",
            XblMultiplayerSessionVisibility::Any,
            5
        };

        QuerySessions(env.XboxLiveContext(), &query);

        // with multiple xuid filters (batch query)
        xuidFilters.clear();
        xuidFilters = { 12345, 67890 };

        query = XblMultiplayerSessionQuery
        {
            "8d050174-412b-4d51-a29b-d55a34edfdb7",
            100,
            true,
            true,
            true,
            xuidFilters.data(),
            xuidFilters.size(),
            nullptr,
            "integration",
            XblMultiplayerSessionVisibility::Full,
            0
        };

        QuerySessions(env.XboxLiveContext(), &query);
    }

    DEFINE_TEST_CASE(TestWriteNewSession)
    {
        TEST_LOG(L"Test starting: TestWriteNewSession");

        MPTestEnv env{};

        auto session = MultiplayerSession::Create(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionJoin(session.Handle(), "\"Hello\"", true, true));
        XblMultiplayerSessionCurrentUserSetStatus(session.Handle(), XblMultiplayerSessionMemberStatus::Active);
        XblMultiplayerSessionConstantsSetVisibility(session.Handle(), XblMultiplayerSessionVisibility::PrivateSession);
        XblMultiplayerSessionConstantsSetMaxMembersInSession(session.Handle(), 10);

        const char* groups[] { "team-buzz", "posse.99" };
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetGroups(session.Handle(), groups, _countof(groups)));

        const char* encounters[]{ "CoffeeShop-757093D8-E41F-49D0-BB13-17A49B20C6B9" };
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetEncounters(session.Handle(), encounters, _countof(encounters)));

        XblMultiplayerSessionMemberRole roles[]
        {
            { "lfg", "friend" },
            { "admin", "super" }
        };
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetRoles(session.Handle(), roles, _countof(roles)));

        // Verify local object is updated
        auto currentUser{ XblMultiplayerSessionCurrentUser(session.Handle()) };
        VERIFY_IS_NOT_NULL(currentUser);

        VERIFY_IS_TRUE(currentUser->Status == XblMultiplayerSessionMemberStatus::Active);
        VERIFY_IS_TRUE(session.Constants()->Visibility == XblMultiplayerSessionVisibility::PrivateSession);
        VERIFY_ARE_EQUAL_UINT(10, session.Constants()->MaxMembersInSession);

        VERIFY_ARE_EQUAL_UINT(_countof(groups), currentUser->GroupsCount);
        for (size_t i = 0; i < _countof(groups); ++i)
        {
            VERIFY_ARE_EQUAL_STR(groups[i], currentUser->Groups[i]);
        }

        VERIFY_ARE_EQUAL_UINT(_countof(encounters), currentUser->EncountersCount);
        for (size_t i = 0; i < _countof(encounters); ++i)
        {
            VERIFY_ARE_EQUAL_STR(encounters[i], currentUser->Encounters[i]);
        }

        VERIFY_ARE_EQUAL_UINT(_countof(roles), currentUser->RolesCount);
        for (size_t i = 0; i < _countof(roles); ++i)
        {
            VERIFY_ARE_EQUAL_STR(roles[i].roleName, currentUser->Roles[i].roleName);
            VERIFY_ARE_EQUAL_STR(roles[i].roleTypeName, currentUser->Roles[i].roleTypeName);
        }

        session.Write(testJson["defaultJsonWrite"]);
    }

    DEFINE_TEST_CASE(TestEmptyWrite)
    {
        TEST_LOG(L"Test starting: TestEmptyWrite");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        // request body should be empty since we didn't change anything
        session.Write(JsonValue{ rapidjson::kObjectType });
    }

    DEFINE_TEST_CASE(TestWriteSessionCapabilities)
    {
        TEST_LOG(L"Test starting: TestWriteSessionCapabilities");

        MPTestEnv env{};

        auto session = MultiplayerSession::Create(env.XboxLiveContext());

        XblMultiplayerSessionCapabilities capabilities{};
        capabilities.Connectivity = true;
        capabilities.Gameplay = true;
        capabilities.Large = true;
        capabilities.SuppressPresenceActivityCheck = true;
        capabilities.ConnectionRequiredForActiveMembers = true;
        capabilities.Crossplay = true;
        capabilities.UserAuthorizationStyle = true;
        capabilities.HasOwners = true;
        capabilities.Searchable = true;

        VERIFY_SUCCEEDED(XblMultiplayerSessionConstantsSetCapabilities(session.Handle(), capabilities));

        // Verify local object is updated
        VERIFY_ARE_EQUAL_INT(0, memcmp(&capabilities, &session.Constants()->SessionCapabilities, sizeof(XblMultiplayerSessionCapabilities)));

        session.Write(testJson["sessionCapabilitiesJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithServersJson)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithServersJson");

        MPTestEnv env{};

        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        JsonDocument serversJson{ rapidjson::Type::kObjectType };
        serversJson.AddMember("server", JsonValue{ 100 }, serversJson.GetAllocator());

        VERIFY_SUCCEEDED(XblMultiplayerSessionSetRawServersJson(session.Handle(), JsonUtils::SerializeJson(serversJson).data()));

        // Verify that the local session object is updated correctly
        VERIFY_IS_TRUE(VerifyJson(serversJson, XblMultiplayerSessionRawServersJson(session.Handle())));

        session.Write(testJson["serversJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetSessionCustomPropertyJson)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetSessionCustomPropertyJson");

        MPTestEnv env{};

        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionSetCustomPropertyJson(session.Handle(), "PropA", "\"ToBeReset\""));
        VERIFY_SUCCEEDED(XblMultiplayerSessionSetCustomPropertyJson(session.Handle(), "PropA", "\"ValueA\""));
        VERIFY_SUCCEEDED(XblMultiplayerSessionSetCustomPropertyJson(session.Handle(), "PropB", "{\"ValueB\":5}"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionSetCustomPropertyJson(session.Handle(), "boolTrue", "true"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionSetCustomPropertyJson(session.Handle(), "boolFalse", "false"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionSetCustomPropertyJson(session.Handle(), "stringTrue", "\"true\""));
        VERIFY_SUCCEEDED(XblMultiplayerSessionSetCustomPropertyJson(session.Handle(), "stringFalse", "\"false\""));
        VERIFY_SUCCEEDED(XblMultiplayerSessionSetCustomPropertyJson(session.Handle(), "number42", "42"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionSetCustomPropertyJson(session.Handle(), "objectA", "{\"name\":\"A\"}"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionSetCustomPropertyJson(session.Handle(), "arrayA", "[1,2,3,4]"));

        const auto& expectedRequest{ testJson["writeCustomPropertyJson"] };
        const auto& expectedPropertiesJson{ expectedRequest["properties"]["custom"] };

        // Verify that the local object is updated correctly
        VERIFY_IS_TRUE(VerifyJson(expectedPropertiesJson, session.Properties()->SessionCustomPropertiesJson));

        session.Write(expectedRequest);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetMatchmakingProperties)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetMatchmakingProperties");

        MPTestEnv env{};

        auto session = MultiplayerSession::Create(env.XboxLiveContext());

        const char targetSessionConstants[] = "{\"foo\":\"test1002\"}";
        VERIFY_SUCCEEDED(XblMultiplayerSessionSetMatchmakingTargetSessionConstantsJson(session.Handle(), targetSessionConstants));

        // Verify local object
        VERIFY_IS_TRUE(VerifyJson(targetSessionConstants, session.Properties()->MatchmakingTargetSessionConstantsJson));

        session.Write(testJson["matchmakingSessionConstantsJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithAddMemberReservation)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithAddMemberReservation");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionAddMemberReservation(session.Handle(), 1234, "{ \"skill\" : 100 }", true));
        VERIFY_SUCCEEDED(XblMultiplayerSessionAddMemberReservation(session.Handle(), 4567, "{ \"down\" : true }", false));

        session.Write(testJson["writeAddMemberJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithDeleteSessionCustomPropertyJson)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithDeleteSessionCustomPropertyJson");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionDeleteCustomPropertyJson(session.Handle(), "PropA"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionDeleteCustomPropertyJson(session.Handle(), "PropB"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionDeleteCustomPropertyJson(session.Handle(), "boolTrue"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionDeleteCustomPropertyJson(session.Handle(), "boolFalse"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionDeleteCustomPropertyJson(session.Handle(), "stringTrue"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionDeleteCustomPropertyJson(session.Handle(), "stringFalse"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionDeleteCustomPropertyJson(session.Handle(), "number42"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionDeleteCustomPropertyJson(session.Handle(), "objectA"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionDeleteCustomPropertyJson(session.Handle(), "arrayA"));

        session.Write(testJson["deleteCustomPropertyJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithJoin1)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithJoin1");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionJoin(session.Handle(), nullptr, false, false));

        session.Write(testJson["memberStatusInactiveJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithJoin2)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithJoin2");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionJoin(session.Handle(), "{ \"testValue\" : 100 }", false, false));

        session.Write(testJson["memberStatusInactiveJson2"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithJoin3)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithJoin3");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionJoin(session.Handle(), "{ \"testValue\" : 100 }", true, true));

        session.Write(testJson["memberStatusInactiveJson3"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithLeave)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithLeave");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionLeave(session.Handle()));
        session.Write(testJson["leaveJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetCurrentUserInactive)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetCurrentUserInactive");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionJoin(session.Handle(), nullptr, false, false));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetStatus(session.Handle(), XblMultiplayerSessionMemberStatus::Inactive));

        session.Write(testJson["memberStatusActiveJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetCurrentUserActive)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetCurrentUserActive");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionJoin(session.Handle(), nullptr, false, false));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetStatus(session.Handle(), XblMultiplayerSessionMemberStatus::Active));

        session.Write(testJson["matchmakingPropertiesJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetCurrentUserSecureDeviceAddressBase64)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetCurrentUserSecureDeviceAddressBase64");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionJoin(session.Handle(), nullptr, false, true));
        XblMultiplayerSessionCurrentUserSetSecureDeviceAddressBase64(session.Handle(), "1234");

        session.Write(testJson["secureDeviceJson"]);
    }

    DEFINE_TEST_CASE(TestXblFormatSecureDeviceAddress)
    {
        TEST_LOG(L"Test starting: TestXblFormatSecureDeviceAddress");

        String deviceId;
        XblFormattedSecureDeviceAddress address{};
        
        deviceId = "12345678901234567890";
        VERIFY_SUCCEEDED(XblFormatSecureDeviceAddress(deviceId.c_str(), &address));
        VERIFY_ARE_EQUAL_STR(address.value, "QUFBQUFBQUExMjM0NTY3ODkwMTIzNDU2Nzg5MA==");

        deviceId = "A1B2C3D4E5F6";
        VERIFY_SUCCEEDED(XblFormatSecureDeviceAddress(deviceId.c_str(), &address));
        VERIFY_ARE_EQUAL_STR(address.value, "QUFBQUFBQUFBMUIyQzNENEU1RjY=");

        // Test nullptr
        VERIFY_FAILED(XblFormatSecureDeviceAddress(nullptr, &address));
        VERIFY_FAILED(XblFormatSecureDeviceAddress("", &address));
        VERIFY_FAILED(XblFormatSecureDeviceAddress(deviceId.c_str(), nullptr));
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetCurrentUserMemberCustomPropertyJson)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetCurrentUserMemberCustomPropertyJson");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionJoin(session.Handle(), nullptr, false, true));

        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetCustomPropertyJson(session.Handle(), "health", "\"toBeReset\""));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetCustomPropertyJson(session.Handle(), "health", "4567"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetCustomPropertyJson(session.Handle(), "boolTrue", "true"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetCustomPropertyJson(session.Handle(), "boolFalse", "false"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetCustomPropertyJson(session.Handle(), "stringTrue", "\"true\""));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetCustomPropertyJson(session.Handle(), "stringFalse", "\"false\""));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetCustomPropertyJson(session.Handle(), "number42", "42"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetCustomPropertyJson(session.Handle(), "objectA", "{\"name\":\"A\"}"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetCustomPropertyJson(session.Handle(), "arrayA", "[1,2,3,4]"));

        const auto& expectedRequest{ testJson["memberCustomJson"] };
        const auto& expectedPropertiesJson{ expectedRequest["members"]["me"]["properties"]["custom"] };

        // Verify that the local User is updated correctly
        auto currentUser{ XblMultiplayerSessionCurrentUser(session.Handle()) };
        VERIFY_IS_TRUE(VerifyJson(expectedPropertiesJson, currentUser->CustomPropertiesJson));

        session.Write(expectedRequest);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithDeleteCurrentUserMemberCustomPropertyJson)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithDeleteCurrentUserMemberCustomPropertyJson");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionJoin(session.Handle(), nullptr, false, true));

        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson(session.Handle(), "health"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson(session.Handle(), "health"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson(session.Handle(), "boolTrue"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson(session.Handle(), "boolFalse"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson(session.Handle(), "stringTrue"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson(session.Handle(), "stringFalse"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson(session.Handle(), "number42"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson(session.Handle(), "objectA"));
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson(session.Handle(), "arrayA"));

        session.Write(testJson["deleteMemberCustomJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithTurnCollection)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithTurnCollection");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        uint32_t turnCollection[] = { 0 };
        VERIFY_SUCCEEDED(XblMultiplayerSessionPropertiesSetTurnCollection(session.Handle(), turnCollection, _countof(turnCollection)));

        // Verify local object
        VERIFY_ARE_EQUAL_UINT(_countof(turnCollection), session.Properties()->TurnCollectionCount);
        VERIFY_ARE_EQUAL_UINT(turnCollection[0], session.Properties()->TurnCollection[0]);

        session.Write(testJson["turnJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithKeywords)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithKeywords");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        const char* keywords[] = { "apple" };
        VERIFY_SUCCEEDED(XblMultiplayerSessionPropertiesSetKeywords(session.Handle(), keywords, _countof(keywords)));

        // Verify local object
        VERIFY_ARE_EQUAL_UINT(_countof(keywords), session.Properties()->KeywordCount);
        VERIFY_ARE_EQUAL_STR(keywords[0], session.Properties()->Keywords[0]);

        session.Write(testJson["keywordsJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithJoinRestriction)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithJoinRestriction");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        // TODO why no HRESULT here?
        XblMultiplayerSessionPropertiesSetJoinRestriction(session.Handle(), XblMultiplayerSessionRestriction::Followed);

        // Verify local object
        VERIFY_IS_TRUE(XblMultiplayerSessionRestriction::Followed == session.Properties()->JoinRestriction);

        session.Write(testJson["joinRestrictionJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithReadRestriction)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithReadRestriction");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        // TODO why no HRESULT here?
        XblMultiplayerSessionPropertiesSetReadRestriction(session.Handle(), XblMultiplayerSessionRestriction::Followed);

        // Verify local object
        VERIFY_IS_TRUE(XblMultiplayerSessionRestriction::Followed == session.Properties()->ReadRestriction);

        session.Write(testJson["TestWriteSessionAsyncWithReadRestrictionExpectedRequest"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetTimeouts)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetTimeouts");

        MPTestEnv env{};
        auto session = MultiplayerSession::Create(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionConstantsSetTimeouts(session.Handle(), 3001000, 3002000, 3003000, 3004000));

        // Verify local object
        VERIFY_ARE_EQUAL_UINT(session.Constants()->MemberReservedTimeout, 3001000);
        VERIFY_ARE_EQUAL_UINT(session.Constants()->MemberInactiveTimeout, 3002000);
        VERIFY_ARE_EQUAL_UINT(session.Constants()->MemberReadyTimeout, 3003000);
        VERIFY_ARE_EQUAL_UINT(session.Constants()->SessionEmptyTimeout, 3004000);

        session.Write(testJson["timeoutsJson"]);
    }
       
    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetQualityOfServiceConnectivityMetrics)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetQualityOfServiceConnectivityMetrics");

        MPTestEnv env{};
        auto session = MultiplayerSession::Create(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionConstantsSetQosConnectivityMetrics(session.Handle(), true, true, true, true));

        // Verify local object
        VERIFY_ARE_EQUAL(true, session.Constants()->EnableMetricsLatency);
        VERIFY_ARE_EQUAL(true, session.Constants()->EnableMetricsBandwidthDown);
        VERIFY_ARE_EQUAL(true, session.Constants()->EnableMetricsBandwidthUp);
        VERIFY_ARE_EQUAL(true, session.Constants()->EnableMetricsCustom);

        session.Write(testJson["qosMetricsJson"]);
    }

    DEFINE_TEST_CASE(TestGetCloudComputePackageJson)
    {
        TEST_LOG(L"Test starting: TestGetCloudComputePackageJson");

        MPTestEnv env{};
        auto session = MultiplayerSession::Create(env.XboxLiveContext());

        const char json[] = "{\"crossSandbox\":true, \"titleId\":\"4567\", \"gsiSet\":\"128ce92a-45d0-4319-8a7e-bd8e940114ec\"}";
        VERIFY_SUCCEEDED(XblMultiplayerSessionConstantsSetCloudComputePackageJson(session.Handle(), json));

        // Validate the local object is updated
        VERIFY_IS_TRUE(VerifyJson(json, session.Constants()->SessionCloudComputePackageConstantsJson));

        session.Write(testJson["cloudComputePackageJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetMemberInitialization)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetMemberInitialization");

        MPTestEnv env{};
        auto session = MultiplayerSession::Create(env.XboxLiveContext());

        XblMultiplayerMemberInitialization memberInitialization
        {
            4001000,
            4002000,
            4003000,
            true,
            4004
        };
        VERIFY_SUCCEEDED(XblMultiplayerSessionConstantsSetMemberInitialization(session.Handle(), memberInitialization));

        // Verify the local object is updated
        VERIFY_IS_NOT_NULL(session.Constants()->MemberInitialization);
        VERIFY_ARE_EQUAL_UINT(memberInitialization.JoinTimeout, session.Constants()->MemberInitialization->JoinTimeout);
        VERIFY_ARE_EQUAL_UINT(memberInitialization.MeasurementTimeout, session.Constants()->MemberInitialization->MeasurementTimeout);
        VERIFY_ARE_EQUAL_UINT(memberInitialization.EvaluationTimeout, session.Constants()->MemberInitialization->EvaluationTimeout);
        VERIFY_ARE_EQUAL(memberInitialization.ExternalEvaluation, session.Constants()->MemberInitialization->ExternalEvaluation);
        VERIFY_ARE_EQUAL_UINT(memberInitialization.MembersNeededToStart, session.Constants()->MemberInitialization->MembersNeededToStart);

        session.Write(testJson["memberInitializationJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetPeerToPeerRequirements)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetPeerToPeerRequirements");

        MPTestEnv env{};
        auto session = MultiplayerSession::Create(env.XboxLiveContext());

        XblMultiplayerPeerToPeerRequirements reqs{ 5001, 5002 };
        VERIFY_SUCCEEDED(XblMultiplayerSessionConstantsSetPeerToPeerRequirements(session.Handle(), reqs));

        // ensure local object is updated
        VERIFY_ARE_EQUAL_UINT(reqs.LatencyMaximum, session.Constants()->PeerToPeerRequirements.LatencyMaximum);
        VERIFY_ARE_EQUAL_UINT(reqs.BandwidthMinimumInKbps, session.Constants()->PeerToPeerRequirements.BandwidthMinimumInKbps);

        session.Write(testJson["peerToPeerJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetPeerToHostRequirements)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetPeerToHostRequirements");

        MPTestEnv env{};
        auto session = MultiplayerSession::Create(env.XboxLiveContext());

        XblMultiplayerPeerToHostRequirements reqs{ 6001, 6002, 6003, XblMultiplayerMetrics::Bandwidth };
        VERIFY_SUCCEEDED(XblMultiplayerSessionConstantsSetPeerToHostRequirements(session.Handle(), reqs));

        // ensure local object is updated
        VERIFY_ARE_EQUAL_UINT(reqs.LatencyMaximum, session.Constants()->PeerToHostRequirements.LatencyMaximum);
        VERIFY_ARE_EQUAL_UINT(reqs.BandwidthDownMinimumInKbps, session.Constants()->PeerToHostRequirements.BandwidthDownMinimumInKbps);
        VERIFY_ARE_EQUAL_UINT(reqs.BandwidthUpMinimumInKbps, session.Constants()->PeerToHostRequirements.BandwidthUpMinimumInKbps);
        VERIFY_IS_TRUE(reqs.HostSelectionMetric == session.Constants()->PeerToHostRequirements.HostSelectionMetric);

        session.Write(testJson["peerToHostJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetInitializationStatus)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetInitializationStatus");

        MPTestEnv env{};
        auto session = MultiplayerSession::Create(env.XboxLiveContext());

        // TODO why no HRESULT here?
        XblMultiplayerSessionSetInitializationSucceeded(session.Handle(), true);

        session.Write(testJson["initializationStatusJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetHostDeviceToken)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetHostDeviceToken");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        // TODO why not XblMultiplayerDeviceToken?
        // TODO why no HRESULT?
        XblDeviceToken token{ "1234" };
        XblMultiplayerSessionSetHostDeviceToken(session.Handle(), token);

        // Verify local object
        VERIFY_ARE_EQUAL_STR(token.Value, session.Properties()->HostDeviceToken.Value);

        session.Write(testJson["hostDeviceTokenJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetMatchmakingServerConnectionPath)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetMatchmakingServerConnectionPath");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        // TODO why no HRESULT?
        const char* connectionString{ "8001" };
        XblMultiplayerSessionSetMatchmakingServerConnectionPath(session.Handle(), connectionString);
        VERIFY_ARE_EQUAL_STR(connectionString, session.Properties()->MatchmakingServerConnectionString);

        session.Write(testJson["serverConnectionPathJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetMatchmakingResubmit)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetMatchmakingResubmit");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        // TODO HRESULT
        XblMultiplayerSessionSetMatchmakingResubmit(session.Handle(), true);
        VERIFY_IS_TRUE(session.Properties()->MatchmakingResubmit);

        session.Write(testJson["matchmakingResubmitJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetServerConnectionStringCandidates)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetServerConnectionStringCandidates");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        const char* candidates[] = { "9001", "9002", "9003" };
        VERIFY_SUCCEEDED(XblMultiplayerSessionSetServerConnectionStringCandidates(session.Handle(), candidates, _countof(candidates)));

VERIFY_ARE_EQUAL_UINT(_countof(candidates), session.Properties()->ServerConnectionStringCandidatesCount);
for (size_t i = 0; i < _countof(candidates); ++i)
{
    VERIFY_ARE_EQUAL_STR(candidates[i], session.Properties()->ServerConnectionStringCandidates[i]);
}

session.Write(testJson["serverConnectionStringCandidatesJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetCurrentUserMembersInGroup)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetCurrentUserMembersInGroup");

        MPTestEnv env{};
        auto session = MultiplayerSession::Create(env.XboxLiveContext());

        VERIFY_SUCCEEDED(XblMultiplayerSessionJoin(session.Handle(), nullptr, false, true));

        uint32_t membersInGroup[] = { 0 };
        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetMembersInGroup(session.Handle(), membersInGroup, _countof(membersInGroup)));

        auto currentUser{ XblMultiplayerSessionCurrentUser(session.Handle()) };
        VERIFY_ARE_EQUAL_UINT(_countof(membersInGroup), currentUser->MembersInGroupCount);
        for (size_t i = 0; i < _countof(membersInGroup); ++i)
        {
            VERIFY_ARE_EQUAL_UINT(membersInGroup[i], currentUser->MembersInGroupIds[i]);
        }

        session.Write(testJson["membersInGroupJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithClosed)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithClosed");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        // TODO should be XblMultiplayerSessionPropertiesSetClosed
        // TODO return HRESULT
        XblMultiplayerSessionSetClosed(session.Handle(), true);
        VERIFY_ARE_EQUAL(true, session.Properties()->Closed);

        session.Write(testJson["closedJson"]);
    }

    TEST_METHOD(TestWriteSessionAsyncWithLocked)
    {
        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        // TODO should be XblMultiplayerSessionPropertiesSetLocked
        // TODO return HRESULT
        XblMultiplayerSessionSetLocked(session.Handle(), true);
        VERIFY_ARE_EQUAL(true, session.Properties()->Locked);

        session.Write(testJson["lockedJson"]);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetCurrentUserQualityOfServiceMeasurements)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetCurrentUserQualityOfServiceMeasurements");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        const auto& expectedRequest{ testJson["qosMeasurementsJson"] };
        const auto& qosJson = expectedRequest["members"]["me"]["properties"]["system"]["measurements"];

        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetQosMeasurements(session.Handle(), JsonUtils::SerializeJson(qosJson).data()));
        VERIFY_IS_TRUE(VerifyJson(qosJson, XblMultiplayerSessionCurrentUser(session.Handle())->QosMeasurementsJson));

        session.Write(expectedRequest);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetCurrentUserQualityOfServiceServerMeasurementsJson)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetCurrentUserQualityOfServiceServerMeasurementsJson");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        const auto& expectedRequest{ testJson["qosServerMeasurementsJson"] };
        const auto& qosJson = expectedRequest["members"]["me"]["properties"]["system"]["serverMeasurements"];

        VERIFY_SUCCEEDED(XblMultiplayerSessionCurrentUserSetServerQosMeasurements(session.Handle(), JsonUtils::SerializeJson(qosJson).data()));
        VERIFY_IS_TRUE(VerifyJson(qosJson, XblMultiplayerSessionCurrentUser(session.Handle())->ServerMeasurementsJson));

        session.Write(expectedRequest);
    }

    DEFINE_TEST_CASE(TestWriteSessionAsyncWithSetMutableRoleSettings)
    {
        TEST_LOG(L"Test starting: TestWriteSessionAsyncWithSetMutableRoleSettings");

        MPTestEnv env{};
        auto session = MultiplayerSession::Get(env.XboxLiveContext());

        uint32_t max{ 1 }, target{ 1 };
        VERIFY_SUCCEEDED(XblMultiplayerSessionSetMutableRoleSettings(session.Handle(), "lfg", "friend", &max, &target));
        VERIFY_SUCCEEDED(XblMultiplayerSessionSetMutableRoleSettings(session.Handle(), "lfg", "other", &max, nullptr));

        auto roles{ session.RoleTypes() };
        for (const auto& roleType : roles)
        {
            if (xsapi_internal_string{ roleType->Name } == "lfg")
            {
                for (size_t i = 0; i < roleType->RoleCount; ++i)
                {
                    if (xsapi_internal_string{ roleType->Roles[i].Name } == "friend")
                    {
                        VERIFY_ARE_EQUAL_UINT(max, roleType->Roles[i].MaxMemberCount);
                        VERIFY_ARE_EQUAL_UINT(target, roleType->Roles[i].TargetCount);
                    }
                    else if (xsapi_internal_string{ roleType->Roles[i].Name } == "other")
                    {
                        VERIFY_ARE_EQUAL_UINT(max, roleType->Roles[i].MaxMemberCount);
                    }
                }
            }
        }

        session.Write(testJson["roleTypesRequestJson"]);
    }

    DEFINE_TEST_CASE(TestSetActivityAsync)
    {
        TEST_LOG(L"Test starting: TestSetActivityAsync");

        MPTestEnv env{};
        auto mock = std::make_shared<HttpMock>( "POST", MPSD_URI "/handles" );

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock*, xsapi_internal_string, xsapi_internal_string body)
            {
                requestWellFormed &= VerifyJson(testJson["activityJson"], body.data());
            });

        XblMultiplayerSessionReference activityRef
        {
            "MockScid", // serviceConfigurationId
            "MockSessionTemplateName", // sessionTemplateName
            "XWIN_7ce12e85-594a-4b3b-9dc3-33b9a4ea57ce" // sessionName
        };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMultiplayerSetActivityAsync(env.XboxLiveContext(), &activityRef, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
    }

    DEFINE_TEST_CASE(TestClearActivityAsync)
    {
        TEST_LOG(L"Test starting: TestClearActivityAsync");

        MPTestEnv env{};

        // XblMultiplayerClearActivityAsync results in 2 service calls; first to query the activity, and then to clear it. Set up
        // a mock for each endpoint
        HttpMock queryActivityMock{ "POST", MPSD_URI };
        queryActivityMock.SetResponseBody(testJson["activitiesForUserResponseJson"]);

        HttpMock clearActivityMock{ "DELETE", MPSD_URI "/handles/7a4d0a99-4e23-4eba-9894-5173cf123fb4" };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMultiplayerClearActivityAsync(env.XboxLiveContext(), MOCK_SCID, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
    }

    DEFINE_TEST_CASE(TestSetTransferHandleAsync)
    {
        TEST_LOG(L"Test starting: TestSetTransferHandleAsync");

        MPTestEnv env{};

        const auto& responseBody{ testJson["transferHandleResponseJson"] };
        auto mock = std::make_shared<HttpMock>( "POST", MPSD_URI "/handles" );
        mock->SetResponseBody(responseBody);

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock*, xsapi_internal_string, xsapi_internal_string body)
            {
                requestWellFormed &= VerifyJson(testJson["transferHandleJson"], body.data());
            });

        XblMultiplayerSessionReference target
        {
            "MockScid", // serviceConfigurationId
            "MockSessionTemplateName", // sessionTemplateName
            "XWIN_7ce12e85-594a-4b3b-9dc3-33b9a4ea57ce" // sessionName
        };

        XblMultiplayerSessionReference origin
        {
            "MockScid", // serviceConfigurationId
            "samplelobbytemplate107", // sessionTemplateName
            "bd6c41c3-01c3-468a-a3b5-3e0fe8133862" // sessionName
        };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMultiplayerSetTransferHandleAsync(env.XboxLiveContext(), target, origin, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);

        XblMultiplayerSessionHandleId handleId{};
        VERIFY_SUCCEEDED(XblMultiplayerSetTransferHandleResult(&async, &handleId));
        VERIFY_ARE_EQUAL_STR_IGNORE_CASE(responseBody["id"].GetString(), handleId.value);
    }

    DEFINE_TEST_CASE(TestGetSearchHandlesAsync_1)
    {
        TEST_LOG(L"Test starting: TestGetSearchHandlesAsync_1");

        MPTestEnv env{};

        MultiplayerSearchDetails::Query(
            env.XboxLiveContext(),
            MOCK_SCID,
            "GlobalLFGTemplate",
            "OrderBy",
            true,
            "SearchQuery"
        );
    }

    DEFINE_TEST_CASE(TestGetSearchHandlesAsync_2)
    {
        TEST_LOG(L"Test starting: TestGetSearchHandlesAsync_2");

        MPTestEnv env{};

        MultiplayerSearchDetails::Query(
            env.XboxLiveContext(),
            MOCK_SCID,
            "GlobalLFGTemplate",
            "OrderBy",
            true,
            "SearchQuery",
            "favorites",
            testJson["searchHandlesWithSocialGroupRequestJson"]
        );
    }

    DEFINE_TEST_CASE(TestSetSearchHandleAsync)
    {
        TEST_LOG(L"Test starting: TestSetSearchHandleAsync");

        MPTestEnv env{};

        XblMultiplayerSessionReference sessionRef
        {
            MOCK_SCID,
            "GlobalLFGSessionTemplateName",
            "LFGSession"
        };

        std::vector<XblMultiplayerSessionTag> tags{ { "micsrequired" }, { "girlsonly" } };
        std::vector<XblMultiplayerSessionNumberAttribute> numberAttributes{ { "Skill_D", 10.145 }, { "Skill_I", 14 } };
        std::vector<XblMultiplayerSessionStringAttribute> stringAttributes{ { "Class", "A" } };

        MultiplayerSearchDetails::Create(
            env.XboxLiveContext(),
            sessionRef,
            tags,
            numberAttributes,
            stringAttributes
        );
    }

    DEFINE_TEST_CASE(TestClearSearchHandleAsync)
    {
        TEST_LOG(L"Test starting: TestClearSearchHandleAsync");

        MPTestEnv env{};

        HttpMock mock{"DELETE", MPSD_URI "/handles/TestHandleId" };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMultiplayerDeleteSearchHandleAsync(env.XboxLiveContext(), "TestHandleId", &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
    }

    DEFINE_TEST_CASE(TestSendInvitesAsync)
    {
        TEST_LOG(L"Test starting: TestSendInvitesAsync");

        MPTestEnv env{};

        std::vector<uint64_t> xuids{ 1111, 2222 };

        auto mock = std::make_shared<HttpMock>( "POST", MPSD_URI "/handles" );

        // Inviting multiple Xuids results in multiple Http calls. Make sure the expected number of calls go out
        size_t requestCount{ 0 };
        bool requestsWellFormed{ true };
        const char* responseIds[] = { "B8704EC5-95CD-408B-BD41-BAA7A2761CC2", "9D74C42E-87DE-47BA-B489-D3A264C9F994" };
        JsonDocument testResponses{ GetTestResponses("TestResponses\\Multiplayer.json") };
        auto inviteRequestJson = JsonUtils::SerializeJson(testResponses["inviteRequestJson"]);
        auto inviteResponseJson = JsonUtils::SerializeJson(testResponses["inviteResponseJson"]);
        JsonDocument expectedRequestBodyJson;
        JsonDocument responseBodyJson;
        expectedRequestBodyJson.Parse(inviteRequestJson.c_str());
        responseBodyJson.Parse(inviteResponseJson.c_str());

        mock->SetMockMatchedCallback(
            [&](HttpMock* mock, xsapi_internal_string uri, xsapi_internal_string body)
            {
                UNREFERENCED_PARAMETER(uri);

                if (requestCount < xuids.size())
                {
                    {
                        auto& a{ expectedRequestBodyJson.GetAllocator() };
                        JsonUtils::SetMember(expectedRequestBodyJson, "invitedXuid", JsonValue{ StringFromUint64Internal(xuids[requestCount]).data(), a });
                        requestsWellFormed &= VerifyJson(expectedRequestBodyJson, body.data());
                    }
                    {
                        auto& a{ responseBodyJson.GetAllocator() };
                        JsonUtils::SetMember(responseBodyJson, "invitedXuid", JsonValue{ StringFromUint64Internal(xuids[requestCount]).data(), a });
                        JsonUtils::SetMember(responseBodyJson, "id", JsonValue{ responseIds[requestCount], a });
                        mock->SetResponseBody(responseBodyJson);
                    }
                }
                requestCount++;
            }
        );

        XblMultiplayerSessionReference sessionRef
        {
            MOCK_SCID,
            "MockSessionTemplateName",
            "XWIN_7ce12e85-594a-4b3b-9dc3-33b9a4ea57ce"
        };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMultiplayerSendInvitesAsync(
            env.XboxLiveContext(),
            &sessionRef,
            xuids.data(),
            xuids.size(),
            MOCK_TITLEID,
            nullptr,
            nullptr,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_ARE_EQUAL_UINT(xuids.size(), requestCount);
        VERIFY_IS_TRUE(requestsWellFormed);

        std::vector<XblMultiplayerInviteHandle> inviteHandles(xuids.size());
        VERIFY_SUCCEEDED(XblMultiplayerSendInvitesResult(&async, inviteHandles.size(), inviteHandles.data()));

        for (size_t i = 0; i < inviteHandles.size(); ++i)
        {
            VERIFY_ARE_EQUAL_STR_IGNORE_CASE(responseIds[i], inviteHandles[i].Data);
        }
    }

    void VerifyGetActivitiesForUsers(MPTestEnv& env, bool withProperties)
    {
        auto mock = std::make_shared<HttpMock>( "POST", MPSD_URI "/handles" );
        mock->SetResponseBody(testJson["activitiesForUserResponseJson"]);

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback([&](HttpMock* mock, xsapi_internal_string uri, xsapi_internal_string body)
        {
            UNREFERENCED_PARAMETER(mock);
            UNREFERENCED_PARAMETER(uri);
            requestWellFormed &= VerifyJson(testJson["activitiesForUserRequestJson"], body.data());
        });

        XAsyncBlock async{};
        uint64_t xuids[] = { 1234 };
        if (withProperties)
        {
            VERIFY_SUCCEEDED(XblMultiplayerGetActivitiesWithPropertiesForUsersAsync(env.XboxLiveContext(), MOCK_SCID, xuids, _countof(xuids), &async));
        }
        else
        {
            VERIFY_SUCCEEDED(XblMultiplayerGetActivitiesForUsersAsync(env.XboxLiveContext(), MOCK_SCID, xuids, _countof(xuids), &async));
        }
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);

        const auto& resultsJson{ testJson["activitiesForUserResponseJson"]["results"].GetArray() };

        size_t resultSize{};
        size_t resultCount{};
        if (withProperties)
        {
            VERIFY_SUCCEEDED(XblMultiplayerGetActivitiesWithPropertiesForUsersResultSize(&async, &resultSize));
        }
        else
        {
            VERIFY_SUCCEEDED(XblMultiplayerGetActivitiesForUsersResultCount(&async, &resultCount));
            VERIFY_ARE_EQUAL_UINT(resultsJson.Size(), resultCount);
        }

        size_t count{ 0 };
        std::vector<char> buffer(resultSize, 0);
        std::vector<XblMultiplayerActivityDetails> activityDetailsVector{};
        XblMultiplayerActivityDetails* activityDetails{};
        if (withProperties)
        {
            VERIFY_SUCCEEDED(XblMultiplayerGetActivitiesWithPropertiesForUsersResult(&async, resultSize, buffer.data(), &activityDetails, &count, nullptr));
            VERIFY_ARE_EQUAL_UINT(resultsJson.Size(), count);
        }
        else
        {
            count = resultCount;
            activityDetailsVector = std::vector<XblMultiplayerActivityDetails>(resultCount);
            VERIFY_SUCCEEDED(XblMultiplayerGetActivitiesForUsersResult(&async, resultCount, activityDetailsVector.data()));
            activityDetails = activityDetailsVector.data();
        }

        for (uint32_t i = 0; i < count; ++i)
        {
            const auto& a{ activityDetails[i] };
            const auto& e{ resultsJson[i] };

            VerifyMultiplayerSessionReference(a.SessionReference, e["sessionRef"]);
            VERIFY_ARE_EQUAL_STR_IGNORE_CASE(e["id"].GetString(), a.HandleId);
            VERIFY_JSON_INT_STRING(e, "titleId", a.TitleId);
            VERIFY_IS_TRUE(multiplayer::Serializers::MultiplayerSessionVisibilityFromString(e["relatedInfo"]["visibility"].GetString()) == a.Visibility);
            VERIFY_IS_TRUE(multiplayer::Serializers::MultiplayerSessionRestrictionFromString(e["relatedInfo"]["joinRestriction"].GetString()) == a.JoinRestriction);
            VERIFY_ARE_EQUAL(e["relatedInfo"]["closed"].GetBool(), a.Closed);
            VERIFY_JSON_INT_STRING(e, "ownerXuid", a.OwnerXuid);
            VERIFY_ARE_EQUAL_UINT(e["relatedInfo"]["maxMembersCount"].GetUint(), a.MaxMembersCount);
            VERIFY_ARE_EQUAL_UINT(e["relatedInfo"]["membersCount"].GetUint(), a.MembersCount);
        }
    }

    void VerifyGetActivitiesForSocialGroup(MPTestEnv& env, bool withProperties)
    {
        auto mock = std::make_shared<HttpMock>( "POST", MPSD_URI "/handles" );
        mock->SetResponseBody(testJson["activitiesForSocialGroupResponseJson"]);

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback([&](HttpMock* mock, xsapi_internal_string uri, xsapi_internal_string body)
        {
            UNREFERENCED_PARAMETER(mock);
            UNREFERENCED_PARAMETER(uri);
            requestWellFormed &= VerifyJson(testJson["activitiesForSocialGroupRequestJson"], body.data());
        });

        XAsyncBlock async{};
        if (withProperties)
        {
            VERIFY_SUCCEEDED(XblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync(
                env.XboxLiveContext(),
                MOCK_SCID,
                env.XboxLiveContext()->Xuid(),
                "friends",
                &async
            ));
        }
        else
        {
            VERIFY_SUCCEEDED(XblMultiplayerGetActivitiesForSocialGroupAsync(
                env.XboxLiveContext(),
                MOCK_SCID,
                env.XboxLiveContext()->Xuid(),
                "friends",
                &async
            ));
        }

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);

        if (withProperties)
        {
            size_t resultSize{};
            VERIFY_SUCCEEDED(XblMultiplayerGetActivitiesWithPropertiesForSocialGroupResultSize(&async, &resultSize));
            VERIFY_ARE_EQUAL_UINT(0, resultSize);
        }
        else
        {
            size_t resultCount{};
            VERIFY_SUCCEEDED(XblMultiplayerGetActivitiesForSocialGroupResultCount(&async, &resultCount));
            VERIFY_ARE_EQUAL_UINT(0, resultCount);
        }
    }

    DEFINE_TEST_CASE(TestGetActivitiesForUsersAsync)
    {
        TEST_LOG(L"Test starting: TestGetActivitiesForUsersAsync");

        MPTestEnv env{};

        VerifyGetActivitiesForUsers(env, false);
        VerifyGetActivitiesForUsers(env, true);
    }

    DEFINE_TEST_CASE(TestGetActivitiesForSocialGroupAsync)
    {
        TEST_LOG(L"Test starting: TestGetActivitiesForSocialGroupAsync");

        MPTestEnv env{};

        VerifyGetActivitiesForSocialGroup(env, false);
        VerifyGetActivitiesForSocialGroup(env, true);
    }

    DEFINE_TEST_CASE(TestCompareMultiplayerSessions)
    {
        TEST_LOG(L"Test starting: TestCompareMultiplayerSessions");

        MPTestEnv env{};

        {
            XblMultiplayerSessionReference sessionRef
            {
                "361D0DAA-620E-4975-B64C-0C32500D41EF",
                "MySessionTemplate",
                "32A53A76-9802-42C7-A28E-4FD483301D8B"
            };

            const uint64_t initiators[] = { 12323 };
            XblMultiplayerSessionInitArgs initArgs
            {
                50, // maxMembersInSession
                XblMultiplayerSessionVisibility::Full,
                initiators,
                _countof(initiators),
                nullptr
            };

            auto lhs = MultiplayerSession::Create(env.XboxLiveContext(), &sessionRef, &initArgs);
            XblMultiplayerSessionSetClosed(lhs.Handle(), true);
            XblMultiplayerSessionSetLocked(lhs.Handle(), true);
            VERIFY_SUCCEEDED(XblMultiplayerSessionSetCustomPropertyJson(lhs.Handle(), "hello", "\"goodbye\""));
            VERIFY_SUCCEEDED(XblMultiplayerSessionJoin(lhs.Handle(), nullptr, false, false));

            auto rhs = MultiplayerSession::Get(env.XboxLiveContext());

            XblMultiplayerSessionChangeTypes expectedChanges
            {
                XblMultiplayerSessionChangeTypes::MemberListChange |
                XblMultiplayerSessionChangeTypes::HostDeviceTokenChange |
                XblMultiplayerSessionChangeTypes::InitializationStateChange |
                XblMultiplayerSessionChangeTypes::MatchmakingStatusChange |
                XblMultiplayerSessionChangeTypes::SessionJoinabilityChange |
                XblMultiplayerSessionChangeTypes::CustomPropertyChange |
                XblMultiplayerSessionChangeTypes::MemberStatusChange // TODO what should member's default status be on join?
            };
            auto changes = XblMultiplayerSessionCompare(rhs.Handle(), lhs.Handle());
            VERIFY_IS_TRUE(changes == expectedChanges);
        }

        {
            // Test MatchmakingStatusChange for different target session refs.
            auto lhs = MultiplayerSession::Create(env.XboxLiveContext());
            auto rhs = MultiplayerSession::Get(env.XboxLiveContext(), defaultSessionReference, testJson["MultiplayerResponseForComparingSessions"]);

            auto changes = XblMultiplayerSessionCompare(rhs.Handle(), lhs.Handle());
            VERIFY_IS_TRUE(static_cast<XblMultiplayerSessionChangeTypes>(changes & XblMultiplayerSessionChangeTypes::MatchmakingStatusChange) == XblMultiplayerSessionChangeTypes::MatchmakingStatusChange);
        }
    }

    // RAII wrapper for session changed RTA event handler
    class SessionChangedHandler
    {
    public:
        SessionChangedHandler(
            XblContextHandle context,
            std::function<void(const XblMultiplayerSessionChangeEventArgs&)> handler
        ) noexcept
            : m_handler{ std::move(handler) }
        {
            VERIFY_SUCCEEDED(XblContextDuplicateHandle(context, &m_context));
            m_token = XblMultiplayerAddSessionChangedHandler(m_context,
                [](void* context, XblMultiplayerSessionChangeEventArgs args)
                {
                    auto pThis{ static_cast<SessionChangedHandler*>(context) };
                    pThis->m_handler(args);
                }, this
            );
        }

        ~SessionChangedHandler() noexcept
        {
            XblMultiplayerRemoveSessionChangedHandler(m_context, m_token);
            XblContextCloseHandle(m_context);
        }

    private:
        XblFunctionContext m_token{ 0 };
        XblContextHandle m_context{ nullptr };
        std::function<void(const XblMultiplayerSessionChangeEventArgs&)> m_handler;
    };

    // RAII wrapper for subscription lost handler
    class SubscriptionLostHandler
    {
    public:
        SubscriptionLostHandler(
            XblContextHandle context,
            std::function<void()> handler
        ) noexcept
            : m_handler{ std::move(handler) }
        {
            VERIFY_SUCCEEDED(XblContextDuplicateHandle(context, &m_context));
            m_token = XblMultiplayerAddSubscriptionLostHandler(m_context,
                [](void* context)
                {
                    auto pThis{ static_cast<SubscriptionLostHandler*>(context) };
                    pThis->m_handler();
                }, this
            );
        }

        ~SubscriptionLostHandler() noexcept
        {
            XblMultiplayerRemoveSubscriptionLostHandler(m_context, m_token);
            XblContextCloseHandle(m_context);
        }

    private:
        XblFunctionContext m_token{ 0 };
        XblContextHandle m_context{ nullptr };
        std::function<void()> m_handler;
    };

    DEFINE_TEST_CASE(TestRTAMultiplayer)
    {
        TEST_LOG(L"Test starting: TestRTAMultiplayer");

        MPTestEnv env{};
        auto& mockRtaService{ MockRealTimeActivityService::Instance() };
        
        mockRtaService.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
        {
            if (uri == MPSD_RTA_URI)
            {
                mockRtaService.CompleteSubscribeHandshake(n, JsonUtils::SerializeJson(testJson["rtaConnectionIdJson"]).data());
            }
        });

        VERIFY_SUCCEEDED(XblMultiplayerSetSubscriptionsEnabled(env.XboxLiveContext(), true));

        auto session = MultiplayerSession::Create(env.XboxLiveContext());
        // Subscriptions are not relevant if we are not in the session
        VERIFY_SUCCEEDED(XblMultiplayerSessionJoin(session.Handle(), nullptr, false, false));

        HttpMock mock{ "PUT", MPSD_URI };
        mock.SetResponseBody(defaultSessionJson);

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMultiplayerWriteSessionAsync(
            env.XboxLiveContext(),
            session.Handle(),
            XblMultiplayerSessionWriteMode::CreateNew,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_SUCCEEDED(XblMultiplayerWriteSessionResult(&async, nullptr));

        Event sessionChanged{};
        const JsonValue& rtaUpdateJson{ testJson["rtaSessionUpdateJson"] };

        SessionChangedHandler sessionChangedHandler{ env.XboxLiveContext(),
            [&](const XblMultiplayerSessionChangeEventArgs& args)
        {
            const auto& tap{ rtaUpdateJson["shoulderTaps"].GetArray()[0] };
            auto resourceSplit{ utils::string_split_internal(tap["resource"].GetString(), '~') };

            VERIFY_ARE_EQUAL_STR(resourceSplit[0], args.SessionReference.Scid);
            VERIFY_ARE_EQUAL_STR(resourceSplit[1], args.SessionReference.SessionTemplateName);
            VERIFY_ARE_EQUAL_STR(resourceSplit[2], args.SessionReference.SessionName);
            VERIFY_ARE_EQUAL_STR(tap["branch"].GetString(), args.Branch);
            VERIFY_ARE_EQUAL_UINT(tap["changeNumber"].GetUint64(), args.ChangeNumber);

            sessionChanged.Set();
        }
        };

        Event subscriptionLost{};
        SubscriptionLostHandler subLostHandler{ env.XboxLiveContext(), [&] { subscriptionLost.Set(); } };

        // Send an rta event and wait until our session changed handler is invoked
        mockRtaService.RaiseEvent(MPSD_RTA_URI, JsonUtils::SerializeJson(rtaUpdateJson).data());
        sessionChanged.Wait();

        // Disconnect socket and wait until our subscription lost handler is invoked
        MockWebsocket::SetConnectHandler([] { return WebsocketResult{ E_FAIL }; });
        mockRtaService.DisconnectClient(env.XboxLiveContext()->Xuid());
        subscriptionLost.Wait();
    }

    DEFINE_TEST_CASE(TestRTADisableMultiplayerSubscriptions)
    {
        TEST_LOG(L"Test starting: TestRTADisableMultiplayerSubscriptions");

        MPTestEnv env{};

        // Auto confirm subscriptions
        auto& mockRtaService{ MockRealTimeActivityService::Instance() };
        mockRtaService.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
        {
            if (uri == MPSD_RTA_URI)
            {
                mockRtaService.CompleteSubscribeHandshake(n, JsonUtils::SerializeJson(testJson["rtaConnectionIdJson"]).data());
            }
        });

        VERIFY_SUCCEEDED(XblMultiplayerSetSubscriptionsEnabled(env.XboxLiveContext(), true));

        Event subLost;
        SubscriptionLostHandler handler{
            env.XboxLiveContext(),
            [&] { subLost.Set(); }
        };

        VERIFY_SUCCEEDED(XblMultiplayerSetSubscriptionsEnabled(env.XboxLiveContext(), false));
        subLost.Wait();
    }

    DEFINE_TEST_CASE(TestMultiplayerSubscribeChangeTypes)
    {
        TEST_LOG(L"Test starting: TestMultiplayerSubscribeChangeTypes");

        MPTestEnv env{};

        auto session = MultiplayerSession::Create(env.XboxLiveContext());
        VERIFY_SUCCEEDED(XblMultiplayerSessionJoin(session.Handle(), nullptr, false, false));

        VERIFY_IS_TRUE(XblMultiplayerSessionSubscribedChangeTypes(session.Handle()) == XblMultiplayerSessionChangeTypes::None);

        XblMultiplayerSessionChangeTypes changeTypes
        {
            XblMultiplayerSessionChangeTypes::CustomPropertyChange |
            XblMultiplayerSessionChangeTypes::HostDeviceTokenChange |
            XblMultiplayerSessionChangeTypes::InitializationStateChange |
            XblMultiplayerSessionChangeTypes::MatchmakingStatusChange |
            XblMultiplayerSessionChangeTypes::MemberCustomPropertyChange |
            XblMultiplayerSessionChangeTypes::MemberListChange |
            XblMultiplayerSessionChangeTypes::MemberStatusChange |
            XblMultiplayerSessionChangeTypes::SessionJoinabilityChange
        };

        VERIFY_SUCCEEDED(XblMultiplayerSessionSetSessionChangeSubscription(session.Handle(), changeTypes));
        VERIFY_IS_TRUE(XblMultiplayerSessionSubscribedChangeTypes(session.Handle()) == changeTypes);

        auto mock = std::make_shared<HttpMock>( "PUT", MPSD_URI );
        mock->SetResponseBody(defaultSessionJson);

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock* /*mock*/, xsapi_internal_string /*uri*/, xsapi_internal_string body)
            {
                // Skip verification of the subId since its a random GUID
                JsonDocument bodyJson;
                bodyJson.Parse(body.data());
                if (bodyJson.HasMember("members") &&
                    bodyJson["members"].HasMember("me") &&
                    bodyJson["members"]["me"].HasMember("properties") &&
                    bodyJson["members"]["me"]["properties"].HasMember("system") &&
                    bodyJson["members"]["me"]["properties"]["system"].HasMember("subscription"))
                {
                    bodyJson["members"]["me"]["properties"]["system"]["subscription"].EraseMember("id");
                }

                requestWellFormed = (testJson["setSessionChangeTypesJson"] == bodyJson);
            }
        );

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMultiplayerWriteSessionAsync(
            env.XboxLiveContext(),
            session.Handle(),
            XblMultiplayerSessionWriteMode::UpdateOrCreateNew,
            &async)
        );

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
        VERIFY_SUCCEEDED(XblMultiplayerWriteSessionResult(&async, nullptr));
    }

    DEFINE_TEST_CASE(Test204Response)
    {
        TEST_LOG(L"Test starting: Test204Response");

        MPTestEnv env{};

        HttpMock mock{ "", MPSD_URI, 204 };
        mock.SetResponseBody(JsonValue{ rapidjson::kObjectType });

        {
            auto session = MultiplayerSession::Create(env.XboxLiveContext());

            XAsyncBlock async{};
            VERIFY_SUCCEEDED(XblMultiplayerWriteSessionAsync(
                env.XboxLiveContext(),
                session.Handle(),
                XblMultiplayerSessionWriteMode::CreateNew,
                &async
            ));

            auto hr = XAsyncGetStatus(&async, true);
            VERIFY_ARE_EQUAL(S_OK, hr);
            hr = XblMultiplayerWriteSessionResult(&async, nullptr);
            VERIFY_ARE_EQUAL(S_OK, hr);
        }

        {
            XblMultiplayerSessionReference ref
            {
                "8d050174-412b-4d51-a29b-d55a34edfdb7",
                "integration",
                "19de0095d8bb41048f19edbbb6bc6b04"
            };

            XAsyncBlock async{};
            VERIFY_SUCCEEDED(XblMultiplayerGetSessionAsync(env.XboxLiveContext(), &ref, &async));

            auto hr = XAsyncGetStatus(&async, true);
            VERIFY_ARE_EQUAL(__HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND), hr);
        }
    }

    DEFINE_TEST_CASE(TestMultiplayerInvalidArgs)
    {
        TEST_LOG(L"Test starting: TestMultiplayerInvalidArgs");

#pragma warning(push)
#pragma warning(disable : 6387)
        MPTestEnv env{};
        auto context{ env.XboxLiveContext() };
        XAsyncBlock async{};
        auto dummySession{ std::make_shared<XblMultiplayerSession>(context->Xuid()) };

        // Create a session with an invalid SessionRef
        // TODO there is no validation here right now
        // TODO should XblMutliplayerSessionCreateHandle return an HR
        //XblMultiplayerSessionReference ref{};
        //XblMultiplayerSessionCreateHandle(context->Xuid(), &ref, nullptr);
        //VERIFY_ARE_EQUAL(E_INVALIDARG, hr);

        // Write null session
        VERIFY_INVALIDARG(XblMultiplayerWriteSessionAsync(context, nullptr, XblMultiplayerSessionWriteMode::UpdateOrCreateNew, &async));

        // Get session with null sessionRef
        VERIFY_INVALIDARG(XblMultiplayerGetSessionAsync(context, nullptr, &async));

        // Query sessions with invalid query params
        XblMultiplayerSessionQuery query
        {
            MOCK_SCID,  // scid
            100,    // maxItems
            false,  // includePrivateSessions
            false,  // includeReservations
            false,  // includeInactiveSessions
            nullptr,    // xuidFilters
            0,  // xuidFiltersCount
            "keyword",    // keywordFilter
            "bar",  // sessionTemplateNameFilter
            XblMultiplayerSessionVisibility::Unknown, // visibilityFilter (invalid, should never be Unknown)
            0 // contractVersionFilter
        };

        VERIFY_INVALIDARG(XblMultiplayerQuerySessionsAsync(context, &query, &async));

        query.VisibilityFilter = XblMultiplayerSessionVisibility::Any;
        query.KeywordFilter = nullptr; // Must specify either Xuid filters or Keyword filters
        VERIFY_INVALIDARG(XblMultiplayerQuerySessionsAsync(context, &query, &async));

        query.IncludeReservations = true; // Must specify Xuid filter to use this
        VERIFY_INVALIDARG(XblMultiplayerQuerySessionsAsync(context, &query, &async));

        query.IncludeReservations = false;
        query.IncludeInactiveSessions = true; // Must specify Xuid filter to use this
        VERIFY_INVALIDARG(XblMultiplayerQuerySessionsAsync(context, &query, &async));

        // Write by handle with null session
        VERIFY_INVALIDARG(XblMultiplayerWriteSessionByHandleAsync(
            context,
            nullptr,
            XblMultiplayerSessionWriteMode::CreateNew,
            "handle",
            &async
        ));

        // Write by handle with null handle
        VERIFY_INVALIDARG(XblMultiplayerWriteSessionByHandleAsync(
            context,
            dummySession.get(),
            XblMultiplayerSessionWriteMode::CreateNew,
            nullptr,
            &async
        ));

        // Get session with null handle
        VERIFY_INVALIDARG(XblMultiplayerGetSessionByHandleAsync(context, nullptr, &async));

        const uint64_t xuids[] = { 1234 };

        // Get activities with null scid
        VERIFY_INVALIDARG(XblMultiplayerGetActivitiesForUsersAsync(context, nullptr, xuids, _countof(xuids), &async));
        VERIFY_INVALIDARG(XblMultiplayerGetActivitiesWithPropertiesForUsersAsync(context, nullptr, xuids, _countof(xuids), &async));

        // Get activities with null xuids array
        VERIFY_INVALIDARG(XblMultiplayerGetActivitiesForUsersAsync(context, MOCK_SCID, nullptr, _countof(xuids), &async));
        VERIFY_INVALIDARG(XblMultiplayerGetActivitiesWithPropertiesForUsersAsync(context, MOCK_SCID, nullptr, _countof(xuids), &async));

        // Get activities with empty xuids array
        VERIFY_INVALIDARG(XblMultiplayerGetActivitiesForUsersAsync(context, MOCK_SCID, xuids, 0, &async));
        VERIFY_INVALIDARG(XblMultiplayerGetActivitiesWithPropertiesForUsersAsync(context, MOCK_SCID, xuids, 0, &async));

        // Set activity with null sessionRef
        VERIFY_INVALIDARG(XblMultiplayerSetActivityAsync(context, nullptr, &async));

        // Clear activity with null scid
        VERIFY_INVALIDARG(XblMultiplayerClearActivityAsync(context, nullptr, &async));

        XblMultiplayerSessionReference ref
        {
            "0001",
            "testTemplate",
            "testSessionName"
        };

        // Send invites with null xuids array
        VERIFY_INVALIDARG(XblMultiplayerSendInvitesAsync(context, &ref, nullptr, _countof(xuids), MOCK_TITLEID, nullptr, nullptr, &async));

        // Send invites with empty xuids array
        VERIFY_INVALIDARG(XblMultiplayerSendInvitesAsync(context, &ref, xuids, 0, MOCK_TITLEID, nullptr, nullptr, &async));

        // Send invites with null sessionRef
        VERIFY_INVALIDARG(XblMultiplayerSendInvitesAsync(context, nullptr, xuids, _countof(xuids), MOCK_TITLEID, nullptr, nullptr, &async));

        VERIFY_SUCCEEDED(XblMultiplayerSetSubscriptionsEnabled(context, true));

        // Setting constants on an pre-existing session
        auto session{ MultiplayerSession::Get(context) };

        VERIFY_ARE_EQUAL(
            E_UNEXPECTED,
            XblMultiplayerSessionConstantsSetTimeouts(session.Handle(), 0, 0, 0, 0)
        );

        VERIFY_ARE_EQUAL(
            E_UNEXPECTED,
            XblMultiplayerSessionConstantsSetQosConnectivityMetrics(session.Handle(), true, true, true, true)
        );

        VERIFY_ARE_EQUAL(
            E_UNEXPECTED,
            XblMultiplayerSessionConstantsSetMemberInitialization(session.Handle(), XblMultiplayerMemberInitialization{})
        );

        VERIFY_ARE_EQUAL(
            E_UNEXPECTED,
            XblMultiplayerSessionConstantsSetPeerToPeerRequirements(session.Handle(), XblMultiplayerPeerToPeerRequirements{})
        );

        VERIFY_ARE_EQUAL(
            E_UNEXPECTED,
            XblMultiplayerSessionConstantsSetPeerToHostRequirements(session.Handle(), XblMultiplayerPeerToHostRequirements{})
        );

        VERIFY_ARE_EQUAL(
            E_UNEXPECTED,
            XblMultiplayerSessionConstantsSetCapabilities(session.Handle(), XblMultiplayerSessionCapabilities{})
        );

        VERIFY_SUCCEEDED(XblMultiplayerSessionJoin(session.Handle(), nullptr, false, false));
        // Double join a session
        VERIFY_ARE_EQUAL(
            E_UNEXPECTED,
            XblMultiplayerSessionJoin(session.Handle(), nullptr, false, false)
        );
        // Leave a session without being joined
        // TODO should this really be an error? even though the join isn't written, this feels like it should be valid
        VERIFY_ARE_EQUAL(
            E_UNEXPECTED,
            XblMultiplayerSessionLeave(session.Handle())
        );
#pragma warning(pop)
    }

    DEFINE_TEST_CASE(TestTournamentsDeprecated)
    {
        TEST_LOG(L"Test starting: TestTournamentsDeprecated");

        XBL_WARNING_PUSH;
        XBL_WARNING_DISABLE_DEPRECATED;
        XblTournamentReference xbltourref{};
        memcpy(xbltourref.Scid, "1234", XBL_SCID_LENGTH);

        HRESULT hr = XblMultiplayerSessionConstantsSetArbitrationTimeouts(nullptr, 0, 0);
        VERIFY_ARE_EQUAL(hr, E_NOTIMPL);

        auto result = XblMultiplayerSessionArbitrationServer(nullptr);
        VERIFY_ARE_EQUAL(result, nullptr);

        auto result2 = XblMultiplayerSessionTournamentsServer(nullptr);
        VERIFY_ARE_EQUAL(result2, nullptr);

        auto result3 = XblMultiplayerSessionArbitrationStatus(nullptr);
        VERIFY_IS_TRUE(result3 == XblTournamentArbitrationStatus::Incomplete);

        hr = XblMultiplayerEventArgsTournamentRegistrationStateChanged(nullptr, nullptr, nullptr);
        VERIFY_ARE_EQUAL(hr, E_NOTIMPL);

        hr = XblMultiplayerEventArgsTournamentGameSessionReady(nullptr, nullptr);
        VERIFY_ARE_EQUAL(hr, E_NOTIMPL);

        auto result4 = XblMultiplayerManagerLobbySessionLastTournamentTeamResult();
        VERIFY_ARE_EQUAL(result4, nullptr);

        XblMultiplayerSessionCapabilities cap{};
        cap.Team = true;

        XblMultiplayerTournamentsServer server{};
        XBL_WARNING_POP;
    }
};

const JsonDocument MultiplayerTests::testJson{ GetTestResponses("TestResponses\\Multiplayer.json") };
const JsonValue& MultiplayerTests::defaultSessionJson{ testJson["defaultSessionDocument"] };

const XblMultiplayerSessionReference MultiplayerTests::defaultSessionReference
{
    "8d050174-412b-4d51-a29b-d55a34edfdb7",
    "integration",
    "19de0095d8bb41048f19edbbb6bc6b04"
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
