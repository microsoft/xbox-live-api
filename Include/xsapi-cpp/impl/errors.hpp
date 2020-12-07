// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <sstream>

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

static xbox_services_error_code_category_impl s_error_code_category_instance;

inline const xbox_services_error_code_category_impl& xbox_services_error_code_category()
{
    return s_error_code_category_instance;
}

static xbox_services_error_condition_category_impl s_error_condition_category_instance;

inline const xbox_services_error_condition_category_impl& xbox_services_error_condition_category()
{
    return s_error_condition_category_instance;
}

inline std::string xbox_services_error_code_category_impl::message(_In_ int errorCode) const _NOEXCEPT
{
    xbox_live_error_code code = static_cast<xbox_live_error_code>(errorCode);
    switch (code)
    {
        case xbox_live_error_code::no_error: return "No error";
        case xbox_live_error_code::bad_alloc: return "bad_alloc";
        case xbox_live_error_code::bad_cast: return "bad_cast";
        case xbox_live_error_code::invalid_argument: return "invalid_argument";
        case xbox_live_error_code::out_of_range: return "out_of_range";
        case xbox_live_error_code::length_error: return "length_error";
        case xbox_live_error_code::range_error: return "range_error";
        case xbox_live_error_code::logic_error: return "logic_error";
        case xbox_live_error_code::runtime_error: return "runtime_error";
        case xbox_live_error_code::json_error: return "json_error";
        case xbox_live_error_code::websocket_error: return "websocket_error";
        case xbox_live_error_code::uri_error: return "uri_error";
        case xbox_live_error_code::generic_error: return "generic_error";

        case xbox_live_error_code::rta_generic_error: return "rta_generic_error";
        case xbox_live_error_code::rta_access_denied: return "rta_access_denied";
        case xbox_live_error_code::rta_subscription_limit_reached: return "rta_subscription_limit_reached";

        case xbox_live_error_code::auth_user_interaction_required: return "auth_user_interaction_required";
        case xbox_live_error_code::auth_user_switched: return "auth_user_switched";
        case xbox_live_error_code::auth_user_cancel: return "auth_user_cancel";
        case xbox_live_error_code::auth_unknown_error: return "auth_unknown_error";
        case xbox_live_error_code::auth_user_not_signed_in: return "auth_user_not_signed_in";
        case xbox_live_error_code::auth_runtime_error: return "auth_runtime_error";
        case xbox_live_error_code::auth_no_token_error: return "auth_no_token_error";

        case xbox_live_error_code::invalid_config: return "invalid_config";
        case xbox_live_error_code::unsupported: return "unsupported";

        case xbox_live_error_code::HR_ERROR_INTERNET_TIMEOUT: return "ERROR_INTERNET_TIMEOUT";
        case xbox_live_error_code::AM_E_XASD_UNEXPECTED: return "AM_E_XASD_UNEXPECTED";
        case xbox_live_error_code::AM_E_XASU_UNEXPECTED: return "AM_E_XASU_UNEXPECTED";
        case xbox_live_error_code::AM_E_XAST_UNEXPECTED: return "AM_E_XAST_UNEXPECTED";
        case xbox_live_error_code::AM_E_XSTS_UNEXPECTED: return "AM_E_XSTS_UNEXPECTED";
        case xbox_live_error_code::AM_E_XDEVICE_UNEXPECTED: return "AM_E_XDEVICE_UNEXPECTED";
        case xbox_live_error_code::AM_E_DEVMODE_NOT_AUTHORIZED: return "AM_E_DEVMODE_NOT_AUTHORIZED";
        case xbox_live_error_code::AM_E_NOT_AUTHORIZED: return "AM_E_NOT_AUTHORIZED";
        case xbox_live_error_code::AM_E_FORBIDDEN: return "AM_E_FORBIDDEN";
        case xbox_live_error_code::AM_E_UNKNOWN_TARGET: return "AM_E_UNKNOWN_TARGET";
        case xbox_live_error_code::AM_E_INVALID_NSAL_DATA: return "AM_E_INVALID_NSAL_DATA";
        case xbox_live_error_code::AM_E_TITLE_NOT_AUTHENTICATED: return "AM_E_TITLE_NOT_AUTHENTICATED";
        case xbox_live_error_code::AM_E_TITLE_NOT_AUTHORIZED: return "AM_E_TITLE_NOT_AUTHORIZED";
        case xbox_live_error_code::AM_E_USER_HASH_MISSING: return "AM_E_USER_HASH_MISSING";
        case xbox_live_error_code::AM_E_USER_NOT_FOUND: return "AM_E_USER_NOT_FOUND";
        case xbox_live_error_code::AM_E_INVALID_ENVIRONMENT: return "AM_E_INVALID_ENVIRONMENT";
        case xbox_live_error_code::AM_E_XASD_TIMEOUT: return "AM_E_XASD_TIMEOUT";
        case xbox_live_error_code::AM_E_XASU_TIMEOUT: return "AM_E_XASU_TIMEOUT";
        case xbox_live_error_code::AM_E_XAST_TIMEOUT: return "AM_E_XAST_TIMEOUT";
        case xbox_live_error_code::AM_E_XSTS_TIMEOUT: return "AM_E_XSTS_TIMEOUT";
        case xbox_live_error_code::AM_E_LIVE_CONNECTION_REQUIRED: return "AM_E_LIVE_CONNECTION_REQUIRED";
        case xbox_live_error_code::AM_E_NO_NETWORK: return "AM_E_NO_NETWORK";
        case xbox_live_error_code::AM_E_XTITLE_UNEXPECTED: return "AM_E_XTITLE_UNEXPECTED";
        case xbox_live_error_code::AM_E_NO_TOKEN_REQUIRED: return "AM_E_NO_TOKEN_REQUIRED";
        case xbox_live_error_code::AM_E_XTITLE_TIMEOUT: return "AM_E_XTITLE_TIMEOUT";
        case xbox_live_error_code::XO_E_DEVMODE_NOT_AUTHORIZED: return "XO_E_DEVMODE_NOT_AUTHORIZED";
        case xbox_live_error_code::XO_E_SYSTEM_UPDATE_REQUIRED: return "XO_E_SYSTEM_UPDATE_REQUIRED";
        case xbox_live_error_code::XO_E_CONTENT_UPDATE_REQUIRED: return "XO_E_CONTENT_UPDATE_REQUIRED";
        case xbox_live_error_code::XO_E_ENFORCEMENT_BAN: return "XO_E_ENFORCEMENT_BAN";
        case xbox_live_error_code::XO_E_THIRD_PARTY_BAN: return "XO_E_THIRD_PARTY_BAN";
        case xbox_live_error_code::XO_E_ACCOUNT_PARENTALLY_RESTRICTED: return "XO_E_ACCOUNT_PARENTALLY_RESTRICTED";
        case xbox_live_error_code::XO_E_ACCOUNT_BILLING_MAINTENANCE_REQUIRED: return "XO_E_ACCOUNT_BILLING_MAINTENANCE_REQUIRED";
        case xbox_live_error_code::XO_E_ACCOUNT_TERMS_OF_USE_NOT_ACCEPTED: return "XO_E_ACCOUNT_TERMS_OF_USE_NOT_ACCEPTED";
        case xbox_live_error_code::XO_E_ACCOUNT_COUNTRY_NOT_AUTHORIZED: return "XO_E_ACCOUNT_COUNTRY_NOT_AUTHORIZED";
        case xbox_live_error_code::XO_E_ACCOUNT_AGE_VERIFICATION_REQUIRED: return "XO_E_ACCOUNT_AGE_VERIFICATION_REQUIRED";
        case xbox_live_error_code::XO_E_ACCOUNT_CURFEW: return "XO_E_ACCOUNT_CURFEW";
        case xbox_live_error_code::XO_E_ACCOUNT_CHILD_NOT_IN_FAMILY: return "XO_E_ACCOUNT_CHILD_NOT_IN_FAMILY";
        case xbox_live_error_code::XO_E_ACCOUNT_CSV_TRANSITION_REQUIRED: return "XO_E_ACCOUNT_CSV_TRANSITION_REQUIRED";
        case xbox_live_error_code::XO_E_ACCOUNT_MAINTENANCE_REQUIRED: return "XO_E_ACCOUNT_MAINTENANCE_REQUIRED";
        case xbox_live_error_code::XO_E_ACCOUNT_TYPE_NOT_ALLOWED: return "XO_E_ACCOUNT_TYPE_NOT_ALLOWED";
        case xbox_live_error_code::XO_E_CONTENT_ISOLATION: return "XO_E_CONTENT_ISOLATION";
        case xbox_live_error_code::XO_E_ACCOUNT_NAME_CHANGE_REQUIRED: return "XO_E_ACCOUNT_NAME_CHANGE_REQUIRED";
        case xbox_live_error_code::XO_E_DEVICE_CHALLENGE_REQUIRED: return "XO_E_DEVICE_CHALLENGE_REQUIRED";
        case xbox_live_error_code::XO_E_SIGNIN_COUNT_BY_DEVICE_TYPE_EXCEEDED: return "XO_E_SIGNIN_COUNT_BY_DEVICE_TYPE_EXCEEDED";
        case xbox_live_error_code::XO_E_PIN_CHALLENGE_REQUIRED: return "XO_E_PIN_CHALLENGE_REQUIRED";
        case xbox_live_error_code::XO_E_RETAIL_ACCOUNT_NOT_ALLOWED: return "XO_E_RETAIL_ACCOUNT_NOT_ALLOWED";
        case xbox_live_error_code::XO_E_SANDBOX_NOT_ALLOWED: return "XO_E_SANDBOX_NOT_ALLOWED";
        case xbox_live_error_code::XO_E_ACCOUNT_SERVICE_UNAVAILABLE_UNKNOWN_USER: return "XO_E_ACCOUNT_SERVICE_UNAVAILABLE_UNKNOWN_USER";
        case xbox_live_error_code::XO_E_GREEN_SIGNED_CONTENT_NOT_AUTHORIZED: return "XO_E_GREEN_SIGNED_CONTENT_NOT_AUTHORIZED";
        case xbox_live_error_code::XO_E_CONTENT_NOT_AUTHORIZED: return "XO_E_CONTENT_NOT_AUTHORIZED";

        case xbox_live_error_code::http_status_204_resource_data_not_found: return "204 - ResourceDataNotFound"; 
        case xbox_live_error_code::http_status_400_bad_request: return "400 - BadRequest";
        case xbox_live_error_code::http_status_401_unauthorized: return "401 - Unauthorized";
        case xbox_live_error_code::http_status_403_forbidden: return "403 - Forbidden";
        case xbox_live_error_code::http_status_404_not_found: return "404 - NotFound";
        case xbox_live_error_code::http_status_405_method_not_allowed: return "405 - MethodNotAllowed";
        case xbox_live_error_code::http_status_406_not_acceptable: return "406 - NotAcceptable";
        case xbox_live_error_code::http_status_407_proxy_authentication_required: return "407 - ProxyAuthenticationRequired";
        case xbox_live_error_code::http_status_408_request_timeout: return "408 - RequestTimeout";
        case xbox_live_error_code::http_status_409_conflict: return "409 - Conflict";
        case xbox_live_error_code::http_status_410_gone: return "410 - Gone";
        case xbox_live_error_code::http_status_411_length_required: return "411 - LengthRequired";
        case xbox_live_error_code::http_status_412_precondition_failed: return "412 - PreconditionFailed";
        case xbox_live_error_code::http_status_413_request_entity_too_large: return "413 - RequestEntityTooLarge";
        case xbox_live_error_code::http_status_414_request_uri_too_long: return "414 - RequestUriTooLong";
        case xbox_live_error_code::http_status_415_unsupported_media_type: return "415 - UnsupportedMediaType";
        case xbox_live_error_code::http_status_416_requested_range_not_satisfiable: return "416 - RequestedRangeNotSatisfiable";
        case xbox_live_error_code::http_status_417_expectation_failed: return "417 - Expectation Failed";
        case xbox_live_error_code::http_status_421_misdirected_request: return "421 - Misdirected Request";
        case xbox_live_error_code::http_status_422_unprocessable_entity: return "422 - Unprocessable Entity";
        case xbox_live_error_code::http_status_423_locked: return "423 - Locked";
        case xbox_live_error_code::http_status_424_failed_dependency: return "424 - Failed Dependency";
        case xbox_live_error_code::http_status_426_upgrade_required: return "426 - Upgrade Required";
        case xbox_live_error_code::http_status_428_precondition_required: return "428 - Precondition Required";
        case xbox_live_error_code::http_status_429_too_many_requests: return "429 - TooManyRequests";
        case xbox_live_error_code::http_status_431_request_header_fields_too_large: return "431 - Request Header Fields Too Large";
        case xbox_live_error_code::http_status_451_unavailable_for_legal_reasons: return "451 - Unavailable For Legal Reasons";
        case xbox_live_error_code::http_status_500_internal_server_error: return "500 - InternalServerError";
        case xbox_live_error_code::http_status_501_not_implemented: return "501 - Not Implemented";
        case xbox_live_error_code::http_status_502_bad_gateway: return "502 - Bad Gateway";
        case xbox_live_error_code::http_status_503_service_unavailable: return "503 - ServiceUnavailable";
        case xbox_live_error_code::http_status_504_gateway_timeout: return "504 - GatewayTimeout";
        case xbox_live_error_code::http_status_505_http_version_not_supported: return "505 - HttpVersionNotSupported";
        case xbox_live_error_code::http_status_506_variant_also_negotiates: return "506 - Variant Also Negotiates";
        case xbox_live_error_code::http_status_507_insufficient_storage: return "507 - Insufficient Storage";
        case xbox_live_error_code::http_status_508_loop_detected: return "508 - Loop Detected";
        case xbox_live_error_code::http_status_510_not_extended: return "510 - Not Extended";
        case xbox_live_error_code::http_status_511_network_authentication_required: return "511 - Network Authentication Required";

        case xbox_live_error_code::HR_INET_E_INVALID_URL: return "INET_E_INVALID_URL";
        case xbox_live_error_code::HR_INET_E_NO_SESSION: return "INET_E_NO_SESSION";
        case xbox_live_error_code::HR_INET_E_CANNOT_CONNECT: return "INET_E_CANNOT_CONNECT";
        case xbox_live_error_code::HR_INET_E_RESOURCE_NOT_FOUND: return "INET_E_RESOURCE_NOT_FOUND";
        case xbox_live_error_code::HR_INET_E_OBJECT_NOT_FOUND: return "INET_E_OBJECT_NOT_FOUND";
        case xbox_live_error_code::HR_INET_E_DATA_NOT_AVAILABLE: return "INET_E_DATA_NOT_AVAILABLE";
        case xbox_live_error_code::HR_INET_E_DOWNLOAD_FAILURE: return "INET_E_DOWNLOAD_FAILURE";
        case xbox_live_error_code::HR_INET_E_AUTHENTICATION_REQUIRED: return "INET_E_AUTHENTICATION_REQUIRED";
        case xbox_live_error_code::HR_INET_E_NO_VALID_MEDIA: return "INET_E_NO_VALID_MEDIA";
        case xbox_live_error_code::HR_INET_E_CONNECTION_TIMEOUT: return "INET_E_CONNECTION_TIMEOUT";
        case xbox_live_error_code::HR_INET_E_INVALID_REQUEST: return "INET_E_INVALID_REQUEST";
        case xbox_live_error_code::HR_INET_E_UNKNOWN_PROTOCOL: return "INET_E_UNKNOWN_PROTOCOL";
        case xbox_live_error_code::HR_INET_E_SECURITY_PROBLEM: return "INET_E_SECURITY_PROBLEM";
        case xbox_live_error_code::HR_INET_E_CANNOT_LOAD_DATA: return "INET_E_CANNOT_LOAD_DATA";
        case xbox_live_error_code::HR_INET_E_CANNOT_INSTANTIATE_OBJECT: return "INET_E_CANNOT_INSTANTIATE_OBJECT";
        case xbox_live_error_code::HR_INET_E_INVALID_CERTIFICATE: return "INET_E_INVALID_CERTIFICATE";
        case xbox_live_error_code::HR_INET_E_REDIRECT_FAILED: return "INET_E_REDIRECT_FAILED";
        case xbox_live_error_code::HR_INET_E_REDIRECT_TO_DIR: return "INET_E_REDIRECT_TO_DIR";
        case xbox_live_error_code::HR_ERROR_NETWORK_UNREACHABLE: return "ERROR_NETWORK_UNREACHABLE";

        default:
        {
            std::stringstream msg;
            msg << "Unknown error: 0x" << std::hex << errorCode;
            return msg.str();
        }
    }
}

inline std::string xbox_services_error_condition_category_impl::message(_In_ int errorCode) const _NOEXCEPT
{
    xbox_live_error_condition code = static_cast<xbox_live_error_condition>(errorCode);

    switch (code)
    {
        case xbox_live_error_condition::no_error: return "No error";
        case xbox_live_error_condition::generic_error: return "Generic Error";
        case xbox_live_error_condition::generic_out_of_range: return "Out of Range";
        case xbox_live_error_condition::auth: return "Authorization Error";
        case xbox_live_error_condition::http: return "HTTP";
        case xbox_live_error_condition::http_404_not_found: return "404 - Not Found";
        case xbox_live_error_condition::http_412_precondition_failed: return "412 - PreconditionFailed";
        case xbox_live_error_condition::http_429_too_many_requests: return "429- Too Many Requests";
        case xbox_live_error_condition::http_service_timeout: return "Service Timeout";
        case xbox_live_error_condition::network: return "Network Error";
        case xbox_live_error_condition::rta: return "Real Time Activity";
        
        default:
        {
            std::stringstream msg;
            msg << "Unknown error: 0x" << std::hex << errorCode;
            return msg.str();
        }
    }
}

inline bool xbox_services_error_condition_category_impl::equivalent(
    _In_ const std::error_code& arbitraryErrorCode,
    _In_ int xboxLiveErrorCondition) const _NOEXCEPT
{
    if (arbitraryErrorCode.category() != xbox_services_error_code_category())
    {
        return false;
    }

    xbox_live_error_code code = static_cast<xbox_live_error_code>(arbitraryErrorCode.value());
    xbox_live_error_condition condition = static_cast<xbox_live_error_condition>(xboxLiveErrorCondition);

    // range 0x80860000 - 0x8086FFFF is reserved for other OnlineId error code
    // put is under auth_msa error condition
    if ((arbitraryErrorCode.value() & 0x80860000) == 0x80860000)
    {
        return condition == xbox_live_error_condition::auth;
    }

    switch (code)
    {
        case xbox_live_error_code::no_error:
            return (condition == xbox_live_error_condition::no_error);

        case xbox_live_error_code::http_status_404_not_found:
            return (condition == xbox_live_error_condition::http_404_not_found ||
                    condition == xbox_live_error_condition::http);

        case xbox_live_error_code::http_status_412_precondition_failed:
            return (condition == xbox_live_error_condition::http_412_precondition_failed ||
                    condition == xbox_live_error_condition::http);

        case xbox_live_error_code::http_status_408_request_timeout:
        case xbox_live_error_code::http_status_503_service_unavailable:
        case xbox_live_error_code::http_status_504_gateway_timeout:
            return (condition == xbox_live_error_condition::http_service_timeout ||
                    condition == xbox_live_error_condition::http);

        case xbox_live_error_code::http_status_429_too_many_requests:
            return (condition == xbox_live_error_condition::http_429_too_many_requests ||
                    condition == xbox_live_error_condition::http);

        case xbox_live_error_code::http_status_500_internal_server_error:
            return (condition == xbox_live_error_condition::http);

        case xbox_live_error_code::http_status_204_resource_data_not_found:
        case xbox_live_error_code::http_status_400_bad_request:
        case xbox_live_error_code::http_status_401_unauthorized:
        case xbox_live_error_code::http_status_402_payment_required:
        case xbox_live_error_code::http_status_403_forbidden:
        case xbox_live_error_code::http_status_405_method_not_allowed:
        case xbox_live_error_code::http_status_406_not_acceptable:
        case xbox_live_error_code::http_status_407_proxy_authentication_required:
        case xbox_live_error_code::http_status_409_conflict:
        case xbox_live_error_code::http_status_410_gone:
        case xbox_live_error_code::http_status_411_length_required:
        case xbox_live_error_code::http_status_413_request_entity_too_large:
        case xbox_live_error_code::http_status_414_request_uri_too_long:
        case xbox_live_error_code::http_status_415_unsupported_media_type:
        case xbox_live_error_code::http_status_416_requested_range_not_satisfiable:
        case xbox_live_error_code::http_status_417_expectation_failed:
        case xbox_live_error_code::http_status_421_misdirected_request:
        case xbox_live_error_code::http_status_422_unprocessable_entity:
        case xbox_live_error_code::http_status_423_locked:
        case xbox_live_error_code::http_status_424_failed_dependency:
        case xbox_live_error_code::http_status_426_upgrade_required:
        case xbox_live_error_code::http_status_428_precondition_required:
        case xbox_live_error_code::http_status_431_request_header_fields_too_large:
        case xbox_live_error_code::http_status_449_retry_with:
        case xbox_live_error_code::http_status_451_unavailable_for_legal_reasons:
        case xbox_live_error_code::http_status_501_not_implemented:
        case xbox_live_error_code::http_status_502_bad_gateway:
        case xbox_live_error_code::http_status_505_http_version_not_supported:
        case xbox_live_error_code::http_status_506_variant_also_negotiates:
        case xbox_live_error_code::http_status_507_insufficient_storage:
        case xbox_live_error_code::http_status_508_loop_detected:
        case xbox_live_error_code::http_status_510_not_extended:
        case xbox_live_error_code::http_status_511_network_authentication_required:
            return (condition == xbox_live_error_condition::http);

        case xbox_live_error_code::auth_user_interaction_required:
        case xbox_live_error_code::auth_user_switched:
        case xbox_live_error_code::auth_unknown_error:
        case xbox_live_error_code::auth_user_cancel:
        case xbox_live_error_code::auth_user_not_signed_in:
        case xbox_live_error_code::auth_runtime_error:
        case xbox_live_error_code::auth_no_token_error:
            return condition == xbox_live_error_condition::auth;

        case xbox_live_error_code::invalid_config:
        case xbox_live_error_code::unsupported:
            return condition == xbox_live_error_condition::generic_error;

        case xbox_live_error_code::AM_E_XASD_UNEXPECTED:
        case xbox_live_error_code::AM_E_XASU_UNEXPECTED:
        case xbox_live_error_code::AM_E_XAST_UNEXPECTED:
        case xbox_live_error_code::AM_E_XSTS_UNEXPECTED:
        case xbox_live_error_code::AM_E_XDEVICE_UNEXPECTED:
        case xbox_live_error_code::AM_E_DEVMODE_NOT_AUTHORIZED:
        case xbox_live_error_code::AM_E_NOT_AUTHORIZED:
        case xbox_live_error_code::AM_E_FORBIDDEN:
        case xbox_live_error_code::AM_E_UNKNOWN_TARGET:
        case xbox_live_error_code::AM_E_INVALID_NSAL_DATA:
        case xbox_live_error_code::AM_E_TITLE_NOT_AUTHENTICATED:
        case xbox_live_error_code::AM_E_TITLE_NOT_AUTHORIZED:
        case xbox_live_error_code::AM_E_USER_HASH_MISSING:
        case xbox_live_error_code::AM_E_USER_NOT_FOUND:
        case xbox_live_error_code::AM_E_INVALID_ENVIRONMENT:
        case xbox_live_error_code::AM_E_XASD_TIMEOUT:
        case xbox_live_error_code::AM_E_XASU_TIMEOUT:
        case xbox_live_error_code::AM_E_XAST_TIMEOUT:
        case xbox_live_error_code::AM_E_XSTS_TIMEOUT:
        case xbox_live_error_code::AM_E_LIVE_CONNECTION_REQUIRED:
        case xbox_live_error_code::AM_E_NO_NETWORK:
        case xbox_live_error_code::AM_E_XTITLE_UNEXPECTED:
        case xbox_live_error_code::AM_E_NO_TOKEN_REQUIRED:
        case xbox_live_error_code::AM_E_XTITLE_TIMEOUT:
        case xbox_live_error_code::XO_E_DEVMODE_NOT_AUTHORIZED:
        case xbox_live_error_code::XO_E_SYSTEM_UPDATE_REQUIRED:
        case xbox_live_error_code::XO_E_CONTENT_UPDATE_REQUIRED:
        case xbox_live_error_code::XO_E_ENFORCEMENT_BAN:
        case xbox_live_error_code::XO_E_THIRD_PARTY_BAN:
        case xbox_live_error_code::XO_E_ACCOUNT_PARENTALLY_RESTRICTED:
        case xbox_live_error_code::XO_E_ACCOUNT_BILLING_MAINTENANCE_REQUIRED:
        case xbox_live_error_code::XO_E_ACCOUNT_TERMS_OF_USE_NOT_ACCEPTED:
        case xbox_live_error_code::XO_E_ACCOUNT_COUNTRY_NOT_AUTHORIZED:
        case xbox_live_error_code::XO_E_ACCOUNT_AGE_VERIFICATION_REQUIRED:
        case xbox_live_error_code::XO_E_ACCOUNT_CURFEW:
        case xbox_live_error_code::XO_E_ACCOUNT_CHILD_NOT_IN_FAMILY:
        case xbox_live_error_code::XO_E_ACCOUNT_CSV_TRANSITION_REQUIRED:
        case xbox_live_error_code::XO_E_ACCOUNT_MAINTENANCE_REQUIRED:
        case xbox_live_error_code::XO_E_ACCOUNT_TYPE_NOT_ALLOWED:
        case xbox_live_error_code::XO_E_CONTENT_ISOLATION:
        case xbox_live_error_code::XO_E_ACCOUNT_NAME_CHANGE_REQUIRED:
        case xbox_live_error_code::XO_E_DEVICE_CHALLENGE_REQUIRED:
        case xbox_live_error_code::XO_E_SIGNIN_COUNT_BY_DEVICE_TYPE_EXCEEDED:
        case xbox_live_error_code::XO_E_PIN_CHALLENGE_REQUIRED:
        case xbox_live_error_code::XO_E_RETAIL_ACCOUNT_NOT_ALLOWED:
        case xbox_live_error_code::XO_E_SANDBOX_NOT_ALLOWED:
        case xbox_live_error_code::XO_E_ACCOUNT_SERVICE_UNAVAILABLE_UNKNOWN_USER:
        case xbox_live_error_code::XO_E_GREEN_SIGNED_CONTENT_NOT_AUTHORIZED:
        case xbox_live_error_code::XO_E_CONTENT_NOT_AUTHORIZED:
        case xbox_live_error_code::HR_ERROR_INTERNET_TIMEOUT:
            return (condition == xbox_live_error_condition::auth);

        case xbox_live_error_code::HR_INET_E_INVALID_URL:
        case xbox_live_error_code::HR_INET_E_NO_SESSION:
        case xbox_live_error_code::HR_INET_E_CANNOT_CONNECT:
        case xbox_live_error_code::HR_INET_E_RESOURCE_NOT_FOUND:
        case xbox_live_error_code::HR_INET_E_OBJECT_NOT_FOUND:
        case xbox_live_error_code::HR_INET_E_DATA_NOT_AVAILABLE:
        case xbox_live_error_code::HR_INET_E_DOWNLOAD_FAILURE:
        case xbox_live_error_code::HR_INET_E_AUTHENTICATION_REQUIRED:
        case xbox_live_error_code::HR_INET_E_NO_VALID_MEDIA:
        case xbox_live_error_code::HR_INET_E_CONNECTION_TIMEOUT:
        case xbox_live_error_code::HR_INET_E_INVALID_REQUEST:
        case xbox_live_error_code::HR_INET_E_UNKNOWN_PROTOCOL:
        case xbox_live_error_code::HR_INET_E_SECURITY_PROBLEM:
        case xbox_live_error_code::HR_INET_E_CANNOT_LOAD_DATA:
        case xbox_live_error_code::HR_INET_E_CANNOT_INSTANTIATE_OBJECT:
        case xbox_live_error_code::HR_INET_E_INVALID_CERTIFICATE:
        case xbox_live_error_code::HR_INET_E_REDIRECT_FAILED:
        case xbox_live_error_code::HR_INET_E_REDIRECT_TO_DIR:
        case xbox_live_error_code::HR_ERROR_NETWORK_UNREACHABLE:
            return (condition == xbox_live_error_condition::network);

        case xbox_live_error_code::out_of_range:
            return (condition == xbox_live_error_condition::generic_out_of_range ||
                condition == xbox_live_error_condition::generic_error);

        case xbox_live_error_code::bad_alloc:
        case xbox_live_error_code::bad_cast:
        case xbox_live_error_code::invalid_argument:
        case xbox_live_error_code::json_error:
        case xbox_live_error_code::length_error:
        case xbox_live_error_code::range_error:
        case xbox_live_error_code::logic_error:
        case xbox_live_error_code::runtime_error:
        case xbox_live_error_code::uri_error:
        case xbox_live_error_code::rta_generic_error:
        case xbox_live_error_code::rta_access_denied:
        case xbox_live_error_code::rta_subscription_limit_reached:
        case xbox_live_error_code::generic_error:
        default:
            return (condition == xbox_live_error_condition::generic_error);
        
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END