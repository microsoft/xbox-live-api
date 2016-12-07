//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once

namespace Microsoft { namespace Xbox { namespace Samples { namespace NetworkMesh {

enum WebErrorStatus
{
    WebErrorStatus_Unknown = 0,
    WebErrorStatus_CertificateCommonNameIsIncorrect = 1,
    WebErrorStatus_CertificateExpired = 2,
    WebErrorStatus_CertificateContainsErrors = 3,
    WebErrorStatus_CertificateRevoked = 4,
    WebErrorStatus_CertificateIsInvalid = 5,
    WebErrorStatus_ServerUnreachable = 6,
    WebErrorStatus_Timeout = 7,
    WebErrorStatus_ErrorHttpInvalidServerResponse = 8,
    WebErrorStatus_ConnectionAborted = 9,
    WebErrorStatus_ConnectionReset = 10,
    WebErrorStatus_Disconnected = 11,
    WebErrorStatus_HttpToHttpsOnRedirection = 12,
    WebErrorStatus_HttpsToHttpOnRedirection = 13,
    WebErrorStatus_CannotConnect = 14,
    WebErrorStatus_HostNameNotResolved = 15,
    WebErrorStatus_OperationCanceled = 16,
    WebErrorStatus_RedirectFailed = 17,
    WebErrorStatus_UnexpectedStatusCode = 18,
    WebErrorStatus_UnexpectedRedirection = 19,
    WebErrorStatus_UnexpectedClientError = 20,
    WebErrorStatus_UnexpectedServerError = 21,
    WebErrorStatus_MultipleChoices = 300,
    WebErrorStatus_MovedPermanently = 301,
    WebErrorStatus_Found = 302,
    WebErrorStatus_SeeOther = 303,
    WebErrorStatus_NotModified = 304,
    WebErrorStatus_UseProxy = 305,
    WebErrorStatus_TemporaryRedirect = 307,
    WebErrorStatus_BadRequest = 400,
    WebErrorStatus_Unauthorized = 401,
    WebErrorStatus_PaymentRequired = 402,
    WebErrorStatus_Forbidden = 403,
    WebErrorStatus_NotFound = 404,
    WebErrorStatus_MethodNotAllowed = 405,
    WebErrorStatus_NotAcceptable = 406,
    WebErrorStatus_ProxyAuthenticationRequired = 407,
    WebErrorStatus_RequestTimeout = 408,
    WebErrorStatus_Conflict = 409,
    WebErrorStatus_Gone = 410,
    WebErrorStatus_LengthRequired = 411,
    WebErrorStatus_PreconditionFailed = 412,
    WebErrorStatus_RequestEntityTooLarge = 413,
    WebErrorStatus_RequestUriTooLong = 414,
    WebErrorStatus_UnsupportedMediaType = 415,
    WebErrorStatus_RequestedRangeNotSatisfiable = 416,
    WebErrorStatus_ExpectationFailed = 417,
    WebErrorStatus_InternalServerError = 500,
    WebErrorStatus_NotImplemented = 501,
    WebErrorStatus_BadGateway = 502,
    WebErrorStatus_ServiceUnavailable = 503,
    WebErrorStatus_GatewayTimeout = 504,
    WebErrorStatus_HttpVersionNotSupported = 505
};

private class Utils
{

public:
    static inline bool IsNullOrEmptyString(__in_opt LPCWSTR pcwsz)
    {
        return (pcwsz == NULL) || (pcwsz[0] == L'\0');
    }

    // Searches for a pattern in the source string and replace all occurrences of it with another string.
    // Pattern must be non-empty
    // Replacement can be empty, in which case all occurrences of Pattern are deleted
    static std::wstring&
    Replace(
        __inout std::wstring& strSource,
        __in PCWSTR pwszPattern,
        __in_opt PCWSTR pwszReplacement,
        __out_opt size_t* pnOccurrencesReplaced = nullptr
        );

    // Searches for a pattern in the source string and replace all occurrences of it with another string.
    // Pattern must be non-empty
    // Replacement can be empty, in which case all occurrences of Pattern are deleted
    // If Replacement is not empty, it will be Uri encoded
    static std::wstring&
    UriReplace(
        __inout std::wstring& strSource,
        __in PCWSTR pwszPattern,
        __in_opt PCWSTR pwszReplacement,
        __out_opt size_t* pnOccurrencesReplaced = nullptr
        );

    static std::wstring
    UriEncode(
        __in std::wstring wstrUriToEncode
        );

    static std::vector<std::wstring>
    StringSplit(
        __in const std::wstring& string,
        __in WCHAR seperator
        );


    static Microsoft::WRL::ComPtr<IStream>
    StringToStream(
        __in Platform::String^ source,
        __out uint32* streamSize
        );

    static Microsoft::WRL::ComPtr<IStream>
    ArrayToStream(
        __in Platform::Array<byte>^ buffer,
        __out uint32* streamSize
        );

    static Platform::Array<byte>^ 
    BufferToArray(
        __in Windows::Storage::Streams::IBuffer^ buffer
        );

    static Platform::Array<byte>^ 
    StringToArray(
        __in Platform::String^ source
        );

    static Platform::String^
    StreamToString(
        __in ISequentialStream* source
        );

    // Converts HTTP status Code (e.g., 200, 401, 304) to HRESULT
    static HRESULT
    ConvertHttpStatusCodeToHR(
        __in uint32 HttpStatusCode
        );

    static Windows::Foundation::PropertyType
    ConvertStringToPropertyType(
        __in Platform::String^ typeName
        );

    static HRESULT
    ConvertExceptionToHRESULT();

    static Platform::String^
    DateTimeToString(
        __in Windows::Foundation::DateTime dateTime
        );

    static Platform::String^
    RemoveBracesFromGuidString(
        __in Platform::String^ guidString
        );

    static Microsoft::WRL::ComPtr<IStream>
    BufferToStream(
        __in Windows::Storage::Streams::IBuffer^ source,
        __out uint32* streamSize
        );

    static Windows::Foundation::TimeSpan
    ConvertSecondsToTimeSpan(
        __in uint32 seconds
        );

    static Windows::Foundation::TimeSpan
    ConvertMillisecondsToTimeSpan(
        __in uint64 milliseconds
        );

    static uint32
    ConvertTimeSpanToSeconds(
        __in Windows::Foundation::TimeSpan timespan
        );

    static int64
    ConvertTimeSpanToMilliseconds(
        __in Windows::Foundation::TimeSpan timespan
        );

    static uint32
    ConvertTimeSpanToDays(
        __in Windows::Foundation::TimeSpan timespan
        );

    static Windows::Foundation::TimeSpan GetCurrentTime();

    static Windows::Data::Json::IJsonValue^ 
    GetNullJsonValue();

    static void
    BufferCopy(
        __in Windows::Storage::Streams::IBuffer^ source,
        __in Windows::Storage::Streams::IBuffer^ destination,
        __in bool append = false
        );

    static Platform::String^ 
    ConvertHResultToString( HRESULT hr );

    static Platform::String^ 
    GetLocale();

    static void 
    SetLocaleMock(Platform::String^ locale);

    static Platform::String^ 
    ConvertHResultToErrorName( HRESULT hr );

    static std::wstring
    UriEncodeSubPathComponents(
        __in std::wstring wstrSubPath
        );

    static void
    LogExceptionDebugInfo(
        __in HRESULT hr,
        __in_opt PCWSTR pwszFunction,
        __in_opt PCWSTR pwszFile,
        __in uint32 uLine
        );

private:
    static Platform::String^ s_locale;
};

template<typename T>
static std::basic_string<T>&
ReplaceSubstring(
    __inout std::basic_string<T>& strSource,
    __in_ecount_z(nPatternLength+1) const T* pszPattern,
    __in const size_t nPatternLength,
    __in_z const T* pszReplacement,
    __out_opt size_t* pnOccurrencesReplaced
)
{
    THROW_INVALIDARGUMENT_IF_NULL( pszPattern );
    THROW_INVALIDARGUMENT_IF_NULL( pszReplacement );

    if ( pnOccurrencesReplaced != nullptr )
    {
        *pnOccurrencesReplaced = 0;
    }

    size_t nReplaced = 0;

    if ( nPatternLength > 0 )
    {
        // Search the string backward for the given pattern first
        size_t nPos = strSource.rfind( pszPattern );

        while ( nPos != std::basic_string<T>::npos )
        {
            strSource.replace( nPos, nPatternLength, pszReplacement );
            ++nReplaced;

            // Find the given pattern first

            if ( nPos == 0 )
            {
                // There is nothing left to look at, break
                break;
            }

            // Find the next match starting from the last replaced position
            nPos = strSource.rfind( pszPattern, nPos - 1 );
        }
    }

    if ( pnOccurrencesReplaced != nullptr )
    {
        *pnOccurrencesReplaced = nReplaced;
    }

    return strSource;
}

}}}}

