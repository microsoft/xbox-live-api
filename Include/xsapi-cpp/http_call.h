// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once
#include "xsapi-c/http_call_c.h"

#if HC_PLATFORM == HC_PLATFORM_ANDROID || HC_PLATFORM_IS_APPLE
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif

#if HC_PLATFORM == HC_PLATFORM_XDK
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN
ref class XboxLiveUser;
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
class xbox_live_user;
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class xbox_live_context_settings;

/// <summary>
/// Enumerates the type of structured data contained in the http response body.
/// </summary>
enum class http_call_response_body_type
{
    /// <summary>
    /// The response body consists of a string.
    /// </summary>
    string_body,

    /// <summary>
    /// The response body consists of a vector of bytes.
    /// </summary>
    vector_body,

    /// <summary>
    /// The response body consists of a JavaScript Object Notation (JSON) object.
    /// </summary>
    json_body
};

// Forward declare
enum class xbox_live_api;


/// <summary>
/// Represents an http response from the Xbox Live service.
/// </summary>
class http_call_response
{
public:
#ifndef DEFAULT_MOVE_ENABLED
    inline http_call_response(http_call_response&& other);
    inline http_call_response& operator=(http_call_response&& other);
#endif
    inline http_call_response(
        _In_ XblHttpCallHandle call,
        _In_ XblHttpCallResponseBodyType type
    );

    /// <summary>
    /// Gets the body type of the response.
    /// </summary>
    inline http_call_response_body_type body_type() const;

    /// <summary>
    /// Gets the response body of the response as a string.
    /// </summary>
    inline string_t response_body_string();

    /// <summary>
    /// Gets the response body of the response as a JSON value.
    /// </summary>
    inline web::json::value response_body_json();

    /// <summary>
    /// Gets the response body of the response as a byte vector.
    /// </summary>
    inline std::vector<unsigned char> response_body_vector();

    /// <summary>
    /// Gets the http headers of the response.
    /// </summary>
    inline web::http::http_headers response_headers();

    /// <summary>
    /// Gets the http status of the response.
    /// </summary>
    inline uint32_t http_status();

    /// <summary>
    /// Gets the error code of the response.
    /// </summary>
    inline std::error_code err_code();

    /// <summary>
    /// Gets the error message of the response.
    /// </summary>
    inline std::string err_message();

    /// <summary>
    /// Gets the eTag of the response.
    /// </summary>
    inline string_t e_tag() const;

    /// <summary>
    /// Gets the response date of the response.
    /// </summary>
    inline string_t response_date() const;

    /// <summary>
    /// Gets the "retry after" value found in the response.
    /// </summary>
    inline std::chrono::seconds retry_after() const;

	inline http_call_response(const http_call_response&);
	inline http_call_response& operator=(http_call_response other);
    inline ~http_call_response();

private:
    XblHttpCallHandle m_handle;
    http_call_response_body_type m_bodyType;
    string_t m_responseBodyString;
    web::json::value m_responseBodyJson;
    std::vector<unsigned char> m_responseBodyVector;
    web::http::http_headers m_responseHeaders;
    uint32_t m_httpStatus;
    std::error_code m_errorCode;
    std::string m_errMessage;

	inline http_call_response() = delete;
};


class http_call
{
public:
    inline http_call(
        _In_ XblHttpCallHandle callHandle,
        _In_ string_t httpMethod,
        _In_ string_t serverName,
        _In_ web::uri pathQueryFragment
    );

#if !XSAPI_NO_PPL
    /// <summary>
    /// Attach the Xbox Live token, sign the request, send the request to the service, and return the response.
    /// </summary>
    inline pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ http_call_response_body_type httpCallResponseBodyType = http_call_response_body_type::json_body
    );

    inline pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ XalUserHandle user,
        _In_ http_call_response_body_type httpCallResponseBodyType = http_call_response_body_type::json_body,
        _In_ bool allUsersAuthRequired = false
    );

    /// <summary>
    /// Send the request without authentication and get the response.
    /// </summary>
    inline pplx::task< std::shared_ptr<http_call_response> > get_response(
        _In_ http_call_response_body_type httpCallResponseBodyType
    );
#endif // !XSAPI_NO_PPL

    /// <summary>
    /// Sets the request body using a string.
    /// </summary>
    inline void set_request_body(_In_ const string_t& value);

    /// <summary>
    /// Sets the request body using a JSON value.
    /// </summary>
    inline void set_request_body(_In_ const web::json::value& value);

    /// <summary>
    /// Sets the request body using a byte array value.
    /// </summary>
    inline void set_request_body(_In_ const std::vector<uint8_t>& value);

    /// <summary>
    /// Sets a custom header.
    /// </summary>
    inline void set_custom_header(
        _In_ const string_t& headerName,
        _In_ const string_t& headerValue);

    /// <summary>
    /// Sets if this is a long http call, and should use the long_http_timeout setting.
    /// </summary>
    inline void set_long_http_call(_In_ bool value);

    /// <summary>
    /// Gets if this is a long http call, and should use the long_http_timeout setting.
    /// </summary>
    inline bool long_http_call() const;

    /// <summary>
    /// Sets if retry is allowed during this call.
    /// </summary>
    inline void set_retry_allowed(_In_ bool value);

    /// <summary>
    /// Get if retry is allowed during this call.
    /// </summary>
    inline bool retry_allowed() const;

    /// <summary>
    /// Sets the content type header value for this call.
    /// </summary>
    inline void set_content_type_header_value(_In_ const string_t& value);

    /// <summary>
    /// Gets the content type header value for this call.
    /// </summary>
    inline string_t content_type_header_value() const;

    /// <summary>
    /// Sets the Xbox Live contract version header value for this call.
    /// </summary>
    inline void set_xbox_contract_version_header_value(_In_ const string_t& value);

    /// <summary>
    /// Gets the Xbox Live contract version header value for this call.
    /// </summary>
    inline string_t xbox_contract_version_header_value() const;

    /// <summary>
    /// Gets the server name for this call.
    /// </summary>
    inline string_t server_name() const;

    /// <summary>
    /// Gets the path for this call.
    /// </summary>
    inline const web::uri& path_query_fragment() const;

    /// <summary>
    /// Gets the http method for this call.
    /// </summary>
    inline string_t http_method() const;

    /// <summary>
    /// Sets a flag indicating if default headers should be added or not.
    /// </summary>
    inline void set_add_default_headers(_In_ bool value);

    /// <summary>
    /// Internal function
    /// </summary>
    inline bool add_default_headers() const;

    inline ~http_call();

private:
    XblHttpCallHandle m_callHandle;
    string_t m_httpMethod;
    string_t m_serverName;
    web::uri m_pathQueryFragment;
    bool m_longHttpCall;
    bool m_retryAllowed;
};

inline std::shared_ptr<http_call> create_xbox_live_http_call(
    _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
    _In_ const string_t& httpMethod,
    _In_ const string_t& serverName,
    _In_ const web::uri& pathQueryFragment
);

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#include "impl/http_call.hpp"
