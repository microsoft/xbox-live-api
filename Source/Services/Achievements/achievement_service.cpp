// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Utils.h"
#include "user_context.h"
#include "xbox_system_factory.h"
#include "xsapi/achievements.h"
#include "xsapi/services.h"
#include "xbox_live_context_impl.h"

#if TV_API
#pragma pack(push, 16)
#include "EtwPlus.h"
#endif

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

#if TV_API
bool achievement_service::s_bHasInitialized = false;
GUID achievement_service::s_eventPlayerSessionId = {0};
std::string achievement_service::s_eventProviderName;
std::mutex achievement_service::m_initLock;

EXTERN_C __declspec(selectany) ETX_FIELD_DESCRIPTOR XSAPI_Update_Achievement_Fields[5] = 
{
    {EtxFieldType_UnicodeString,0},
    {EtxFieldType_UnicodeString,0},
    {EtxFieldType_GUID,0},
    {EtxFieldType_UnicodeString,0},
    {EtxFieldType_UInt32,0}
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
    {0},
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

achievement_service::achievement_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
    _In_ std::weak_ptr<xbox_live_context_impl> xboxLiveContextImpl
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig)),
    m_xboxLiveContextImpl(std::move(xboxLiveContextImpl))
{
}

pplx::task<xbox::services::xbox_live_result<void>> 
achievement_service::update_achievement(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& achievementId,
    _In_ uint32_t percentComplete
    )
{
    uint32_t titleId = m_appConfig->title_id();
    const string_t& serviceConfigurationId = m_appConfig->scid();

    return update_achievement(xboxUserId, titleId, serviceConfigurationId, achievementId, percentComplete);
}


pplx::task<xbox::services::xbox_live_result<void>> 
achievement_service::update_achievement(
    _In_ const string_t& xboxUserId,
    _In_ uint32_t titleId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& achievementId,
    _In_ uint32_t percentComplete
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(xboxUserId.empty(), void, "xbox user id is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(serviceConfigurationId.empty(), void, "serviceConfigurationId is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(achievementId.empty(), void, "achievementId is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(percentComplete > 100, void, "percentComplete is greater than 100");

#if TV_API
    {
        std::lock_guard<std::mutex> lock(m_initLock);
        if (!s_bHasInitialized)
        {
            HRESULT hr = CoCreateGuid(&s_eventPlayerSessionId);
            if (FAILED(hr))
            {
                return pplx::task_from_result(xbox::services::xbox_live_result<void>(static_cast<xbox_live_error_code>(hr)));
            }

            std::error_code errC = utils::guid_from_string(serviceConfigurationId, const_cast<GUID*>(&XSAPI_Update_Achievement_Provider.Guid), false);
            if (errC)
            {
                return pplx::task_from_result(xbox::services::xbox_live_result<void>(errC));
            }

            CHAR strTitleId[16] = "";
            sprintf_s( strTitleId, "%0.8X", titleId );

            std::stringstream ss;
            ss << "XSAPI_";
            ss << strTitleId;
            s_eventProviderName = ss.str();
            XSAPI_Update_Achievement_Provider.Name = s_eventProviderName.c_str();

            ULONG errorCode = EtxRegister(&XSAPI_Update_Achievement_Provider, &XSAPI_Update_Achievement_Handle);
            hr = HRESULT_FROM_WIN32(errorCode);
            if (FAILED(hr))
            {
                return pplx::task_from_result(xbox::services::xbox_live_result<void>(static_cast<xbox_live_error_code>(hr)));
            }

            s_bHasInitialized = true;
        }
    }
#endif

    auto subPath = update_achievement_sub_path(
        xboxUserId,
        serviceConfigurationId
        );

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("achievements"), m_appConfig),
        subPath,
        xbox_live_api::update_achievement
        );
    httpCall->set_xbox_contract_version_header_value(_T("2"));

    web::json::value achievementJson;
    achievementJson[_T("id")] = web::json::value::string(achievementId);
    achievementJson[_T("percentComplete")] = web::json::value::number(static_cast<double>(percentComplete));

    web::json::value achievementsJson = web::json::value::array();
    achievementsJson[0] = achievementJson;

    web::json::value request;
    request[_T("action")] = web::json::value::string(_T("progressUpdate"));
    request[_T("serviceConfigId")] = web::json::value::string(serviceConfigurationId);
    request[_T("titleId")] = web::json::value::number(titleId);
    request[_T("userId")] = web::json::value::string(xboxUserId);
    request[_T("achievements")] = achievementsJson;

    httpCall->set_request_body(request.serialize());

    auto userContext = m_userContext;
    auto xboxLiveContextImpl = m_xboxLiveContextImpl.lock();

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([userContext, xboxUserId, titleId, serviceConfigurationId, achievementId, percentComplete, xboxLiveContextImpl](std::shared_ptr<http_call_response> response)
    {
        if (response->err_code() == xbox_live_error_condition::network ||
            response->err_code() == xbox_live_error_code::http_status_429_too_many_requests ||
            response->err_code() == xbox_live_error_code::generic_error ||
            (response->err_code().value() >= 500 && response->err_code().value() < 600))
        {
#if TV_API | UWP_API
            if( xboxLiveContextImpl )
            {
                return write_offline_update_achievement(
                    xboxLiveContextImpl,
                    achievementId,
                    percentComplete
                    );
            }
#endif

            return xbox_live_result<void>(response->err_code(), response->err_message());
        }
        else
        {
            return xbox_live_result<void>(response->err_code(), response->err_message());
        }
    });
    
    return utils::create_exception_free_task<void>(
        task
        );

}

#if TV_API

ULONG
achievement_service::event_write_achievement_update(
    _In_ PCWSTR userId,
    _In_ PCWSTR achievementId,
    _In_ const uint32_t percentComplete
    )
{
    if( achievementId == nullptr) return ERROR_BAD_ARGUMENTS;
    if( userId == nullptr ) return ERROR_BAD_ARGUMENTS;

    static const uint32_t EventWriteAchievementUpdate_ArgCount = 5;
    static const uint32_t EventWriteAchievementUpdate_ScratchSize = 64;
    EVENT_DATA_DESCRIPTOR eventData[EventWriteAchievementUpdate_ArgCount] = {0};
    UINT8 scratch[EventWriteAchievementUpdate_ScratchSize] = {0};

    EtxFillCommonFields_v7(&eventData[0], scratch, EventWriteAchievementUpdate_ScratchSize);

    EventDataDescCreate(&eventData[1], userId, (ULONG)((wcslen(userId) + 1) * sizeof(WCHAR)));
    EventDataDescCreate(&eventData[2], &s_eventPlayerSessionId, sizeof(GUID));
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
achievement_service::write_offline_update_achievement(
    _In_ std::shared_ptr<xbox::services::xbox_live_context_impl> xboxLiveContextImpl,
    _In_ const string_t& achievementId,
    _In_ uint32_t percentComplete
    )
{
    ULONG errorCode = event_write_achievement_update(
        xboxLiveContextImpl->xbox_live_user_id().c_str(),
        achievementId.c_str(),
        percentComplete
        );
    HRESULT hr = HRESULT_FROM_WIN32(errorCode);
    std::error_code errc = static_cast<xbox_live_error_code>(hr);
    return xbox_live_result<void>(errc);
}
#endif

#if UWP_API
xbox::services::xbox_live_result<void>
achievement_service::write_offline_update_achievement(
    _In_ std::shared_ptr<xbox::services::xbox_live_context_impl> xboxLiveContextImpl,
    _In_ const string_t& achievementId,
    _In_ uint32_t percentComplete
    )
{
    web::json::value properties = web::json::value::object();
    properties[_T("AchievementId")] = web::json::value(achievementId);
    properties[_T("PercentComplete")] = percentComplete;
    web::json::value measurements = web::json::value::object();
    return xboxLiveContextImpl->events_service().write_in_game_event(_T("AchievementUpdate"), properties, measurements);
}
#endif

pplx::task<xbox::services::xbox_live_result<achievements_result>>
achievement_service::get_achievements_for_title_id(
    _In_ const string_t& xboxUserId,
    _In_ uint32_t titleId,
    _In_ achievement_type type,
    _In_ bool unlockedOnly,
    _In_ achievement_order_by orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(xboxUserId.empty(), achievements_result, "xbox user id is empty");

    std::vector<uint32_t> titles;
    titles.push_back(titleId);

    return get_achievements(
        xboxUserId,
        titles,
        type,
        unlockedOnly,
        orderBy,
        skipItems,
        maxItems,
        string_t()
        );
}

pplx::task<xbox::services::xbox_live_result<achievement>>
achievement_service::get_achievement(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& achievementId
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(xboxUserId.empty(), achievement, "xbox user id is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(serviceConfigurationId.empty(), achievement, "service configuration id is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(achievementId.empty(), achievement, "achievement id is empty");

    auto subPath = achievement_by_id_sub_path(
        xboxUserId,
        serviceConfigurationId,
        achievementId
        );

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("achievements"), m_appConfig),
        subPath,
        xbox_live_api::get_achievement
        );
    httpCall->set_xbox_contract_version_header_value(_T("2"));

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        auto achievementResult = achievements_result::_Deserialize(response->response_body_json());
        auto results = utils::generate_xbox_live_result<achievements_result>(
            achievementResult,
            response
            );

        if (!results.err())
        {
            if (results.payload().items().size() == 1)
            {
                const auto& achievementResult2 = results.payload().items()[0];
                return xbox_live_result<achievement>(achievementResult2, results.err(), results.err_message());
            }
            else if (results.payload().items().size() > 1)
            {
                return xbox_live_result<achievement>(xbox_live_error_code::runtime_error, "Return payload was larger than expected");
            }
            else
            {
                return xbox_live_result<achievement>(xbox_live_error_code::runtime_error, "The achievement is not found");
            }
        }

        return xbox_live_result<achievement>(results.err(), results.err_message());
    });
    
    return utils::create_exception_free_task<achievement>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<achievements_result>>
achievement_service::get_achievements(
    _In_ const string_t& xboxUserId,
    _In_ const std::vector<uint32_t>& titleIds,
    _In_ achievement_type type,
    _In_ bool unlockedOnly,
    _In_ achievement_order_by orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const string_t& continuationToken
    )
{
    xbox_live_result<string_t> stringType = convert_type_to_string(type);
    RETURN_TASK_CPP_IF_ERR(stringType, achievements_result);
    xbox_live_result<string_t> orderType = convert_order_by_to_string(orderBy);
    RETURN_TASK_CPP_IF_ERR(orderType, achievements_result);

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

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("achievements"), m_appConfig),
        subPath,
        xbox_live_api::get_achievements
        );
    httpCall->set_xbox_contract_version_header_value(_T("2"));

    auto userContext = m_userContext;
    auto xboxLiveContextSettings = m_xboxLiveContextSettings;
    auto appConfig = m_appConfig;
    auto xboxLiveContextImpl = m_xboxLiveContextImpl;

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([userContext, xboxLiveContextSettings, appConfig, xboxLiveContextImpl, xboxUserId, titleIds, type, unlockedOnly, orderBy](std::shared_ptr<http_call_response> response)
    {
        if (response->response_body_json().size() > 0)
        {
            auto jsonResult = xbox::services::achievements::achievements_result::_Deserialize(response->response_body_json());
            auto achievementsResult = utils::generate_xbox_live_result<achievements_result>(
                jsonResult,
                response
                );
            
            auto& achievement = achievementsResult.payload();
            achievement._Init_next_page_info(
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

            return achievementsResult;
        }
        else
        {
            return xbox_live_result<achievements_result>(response->err_code(), response->err_message());
        }
    });

    return utils::create_exception_free_task<achievements_result>(
        task
        );
}

xbox_live_result<string_t>
achievement_service::convert_type_to_string(
    _In_ achievement_type type
    )
{
    switch (type)
    {
    case achievement_type::persistent:
        return xbox_live_result<string_t>(_T("persistent"));

    case achievement_type::challenge:
        return xbox_live_result<string_t>(_T("challenge"));

    case achievement_type::all:
        return xbox_live_result<string_t>(string_t());

    default:
        return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "Unsupported achievement type");
    }
}

xbox_live_result<string_t>
achievement_service::convert_order_by_to_string(
    _In_ achievement_order_by orderBy
    )
{
    switch (orderBy)
    {
    case achievement_order_by::default_order:
        return xbox_live_result<string_t>(string_t());

    case achievement_order_by::title_id:
        return xbox_live_result<string_t>(_T("title"));

    case achievement_order_by::unlock_time:
        return xbox_live_result<string_t>(_T("unlocktime"));

    default:
        return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "Unsupported achievement order");
    }
}

const string_t
achievement_service::achievements_sub_path(
    _In_ const string_t& xboxUserId,
    _In_ const std::vector<uint32_t>& titleIds,
    _In_ const string_t& type,
    _In_ bool unlockedOnly,
    _In_ const string_t& orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const string_t& continuationToken
    )
{
    web::uri_builder subPathBuilder;
    stringstream_t path;
    path << _T("/users/xuid(");
    path << xboxUserId;
    path << _T(")/achievements");

    subPathBuilder.append_path(path.str());

    stringstream_t titleQuery;
    auto &last = titleIds.back();
    for (const uint32_t& titleId : titleIds)
    {
        titleQuery << titleId;
        if (&titleId != &last)
        {
            titleQuery << _T(",");
        }
    }
    subPathBuilder.append_query(_T("titleId"), titleQuery.str());

    if (!type.empty())
    {
        subPathBuilder.append_query(_T("types"), type);
    }

    if (unlockedOnly)
    {
        subPathBuilder.append_query(_T("unlockedOnly=true"));
    }

    if (!orderBy.empty())
    {
        subPathBuilder.append_query(_T("orderBy"), orderBy);
    }

    utils::append_paging_info(
        subPathBuilder,
        skipItems,
        maxItems,
        continuationToken
        );

    return subPathBuilder.to_string();
}

const string_t
achievement_service::update_achievement_sub_path(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& serviceConfigurationId
    )
{
    stringstream_t ss;
    ss << _T("/users/xuid(") << xboxUserId << _T(")/achievements/") << serviceConfigurationId << _T("/update");
    return ss.str();
}

const string_t
achievement_service::achievement_by_id_sub_path(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& achievementId
    )
{
    stringstream_t ss;
    ss << _T("/users/xuid(") << xboxUserId << _T(")/achievements/") << serviceConfigurationId << _T("/") << achievementId;
    return ss.str();
}

#if TV_API
#pragma pack(pop)
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END