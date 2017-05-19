// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

namespace xbox
{
    namespace services
    {
        class xbox_live_context;
    }
}

namespace ATG
{
    // Key-Value pair of HTTP call header and value to be inserted into the call
    // or the header and value returned in the responce body
    class HttpHeader
    {
    public:
        HttpHeader(const wchar_t *header, const wchar_t *value) : m_header(header), m_value(value) {}

        const std::wstring &Header() const { return m_header; }
        const std::wstring &Value() const { return m_value; }
    private:
        std::wstring m_header;
        std::wstring m_value;
    };

    // Holds either the response from the web service call or the error from attempting to make the call.
    // The process method calls the callback that was set with the associated call for either handling the
    // error of processing the response body.
    class HttpResponse
    {
    public:
        HttpResponse() : m_errorCode(S_OK), m_errorMessage(L"") {}

        // This is the first method that should be called when processing the call to determine if errors 
        // should be handled or the response can be parsed.
        bool IsError() const { return m_errorCode != S_OK; }

        // Getters for error information
        long ErrorCode() const { return m_errorCode; }
        const std::wstring &ErrorMessage() const { return m_errorMessage; }

        //  Getters for retrieving information from a successful request
        unsigned long HttpResponseCode() const { return m_httpResponseCode; }
        const std::vector<HttpHeader> &ResponseHeaders() const { return m_responseHeaders; }
        std::shared_ptr<unsigned char> ResponseBody() const { return m_responseBody; }
        size_t ResponseBodySize() const { return m_responseBodySize; }

        void SetError(long errorCode, const std::wstring &errorMessage) { m_errorCode = errorCode; m_errorMessage = errorMessage; }

        void SetResponseCode(unsigned long response) { m_httpResponseCode = response; }
        void ParseHeaders(const std::wstring &headers);
        void SetResponseBody(unsigned char *body, size_t bodySize) { m_responseBody = std::shared_ptr<unsigned char>(body, std::default_delete<unsigned char[]>()); m_responseBodySize = bodySize; }
        void SetCallback(std::function<void(HttpResponse *)> callback) { m_callback = callback; }

        void Process() { m_callback(this); }
    private:
        long          m_errorCode;
        std::wstring  m_errorMessage;

        unsigned long                       m_httpResponseCode;
        std::vector<HttpHeader>             m_responseHeaders;
        size_t                              m_responseBodySize;
        std::shared_ptr<unsigned char>      m_responseBody;
        std::function<void(HttpResponse *)> m_callback;
    };

    // Singleton class for managing HTTP calls made to web services.  All responses and errors are buffered
    // and returned via the DoWork method to allow processing of the responses to done when and where the
    // title can.
    class HttpCallManager
    {
    public:
        HttpCallManager();
        HttpCallManager(HttpCallManager&& moveFrom) = default;
        HttpCallManager& operator=(HttpCallManager&& moveFrom) = default;

        HttpCallManager(HttpCallManager const&) = delete;
        HttpCallManager& operator=(HttpCallManager const&) = delete;

        ~HttpCallManager();

        std::vector<HttpResponse> DoWork();

        HRESULT MakeHttpCall(const wchar_t *verb, 
                             const wchar_t *uri,
                             const std::vector<HttpHeader> &headers,
                             std::function<void(HttpResponse *)> callback);

        HRESULT MakeHttpCall(const wchar_t *verb, 
                             const wchar_t *uri,
                             const std::vector<HttpHeader> &headers,
                             std::vector<unsigned char> &bodyContent,
                             std::function<void(HttpResponse *)> callback);

        // These calls may send the call from a separate thread due to a call to GetTokenAndSignatureAsync
        // to add the XSTS token to the call.
        HRESULT MakeHttpCallWithAuth(std::shared_ptr<xbox::services::xbox_live_context> userContext,
                                     const wchar_t *verb,
                                     const wchar_t *uri,
                                     const std::vector<HttpHeader> &headers,
                                     std::function<void(HttpResponse *)> callback);

        HRESULT MakeHttpCallWithAuth(std::shared_ptr<xbox::services::xbox_live_context> userContext,
                                     const wchar_t *verb,
                                     const wchar_t *uri,
                                     const std::vector<HttpHeader> &headers,
                                     std::vector<unsigned char> &bodyContent,
                                     std::function<void(HttpResponse *)> callback);

        void SetTimeout(unsigned long timeoutMS);

    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pImpl;

        friend class HttpCallback;
    };
}