// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

XblMultiplayerSessionMember MultiplayerSessionMember::Construct(
    _In_ bool isCurrentUser,
    _In_ const xsapi_internal_string& memberIdToWrite,
    _In_ uint64_t xuid,
    _In_opt_z_ const char* customConstantsJson,
    _In_ bool initializeRequested
)
{
    XblMultiplayerSessionMember member{};

    auto memberInternal = Make<MultiplayerSessionMember>(memberIdToWrite);
    member.Internal = memberInternal;

    member.Xuid = xuid;
    member.IsCurrentUser = isCurrentUser;
    if (customConstantsJson)
    {
        memberInternal->m_customConstantsJson = customConstantsJson;
        member.CustomConstantsJson = memberInternal->m_customConstantsJson.data();
    }
    member.Nat = XblNetworkAddressTranslationSetting::Unknown;
    member.InitializationFailureCause = XblMultiplayerMeasurementFailure::None;
    memberInternal->m_subscribedChangeTypes = XblMultiplayerSessionChangeTypes::None;
    member.InitializeRequested = initializeRequested;
    return member;
}

MultiplayerSessionMember::MultiplayerSessionMember(const xsapi_internal_string& memberIdToWrite)
    : MultiplayerSessionMember()
{
    m_memberIdToWrite = memberIdToWrite;
    m_newMember = true;
}

MultiplayerSessionMember::MultiplayerSessionMember(const MultiplayerSessionMember& other) :
    m_customConstantsJson(other.m_customConstantsJson),
    m_customPropertiesString(other.m_customPropertiesString),
    m_secureDeviceAddressBase64(other.m_secureDeviceAddressBase64),
    m_roles(other.m_roles),
    m_teamId(other.m_teamId),
    m_initialTeam(other.m_initialTeam),
    m_membersInGroupIds(other.m_membersInGroupIds),
    m_subscribedChangeTypes(other.m_subscribedChangeTypes),
    m_matchmakingResultServerMeasurementsJson(other.m_matchmakingResultServerMeasurementsJson),
    m_serverMeasurementsJson(other.m_serverMeasurementsJson),
    m_qosMeasurementsJson(other.m_qosMeasurementsJson),
    m_subscriptionId(other.m_subscriptionId),
    m_rtaConnectionId(other.m_rtaConnectionId),
    m_memberIdToWrite(other.m_memberIdToWrite),
    m_newMember(other.m_newMember),
    m_writeConstants(other.m_writeConstants),
    m_writeIsActive(other.m_writeIsActive),
    m_writeRoleInfo(other.m_writeRoleInfo),
    m_writeSecureDeviceAddressBase64(other.m_writeSecureDeviceAddressBase64),
    m_writeQoSMeasurementsJson(other.m_writeQoSMeasurementsJson),
    m_writeServerMeasurementsJson(other.m_writeServerMeasurementsJson),
    m_writeMembersInGroup(other.m_writeMembersInGroup),
    m_writeGroups(other.m_writeGroups),
    m_writeEncounters(other.m_writeEncounters),
    m_writeSubscribedChangeTypes(other.m_writeSubscribedChangeTypes),
    m_writeResults(other.m_writeResults),
    m_writeCustomPropertiesJson(other.m_writeCustomPropertiesJson)
{
    for (auto group : other.m_groups)
    {
        m_groups.push_back(Make(group));
    }
    for (auto encounter : other.m_encounters)
    {
        m_encounters.push_back(Make(encounter));
    }
    for (auto& role : m_roles)
    {
        role.roleName = Make(role.roleName);
        role.roleTypeName = Make(role.roleTypeName);
    }

    JsonUtils::CopyFrom(m_customPropertiesJson, other.m_customPropertiesJson);
    JsonUtils::CopyFrom(m_resultsJson, other.m_resultsJson);
}

MultiplayerSessionMember::~MultiplayerSessionMember()
{
    for (auto group : m_groups)
    {
        Delete(group);
    }
    for (auto encounter : m_encounters)
    {
        Delete(encounter);
    }
    for (auto& role : m_roles)
    {
        Delete(role.roleName);
        Delete(role.roleTypeName);
    }
}

xsapi_internal_string MultiplayerSessionMember::MemberId() const
{
    return m_member->IsCurrentUser ? "me" : m_memberIdToWrite;
}

void 
MultiplayerSessionMember::SetSecureDeviceBaseAddress64(_In_ const xsapi_internal_string& deviceBaseAddress)
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockMember };
    m_secureDeviceAddressBase64 = deviceBaseAddress;
    m_member->SecureDeviceBaseAddress64 = m_secureDeviceAddressBase64.data();
    m_writeSecureDeviceAddressBase64 = true;
}

void
MultiplayerSessionMember::SetRoles(_In_ const xsapi_internal_vector<XblMultiplayerSessionMemberRole>& roles)
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockMember };

    for (auto& role : m_roles)
    {
        Delete(role.roleName);
        Delete(role.roleTypeName);
    }
    m_roles = roles;
    for (auto& role : m_roles)
    {
        role.roleName = Make(role.roleName);
        role.roleTypeName = Make(role.roleTypeName);
    }
    m_member->Roles = m_roles.data();
    m_member->RolesCount = m_roles.size();
    m_writeRoleInfo = true;
}

const xsapi_internal_vector<const char*>& MultiplayerSessionMember::GroupsUnsafe() const
{
    return m_groups;
}

void MultiplayerSessionMember::SetGroups(
    _In_reads_(groupsCount) const char** groups,
    _In_ size_t groupsCount
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockMember };
    for (auto& group : m_groups)
    {
        Delete(group);
    }
    m_groups.clear();
    for (size_t i = 0; i < groupsCount; ++i)
    {
        m_groups.push_back(Make(groups[i]));
    }
    m_member->Groups = m_groups.data();
    m_member->GroupsCount = m_groups.size();
    m_writeGroups = true;
}

const xsapi_internal_vector<const char*>&
MultiplayerSessionMember::EncountersUnSafe() const
{
    return m_encounters;
}

void 
MultiplayerSessionMember::SetEncounters(
    _In_reads_(encountersCount) const char** encounters,
    _In_ size_t encountersCount
)
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockMember };
    for (auto& encounter : m_encounters)
    {
        Delete(encounter);
    }
    m_encounters.clear();
    for (uint32_t i = 0; i < encountersCount; ++i)
    {
        m_encounters.push_back(Make(encounters[i]));
    }
    m_member->Encounters = m_encounters.data();
    m_member->EncountersCount = m_encounters.size();
    m_writeEncounters = true;
}

HRESULT MultiplayerSessionMember::SetStatus(
    _In_ XblMultiplayerSessionMemberStatus status
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockMember };
    XSAPI_ASSERT(m_member->IsCurrentUser);
    if (status != XblMultiplayerSessionMemberStatus::Active && status != XblMultiplayerSessionMemberStatus::Inactive)
    {
        return E_INVALIDARG;
    }

    m_member->Status = status;
    m_writeIsActive = true;
    return S_OK;
}

void MultiplayerSessionMember::StateLock() const
{
    m_lockMember.lock();
}

void MultiplayerSessionMember::StateUnlock() const
{
    m_lockMember.unlock();
}

const xsapi_internal_vector<uint32_t>& MultiplayerSessionMember::MembersInGroupUnsafe() const
{
    return m_membersInGroupIds;
}

void MultiplayerSessionMember::SetMembersInGroup(
    _In_ const xsapi_internal_vector<uint32_t>& membersInGroup
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockMember };
    m_membersInGroupIds = membersInGroup;
    m_member->MembersInGroupIds = m_membersInGroupIds.data();
    m_member->MembersInGroupCount = static_cast<uint32_t>(m_membersInGroupIds.size());
    m_writeMembersInGroup = true;
}

HRESULT
MultiplayerSessionMember::SetCustomPropertyJson(
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& valueJson
)
{
    if (name.empty())
    {
        return E_INVALIDARG;
    }

    if (!m_member->IsCurrentUser)
    {
        return E_UNEXPECTED;
    }

    std::lock_guard<std::recursive_mutex> lock{ m_lockMember };

    auto hr = JsonUtils::SetMember(m_customPropertiesJson, name, valueJson);
    if (SUCCEEDED(hr))
    {
        m_customPropertiesString = JsonUtils::SerializeJson(m_customPropertiesJson);
        m_member->CustomPropertiesJson = m_customPropertiesString.data();
        m_writeCustomPropertiesJson = true;
    }

    return hr;
}

void 
MultiplayerSessionMember::DeleteCustomPropertyJson(
    _In_ const xsapi_internal_string& name
    )
{
    SetCustomPropertyJson(name, JsonValue());
}

HRESULT MultiplayerSessionMember::SetQosMeasurementsJson(
    _In_ const xsapi_internal_string& qosMeasurementsJson
    )
{
    auto hr = JsonUtils::ValidateJson(qosMeasurementsJson.data());
    if (SUCCEEDED(hr))
    {
        std::lock_guard<std::recursive_mutex> lock{ m_lockMember };
        m_qosMeasurementsJson = qosMeasurementsJson;
        m_member->QosMeasurementsJson = m_qosMeasurementsJson.data();
        m_writeQoSMeasurementsJson = true;
    }
    return hr;
}

HRESULT MultiplayerSessionMember::SetServerMeasurementsJson(
    _In_ const xsapi_internal_string& serverMeasurementsJson
    )
{
    auto hr = JsonUtils::ValidateJson(serverMeasurementsJson.data());
    if (SUCCEEDED(hr))
    {
        std::lock_guard<std::recursive_mutex> lock{ m_lockMember };
        m_serverMeasurementsJson = serverMeasurementsJson;
        m_member->ServerMeasurementsJson = m_serverMeasurementsJson.data();
        m_writeServerMeasurementsJson = true;
    }
    return hr;
}

XblMultiplayerSessionChangeTypes 
MultiplayerSessionMember::SubscribedChangeTypes() const
{
    return m_subscribedChangeTypes;
}

void 
MultiplayerSessionMember::SetSessionChangeSubscription(
    _In_ XblMultiplayerSessionChangeTypes changeTypes, 
    _In_ const xsapi_internal_string& subscriptionId
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockMember };
    m_subscribedChangeTypes = changeTypes;
    m_subscriptionId = subscriptionId;
    m_writeSubscribedChangeTypes = true;
}

const JsonValue& MultiplayerSessionMember::CustomPropertiesJsonUnsafe() const
{
    return m_customPropertiesJson;
}

void MultiplayerSessionMember::SetRtaConnectionId(
    _In_ const xsapi_internal_string& rtaConnectionId
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockMember };
    LOGS_DEBUG << "MultiplayerSessionMember::SetRtaConnectionId " << rtaConnectionId;
    m_rtaConnectionId = rtaConnectionId;
}

Result<XblMultiplayerSessionMember> MultiplayerSessionMember::Deserialize(
    _In_ const JsonValue& json
)
{
    XblMultiplayerSessionMember returnResult{};
    if (json.IsNull())
    {
        return returnResult;
    }
    auto returnResultInternal = Make<MultiplayerSessionMember>();
    returnResult.Internal = returnResultInternal;

    bool reserved = false;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(json, "reserved", reserved));
    bool active = false;
    bool ready = false;


    returnResultInternal->m_customConstantsJson = "";
    returnResult.Xuid = 0;
    returnResult.InitializeRequested = false;
    returnResultInternal->m_teamId = "";
    returnResultInternal->m_initialTeam = "";
    returnResultInternal->m_matchmakingResultServerMeasurementsJson = "";

    if (json.IsObject() && json.HasMember("constants"))
    {
        const JsonValue& constantsJson = json["constants"];

        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonFieldAsString(constantsJson, "custom", returnResultInternal->m_customConstantsJson, false));
        
        if (constantsJson.IsObject() && constantsJson.HasMember("custom"))
        {
            const JsonValue& constantsCustomJson = constantsJson["custom"];
            if (constantsCustomJson.IsObject() && constantsCustomJson.HasMember("matchmakingResult"))
            {
                const JsonValue& constantsCustomMatchmakingResultJson = constantsCustomJson["matchmakingResult"];
                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(constantsCustomMatchmakingResultJson, "initialTeam", returnResultInternal->m_initialTeam, false));                
            }
        }

        if (constantsJson.IsObject() && constantsJson.HasMember("system"))
        {
            const JsonValue& constantsSystemJson = constantsJson["system"];

            xsapi_internal_string xuid;
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(constantsSystemJson, "xuid", xuid));
            returnResult.Xuid = utils::internal_string_to_uint64(xuid);
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(constantsSystemJson, "initialize", returnResult.InitializeRequested));
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(constantsSystemJson, "team", returnResultInternal->m_teamId));
            
            if (constantsSystemJson.IsObject() && constantsSystemJson.HasMember("matchmakingResult"))
            {
                const JsonValue& constantsSystemMatchmakingResultJson = constantsSystemJson["matchmakingResult"];
                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonFieldAsString(constantsSystemMatchmakingResultJson, "serverMeasurements", returnResultInternal->m_matchmakingResultServerMeasurementsJson, false));
            }
        }
        else
        {
            //required
            return WEB_E_INVALID_JSON_STRING;
        }
    }
    else
    {
        //required
        return WEB_E_INVALID_JSON_STRING;
    }

    returnResult.CustomConstantsJson = returnResultInternal->m_customConstantsJson.data();
    returnResult.InitialTeam = returnResultInternal->m_initialTeam.data();
    returnResult.MatchmakingResultServerMeasurementsJson = returnResultInternal->m_matchmakingResultServerMeasurementsJson.data();

    returnResultInternal->m_secureDeviceAddressBase64 = "";
    returnResultInternal->m_serverMeasurementsJson = "";
    returnResultInternal->m_qosMeasurementsJson = "";
    returnResultInternal->m_customPropertiesString = "";

    if (json.IsObject() && json.HasMember("properties"))
    {
        const JsonValue& propertiesJson = json["properties"];
        if (propertiesJson.IsObject() && propertiesJson.HasMember("custom"))
        {
            JsonUtils::CopyFrom(returnResultInternal->m_customPropertiesJson, propertiesJson["custom"]);
            returnResultInternal->m_customPropertiesString = JsonUtils::SerializeJson(returnResultInternal->m_customPropertiesJson);
        }

        if (propertiesJson.IsObject() && propertiesJson.HasMember("system"))
        {
            const JsonValue& propertiesSystemJson = propertiesJson["system"];

            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(propertiesSystemJson, "secureDeviceAddress", returnResultInternal->m_secureDeviceAddressBase64));
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonFieldAsString(propertiesSystemJson, "serverMeasurements", returnResultInternal->m_serverMeasurementsJson, false));
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonFieldAsString(propertiesSystemJson, "measurements", returnResultInternal->m_qosMeasurementsJson, false));
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<uint32_t>(JsonUtils::JsonIntExtractor, propertiesSystemJson, "initializationGroup", returnResultInternal->m_membersInGroupIds, false));
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(propertiesSystemJson, "active", active));
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(propertiesSystemJson, "ready", ready));

            if (propertiesSystemJson.IsObject() && propertiesSystemJson.HasMember("subscription"))
            {
                const JsonValue& propertiesSystemSubscriptionJson = propertiesSystemJson["subscription"];

                xsapi_internal_vector<xsapi_internal_string> changeTypes;
                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<xsapi_internal_string>(JsonUtils::JsonStringExtractor, propertiesSystemSubscriptionJson, "changeTypes", changeTypes, false));
                returnResultInternal->m_subscribedChangeTypes = Serializers::MultiplayerSessionChangeTypesFromStringVector(changeTypes);
            }

            if (propertiesSystemJson.IsObject() && propertiesSystemJson.HasMember("groups"))
            {
                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<const char*>(JsonUtils::JsonUtf8Extractor, propertiesSystemJson, "groups", returnResultInternal->m_groups, false));
            }

            if (propertiesSystemJson.IsObject() && propertiesSystemJson.HasMember("encounters"))
            {
                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<const char*>(JsonUtils::JsonUtf8Extractor, propertiesSystemJson, "encounters", returnResultInternal->m_encounters, false));
            }
        }
        else
        {
            //required
            return WEB_E_INVALID_JSON_STRING;
        }
    }
    else
    {
        //required
        return WEB_E_INVALID_JSON_STRING;
    }

    returnResult.SecureDeviceBaseAddress64 = returnResultInternal->m_secureDeviceAddressBase64.data();
    returnResult.ServerMeasurementsJson = returnResultInternal->m_serverMeasurementsJson.data();
    returnResult.QosMeasurementsJson = returnResultInternal->m_qosMeasurementsJson.data();
    returnResult.MembersInGroupIds = returnResultInternal->m_membersInGroupIds.data();
    returnResult.MembersInGroupCount = static_cast<uint32_t>(returnResultInternal->m_membersInGroupIds.size());
    returnResult.Groups = returnResultInternal->m_groups.data();
    returnResult.GroupsCount = static_cast<uint32_t>(returnResultInternal->m_groups.size());
    returnResult.Encounters = returnResultInternal->m_encounters.data();
    returnResult.EncountersCount = static_cast<uint32_t>(returnResultInternal->m_encounters.size());
    returnResult.CustomPropertiesJson = returnResultInternal->m_customPropertiesString.data();

    if (active)
    {
        returnResult.Status = XblMultiplayerSessionMemberStatus::Active;
    }
    else if (ready)
    {
        returnResult.Status = XblMultiplayerSessionMemberStatus::Ready;
    }
    else if (reserved)
    {
        returnResult.Status = XblMultiplayerSessionMemberStatus::Reserved;
    }
    else
    {
        returnResult.Status = XblMultiplayerSessionMemberStatus::Inactive;
    }
    xsapi_internal_string gamertag, deviceToken, nat;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "gamertag", gamertag));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "deviceToken", deviceToken));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "nat", nat));
    utils::strcpy(returnResult.Gamertag, sizeof(returnResult.Gamertag), gamertag.c_str());
    utils::strcpy(returnResult.DeviceToken.Value, sizeof(returnResult.DeviceToken.Value), deviceToken.c_str());
    returnResult.Nat = Serializers::MultiplayerNatSettingFromString(nat);
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(json, "turn", returnResult.IsTurnAvailable));

    if (json.IsObject() && json.HasMember("roles"))
    {
        const JsonValue& rolesJson = json["roles"];
        if (!rolesJson.IsNull() && rolesJson.IsObject())
        {
            for (const auto& rolePair : rolesJson.GetObject())
            {
                XblMultiplayerSessionMemberRole role{};
                role.roleTypeName = Make(rolePair.name.GetString());
                role.roleName = Make(rolePair.value.GetString());
                returnResultInternal->m_roles.push_back(std::move(role));
            }
            returnResult.Roles = returnResultInternal->m_roles.data();
            returnResult.RolesCount = returnResultInternal->m_roles.size();
        }
    }

    xsapi_internal_string titleIdString;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "activeTitleId", titleIdString));
    if (!titleIdString.empty())
    {
        returnResult.ActiveTitleId = utils::internal_string_to_uint32(titleIdString);
    }

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(json, "joinTime", returnResult.JoinTime));
    xsapi_internal_string initializationFailure;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "initializationFailure", initializationFailure))
    returnResult.InitializationFailureCause = Serializers::MultiplayerMeasurementFailureFromString(initializationFailure);
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(json, "initializationEpisode", returnResult.InitializationEpisode));

    return returnResult;
}

xsapi_internal_vector<xsapi_internal_string> MultiplayerSessionMember::GetVectorViewForChangeTypes(
    _In_ XblMultiplayerSessionChangeTypes changeTypes
)
{
    xsapi_internal_vector<xsapi_internal_string> resultVector;
    if ((changeTypes & XblMultiplayerSessionChangeTypes::Everything) == XblMultiplayerSessionChangeTypes::Everything)
    {
        resultVector.push_back("everything");
    }
    if ((changeTypes & XblMultiplayerSessionChangeTypes::HostDeviceTokenChange) == XblMultiplayerSessionChangeTypes::HostDeviceTokenChange)
    {
        resultVector.push_back("host");
    }
    if ((changeTypes & XblMultiplayerSessionChangeTypes::InitializationStateChange) == XblMultiplayerSessionChangeTypes::InitializationStateChange)
    {
        resultVector.push_back("initialization");
    }
    if ((changeTypes & XblMultiplayerSessionChangeTypes::MatchmakingStatusChange) == XblMultiplayerSessionChangeTypes::MatchmakingStatusChange)
    {
        resultVector.push_back("matchmakingStatus");
    }
    if ((changeTypes & XblMultiplayerSessionChangeTypes::MemberListChange) == XblMultiplayerSessionChangeTypes::MemberListChange)
    {
        resultVector.push_back("membersList");
    }
    if ((changeTypes & XblMultiplayerSessionChangeTypes::MemberStatusChange) == XblMultiplayerSessionChangeTypes::MemberStatusChange)
    {
        resultVector.push_back("membersStatus");
    }
    if ((changeTypes & XblMultiplayerSessionChangeTypes::SessionJoinabilityChange) == XblMultiplayerSessionChangeTypes::SessionJoinabilityChange)
    {
        resultVector.push_back("joinability");
    }
    if ((changeTypes & XblMultiplayerSessionChangeTypes::CustomPropertyChange) == XblMultiplayerSessionChangeTypes::CustomPropertyChange)
    {
        resultVector.push_back("customProperty");
    }
    if ((changeTypes & XblMultiplayerSessionChangeTypes::MemberCustomPropertyChange) == XblMultiplayerSessionChangeTypes::MemberCustomPropertyChange)
    {
        resultVector.push_back("membersCustomProperty");
    }

    return resultVector;
}

void MultiplayerSessionMember::Serialize(_Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator)
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockMember };
    json.SetObject();
    if (m_newMember)
    {
        JsonValue systemConstantsJson(rapidjson::kObjectType);
        systemConstantsJson.AddMember("xuid", JsonValue(utils::uint64_to_internal_string(m_member->Xuid).c_str(), allocator).Move(), allocator);
        if (m_member->InitializeRequested)
        {
            systemConstantsJson.AddMember("initialize", m_member->InitializeRequested, allocator);
        }

        JsonValue constantsJson(rapidjson::kObjectType);
        constantsJson.AddMember("system", systemConstantsJson, allocator);
        if (!m_customConstantsJson.empty())
        {
            JsonDocument customJson{ &allocator };
            customJson.Parse(m_customConstantsJson.data());
            if (!customJson.IsNull())
            {
                constantsJson.AddMember("custom", customJson, allocator);
            }
        }
        json.AddMember("constants", constantsJson, allocator);
    }

    if (m_newMember || m_member->IsCurrentUser)
    {
        JsonValue propertiesJson(rapidjson::kObjectType);
        JsonValue systemPropertiesJson(rapidjson::kObjectType);

        if (m_writeIsActive)
        {
            bool isActive = m_member->Status == XblMultiplayerSessionMemberStatus::Active;
            systemPropertiesJson.AddMember("active", isActive, allocator);
            if (!isActive)
            {
                systemPropertiesJson.AddMember("ready", isActive, allocator);
            }
            else
            {
                if (!m_rtaConnectionId.empty())
                {
                    systemPropertiesJson.AddMember("connection", JsonValue(m_rtaConnectionId.c_str(), allocator).Move(), allocator);
                    LOGS_DEBUG << "MultiplayerSessionMember::Serialize " << m_rtaConnectionId << " for " << m_member->Xuid;
                }
            }
        }

        if (m_writeRoleInfo && m_member->RolesCount > 0)
        {
            JsonValue rolesJson(rapidjson::kObjectType);
            for (uint32_t i = 0; i < m_member->RolesCount; ++i)
            {
                rolesJson.AddMember(JsonValue(m_member->Roles[i].roleTypeName, allocator).Move(), JsonValue(m_member->Roles[i].roleName, allocator).Move(), allocator);
            }
            json.AddMember("roles", rolesJson, allocator);
        }

        if (m_writeSubscribedChangeTypes)
        {
            JsonValue subscriptionJson(rapidjson::kObjectType);

            subscriptionJson.AddMember("id", JsonValue(m_subscriptionId.c_str(), allocator).Move(), allocator);

            JsonValue changeTypesJson;
            JsonUtils::SerializeVector(JsonUtils::JsonStringSerializer, GetVectorViewForChangeTypes(m_subscribedChangeTypes), changeTypesJson, allocator);
            subscriptionJson.AddMember("changeTypes", changeTypesJson, allocator);

            systemPropertiesJson.AddMember("subscription", subscriptionJson, allocator);
        }

        if (m_writeSecureDeviceAddressBase64)
        {
            systemPropertiesJson.AddMember("secureDeviceAddress", JsonValue(m_member->SecureDeviceBaseAddress64, allocator).Move(), allocator);
        }

        if (m_writeMembersInGroup)
        {
            JsonValue initializationGroupJson(rapidjson::kArrayType);
            JsonUtils::SerializeVector<uint32_t>(JsonUtils::JsonIntSerializer, m_membersInGroupIds, initializationGroupJson, allocator);
            systemPropertiesJson.AddMember("initializationGroup", initializationGroupJson, allocator);
        }

        if (m_writeGroups)
        {
            JsonValue groupsJson(rapidjson::kArrayType);
            JsonUtils::SerializeVector<const char*>(JsonUtils::JsonUtf8Serializer, m_groups, groupsJson, allocator);
            systemPropertiesJson.AddMember("groups", groupsJson, allocator);
        }

        if (m_writeEncounters)
        {
            JsonValue encountersJson(rapidjson::kArrayType);
            JsonUtils::SerializeVector<const char*>(JsonUtils::JsonUtf8Serializer, m_encounters, encountersJson, allocator);
            systemPropertiesJson.AddMember("encounters", encountersJson, allocator);
        }

        if (m_writeQoSMeasurementsJson)
        {
            JsonDocument measurementsJson{ &allocator };
            measurementsJson.Parse(m_qosMeasurementsJson.data());
            systemPropertiesJson.AddMember("measurements", measurementsJson, allocator);
        }

        if (m_writeServerMeasurementsJson)
        {
            JsonDocument serverMeasurementsJson{ &allocator };
            serverMeasurementsJson.Parse(m_serverMeasurementsJson.data());
            systemPropertiesJson.AddMember("serverMeasurements", serverMeasurementsJson, allocator);
        }

        if (systemPropertiesJson.MemberCount())
        {
            propertiesJson.AddMember("system", systemPropertiesJson, allocator);
        }

        if (m_writeCustomPropertiesJson)
        {
            propertiesJson.AddMember("custom", JsonValue{}.CopyFrom(m_customPropertiesJson, allocator).Move(), allocator);
        }

        if (propertiesJson.MemberCount())
        {
            json.AddMember("properties", propertiesJson, allocator);
        }
    }
}

MultiplayerSessionMember* MultiplayerSessionMember::Get(const XblMultiplayerSessionMember* member)
{
    if (member == nullptr || member->Internal == nullptr)
    {
        XSAPI_ASSERT(false);
    }
    return static_cast<MultiplayerSessionMember*>(member->Internal);
}

void MultiplayerSessionMember::SetExternalMemberPointer(XblMultiplayerSessionMember& member)
{
    auto internalMember = Get(&member);
    internalMember->m_member = &member;

    member.InitialTeam = internalMember->m_initialTeam.empty() ? nullptr : internalMember->m_initialTeam.data();
    member.CustomConstantsJson = internalMember->m_customConstantsJson.empty() ? nullptr :internalMember->m_customConstantsJson.data();
    member.SecureDeviceBaseAddress64 = internalMember->m_secureDeviceAddressBase64.empty() ? nullptr : internalMember->m_secureDeviceAddressBase64.data();
    member.Roles = internalMember->m_roles.empty() ? nullptr : internalMember->m_roles.data();
    member.CustomPropertiesJson = internalMember->m_customPropertiesString.empty() ? nullptr : internalMember->m_customPropertiesString.data();
    member.MatchmakingResultServerMeasurementsJson = internalMember->m_matchmakingResultServerMeasurementsJson.empty() ? nullptr : internalMember->m_matchmakingResultServerMeasurementsJson.data();
    member.ServerMeasurementsJson = internalMember->m_serverMeasurementsJson.empty() ? nullptr : internalMember->m_serverMeasurementsJson.data();
    member.MembersInGroupIds = internalMember->m_membersInGroupIds.empty() ? nullptr : internalMember->m_membersInGroupIds.data();
    member.QosMeasurementsJson = internalMember->m_qosMeasurementsJson.empty() ? nullptr : internalMember->m_qosMeasurementsJson.data();
    member.Groups = internalMember->m_groups.empty() ? nullptr : internalMember->m_groups.data();
    member.Encounters = internalMember->m_encounters.empty() ? nullptr : internalMember->m_encounters.data();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
