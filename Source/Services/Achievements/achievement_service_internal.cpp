// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "achievements_internal.h"
#include "xbox_live_context_internal.h"
#include "xsapi-c/errors_c.h"

#include "real_time_activity_manager.h"

#if HC_PLATFORM == HC_PLATFORM_XDK
#pragma pack(push, 16)
#include "EtwPlus.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

#if HC_PLATFORM == HC_PLATFORM_XDK

EXTERN_C __declspec(selectany) ETX_FIELD_DESCRIPTOR XSAPI_Update_Achievement_Fields[5] =
{
    { EtxFieldType_UnicodeString,0 },
    { EtxFieldType_UnicodeString,0 },
    { EtxFieldType_GUID,0 },
    { EtxFieldType_UnicodeString,0 },
    { EtxFieldType_UInt32,0 }
};

EXTERN_C __declspec(selectany) ETX_EVENT_DESCRIPTOR XSAPI_Update_Achievement_Events[1] =
{
    {
        { 1, 1, 0, 0, 0, 0, 0x0 },
        "AchievementUpdate",
        "0.7.IGAU-1.0",
        XSAPI_Update_Achievement_Fields,
        5,
        0,
        EtxEventEnabledState_Undefined,
        EtxEventEnabledState_ProviderDefault,
        EtxPopulationSample_Undefined,
        EtxPopulationSample_UseProviderPopulationSample,
        EtxEventLatency_Undefined,
        EtxEventLatency_ProviderDefault,
        EtxEventPriority_Undefined,
        EtxEventPriority_ProviderDefault
    }
};

EXTERN_C __declspec(selectany) REGHANDLE XSAPI_Update_Achievement_Handle = (REGHANDLE)0;

EXTERN_C __declspec(selectany) ETX_PROVIDER_DESCRIPTOR XSAPI_Update_Achievement_Provider =
{
    "",
    { 0 },
    1,
    (ETX_EVENT_DESCRIPTOR*)&XSAPI_Update_Achievement_Events,
    0,
    EtxProviderEnabledState_Undefined,
    EtxProviderEnabledState_OnByDefault,
    0,
    100,
    EtxProviderLatency_Undefined,
    EtxProviderLatency_RealTime,
    EtxProviderPriority_Undefined,
    EtxProviderPriority_Critical
};
#endif

AchievementsService::AchievementsService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings,
    _In_ std::shared_ptr<AppConfig> appConfig,
    _In_ std::weak_ptr<XblContext> xboxLiveContextImpl,
    _In_ std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> rtaManager
) :
    m_user{ std::move(user) },
    m_xboxLiveContextSettings{ std::move(xboxLiveContextSettings) },
    m_appConfig{ std::move(appConfig) },
    m_rtaManager{ std::move(rtaManager) },
    m_xboxLiveContextImpl{ std::move(xboxLiveContextImpl) }
{
}

HRESULT AchievementsService::UpdateAchievement(
    _In_ uint64_t xboxUserId,
    _In_ const String& achievementId,
    _In_ uint32_t percentComplete,
    _In_ AsyncContext<Result<void>> async
) const noexcept
{
    return UpdateAchievement(
        xboxUserId,
        m_appConfig->TitleId(),
        m_appConfig->Scid(),
        achievementId,
        percentComplete,
        std::move(async)
    );
}

HRESULT AchievementsService::UpdateAchievement(
    _In_ uint64_t xboxUserId,
    _In_ uint32_t titleId,
    _In_ const String& scid,
    _In_ const String& achievementId,
    _In_ uint32_t percentComplete,
    _In_ AsyncContext<Result<void>> async
) const noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(titleId == 0);
    RETURN_HR_INVALIDARGUMENT_IF(scid.empty());
    RETURN_HR_INVALIDARGUMENT_IF(achievementId.empty());
    RETURN_HR_INVALIDARGUMENT_IF(percentComplete > 100);

    // Achievements service is doing case sensitive comparison on scid and always expects it to be lower case
    String lowercaseScid = utils::ToLower(scid);

#if HC_PLATFORM == HC_PLATFORM_XDK
    {
        auto state = GlobalState::Get();
        if (!state)
        {
            return E_XBL_NOT_INITIALIZED;
        }
        // Register ETX provider if it hasn't been registered yet
        if (XSAPI_Update_Achievement_Handle == 0)
        {
            string_t wScid = utils::string_t_from_internal_string(lowercaseScid);
            std::error_code errC = utils::guid_from_string(wScid, const_cast<GUID*>(&XSAPI_Update_Achievement_Provider.Guid), false);
            if (errC)
            {
                return utils::convert_xbox_live_error_code_to_hresult(errC);
            }

            XSAPI_Update_Achievement_Provider.Name = state->AchievementsProviderName().data();

            ULONG errorCode = EtxRegister(&XSAPI_Update_Achievement_Provider, &XSAPI_Update_Achievement_Handle);
            RETURN_HR_IF_FAILED(HRESULT_FROM_WIN32(errorCode));
        }
    }
#endif

    Stringstream subPath;
    subPath << ("/users/xuid(") << xboxUserId << (")/achievements/") << lowercaseScid << ("/update");

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    HRESULT hr = httpCall->Init(
        m_xboxLiveContextSettings,
        "POST",
        XblHttpCall::BuildUrl("achievements", subPath.str()),
        xbox_live_api::update_achievement
    );
    RETURN_HR_IF_FAILED(hr);

    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(2));

    JsonDocument request(rapidjson::kObjectType);
    JsonDocument::AllocatorType& allocator = request.GetAllocator();

    JsonValue achievementJson(rapidjson::kObjectType);
    achievementJson.AddMember("id", JsonValue (achievementId.c_str(), allocator).Move(), allocator);
    achievementJson.AddMember("percentComplete", percentComplete, allocator);

    JsonValue achievementsJson = JsonValue(rapidjson::kArrayType);
    achievementsJson.PushBack(achievementJson, allocator);

    request.AddMember("action", "progressUpdate", allocator);
    request.AddMember("serviceConfigId", JsonValue(lowercaseScid.c_str(), allocator).Move(), allocator);
    request.AddMember("titleId", titleId, allocator);
    request.AddMember("userId", JsonValue(utils::uint64_to_internal_string(xboxUserId).c_str(), allocator).Move(), allocator);
    request.AddMember("achievements", achievementsJson, allocator);

    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(JsonUtils::SerializeJson(request)));

    hr = httpCall->Perform(AsyncContext<HttpResult>{
        async.Queue().DeriveWorkerQueue(),
        [
            async,
            achievementId,
            percentComplete,
            xboxLiveContext{ m_xboxLiveContextImpl.lock() }
        ]
    (HttpResult httpResult)
    {
        bool shouldWriteOffline{ false };
        HRESULT hrHttp{ S_OK };

        // Errors from the task include auth errors when getting the token and sig
        HRESULT hrTask = httpResult.Hresult();

        auto xblConditionTask = XblGetErrorCondition(hrTask);
        if (xblConditionTask == XblErrorCondition::Network ||
            xblConditionTask == XblErrorCondition::Auth ||
            xblConditionTask == XblErrorCondition::GenericError)
        {
            shouldWriteOffline = true;
        }

        if (SUCCEEDED(hrTask) && httpResult.Payload())
        {
            // Errors from the http stack include network errors 
            hrHttp = httpResult.Payload()->Result();
            auto xblConditionHttp = XblGetErrorCondition(hrHttp);

            // The HTTP status code
            auto statusCode = httpResult.Payload()->HttpStatus();

            if (xblConditionHttp == XblErrorCondition::Network ||
                xblConditionHttp == XblErrorCondition::Auth ||
                xblConditionHttp == XblErrorCondition::Http429TooManyRequests ||
                xblConditionHttp == XblErrorCondition::GenericError ||
                (statusCode >= 500 && statusCode < 600))
            {
                shouldWriteOffline = true;
            }
        }

        // If any error or HTTP status code falls into one of these known buckets, 
        // then write the achievement unlock event so it can be sent when the device is back online
        // even if the game isn't running
        if (shouldWriteOffline && xboxLiveContext)
        {
            async.Complete(WriteOfflineUpdateAchievement(
                xboxLiveContext,
                achievementId,
                percentComplete
            ));
        }
        else
        {
            async.Complete(FAILED(hrTask) ? hrTask : hrHttp);
        }
    }
    });

    return hr;
}

#if HC_PLATFORM == HC_PLATFORM_XDK

ULONG AchievementsService::EventWriteAchievementUpdate(
    _In_ PCWSTR userId,
    _In_ PCWSTR achievementId,
    _In_ const uint32_t percentComplete
)
{
    if (achievementId == nullptr) return ERROR_BAD_ARGUMENTS;
    if (userId == nullptr) return ERROR_BAD_ARGUMENTS;

    static const uint32_t EventWriteAchievementUpdate_ArgCount = 5;
    static const uint32_t EventWriteAchievementUpdate_ScratchSize = 64;
    EVENT_DATA_DESCRIPTOR eventData[EventWriteAchievementUpdate_ArgCount] = { 0 };
    UINT8 scratch[EventWriteAchievementUpdate_ScratchSize] = { 0 };

    EtxFillCommonFields_v7(&eventData[0], scratch, EventWriteAchievementUpdate_ScratchSize);

    EventDataDescCreate(&eventData[1], userId, (ULONG)((wcslen(userId) + 1) * sizeof(WCHAR)));
    EventDataDescCreate(&eventData[2], &GlobalState::Get()->AchievementsSessionId(), sizeof(GUID));
    EventDataDescCreate(&eventData[3], achievementId, (ULONG)((wcslen(achievementId) + 1) * sizeof(WCHAR)));
    EventDataDescCreate(&eventData[4], &percentComplete, sizeof(percentComplete));

    return EtxEventWrite(
        &XSAPI_Update_Achievement_Events[0],
        &XSAPI_Update_Achievement_Provider,
        XSAPI_Update_Achievement_Handle,
        EventWriteAchievementUpdate_ArgCount,
        eventData
    );
}

HRESULT
AchievementsService::WriteOfflineUpdateAchievement(
    _In_ std::shared_ptr<XblContext> xboxLiveContextImpl,
    _In_ const String& achievementId,
    _In_ uint32_t percentComplete
)
{
    ULONG errorCode = EventWriteAchievementUpdate(
        utils::uint64_to_string_t(xboxLiveContextImpl->Xuid()).c_str(),
        convert::utf8_to_utf16(achievementId).c_str(),
        percentComplete
    );
    HRESULT hr = HRESULT_FROM_WIN32(errorCode);
    return hr;
}

#else

HRESULT AchievementsService::WriteOfflineUpdateAchievement(
    _In_ std::shared_ptr<XblContext> xboxLiveContextImpl,
    _In_ const String& achievementId,
    _In_ uint32_t percentComplete
)
{
    JsonDocument properties(rapidjson::kObjectType);
    properties.AddMember("AchievementId", JsonValue(achievementId.c_str(), properties.GetAllocator()).Move(), properties.GetAllocator());
    properties.AddMember("PercentComplete", percentComplete, properties.GetAllocator());
    auto propertiesStrUtf8 = JsonUtils::SerializeJson(properties);
#if XSAPI_EVENTS_SERVICE
    HRESULT hr = xboxLiveContextImpl->EventsService()->WriteInGameEvent("AchievementUpdate", propertiesStrUtf8.c_str(), nullptr);
#else // aka XSAPI_UNIT_TESTS
    HRESULT hr = E_FAIL; 
    UNREFERENCED_PARAMETER(propertiesStrUtf8);
#endif
    return hr;
}
#endif

HRESULT AchievementsService::GetAchievementsForTitle(
    _In_ uint64_t xboxUserId,
    _In_ uint32_t titleId,
    _In_ XblAchievementType type,
    _In_ bool unlockedOnly,
    _In_ XblAchievementOrderBy orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ AsyncContext<Result<std::shared_ptr<XblAchievementsResult>>> async
) const noexcept
{
    return GetAchievements(
        xboxUserId,
        Vector<uint32_t>{ titleId },
        type,
        unlockedOnly,
        orderBy,
        skipItems,
        maxItems,
        String{},
        std::move(async)
    );
}

HRESULT AchievementsService::GetAchievement(
    _In_ uint64_t xboxUserId,
    _In_ const String& serviceConfigurationId,
    _In_ const String& achievementId,
    _In_ AsyncContext<Result<std::shared_ptr<XblAchievementsResult>>> async
) const noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(serviceConfigurationId.empty());
    RETURN_HR_INVALIDARGUMENT_IF(achievementId.empty());

    Stringstream subPath;
    subPath << ("/users/xuid(") << xboxUserId << (")/achievements/") << utils::ToLower(serviceConfigurationId) << ("/") << achievementId;

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    HRESULT hr = httpCall->Init(
        m_xboxLiveContextSettings,
        "GET",
        XblHttpCall::BuildUrl("achievements", subPath.str()),
        xbox_live_api::get_achievement
    );
    RETURN_HR_IF_FAILED(hr);
    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(2));

    hr = httpCall->Perform(AsyncContext<HttpResult>{
        async.Queue().DeriveWorkerQueue(),
        [
            async,
            service{ shared_from_this() }
        ]
    (HttpResult httpResult)
    {
        HRESULT hr = httpResult.Hresult();
        if (FAILED(hr))
        {
            return async.Complete(hr);
        }

        hr = httpResult.Payload()->Result();
        if (FAILED(hr))
        {
            return async.Complete(hr);
        }
        auto result = XblAchievementsResult::Deserialize(httpResult.Payload()->GetResponseBodyJson(), service);

        if (Succeeded(result))
        {
            auto& achievements = result.Payload()->Achievements();
            if (achievements.size() == 1)
            {
                async.Complete({ result });
            }
            else if (achievements.size() > 1)
            {
                LOG_DEBUG("get_achievement:Return payload was larger than expected");
                async.Complete({ E_UNEXPECTED });
            }
            else
            {
                LOG_DEBUG("get_achievement:The achievement is not found");
                async.Complete({ E_UNEXPECTED });
            }
        }
        else
        {
            async.Complete({ result.Hresult() });
        }
    }
    });

    return hr;
}

HRESULT AchievementsService::GetAchievements(
    _In_ uint64_t xboxUserId,
    _In_ const Vector<uint32_t>& titleIds,
    _In_ XblAchievementType type,
    _In_ bool unlockedOnly,
    _In_ XblAchievementOrderBy orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const String& continuationToken,
    _In_ AsyncContext<Result<std::shared_ptr<XblAchievementsResult>>> async
) const noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(type == XblAchievementType::Unknown);
    
    auto subPath = GetAchievementsSubpath(
        xboxUserId,
        titleIds,
        type,
        unlockedOnly,
        orderBy,
        skipItems,
        maxItems,
        continuationToken
    );

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    HRESULT hr = httpCall->Init(
        m_xboxLiveContextSettings,
        "GET",
        XblHttpCall::BuildUrl("achievements", subPath),
        xbox_live_api::get_achievement
    );
    RETURN_HR_IF_FAILED(hr);
    RETURN_HR_IF_FAILED(httpCall->SetXblServiceContractVersion(2));

    hr = httpCall->Perform(AsyncContext<HttpResult>{
        async.Queue().DeriveWorkerQueue(),
        [
            service{ shared_from_this() },
            async,
            xboxUserId,
            titleIds,
            type,
            unlockedOnly,
            orderBy
        ]
    (HttpResult httpResult)
    {
        HRESULT hr = httpResult.Hresult();
        if (FAILED(hr))
        {
            return async.Complete(hr);
        }

        hr = httpResult.Payload()->Result();
        if (FAILED(hr))
        {
            return async.Complete(hr);
        }

        if (!httpResult.Payload()->GetResponseBodyJson().IsNull())
        {
            auto result = XblAchievementsResult::Deserialize(httpResult.Payload()->GetResponseBodyJson(), service);

            if (Succeeded(result))
            {
                result.Payload()->SetNextPageQueryParameters(
                    xboxUserId,
                    titleIds,
                    static_cast<XblAchievementType>(type),
                    unlockedOnly,
                    static_cast<XblAchievementOrderBy>(orderBy)
                );
            }
            async.Complete(result);
        }
    }
    });

    return hr;
}

XblFunctionContext AchievementsService::AddAchievementProgressChangeHandler(
    _In_ AchievementProgressChangeHandler handler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_lock };

    if (!m_achievementProgressChangeSubscription)
    {
        m_achievementProgressChangeSubscription = MakeShared<AchievementProgressChangeSubscription>(m_user.Xuid(), m_appConfig->Scid());
        m_rtaManager->AddSubscription(m_user, m_achievementProgressChangeSubscription);
    }
    return m_achievementProgressChangeSubscription->AddHandler(std::move(handler));
}

void AchievementsService::RemoveAchievementProgressChangeHandler(
    _In_ XblFunctionContext token
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_lock };

    if (m_achievementProgressChangeSubscription)
    {
        if (m_achievementProgressChangeSubscription->RemoveHandler(token) == 0)
        {
            m_rtaManager->RemoveSubscription(m_user, m_achievementProgressChangeSubscription);
            m_achievementProgressChangeSubscription.reset();
        }
    }
}

String AchievementsService::GetAchievementsSubpath(
    _In_ uint64_t xboxUserId,
    _In_ const Vector<uint32_t>& titleIds,
    _In_ XblAchievementType type,
    _In_ bool unlockedOnly,
    _In_ XblAchievementOrderBy orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const String& continuationToken
)
{
    xbox::services::uri_builder subPathBuilder;
    Stringstream path;
    path << ("/users/xuid(");
    path << xboxUserId;
    path << (")/achievements");

    subPathBuilder.append_path(path.str());

    Stringstream titleQuery;
    auto &last = titleIds.back();
    for (const auto& titleId : titleIds)
    {
        titleQuery << titleId;
        if (&titleId != &last)
        {
            titleQuery << (",");
        }
    }
    subPathBuilder.append_query("titleId", titleQuery.str());

    if (type != XblAchievementType::All)
    {
        subPathBuilder.append_query("types", EnumName(type));
    }

    if (unlockedOnly)
    {
        subPathBuilder.append_query("unlockedOnly=true");
    }

    switch (orderBy)
    {
    case XblAchievementOrderBy::TitleId:
    {
        subPathBuilder.append_query("orderBy", "title");
        break;
    }
    case XblAchievementOrderBy::UnlockTime:
    {
        subPathBuilder.append_query("orderBy", "unlocktime");
        break;
    }
    default: break;
    }

    utils::append_paging_info(
        subPathBuilder,
        skipItems,
        maxItems,
        continuationToken
    );

    return subPathBuilder.to_string();
}

void AchievementsService::CleanupAchievement(XblAchievement& a)
{
    Delete(a.id);
    Delete(a.serviceConfigurationId);
    Delete(a.name);
    for (size_t i = 0; i < a.titleAssociationsCount; ++i)
    {
        Delete(a.titleAssociations[i].name);
    }
    DeleteArray(a.titleAssociations, a.titleAssociationsCount);
    for (size_t i = 0; i < a.mediaAssetsCount; ++i)
    {
        Delete(a.mediaAssets[i].name);
        Delete(a.mediaAssets[i].url);
    }
    DeleteArray(a.mediaAssets, a.mediaAssetsCount);
    DeleteArray(a.platformsAvailableOn, a.platformsAvailableOnCount);
    Delete(a.unlockedDescription);
    Delete(a.lockedDescription);
    Delete(a.productId);
    for (size_t i = 0; i < a.rewardsCount; ++i)
    {
        Delete(a.rewards[i].name);
        Delete(a.rewards[i].description);
        Delete(a.rewards[i].value);
        Delete(a.rewards[i].valueType);
        if (a.rewards[i].mediaAsset)
        {
            Delete(a.rewards[i].mediaAsset->name);
            Delete(a.rewards[i].mediaAsset->url);
        }
    }
    DeleteArray(a.rewards, a.rewardsCount);
    Delete(a.deepLink);

    ZeroMemory(&a, sizeof(XblAchievement));
}

XblAchievementProgressState ProgressStateFromString(
    _In_ const xsapi_internal_string& value
)
{
    if (utils::str_icmp_internal(value, "Achieved") == 0)
    {
        return XblAchievementProgressState::Achieved;
    }
    else if (utils::str_icmp_internal(value, "NotStarted") == 0)
    {
        return XblAchievementProgressState::NotStarted;
    }
    else if (utils::str_icmp_internal(value, "InProgress") == 0)
    {
        return XblAchievementProgressState::InProgress;
    }

    return XblAchievementProgressState::Unknown;
}

XblAchievementType AchievementTypeFromString(
    _In_ const xsapi_internal_string& value
)
{
    if (utils::str_icmp_internal(value, "Persistent") == 0)
    {
        return XblAchievementType::Persistent;
    }
    else if (utils::str_icmp_internal(value, "Challenge") == 0)
    {
        return XblAchievementType::Challenge;
    }

    return XblAchievementType::Unknown;
}

XblAchievementParticipationType ParticipationTypeFromString(
    _In_ const xsapi_internal_string& value
)
{
    if (utils::str_icmp_internal(value, "Individual") == 0)
    {
        return XblAchievementParticipationType::Individual;
    }
    else if (utils::str_icmp_internal(value, "Group") == 0)
    {
        return XblAchievementParticipationType::Group;
    }

    return XblAchievementParticipationType::Unknown;
}

XblAchievementMediaAssetType MediaAssetTypeFromString(
    _In_ const xsapi_internal_string& value
)
{
    if (utils::str_icmp_internal(value, "Icon") == 0)
    {
        return XblAchievementMediaAssetType::Icon;
    }
    else if (utils::str_icmp_internal(value, "Art") == 0)
    {
        return XblAchievementMediaAssetType::Art;
    }

    return XblAchievementMediaAssetType::Unknown;
}

XblAchievementRewardType RewardTypeFromString(
    _In_ const xsapi_internal_string& value
)
{
    if (utils::str_icmp_internal(value, "Gamerscore") == 0)
    {
        return XblAchievementRewardType::Gamerscore;
    }
    else if (utils::str_icmp_internal(value, "InApp") == 0)
    {
        return XblAchievementRewardType::InApp;
    }
    else if (utils::str_icmp_internal(value, "Art") == 0)
    {
        return XblAchievementRewardType::Art;
    }

    return XblAchievementRewardType::Unknown;
}

Result<XblAchievementTitleAssociation> AchievementsService::DeserializeTitleAssociation(
    const JsonValue& json
)
{
    XblAchievementTitleAssociation titleAssociation{};

    if (json.IsNull())
    {
        return Result<XblAchievementTitleAssociation>{ titleAssociation };
    }

    HRESULT errc = S_OK;

    xsapi_internal_string name;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "name", name, true));
    titleAssociation.name = Make(name);
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(json, "id", titleAssociation.titleId, true));

    return Result<XblAchievementTitleAssociation>{ titleAssociation, errc };
}


Result<XblAchievementRequirement> AchievementsService::DeserializeRequirement(
    const JsonValue& json
)
{
    XblAchievementRequirement requirement{};

    if (json.IsNull())
    {
        return Result<XblAchievementRequirement>{ requirement };
    }

    xsapi_internal_string id, current, target;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "id", id, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "current", current, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "target", target, true));
    requirement.id = Make(id);
    requirement.currentProgressValue = Make(current);
    requirement.targetProgressValue = Make(target);

    return Result<XblAchievementRequirement>{ requirement, S_OK };
}

Result<XblAchievementProgression> AchievementsService::DeserializeProgression(
    const JsonValue& json
)
{
    XblAchievementProgression progression{};

    if (json.IsNull())
    {
        return Result<XblAchievementProgression>{ progression };
    }

    HRESULT errc = S_OK;

    xsapi_internal_vector<XblAchievementRequirement> requirementsVector;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<XblAchievementRequirement>(
        DeserializeRequirement,
        json, "requirements", requirementsVector, true
        ));

    progression.requirements = MakeArray(requirementsVector);
    progression.requirementsCount = requirementsVector.size();
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(json, "timeUnlocked", progression.timeUnlocked, true));

    return Result<XblAchievementProgression>{ progression, xbox::services::legacy::ConvertHr(errc) };
}

Result<XblAchievementTimeWindow> AchievementsService::DeserializeTimeWindow(
    const JsonValue& json
)
{
    XblAchievementTimeWindow timeWindow{};

    if (json.IsNull())
    {
        return Result<XblAchievementTimeWindow>{ timeWindow };
    }

    HRESULT errc = S_OK;

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(json, "startDate", timeWindow.startDate, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(json, "endDate", timeWindow.endDate, true));

    return Result<XblAchievementTimeWindow>{ timeWindow, xbox::services::legacy::ConvertHr(errc) };
}

Result<XblAchievementMediaAsset> AchievementsService::DeserializeMediaAsset(
    const JsonValue& json
)
{
    XblAchievementMediaAsset mediaAsset{};

    if (json.IsNull())
    {
        return Result<XblAchievementMediaAsset>{ mediaAsset };
    }

    xsapi_internal_string name, type, url;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "name", name, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "type", type, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "url", url, true));
    mediaAsset.name = Make(name);
    mediaAsset.mediaAssetType = MediaAssetTypeFromString(type);
    mediaAsset.url = Make(url);

    return Result<XblAchievementMediaAsset>{ mediaAsset, S_OK };
}

Result<XblAchievementReward> AchievementsService::DeserializeReward(
    const JsonValue& json
)
{
    XblAchievementReward reward{};

    if (json.IsNull())
    {
        return Result<XblAchievementReward>{ reward };
    }

    HRESULT errc = S_OK;
    xsapi_internal_string name, description, value, rewardType, valueType;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "name", name, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "description", description, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "value", value, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "type", rewardType, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "valueType", valueType, true));
    reward.name = Make(name);
    reward.description = Make(description);
    reward.value = Make(value);
    reward.rewardType = RewardTypeFromString(rewardType);
    reward.valueType = Make(valueType);

    if (json.IsObject() && json.HasMember("mediaAsset"))
    {
        auto mediaAssetResult = DeserializeMediaAsset(json["mediaAsset"]);
        reward.mediaAsset = Make<XblAchievementMediaAsset>(mediaAssetResult.ExtractPayload());

        if (Failed(mediaAssetResult) || FAILED(errc))
        {
            return Result<XblAchievementReward>{ reward, E_FAIL };
        }

        return Result<XblAchievementReward>{ reward };
    }
    else
    {
        reward.mediaAsset = Make<XblAchievementMediaAsset>(XblAchievementMediaAsset());
        return Result<XblAchievementReward>{ reward, E_FAIL };
    }
}

Result<XblAchievement> AchievementsService::DeserializeAchievement(
    const JsonValue& json
)
{
    XblAchievement a{};

    if (json.IsNull())
    {
        return Result<XblAchievement>{ a };
    }

    HRESULT errCode = S_OK;

    xsapi_internal_string id, serviceConfigId, name;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "id", id, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "serviceConfigId", serviceConfigId, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "name", name, true));
    a.id = Make(id);
    a.serviceConfigurationId = Make(serviceConfigId);
    a.name = Make(name);

    xsapi_internal_vector<XblAchievementTitleAssociation> titleAssociationVector;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<XblAchievementTitleAssociation>(
        DeserializeTitleAssociation,
        json, "titleAssociations", titleAssociationVector, true
        ));
    a.titleAssociations = MakeArray(titleAssociationVector);
    a.titleAssociationsCount = titleAssociationVector.size();
    xsapi_internal_string progressState;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "progressState", progressState));
    a.progressState = ProgressStateFromString(progressState);

    xsapi_internal_vector<XblAchievementMediaAsset> mediaAssetsVector;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<XblAchievementMediaAsset>(
        DeserializeMediaAsset,
        json, "mediaAssets", mediaAssetsVector, true
        ));

    a.mediaAssets = MakeArray(mediaAssetsVector);
    a.mediaAssetsCount = mediaAssetsVector.size();

    xsapi_internal_vector<xsapi_internal_string> platformsVector;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<xsapi_internal_string>(JsonUtils::JsonStringExtractor, json, "platforms", platformsVector, true));
    a.platformsAvailableOn = const_cast<const char**>(MakeArray(platformsVector));
    a.platformsAvailableOnCount = platformsVector.size();

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(json, "isSecret", a.isSecret));
    xsapi_internal_string description, lockedDescription, productId;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "description", description, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "lockedDescription", lockedDescription, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "productId", productId, true));
    a.unlockedDescription = Make(description);
    a.lockedDescription = Make(lockedDescription);
    a.productId = Make(productId);

    xsapi_internal_string achievementType, participationType;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "achievementType", achievementType, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "participationType", participationType, true));
    a.type = AchievementTypeFromString(achievementType);
    a.participationType = ParticipationTypeFromString(participationType);

    Result<XblAchievementTimeWindow> timeWindowResult{ XblAchievementTimeWindow() };
    if (json.IsObject() && json.HasMember("timeWindow"))
    {
        timeWindowResult = DeserializeTimeWindow(json["timeWindow"]);
    }
    else
    {
        //required
        return WEB_E_INVALID_JSON_STRING;
    }
    a.available = timeWindowResult.ExtractPayload();


    xsapi_internal_vector<XblAchievementReward> rewardsVector;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<XblAchievementReward>(
        DeserializeReward,
        json, "rewards", rewardsVector, true
        ));

    a.rewards = MakeArray(rewardsVector);
    a.rewardsCount = rewardsVector.size();

    std::chrono::seconds seconds{};
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringTimespanInSeconds(json, "estimatedTime", seconds, true));
    a.estimatedUnlockTime = seconds.count(); 
    xsapi_internal_string deeplink;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "deeplink", deeplink, true));
    a.deepLink = Make(deeplink);

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(json, "isRevoked", a.isRevoked, true));

    Result<XblAchievementProgression> progressionResult{ XblAchievementProgression() };
    if (json.IsObject() && json.HasMember("progression"))
    {
        progressionResult = DeserializeProgression(json["progression"]);
    }
    else
    {
        //required
        return WEB_E_INVALID_JSON_STRING;
    }
    a.progression = progressionResult.ExtractPayload();

    if (Failed(progressionResult) || Failed(timeWindowResult) || FAILED(errCode))
    {
        CleanupAchievement(a);
        return Result<XblAchievement>{ E_FAIL };
    }

    return Result<XblAchievement>{ a };
}

#if HC_PLATFORM == HC_PLATFORM_XDK
#pragma pack(pop)
#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END