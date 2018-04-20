// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Utils.h"
#include "user_context.h"
#include "xbox_system_factory.h"
#include "xsapi/entertainment_profile.h"

#if TV_API || UNIT_TEST_SERVICES
string_t c_entertainmentProfileListContractVersionHeaderValue = _T("2");

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_CPP_BEGIN

entertainment_profile_list_xbox_one_pins::entertainment_profile_list_xbox_one_pins(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

pplx::task<xbox::services::xbox_live_result<void>> entertainment_profile_list_xbox_one_pins::add_item(
    _In_ xbox::services::marketplace::media_item_type itemType,
    _In_ const string_t& providerId,
    _In_ const string_t& provider,
    _In_ const web::uri& imageUrl,
    _In_ const web::uri& providerLogoImageUrl,
    _In_ const string_t& title,
    _In_ const string_t& subTitle,
    _In_ const string_t& locale
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(locale.empty(), void, "locale is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(provider.empty(), void, "provider is empty");

    auto subPath = entertainment_profile_list_xbox_one_pins_add_item_sub_path(utils::string_t_from_internal_string(m_userContext->xbox_user_id()));

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("eplists"), m_appConfig),
        subPath,
        xbox_live_api::xbox_one_pins_add_item
        );

    httpCall->set_xbox_contract_version_header_value(c_entertainmentProfileListContractVersionHeaderValue);

    web::json::value request;
    web::json::value item;
    item[_T("ContentType")] = web::json::value::string(xbox::services::marketplace::catalog_service::_Convert_media_item_type_to_string(itemType));
    if(providerId.empty())
    {
        item[_T("ProviderId")] = web::json::value::null();
    }
    else
    {
        item[_T("ProviderId")] = web::json::value::string(providerId);
    }
    item[_T("Provider")] = web::json::value::string(provider);
    item[_T("Locale")] = web::json::value::string(locale);
    if(!imageUrl.is_empty())
    {
        item[_T("ImageUrl")] = web::json::value::string(imageUrl.to_string());
    }
    if(!providerLogoImageUrl.is_empty())
    {
        item[_T("AltImageUrl")] = web::json::value::string(providerLogoImageUrl.to_string());
    }
    if(!title.empty())
    {
        item[_T("Title")] = web::json::value::string(title);
    }
    if(!subTitle.empty())
    {
        item[_T("SubTitle")] = web::json::value::string(subTitle);
    }
    std::vector<web::json::value> items;
    items.push_back(item);
    request[_T("Items")] = web::json::value::array(items);

    httpCall->set_request_body(request);

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<xbox::services::http_call_response> response)
    {
        return xbox_live_result<void>(response->err_code(), response->err_message());
    });

    return utils::create_exception_free_task<void>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<void>> entertainment_profile_list_xbox_one_pins::remove_item(
    _In_ const string_t& providerId,
    _In_ const string_t& provider
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(provider.empty(), void, "provider is empty");

    auto subPath = entertainment_profile_list_xbox_one_pins_remove_item_sub_path(utils::string_t_from_internal_string(m_userContext->xbox_user_id()));

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("eplists"), m_appConfig),
        subPath,
        xbox_live_api::xbox_one_pins_remove_item
        );

    httpCall->set_xbox_contract_version_header_value(c_entertainmentProfileListContractVersionHeaderValue);

    web::json::value request;
    web::json::value item;
    if(providerId.empty())
    {
        item[_T("ProviderId")] = web::json::value::Null;
    }
    else
    {
        item[_T("ProviderId")] = web::json::value::string(providerId);
    }
    item[_T("Provider")] = web::json::value::string(provider);

    std::vector<web::json::value> items;
    items.push_back(item);
    request[_T("Items")] = web::json::value::array(items);

    httpCall->set_request_body(request);

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<xbox::services::http_call_response> response)
    {
        return xbox_live_result<void>(response->err_code(), response->err_message());
    });
    return utils::create_exception_free_task<void>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<entertainment_profile_list_contains_item_result>> entertainment_profile_list_xbox_one_pins::contains_item(
    _In_ const string_t& providerId,
    _In_ const string_t& provider
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(provider.empty(), entertainment_profile_list_contains_item_result, "provider is empty");

    auto subPath = entertainment_profile_list_xbox_one_pins_contains_item_sub_path(utils::string_t_from_internal_string(m_userContext->xbox_user_id()));

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("eplists"), m_appConfig),
        subPath,
        xbox_live_api::xbox_one_pins_contains_item
        );

    httpCall->set_xbox_contract_version_header_value(c_entertainmentProfileListContractVersionHeaderValue);

    web::json::value request;
    web::json::value item;
    if(providerId.empty())
    {
        item[_T("ProviderId")] = web::json::value::Null;
    }
    else
    {
        item[_T("ProviderId")] = web::json::value::string(providerId);
    }
    item[_T("Provider")] = web::json::value::string(provider);

    std::vector<web::json::value> items;
    items.push_back(item);
    request[_T("Items")] = web::json::value::array(items);

    httpCall->set_request_body(request);

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<xbox::services::http_call_response> response)
    {
        auto itemResult = entertainment_profile_list_contains_item_result::_Deserialize(response->response_body_json());
        return utils::generate_xbox_live_result<entertainment_profile_list_contains_item_result>(
            itemResult,
            response
            );
    });
    return utils::create_exception_free_task<entertainment_profile_list_contains_item_result>(
        task
        );
}

string_t entertainment_profile_list_xbox_one_pins::entertainment_profile_list_xbox_one_pins_add_item_sub_path(
    _In_ const string_t& xboxUserId
    )
{
    stringstream_t ss;
    ss << _T("/users/xuid(") << xboxUserId << _T(")/lists/PINS/XBLPins");
    return ss.str();
}

string_t entertainment_profile_list_xbox_one_pins::entertainment_profile_list_xbox_one_pins_remove_item_sub_path(
    _In_ const string_t& xboxUserId
    )
{
    stringstream_t ss;
    ss << _T("/users/xuid(") << xboxUserId << _T(")/lists/PINS/XBLPins/RemoveItems");
    return ss.str();
}

string_t entertainment_profile_list_xbox_one_pins::entertainment_profile_list_xbox_one_pins_contains_item_sub_path(
    _In_ const string_t& xboxUserId
    )
{
    stringstream_t ss;
    ss << _T("/users/xuid(") << xboxUserId << _T(")/lists/PINS/XBLPins/ContainsItems");
    return ss.str();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_CPP_END
#endif