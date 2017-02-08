//
//  XboxProviderBridge.cpp
//  XboxLiveServices
//
//  Created by Samuel Walker on 3/1/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#include "XboxProviderBridge.h"
#include "user_impl.h"
#include "utils.h"
#include "user_context.h"
#include "xbox_system_factory.h"

#pragma mark - Bridge

using namespace pplx;
using namespace xbox::services;
using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

string_t serviceVersionNumberWithRequestType(XboxProviderRequestType type) {
    switch (type) {
        case XboxProviderRequestType::XboxProfile: return _T("2");
        case XboxProviderRequestType::MicrosoftProfile: return _T("4");
        case XboxProviderRequestType::AccountTroubleShooter: return _T("4");
        case XboxProviderRequestType::CheckGamertag: return _T("1");
        case XboxProviderRequestType::ClaimGamertag: return _T("1");
        case XboxProviderRequestType::GamertagSuggestions: return _T("1");
        case XboxProviderRequestType::RealNameSettings: return _T("4");
    }
}

string_t serviceEndpointWithRequestType(XboxProviderRequestType type) {
    switch (type) {
        case XboxProviderRequestType::XboxProfile: return _T("profile");
        case XboxProviderRequestType::MicrosoftProfile: return _T("accounts");
        case XboxProviderRequestType::AccountTroubleShooter: return _T("accountstroubleshooter");
        case XboxProviderRequestType::CheckGamertag: return _T("user.mgt");
        case XboxProviderRequestType::ClaimGamertag: return _T("accounts");
        case XboxProviderRequestType::GamertagSuggestions: return _T("user.mgt");
        case XboxProviderRequestType::RealNameSettings: return _T("privacy");
    }
}

web::json::value json_body_profile_gamertag_check(string_t gamertag, string_t reservationId)
{
    web::json::value serializedObject;
    serializedObject[_T("Gamertag")] = web::json::value::string(gamertag);
    serializedObject[_T("ReservationId")] = web::json::value::string(reservationId);
    
    return serializedObject;
}

web::json::value json_body_profile_gamertag_claim(string_t gamertag, bool preview, string_t reservationId)
{
    web::json::value serializedObject;
    serializedObject[_T("gamertag")] = web::json::value::string(gamertag);
    serializedObject[_T("preview")] = web::json::value::boolean(preview);
    serializedObject[_T("reservationId")] = web::json::value::string(reservationId);
    
    return serializedObject;
}

web::json::value json_body_generate_gamertags(int count, string_t locale, string_t seed)
{
    web::json::value serializedObject;
    serializedObject[_T("Algorithm")] = web::json::value::number(1);
    serializedObject[_T("Count")] = web::json::value::number(count);
    serializedObject[_T("Locale")] = web::json::value::string(locale);
    serializedObject[_T("Seed")] = web::json::value::string(seed);
    
    return serializedObject;
}

// Gamertag is already provided
const string_t settings[] = {
    _T("AppDisplayName"),
    _T("GameDisplayPicRaw"),
    _T("Gamerscore"),
};

XboxProviderBridge::XboxProviderBridge()
{
    m_appConfig = xbox_live_app_config::get_app_config_singleton();
    m_xboxLiveContextSettings = std::make_shared<xbox::services::xbox_live_context_settings>();
    
    serializedSettings = serializeSettingsToJSON();
    settingsQuery = createSettingsQuery();
}

std::shared_ptr<http_call>
XboxProviderBridge::xbox_provider_http_call(XboxProviderRequestType type, string_t httpMethod, string_t query)
{
    string_t contractVersion = serviceVersionNumberWithRequestType(type);
    string_t host = serviceEndpointWithRequestType(type);
    auto endpoint = utils::create_xboxlive_endpoint(host, m_appConfig);

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
                                                                                               m_xboxLiveContextSettings,
                                                                                               httpMethod,
                                                                                               endpoint,
                                                                                               query,
                                                                                               xbox_live_api::unspecified);
    
    httpCall->set_retry_allowed(true);
    httpCall->set_xbox_contract_version_header_value(contractVersion);
    
    return httpCall;
}

pplx::task<std::shared_ptr<http_call_response>>
XboxProviderBridge::get_response(std::shared_ptr<http_call> httpCall, http_call_response_body_type bodyType)
{
    return httpCall->get_response_with_auth(bodyType);
}

xbox_live_result<xbox_user_profile>
XboxProviderBridge::get_xbox_profile()
{
    auto httpCall = xbox_provider_http_call(XboxProviderRequestType::XboxProfile, _T("POST"), _T("/users/batch/profile/settings"));
    
    std::vector<string_t> xboxUserIds = { m_xuid };
    web::json::value request;
    request[_T("userIds")] = utils::serialize_vector<string_t>(utils::json_string_serializer, xboxUserIds);
    request[_T("settings")] = serializedSettings;
    
    auto requestBody = request.serialize();
    httpCall->set_request_body(request.serialize());
    
    auto response = get_response(httpCall, http_call_response_body_type::json_body).get();
    if (response->err_code() != xbox_live_error_code::no_error)
    {
        return xbox_live_result<xbox_user_profile>(response->err_code(), response->err_message());
    }
    
    std::error_code errc = xbox_live_error_code::no_error;
    auto profileVector = utils::extract_json_vector<xbox_user_profile>(
                                                                       xbox_user_profile::_Deserialize,
                                                                       response->response_body_json(),
                                                                       _T("profileUsers"),
                                                                       errc,
                                                                       true
                                                                       );
    
    if (profileVector.size() > 1)
    {
        return xbox_live_result<xbox_user_profile>(response->err_code(), response->err_message());
    }
    
    return xbox_live_result<xbox_user_profile>(profileVector[0]);
}

xbox_live_result<web::json::value> XboxProviderBridge::get_microsoft_profile(bool isNewAccount)
{
    XboxProviderRequestType type = isNewAccount ? XboxProviderRequestType::AccountTroubleShooter : XboxProviderRequestType::MicrosoftProfile;
    auto httpCall = xbox_provider_http_call(type, _T("GET"), _T("/users/current/profile"));
    
    auto response = get_response(httpCall, http_call_response_body_type::json_body).get();
    if (response == nullptr) {
        return xbox_live_result<web::json::value>(xbox_live_error_code::runtime_error, "User is not signed in yet");
    }
    if (response->err_code() != xbox_live_error_code::no_error)
    {
        return xbox_live_result<web::json::value>(response->err_code(), response->err_message());
    }
    
    return xbox_live_result<web::json::value>(response->response_body_json());
}

xbox::services::xbox_live_result<web::json::value> XboxProviderBridge::post_microsoft_profile(bool isNewAccount, web::json::value body)
{
    XboxProviderRequestType type = isNewAccount ? XboxProviderRequestType::AccountTroubleShooter : XboxProviderRequestType::MicrosoftProfile;
    string_t path = isNewAccount ? _T("/Users/current/profile") : _T("/users/current/profile");
    auto httpCall = xbox_provider_http_call(type, _T("POST"), path);
    httpCall->set_request_body(body);
    
    auto response = get_response(httpCall, http_call_response_body_type::json_body).get();
    if (response == nullptr) {
        return xbox_live_result<web::json::value>(xbox_live_error_code::runtime_error, "User is not signed in yet");
    }
    
    if (response->err_code() != xbox_live_error_code::no_error)
    {
        return xbox_live_result<web::json::value>(response->err_code(), response->err_message());
    }
    
    return xbox_live_result<web::json::value>(response->response_body_json());
}

xbox::services::xbox_live_result<web::json::value> XboxProviderBridge::set_up_microsoft_profile(string_t locale)
{
    auto result = get_microsoft_profile(true);
    
    if (result.err())
    {
        return result;
    }
    
    auto body = result.payload();
    body = configure_microsoft_profile_body(body, locale);
    
    return post_microsoft_profile(true, body);
}

web::json::value XboxProviderBridge::configure_microsoft_profile_body(_In_ web::json::value body, _In_ string_t locale)
{
    auto date = utility::datetime::utc_now();
    body[_T("touAcceptanceDate")] = web::json::value::string(date.to_string(utility::datetime::date_format::ISO_8601));
    body[_T("partnerOptin")] = web::json::value::boolean(false);
    body[_T("msftOptin")] = web::json::value::boolean(false);
    
    std::error_code errc = xbox_live_error_code::no_error;
    auto legalCountry = utils::extract_json_string(body, _T("legalCountry"), errc);
    if (legalCountry.length() == 0)
    {
        body[_T("legalCountry")] = web::json::value::string(locale);
    }
    
    return body;
}

xbox::services::xbox_live_result<bool> XboxProviderBridge::check_gamertag(_In_ string_t gamertag)
{
    auto httpCall = xbox_provider_http_call(XboxProviderRequestType::CheckGamertag, _T("POST"), _T("/gamertags/reserve"));
    
    web::json::value body = json_body_profile_gamertag_check(gamertag, m_xuid);
    string_t serializedBody = body.serialize();
    httpCall->set_request_body(serializedBody);
    
    auto response = get_response(httpCall, http_call_response_body_type::json_body).get();
    if (response == nullptr) {
        return xbox_live_result<bool>(xbox_live_error_code::runtime_error, "User is not signed in yet");
    }
    
    uint32_t statusCode = response->http_status();
    if (statusCode != 200 && statusCode != 409)
    {
        // We are getting back an unexpected result
        return xbox_live_result<bool>(response->err_code(), response->err_message());
    }
    
    bool isValid = statusCode == 200;
    return xbox_live_result<bool>(isValid);
}

xbox::services::xbox_live_result<bool> XboxProviderBridge::claim_gamertag(_In_ string_t gamertag)
{
    auto httpCall = xbox_provider_http_call(XboxProviderRequestType::ClaimGamertag, _T("POST"), _T("/users/current/profile/gamertag"));
    
    web::json::value body = json_body_profile_gamertag_claim(gamertag, false, m_xuid);
    string_t serializedBody = body.serialize();
    httpCall->set_request_body(serializedBody);
    
    auto response = get_response(httpCall, http_call_response_body_type::json_body).get();
    if (response == nullptr) {
        return xbox_live_result<bool>(xbox_live_error_code::runtime_error, "User is not signed in yet");
    }
    
    uint32_t statusCode = response->http_status();
    if (statusCode != 200 && statusCode != 409)
    {
        // We are getting back an unexpected result
        return xbox_live_result<bool>(response->err_code(), response->err_message());
    }
    
    bool isValid = statusCode == 200;
    return xbox_live_result<bool>(isValid);
}

xbox::services::xbox_live_result<std::vector<string_t>>
XboxProviderBridge::get_gamertag_suggestions(_In_ string_t seed, _In_ string_t locale)
{
    auto httpCall = xbox_provider_http_call(XboxProviderRequestType::GamertagSuggestions, _T("POST"), _T("/gamertags/generate"));
    
    web::json::value body = json_body_generate_gamertags(3, locale, seed);
    string_t serializedBody = body.serialize();
    httpCall->set_request_body(serializedBody);
    
    auto response = get_response(httpCall, http_call_response_body_type::json_body).get();
    if (response == nullptr) {
        return xbox_live_result<std::vector<string_t>>(xbox_live_error_code::runtime_error, "User is not signed in yet");
    }
    
    if (response->err_code() != xbox_live_error_code::no_error)
    {
        return xbox_live_result<std::vector<string_t>>(response->err_code(), response->err_message());
    }
    
    std::error_code errc = xbox_live_error_code::no_error;
    std::vector<string_t> result = utils::extract_json_vector<string_t>(utils::json_string_extractor, response->response_body_json(), _T("Gamertags"), errc, true);
    
    return xbox_live_result<std::vector<string_t>>(result);
}

xbox::services::xbox_live_result<web::json::value> XboxProviderBridge::get_real_name_settings()
{
    stringstream_t ss;
    ss << "/users/xuid(";
    ss << m_xuid;
    ss << ")/privacy/settings";
    string_t query = ss.str();
    
    auto httpCall = xbox_provider_http_call(XboxProviderRequestType::RealNameSettings, _T("GET"), query);
    
    auto response = get_response(httpCall, http_call_response_body_type::json_body).get();
    if (response == nullptr) {
        return xbox_live_result<web::json::value>(xbox_live_error_code::runtime_error, "User is not signed in yet");
    }
    
    if (response->err_code() != xbox_live_error_code::no_error)
    {
        return xbox_live_result<web::json::value>(response->err_code(), response->err_message());
    }
        
    return xbox_live_result<web::json::value>(response->response_body_json());
}

xbox::services::xbox_live_result<bool> XboxProviderBridge::post_real_name_settings(_In_ web::json::value body)
{
    stringstream_t ss;
    ss << "/users/xuid(";
    ss << m_xuid;
    ss << ")/privacy/settings";
    string_t query = ss.str();
    
    auto httpCall = xbox_provider_http_call(XboxProviderRequestType::RealNameSettings, _T("PUT"), query);
    httpCall->set_request_body(body);
    
    auto response = get_response(httpCall, http_call_response_body_type::json_body).get();
    if (response == nullptr) {
        return xbox_live_result<bool>(xbox_live_error_code::runtime_error, "User is not signed in yet");
    }
    
    return xbox_live_result<bool>(response->err_code() == xbox_live_error_code::no_error);
}

xbox::services::xbox_live_result<bool> XboxProviderBridge::set_up_real_name_settings()
{
    auto result = get_real_name_settings();
    
    if (result.err())
    {
        return false;
    }
    
    auto body = result.payload();
    body = configure_real_name_settings(body);
    
    if (body != web::json::value::Null) {
        return post_real_name_settings(body);
    }
    
    return false;
}

web::json::value XboxProviderBridge::configure_real_name_settings(_In_ web::json::value body)
{
    auto ShareIdentityKey = _T("ShareIdentity");
    auto ShareIdentityTransitivityKey = _T("ShareIdentityTransitively");
    
    web::json::value field(utils::extract_json_field(body, "settings", false));
    std::error_code errc = xbox_live_error_code::no_error;
    const web::json::array settings = utils::extract_json_as_array(field, errc);
    if (errc == xbox_live_error_code::json_error) {
        return web::json::value::Null;
    }
    
    std::vector<web::json::value> result;
    
    for (const auto& element : settings)
    {
        std::error_code errc = xbox_live_error_code::no_error;
        
        auto setting = utils::extract_json_string(element, _T("setting"), errc);
        if (setting.compare(ShareIdentityKey) == 0)
        {
            auto shareIdentity = utils::extract_json_string(element, _T("value"), errc);
            if (errc != xbox_live_error_code::no_error) {
                return web::json::value::Null;
            }
            
            if (shareIdentity.compare(_T("NotSet")) != 0) {
                return web::json::value::Null;
            }
        }
        else if (setting.compare(ShareIdentityTransitivityKey) == 0)
        {
            auto shareIdentityTransitivity = utils::extract_json_string(element, _T("value"), errc);
            if (errc != xbox_live_error_code::no_error) {
                return web::json::value::Null;
            }
            
            if (shareIdentityTransitivity.compare(_T("NotSet")) != 0) {
                return web::json::value::Null;
            }
        }
    }
    
    {
        web::json::value setting;
        setting[_T("setting")] = web::json::value::string(ShareIdentityKey);
        setting[_T("value")] = web::json::value::string(_T("PeopleOnMyList"));
        result.push_back(setting);
    }
    
    {
        web::json::value setting;
        setting[_T("setting")] = web::json::value::string(ShareIdentityTransitivityKey);
        setting[_T("value")] = web::json::value::string(_T("Everyone"));
        result.push_back(setting);
    }
    
    int i = 0;
    web::json::value jsonArray = web::json::value::array();
    for (auto& s : result)
    {
        jsonArray[i++] = s;
    }
    
    web::json::value resultBody;
    resultBody[_T("settings")] = jsonArray;
    
    return resultBody;
}

string_t XboxProviderBridge::createSettingsQuery()
{
    stringstream_t source;
    uint32_t arraySize = ARRAYSIZE(settings);
    for (uint32_t i = 0; i < arraySize; ++i)
    {
        const string_t& setting = settings[i];
        source << web::http::uri::encode_uri(setting);
        if (i + 1 != arraySize)
        {
            source << _T(",");
        }
    }
    
    return source.str();
}

web::json::value XboxProviderBridge::serializeSettingsToJSON()
{
    uint32_t size = ARRAYSIZE(settings);
    std::vector<string_t> settingsVector(settings, settings + size);
    return utils::serialize_vector<string_t>(utils::json_string_serializer, settingsVector);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END
