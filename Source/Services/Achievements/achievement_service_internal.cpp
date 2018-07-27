// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

#include "xsapi/achievements.h"
#include "achievements_internal.h"

#include "http_call_impl.h"
#include "user_context.h"
#include "Utils.h"
#include "xsapi/services.h"
#include "xbox_live_context_impl.h"
#include "xbox_live_app_config_internal.h"
#include "xbox_system_factory.h"

#if TV_API
#pragma pack(push, 16)
#include "EtwPlus.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

#if TV_API

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

achievement_service_internal::achievement_service_internal(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox_live_app_config_internal> appConfig,
    _In_ std::weak_ptr<xbox_live_context_impl> xboxLiveContextImpl
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig)),
    m_xboxLiveContextImpl(std::move(xboxLiveContextImpl))
{
}

_XSAPIIMP xbox_live_result<void>
achievement_service_internal::update_achievement(
    _In_ const xsapi_internal_string& xboxUserId,
    _In_ const xsapi_internal_string& achievementId,
    _In_ uint32_t percentComplete,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<void>> callback
    )
{
    return update_achievement(
        xboxUserId,
        m_appConfig->title_id(),
        m_appConfig->scid(),
        achievementId,
        percentComplete,
        queue,
        callback
        );
}

_XSAPIIMP xbox_live_result<void>
achievement_service_internal::update_achievement(
    _In_ const xsapi_internal_string& xboxUserId,
    _In_ uint32_t titleId,
    _In_ const xsapi_internal_string& serviceConfigurationId,
    _In_ const xsapi_internal_string& achievementId,
    _In_ uint32_t percentComplete,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<void>> callback
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(xboxUserId.empty(), void, "xbox user id is empty");
    RETURN_CPP_INVALIDARGUMENT_IF(titleId == 0, void, "title id is empty");
    RETURN_CPP_INVALIDARGUMENT_IF(serviceConfigurationId.empty(), void, "serviceConfigurationId is empty");
    RETURN_CPP_INVALIDARGUMENT_IF(achievementId.empty(), void, "achievementId is empty");
    RETURN_CPP_INVALIDARGUMENT_IF(percentComplete > 100, void, "percentComplete is greater than 100");

    xsapi_internal_string scid = serviceConfigurationId;
#if TV_API
    {
        auto xsapiSingleton = get_xsapi_singleton();
        std::lock_guard<std::mutex> lock(xsapiSingleton->m_achievementServiceInitLock);
        if (!xsapiSingleton->m_bHasAchievementServiceInitialized)
        {
            HRESULT hr = CoCreateGuid(&xsapiSingleton->m_eventPlayerSessionId);
            if (FAILED(hr))
            {
                return xbox_live_result<void>(static_cast<xbox_live_error_code>(hr));
            }
            
            string_t wScid = utils::string_t_from_internal_string(scid);
            std::error_code errC = utils::guid_from_string(wScid, const_cast<GUID*>(&XSAPI_Update_Achievement_Provider.Guid), false);
            if (errC)
            {
                return xbox_live_result<void>(errC);
            }
            scid = utils::internal_string_from_string_t(wScid);

            CHAR strTitleId[16] = "";
            sprintf_s(strTitleId, "%0.8X", titleId);

            std::stringstream ss;
            ss << "XSAPI_";
            ss << strTitleId;
            xsapiSingleton->m_eventProviderName = ss.str();
            XSAPI_Update_Achievement_Provider.Name = xsapiSingleton->m_eventProviderName.c_str();

            ULONG errorCode = EtxRegister(&XSAPI_Update_Achievement_Provider, &XSAPI_Update_Achievement_Handle);
            hr = HRESULT_FROM_WIN32(errorCode);
            if (FAILED(hr))
            {
                return xbox_live_result<void>(static_cast<xbox_live_error_code>(hr));
            }

            xsapiSingleton->m_bHasAchievementServiceInitialized = true;
        }
    }
#endif

    auto subPath = update_achievement_sub_path(
        xboxUserId,
        serviceConfigurationId
    );

    std::shared_ptr<http_call_internal> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        "POST",
        utils::create_xboxlive_endpoint("achievements", m_appConfig),
        utils::string_t_from_internal_string(subPath),
        xbox_live_api::update_achievement
    );
    httpCall->set_xbox_contract_version_header_value(_T("2"));

    web::json::value achievementJson;
    achievementJson[_T("id")] = web::json::value::string(utils::string_t_from_internal_string(achievementId));
    achievementJson[_T("percentComplete")] = web::json::value::number(static_cast<double>(percentComplete));

    web::json::value achievementsJson = web::json::value::array();
    achievementsJson[0] = achievementJson;

    web::json::value request;
    request[_T("action")] = web::json::value::string(_T("progressUpdate"));
    request[_T("serviceConfigId")] = web::json::value::string(utils::string_t_from_internal_string(serviceConfigurationId));
    request[_T("titleId")] = web::json::value::number(titleId);
    request[_T("userId")] = web::json::value::string(utils::string_t_from_internal_string(xboxUserId));
    request[_T("achievements")] = achievementsJson;

    httpCall->set_request_body(utils::internal_string_from_string_t(request.serialize()));

    auto xboxLiveContextImpl = m_xboxLiveContextImpl.lock();
    httpCall->get_response_with_auth(
        m_userContext,
        http_call_response_body_type::json_body,
        false,
        queue,
        [callback, achievementId, percentComplete, xboxLiveContextImpl](std::shared_ptr<http_call_response_internal> response)
        {
            if (response->err_code() == xbox_live_error_condition::network ||
                response->err_code() == xbox_live_error_code::http_status_429_too_many_requests ||
                response->err_code() == xbox_live_error_code::generic_error ||
                (response->err_code().value() >= 500 && response->err_code().value() < 600))
            {
    #if TV_API | UWP_API
                if (xboxLiveContextImpl)
                {
                    callback(write_offline_update_achievement(
                        xboxLiveContextImpl,
                        achievementId,
                        percentComplete
                    ));
                }
    #endif
            }
            callback(utils::generate_xbox_live_result<void>(xbox_live_result<void>(), response));
        });

    return xbox_live_result<void>();
}

#if TV_API

ULONG
achievement_service_internal::event_write_achievement_update(
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
    EventDataDescCreate(&eventData[2], &get_xsapi_singleton()->m_eventPlayerSessionId, sizeof(GUID));
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

xbox::services::xbox_live_result<void>
achievement_service_internal::write_offline_update_achievement(
    _In_ std::shared_ptr<xbox::services::xbox_live_context_impl> xboxLiveContextImpl,
    _In_ const xsapi_internal_string& achievementId,
    _In_ uint32_t percentComplete
)
{
    ULONG errorCode = event_write_achievement_update(
        utils::string_t_from_internal_string(xboxLiveContextImpl->xbox_live_user_id()).c_str(),
        utils::string_t_from_internal_string(achievementId).c_str(),
        percentComplete
    );
    HRESULT hr = HRESULT_FROM_WIN32(errorCode);
    std::error_code errc = static_cast<xbox_live_error_code>(hr);
    return xbox_live_result<void>(errc);
}
#endif

#if UWP_API
xbox::services::xbox_live_result<void>
achievement_service_internal::write_offline_update_achievement(
    _In_ std::shared_ptr<xbox::services::xbox_live_context_impl> xboxLiveContextImpl,
    _In_ const xsapi_internal_string& achievementId,
    _In_ uint32_t percentComplete
)
{
    web::json::value properties = web::json::value::object();
    properties[_T("AchievementId")] = web::json::value(utils::string_t_from_internal_string(achievementId));
    properties[_T("PercentComplete")] = percentComplete;
    web::json::value measurements = web::json::value::object();
    return xboxLiveContextImpl->events_service().write_in_game_event(_T("AchievementUpdate"), properties, measurements);
}
#endif

_XSAPIIMP xbox_live_result<void>
achievement_service_internal::get_achievements_for_title_id(
    _In_ const xsapi_internal_string& xboxUserId,
    _In_ uint32_t titleId,
    _In_ achievement_type type,
    _In_ bool unlockedOnly,
    _In_ achievement_order_by orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<achievements_result_internal>>> callback
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(xboxUserId.empty(), void, "xbox user id is empty");

    xsapi_internal_vector<uint32_t> titles;
    titles.push_back(titleId);

    return get_achievements(
        xboxUserId,
        titles,
        type,
        unlockedOnly,
        orderBy,
        skipItems,
        maxItems,
        xsapi_internal_string(),
        queue,
        callback
    );
}

_XSAPIIMP xbox_live_result<void>
achievement_service_internal::get_achievement(
    _In_ const xsapi_internal_string& xboxUserId,
    _In_ const xsapi_internal_string& serviceConfigurationId,
    _In_ const xsapi_internal_string& achievementId,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<achievement_internal>>> callback
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(xboxUserId.empty(), void, "xbox user id is empty");
    RETURN_CPP_INVALIDARGUMENT_IF(serviceConfigurationId.empty(), void, "service configuration id is empty");
    RETURN_CPP_INVALIDARGUMENT_IF(achievementId.empty(), void, "achievement id is empty");

    auto subPath = achievement_by_id_sub_path(
        xboxUserId,
        serviceConfigurationId,
        achievementId
    );

    std::shared_ptr<http_call_internal> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        "GET",
        utils::create_xboxlive_endpoint("achievements", m_appConfig),
        utils::string_t_from_internal_string(subPath),
        xbox_live_api::get_achievement
    );
    httpCall->set_xbox_contract_version_header_value(_T("2"));

    httpCall->get_response_with_auth(
        m_userContext,
        http_call_response_body_type::json_body,
        false,
        queue,
        [callback](std::shared_ptr<http_call_response_internal> response)
        {
            auto achievementResult = achievements_result_internal::_Deserialize(response->response_body_json());
            auto results = utils::generate_xbox_live_result<std::shared_ptr<achievements_result_internal>>(
                achievementResult,
                response
                );

            if (!results.err())
            {
                if (results.payload()->items().size() == 1)
                {
                    auto items = results.payload()->items();
                    const auto& achievementResult2 = items[0];
                    callback(xbox_live_result<std::shared_ptr<achievement_internal>>(achievementResult2, results.err(), results.err_message()));
                }
                else if (results.payload()->items().size() > 1)
                {
                    callback(xbox_live_result<std::shared_ptr<achievement_internal>>(xbox_live_error_code::runtime_error, "Return payload was larger than expected"));
                }
                else
                {
                    callback(xbox_live_result<std::shared_ptr<achievement_internal>>(xbox_live_error_code::runtime_error, "The achievement is not found"));
                }
            }
            else
            {
                callback(xbox_live_result<std::shared_ptr<achievement_internal>>(results.err(), results.err_message()));
            }

        });

    return xbox_live_result<void>();
}

_XSAPIIMP xbox_live_result<void>
achievement_service_internal::get_achievements(
    _In_ const xsapi_internal_string& xboxUserId,
    _In_ const xsapi_internal_vector<uint32_t>& titleIds,
    _In_ achievement_type type,
    _In_ bool unlockedOnly,
    _In_ achievement_order_by orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const xsapi_internal_string& continuationToken,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<achievements_result_internal>>> callback
)
{
    xbox_live_result<xsapi_internal_string> stringType = convert_type_to_string(type);
    RETURN_CPP_IF_ERR(stringType, void);
    xbox_live_result<xsapi_internal_string> orderType = convert_order_by_to_string(orderBy);
    RETURN_CPP_IF_ERR(orderType, void);

    auto subPath = achievements_sub_path(
        xboxUserId,
        titleIds,
        stringType.payload(),
        unlockedOnly,
        orderType.payload(),
        skipItems,
        maxItems,
        continuationToken
    );

    std::shared_ptr<http_call_internal> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        "GET",
        utils::create_xboxlive_endpoint("achievements", m_appConfig),
        utils::string_t_from_internal_string(subPath),
        xbox_live_api::get_achievements
    );
    httpCall->set_xbox_contract_version_header_value(_T("2"));

    auto userContext = m_userContext;
    auto xboxLiveContextSettings = m_xboxLiveContextSettings;
    auto appConfig = m_appConfig;
    auto xboxLiveContextImpl = m_xboxLiveContextImpl;

    auto task = httpCall->get_response_with_auth(
        m_userContext,
        http_call_response_body_type::json_body,
        false,
        queue,
        [callback, userContext, xboxLiveContextSettings, appConfig, xboxLiveContextImpl, xboxUserId, titleIds, type, unlockedOnly, orderBy](std::shared_ptr<http_call_response_internal> response)
        {
            if (response->response_body_json().size() > 0)
            {
                auto jsonResult = achievements_result_internal::_Deserialize(response->response_body_json());
                auto achievementsResult = utils::generate_xbox_live_result<std::shared_ptr<achievements_result_internal>>(
                    jsonResult,
                    response
                    );

                auto& achievement = achievementsResult.payload();
                if (achievement != nullptr)
                {
                    achievement->_Init_next_page_info(
                        userContext,
                        xboxLiveContextSettings,
                        appConfig,
                        xboxLiveContextImpl,
                        xboxUserId,
                        titleIds,
                        type,
                        unlockedOnly,
                        orderBy
                    );
                }
                
                callback(utils::generate_xbox_live_result<std::shared_ptr<achievements_result_internal>>(achievementsResult, response));
            }
            else
            {
                callback(utils::generate_xbox_live_result<std::shared_ptr<achievements_result_internal>>(xbox_live_result<std::shared_ptr<achievements_result_internal>>(), response));
            }
        });

    return xbox_live_result<void>();
}

xbox_live_result<xsapi_internal_string>
achievement_service_internal::convert_type_to_string(
    _In_ achievement_type type
)
{
    switch (type)
    {
    case achievement_type::persistent:
        return xbox_live_result<xsapi_internal_string>("persistent");

    case achievement_type::challenge:
        return xbox_live_result<xsapi_internal_string>("challenge");

    case achievement_type::all:
        return xbox_live_result<xsapi_internal_string>(xsapi_internal_string());

    default:
        return xbox_live_result<xsapi_internal_string>(xbox_live_error_code::invalid_argument, "Unsupported achievement type");
    }
}

xbox_live_result<xsapi_internal_string>
achievement_service_internal::convert_order_by_to_string(
    _In_ achievement_order_by orderBy
)
{
    switch (orderBy)
    {
    case achievement_order_by::default_order:
        return xbox_live_result<xsapi_internal_string>(xsapi_internal_string());

    case achievement_order_by::title_id:
        return xbox_live_result<xsapi_internal_string>("title");

    case achievement_order_by::unlock_time:
        return xbox_live_result<xsapi_internal_string>("unlocktime");

    default:
        return xbox_live_result<xsapi_internal_string>(xbox_live_error_code::invalid_argument, "Unsupported achievement order");
    }
}

const xsapi_internal_string
achievement_service_internal::achievements_sub_path(
    _In_ const xsapi_internal_string& xboxUserId,
    _In_ const xsapi_internal_vector<uint32_t>& titleIds,
    _In_ const xsapi_internal_string& type,
    _In_ bool unlockedOnly,
    _In_ const xsapi_internal_string& orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const xsapi_internal_string& continuationToken
)
{
    web::uri_builder subPathBuilder;
    xsapi_internal_stringstream path;
    path << ("/users/xuid(");
    path << xboxUserId.c_str();
    path << (")/achievements");

    subPathBuilder.append_path(utils::string_t_from_internal_string(path.str()));

    xsapi_internal_stringstream titleQuery;
    auto &last = titleIds.back();
    for (const uint32_t& titleId : titleIds)
    {
        titleQuery << titleId;
        if (&titleId != &last)
        {
            titleQuery << (",");
        }
    }
    subPathBuilder.append_query(_T("titleId"), utils::string_t_from_internal_string(titleQuery.str()));

    if (!type.empty())
    {
        subPathBuilder.append_query(_T("types"), utils::string_t_from_internal_string(type));
    }

    if (unlockedOnly)
    {
        subPathBuilder.append_query(_T("unlockedOnly=true"));
    }

    if (!orderBy.empty())
    {
        subPathBuilder.append_query(_T("orderBy"), utils::string_t_from_internal_string(orderBy));
    }

    utils::append_paging_info(
        subPathBuilder,
        skipItems,
        maxItems,
        utils::string_t_from_internal_string(continuationToken)
    );

    return utils::internal_string_from_string_t(subPathBuilder.to_string());
}

const xsapi_internal_string
achievement_service_internal::update_achievement_sub_path(
    _In_ const xsapi_internal_string& xboxUserId,
    _In_ const xsapi_internal_string& serviceConfigurationId
)
{
    xsapi_internal_stringstream ss;
    ss << ("/users/xuid(") << xboxUserId << (")/achievements/") << serviceConfigurationId << ("/update");
    return ss.str();
}

const xsapi_internal_string
achievement_service_internal::achievement_by_id_sub_path(
    _In_ const xsapi_internal_string& xboxUserId,
    _In_ const xsapi_internal_string& serviceConfigurationId,
    _In_ const xsapi_internal_string& achievementId
)
{
    xsapi_internal_stringstream ss;
    ss << ("/users/xuid(") << xboxUserId << (")/achievements/") << serviceConfigurationId << ("/") << achievementId;
    return ss.str();
}

#if TV_API
#pragma pack(pop)
#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END