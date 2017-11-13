// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once

#if TV_API
    NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN
        ref class XboxLiveUser;
    NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
#elif UNIT_TEST_SERVICES || !XSAPI_CPP
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
    /// <summary>
    /// Internal function
    /// </summary>
    http_call_response(
        _In_ const string_t& xboxUserId,
        _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
        _In_ const string_t& baseUrl,
        _In_ const web::http::http_request& request,
        _In_ const http_call_request_message& requestBody,
        _In_ xbox_live_api xboxLiveApi,
        _In_ const web::http::http_response& response
        );

#ifndef DEFAULT_MOVE_ENABLED
    http_call_response(http_call_response&& other);
    http_call_response& operator=(http_call_response&& other);
#endif

    /// <summary>
    /// Gets the body type of the response.
    /// </summary>
    _XSAPIIMP http_call_response_body_type body_type() const { return m_httpCallResponseBodyType; }

    /// <summary>
    /// Gets the response body of the response as a string.
    /// </summary>
    _XSAPIIMP const string_t& response_body_string() const { return m_responseBodyString; }

    /// <summary>
    /// Gets the response body of the response as a JSON value.
    /// </summary>
    _XSAPIIMP const web::json::value& response_body_json() const { return m_responseBodyJson; }

    /// <summary>
    /// Gets the response body of the response as a byte vector.
    /// </summary>
    _XSAPIIMP const std::vector<unsigned char>& response_body_vector() const { return m_responseBodyVector; }

    /// <summary>
    /// Gets the http headers of the response.
    /// </summary>
    _XSAPIIMP const web::http::http_headers& response_headers() const { return m_responseHeaders; }

    /// <summary>
    /// Gets the http status of the response.
    /// </summary>
    _XSAPIIMP uint32_t http_status() const { return m_httpStatus; }

    /// <summary>
    /// Gets the error code of the response.
    /// </summary>
    _XSAPIIMP const std::error_code& err_code() const { return m_errorCode; }

    /// <summary>
    /// Gets the error message of the response.
    /// </summary>
    _XSAPIIMP const std::string& err_message() const { return m_errorMessage; }

    /// <summary>
    /// Gets the eTag of the response.
    /// </summary>
    _XSAPIIMP const string_t& e_tag() const { return m_eTag; }

    /// <summary>
    /// Gets the response date of the response.
    /// </summary>
    _XSAPIIMP const string_t& response_date() const { return m_responseDate; }

    /// <summary>
    /// Gets the "retry after" value found in the response.
    /// </summary>
    _XSAPIIMP const std::chrono::seconds& retry_after() const { return m_retryAfter; }

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_error(_In_ const std::error_code& errCode, _In_ const std::string& errMessage)
    {
        m_errorCode = errCode;
        m_errorMessage = errMessage;
    }

    /// <summary>
    /// Internal function
    /// </summary>
    void _Add_response_header(_In_ const string_t& headerName, _In_ const string_t& headerValue)
    {
        m_responseHeaders.add(headerName, headerValue);
    }

    /// <summary>
    /// Internal function
    /// </summary>
    void _Remove_response_header(_In_ const string_t& headerName)
    {
        m_responseHeaders.remove(headerName);
    }

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_response_body(_In_ const string_t& responseBodyString);

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_response_body(_In_ const std::vector<unsigned char>& responseBodyVector);

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_response_body(_In_ const web::json::value& responseBodyJson);

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_timing(
        _In_ const chrono_clock_t::time_point& requestTime,
        _In_ const chrono_clock_t::time_point& responseTime
        );

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_error_info(
        _In_ const std::error_code& errCode,
        _In_ const std::string& errMessage
        );

    /// <summary>
    /// Internal function
    /// </summary>
    void _Route_service_call() const;

    /// <summary>
    /// Internal function
    /// </summary>
    const chrono_clock_t::time_point& _Local_response_time() const;

    /// <summary>
    /// Internal function
    /// </summary>
    std::shared_ptr<xbox_live_context_settings> _Context_settings() const;

    /// <summary>
    /// Internal function
    /// </summary>
    const web::http::http_request& _Request() const;

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_full_url(_In_ const string_t& url);

private:
    void record_service_result() const;
    std::string get_throttling_error_message() const;

    http_call_response_body_type m_httpCallResponseBodyType;
    std::vector<unsigned char> m_responseBodyVector;
    string_t m_responseBodyString;
    web::json::value m_responseBodyJson;

    uint32_t m_httpStatus;
    std::error_code m_errorCode;
    std::string m_errorMessage;

    web::http::http_headers m_responseHeaders;
    string_t m_eTag;
    string_t m_responseDate;
    std::chrono::seconds m_retryAfter;
    chrono_clock_t::time_point m_requestTime;
    chrono_clock_t::time_point m_responseTime;

    string_t m_xboxUserId;
    std::shared_ptr<xbox_live_context_settings> m_xboxLiveContextSettings;
    string_t m_fullUrl;
    web::http::http_request m_request;
    http_call_request_message m_requestBody;
    xbox_live_api m_xboxLiveApi;

    string_t response_body_to_string() const;

    static std::chrono::seconds extract_retry_after_from_header(
        _In_ const web::http::http_headers& responseHeaders
        );
};


class http_call
{
public:
    /// <summary>
    /// Attach the Xbox Live token, sign the request, send the request to the service, and return the response.
    /// </summary>
    virtual pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ http_call_response_body_type httpCallResponseBodyType = http_call_response_body_type::json_body
        ) = 0;

#if XSAPI_XDK_AUTH
    virtual pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ Windows::Xbox::System::User^ user,
        _In_ http_call_response_body_type httpCallResponseBodyType = http_call_response_body_type::json_body,
        _In_ bool allUsersAuthRequired = false
        ) = 0;
#endif 

#if XSAPI_NONXDK_CPP_AUTH
    virtual pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ std::shared_ptr<system::xbox_live_user> user,
        _In_ http_call_response_body_type httpCallResponseBodyType = http_call_response_body_type::json_body,
        _In_ bool allUsersAuthRequired = false
        ) = 0;
#endif 

#if XSAPI_NONXDK_WINRT_AUTH
    virtual pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ Microsoft::Xbox::Services::System::XboxLiveUser^ user,
        _In_ http_call_response_body_type httpCallResponseBodyType = http_call_response_body_type::json_body,
        _In_ bool allUsersAuthRequired = false
        ) = 0;
#endif
    
    /// <summary>
    /// Send the request without authentication and get the response.
    /// </summary>
    virtual pplx::task< std::shared_ptr<http_call_response> > get_response(
        _In_ http_call_response_body_type httpCallResponseBodyType
        ) = 0;

    /// <summary>
    /// Sets the request body using a string.
    /// </summary>
    virtual void set_request_body(_In_ const string_t& value) = 0;

    /// <summary>
    /// Sets the request body using a JSON value.
    /// </summary>
    virtual void set_request_body(_In_ const web::json::value& value) = 0;

    /// <summary>
    /// Sets the request body using a byte array value.
    /// </summary>
    virtual void set_request_body(_In_ const std::vector<uint8_t>& value) = 0;

    /// <summary>
    /// Sets a custom header.
    /// </summary>
    virtual void set_custom_header(
        _In_ const string_t& headerName,
        _In_ const string_t& headerValue) = 0;

    /// <summary>
    /// Sets if this is a long http call, and should use the long_http_timeout setting.
    /// </summary>
    virtual void set_long_http_call(_In_ bool value) = 0;

    /// <summary>
    /// Gets if this is a long http call, and should use the long_http_timeout setting.
    /// </summary>
    virtual bool long_http_call() const = 0;

    /// <summary>
    /// Sets if retry is allowed during this call.
    /// </summary>
    virtual void set_retry_allowed(_In_ bool value) = 0;

    /// <summary>
    /// Get if retry is allowed during this call.
    /// </summary>
    virtual bool retry_allowed() const = 0;

    /// <summary>
    /// Sets the content type header value for this call.
    /// </summary>
    virtual void set_content_type_header_value(_In_ const string_t& value) = 0;

    /// <summary>
    /// Gets the content type header value for this call.
    /// </summary>
    virtual const string_t& content_type_header_value() const = 0;

    /// <summary>
    /// Sets the Xbox Live contract version header value for this call.
    /// </summary>
    virtual void set_xbox_contract_version_header_value(_In_ const string_t& value) = 0;

    /// <summary>
    /// Gets the Xbox Live contract version header value for this call.
    /// </summary>
    virtual const string_t& xbox_contract_version_header_value() const = 0;

    /// <summary>
    /// Gets the server name for this call.
    /// </summary>
    virtual const string_t& server_name() const = 0;

    /// <summary>
    /// Gets the path for this call.
    /// </summary>
    virtual const web::uri& path_query_fragment() const = 0;

    /// <summary>
    /// Gets the http method for this call.
    /// </summary>
    virtual const string_t& http_method() const = 0;

    /// <summary>
    /// Sets a flag indicating if default headers should be added or not.
    /// </summary>
    virtual void set_add_default_headers(_In_ bool value) = 0;
    
    /// <summary>
    /// Internal function
    /// </summary>
    virtual bool add_default_headers() const = 0;

    virtual pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ const std::shared_ptr<xbox::services::user_context>& userContext,
        _In_ http_call_response_body_type httpCallResponseBodyType = http_call_response_body_type::json_body,
        _In_ bool allUsersAuthRequired = false
        ) = 0;

    /// <summary>
    /// Internal function
    /// </summary>
    virtual pplx::task<std::shared_ptr<http_call_response>> _Internal_get_response_with_auth(
        _In_ const std::shared_ptr<xbox::services::user_context>& userContext,
        _In_ http_call_response_body_type httpCallResponseBodyType = http_call_response_body_type::json_body,
        _In_ bool allUsersAuthRequired = false
        ) = 0;

    virtual ~http_call(){}
};

_XSAPIIMP std::shared_ptr<http_call> create_xbox_live_http_call(
    _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
    _In_ const string_t& httpMethod,
    _In_ const string_t& serverName,
    _In_ const web::uri& pathQueryFragment
    );

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END