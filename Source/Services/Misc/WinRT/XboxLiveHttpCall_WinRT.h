// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "http_call_impl.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

public enum class HttpCallResponseBodyType
{
    StringBody = xbox::services::http_call_response_body_type::string_body,
    VectorBody = xbox::services::http_call_response_body_type::vector_body,
    JsonBody = xbox::services::http_call_response_body_type::json_body
};

public ref class XboxLiveHttpCallResponse sealed
{
public:
    /// <summary>
    /// Gets the body type of the response.
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(BodyType, body_type, HttpCallResponseBodyType);

    /// <summary>
    /// Gets the response body of the response as a string.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(ResponseBodyString, response_body_string);

    /// <summary>
    /// Gets the response body of the response as a JSON value.
    /// </summary>
    property Platform::String^ ResponseBodyJson { Platform::String^ get(); }

    /// <summary>
    /// Gets the response body of the response as a byte vector.
    /// </summary>
    property Platform::Array<byte>^ ResponseBodyVector { Platform::Array<byte>^ get(); }

    /// <summary>
    /// Gets the http headers of the response.
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ Headers
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ get();
    }

    /// <summary>
    /// Gets the http status of the response.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(HttpStatus, http_status, int);

    /// <summary>
    /// Gets the error code of the response.
    /// </summary>
    property int ErrorCode { int get(); }

    /// <summary>
    /// Gets the error message of the response.
    /// </summary>
    property Platform::String^ ErrorMessage { Platform::String^ get(); }

    /// <summary>
    /// Gets the eTag of the response.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(ETag, e_tag);

    /// <summary>
    /// Gets the response date of the response.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(ResponseDate, response_date);

    /// <summary>
    /// Gets the "retry after" value found in the response.
    /// </summary>
    property int64_t RetryAfterInSeconds { int64_t get(); }

internal:
    XboxLiveHttpCallResponse(
        _In_ std::shared_ptr<xbox::services::http_call_response> cppObj
        );

private:
    Platform::Collections::Map<Platform::String^, Platform::String^>^ m_headersMap;
    Platform::Array<byte>^ m_vec;
    std::shared_ptr<xbox::services::http_call_response> m_cppObj;
};


/// <summary>
/// Defines methods used to retrieve objects that in turn provide access to different Xbox Live service components.
/// </summary> 
public ref class XboxLiveHttpCall sealed
{
public:
    static XboxLiveHttpCall^ CreateXboxLiveHttpCall(
        _In_ Microsoft::Xbox::Services::XboxLiveContextSettings^ settings,
        _In_ Platform::String^ httpMethod,
        _In_ Platform::String^ serverName,
        _In_ Platform::String^ pathQueryFragment
        );

    /// <summary>
    /// Attach the Xbox Live token, sign the request, send the request to the service, and return the response.
    /// </summary>
    Windows::Foundation::IAsyncOperation<XboxLiveHttpCallResponse^>^ GetResponseWithAuth(
#if TV_API
        _In_ Windows::Xbox::System::User^ user,
#else
        _In_ Microsoft::Xbox::Services::System::XboxLiveUser^ user,
#endif
        _In_ HttpCallResponseBodyType httpCallResponseBodyType
        );

    /// <summary>
    /// Send the request to the service, and return the response without an Xbox Live token
    /// </summary>
    Windows::Foundation::IAsyncOperation<XboxLiveHttpCallResponse^>^ GetResponseWithoutAuth(
        _In_ HttpCallResponseBodyType httpCallResponseBodyType
        );

    /// <summary>
    /// Sets the request body using a string.
    /// </summary>
    void SetRequestBody(_In_ Platform::String^ value);

    /// <summary>
    /// Sets the request body using a byte array value.
    /// </summary>
    void SetRequestBodyArray(_In_ const Platform::Array<byte>^ requestBodyArray);

    /// <summary>
    /// Sets a custom header.
    /// </summary>
    void SetCustomHeader(_In_ Platform::String^ headerName, _In_opt_ Platform::String^ headerValue);

    /// <summary>
    /// Sets if retry is allowed during this call.
    /// </summary>
    DEFINE_PTR_PROP_GETSET_OBJ(RetryAllowed, retry_allowed, bool);

    /// <summary>
    /// Get if retry is allowed during this call.
    /// </summary>
    DEFINE_PTR_PROP_GETSET_STR_OBJ(ContentTypeHeaderValue, content_type_header_value);

    /// <summary>
    /// Sets the content type header value for this call.
    /// </summary>
    DEFINE_PTR_PROP_GETSET_STR_OBJ(ContractVersionHeaderValue, xbox_contract_version_header_value);

    /// <summary>
    /// Gets the server name for this call.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(ServerName, server_name);

    /// <summary>
    /// Gets the path for this call.
    /// </summary>
    property Platform::String^ PathQueryFragment { Platform::String^ get(); }

    /// <summary>
    /// Gets the http method for this call.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(HttpMethod, http_method);

internal:
    XboxLiveHttpCall(
        _In_ std::shared_ptr<xbox::services::http_call> cppObj
        );

private:
    std::shared_ptr<xbox::services::http_call> m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_END
