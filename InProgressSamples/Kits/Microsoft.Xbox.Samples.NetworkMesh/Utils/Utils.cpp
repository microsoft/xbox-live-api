// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Utils.h"
#include "ISO8601.h"
#include "Robuffer.h"
#include "Configuration.h"

// for conversion from seconds (int) to TimeSpan (perhaps use _XTIME_TICKS_PER_TIME_T instead)
#define TICKS_PER_SECOND 10000000i64

#define SECONDS_PER_DAY 86400

using namespace Microsoft::Xbox::Samples::NetworkMesh;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;

std::wstring&
Utils::Replace(
    __inout std::wstring& strSource,
    __in PCWSTR pwszPattern,
    __in_opt PCWSTR pwszReplacement,
    __out_opt size_t* pnOccurrencesReplaced
    )
{
    THROW_INVALIDARGUMENT_IF( Utils::IsNullOrEmptyString( pwszPattern ));
    const size_t nPatternLength = wcslen( pwszPattern );

    return ReplaceSubstring<WCHAR>(
        strSource,
        pwszPattern,
        nPatternLength,
        ( ( pwszReplacement != nullptr ) ? pwszReplacement : L"" ),
        pnOccurrencesReplaced
        );
}

std::vector<std::wstring>
Utils::StringSplit(
    __in const std::wstring& string,
    __in WCHAR seperator
)
{
    std::vector<std::wstring> vSubStrings;

    if ( !string.empty() )
    {
        size_t posStart = 0, posFound = 0;
        while ( posFound != std::wstring::npos && posStart < string.length() )
        {
            posFound = string.find( seperator, posStart);
            if ( posFound != std::wstring::npos )
            {
                if ( posFound != posStart )
                {
                    // this substring is not empty
                    vSubStrings.push_back( string.substr( posStart, posFound - posStart ) );
                }
                posStart = posFound + 1;
            }
            else
            {
                vSubStrings.push_back( string.substr( posStart ) );
            }
        }
    }

    return vSubStrings;
}

Platform::String^
Utils::DateTimeToString(
    __in Windows::Foundation::DateTime dateTime
    )
{
    // It's OK for us not be able to handle BC time.
    THROW_HR_IF( dateTime.UniversalTime < 0, E_BOUNDS);

    FILETIME fileTime;
    fileTime.dwLowDateTime = (dateTime.UniversalTime & (DWORD)-1);
    fileTime.dwHighDateTime = (dateTime.UniversalTime >> 32 & (DWORD)-1);

    WCHAR dateString[ISO8601_MAX_CCH];
    THROW_IF_HR_FAILED(
        FILETIMEToISO8601W( &fileTime, false, dateString, ARRAYSIZE(dateString), FALSE)
        );

    return ref new String(dateString);
}

String^
Utils::RemoveBracesFromGuidString(
    __in String^ guid
    )
{
    std::wstring strGuid = guid->ToString()->Data();

    if(strGuid.length() > 0 && strGuid[0] == L'{')
    {
        // Remove the {
        strGuid.erase(0, 1);
    }

    if(strGuid.length() > 0 && strGuid[strGuid.length() - 1] == L'}')
    {
        // Remove the }
        strGuid.erase(strGuid.end() - 1, strGuid.end());
    }

    return ref new String(strGuid.c_str());
}

Windows::Foundation::TimeSpan
Utils::ConvertMillisecondsToTimeSpan(
    __in uint64 milliseconds
    )
{
    Windows::Foundation::TimeSpan ts;
    ts.Duration = (TICKS_PER_SECOND/1000) * milliseconds;
    return ts;
}

Windows::Foundation::TimeSpan
Utils::ConvertSecondsToTimeSpan(
    __in uint32 seconds
    )
{
    Windows::Foundation::TimeSpan ts;
    ts.Duration = TICKS_PER_SECOND * seconds;
    return ts;
}

uint32
Utils::ConvertTimeSpanToSeconds(
    __in Windows::Foundation::TimeSpan timespan
    )
{
    int64 seconds = timespan.Duration / TICKS_PER_SECOND;
    THROW_INVALIDARGUMENT_IF( seconds < 0 || seconds > UINT32_MAX );

    return static_cast<uint32>(seconds);
}

int64
Utils::ConvertTimeSpanToMilliseconds(
    __in Windows::Foundation::TimeSpan timespan
    )
{
    int64 milliseconds = timespan.Duration / (TICKS_PER_SECOND/1000);
    return milliseconds;
}

uint32
Utils::ConvertTimeSpanToDays(
    __in Windows::Foundation::TimeSpan timespan
    )
{
    int64 days = (timespan.Duration / TICKS_PER_SECOND) / SECONDS_PER_DAY;
    THROW_INVALIDARGUMENT_IF( days < 0 || days > UINT32_MAX );

    return static_cast<uint32>(days);
}

typedef union tagTU
{
    FILETIME ft;
    ULARGE_INTEGER ui;
} TU;

Windows::Foundation::TimeSpan Utils::GetCurrentTime()
{
    SYSTEMTIME curTime = {0};
    GetSystemTime(&curTime);

    HRESULT hr;
    TU ftTime = {0};
    if (SystemTimeToFileTime(&curTime, &ftTime.ft))
    {
        Windows::Foundation::TimeSpan ts;
        ts.Duration = ftTime.ui.QuadPart;
        return ts;
    }
    else
    {
        hr = HRESULT_FROM_WIN32( GetLastError() );
        THROW_HR(hr);
    }
}

Platform::String^ Utils::ConvertHResultToString( HRESULT hr )
{
    WCHAR tmp[ 1024 ];
    _snwprintf_s( tmp, _countof( tmp ), _TRUNCATE, L"0x%0.8x", hr);
    return ref new Platform::String(tmp);
}

Platform::String^ Utils::ConvertHResultToErrorName( HRESULT hr )
{
    switch( hr )
    {
        // Generic errors
        case S_OK: return L"S_OK";
        case S_FALSE: return L"S_FALSE";
        case E_OUTOFMEMORY: return L"E_OUTOFMEMORY";
        case E_ACCESSDENIED: return L"E_ACCESSDENIED";
        case E_INVALIDARG: return L"E_INVALIDARG";
        case E_UNEXPECTED: return L"E_UNEXPECTED";
        case E_ABORT: return L"E_ABORT";
        case E_FAIL: return L"E_FAIL";
        case E_NOTIMPL: return L"E_NOTIMPL";
        case E_ILLEGAL_METHOD_CALL: return L"E_ILLEGAL_METHOD_CALL";
        case 0x8007274C: return L"WSATIMEOUT";

        // Authentication specific errors
        case 0x87DD0003: return L"AM_E_XASD_UNEXPECTED";
        case 0x87DD0004: return L"AM_E_XASU_UNEXPECTED";
        case 0x87DD0005: return L"AM_E_XAST_UNEXPECTED";
        case 0x87DD0006: return L"AM_E_XSTS_UNEXPECTED";
        case 0x87DD0007: return L"AM_E_XDEVICE_UNEXPECTED";
        case 0x87DD0008: return L"AM_E_DEVMODE_NOT_AUTHORIZED";
        case 0x87DD0009: return L"AM_E_NOT_AUTHORIZED";
        case 0x87DD000A: return L"AM_E_FORBIDDEN";
        case 0x87DD000B: return L"AM_E_UNKNOWN_TARGET";
        case 0x87DD000C: return L"AM_E_NSAL_READ_FAILED";
        case 0x87DD000D: return L"AM_E_TITLE_NOT_AUTHENTICATED";
        case 0x87DD000E: return L"AM_E_TITLE_NOT_AUTHORIZED";
        case 0x87DD000F: return L"AM_E_DEVICE_NOT_AUTHENTICATED";
        case 0x87DD0010: return L"AM_E_INVALID_USER_INDEX";
        case 0x8015DC00: return L"XO_E_DEVMODE_NOT_AUTHORIZED";
        case 0x8015DC01: return L"XO_E_SYSTEM_UPDATE_REQUIRED";
        case 0x8015DC02: return L"XO_E_CONTENT_UPDATE_REQUIRED";
        case 0x8015DC03: return L"XO_E_ENFORCEMENT_BAN";
        case 0x8015DC04: return L"XO_E_THIRD_PARTY_BAN";
        case 0x8015DC05: return L"XO_E_ACCOUNT_PARENTALLY_RESTRICTED";
        case 0x8015DC06: return L"XO_E_DEVICE_SUBSCRIPTION_NOT_ACTIVATED";
        case 0x8015DC08: return L"XO_E_ACCOUNT_BILLING_MAINTENANCE_REQUIRED";
        case 0x8015DC09: return L"XO_E_ACCOUNT_CREATION_REQUIRED";
        case 0x8015DC0A: return L"XO_E_ACCOUNT_TERMS_OF_USE_NOT_ACCEPTED";
        case 0x8015DC0B: return L"XO_E_ACCOUNT_COUNTRY_NOT_AUTHORIZED";
        case 0x8015DC0C: return L"XO_E_ACCOUNT_AGE_VERIFICATION_REQUIRED";
        case 0x8015DC0D: return L"XO_E_ACCOUNT_CURFEW";
        case 0x8015DC0E: return L"XO_E_ACCOUNT_ZEST_MAINTENANCE_REQUIRED";
        case 0x8015DC0F: return L"XO_E_ACCOUNT_CSV_TRANSITION_REQUIRED";
        case 0x8015DC10: return L"XO_E_ACCOUNT_MAINTENANCE_REQUIRED";
        case 0x8015DC11: return L"XO_E_ACCOUNT_TYPE_NOT_ALLOWED";
        case 0x8015DC12: return L"XO_E_CONTENT_ISOLATION (Verify SCID / Sandbox)";
        case 0x8015DC13: return L"XO_E_ACCOUNT_NAME_CHANGE_REQUIRED";
        case 0x8015DC14: return L"XO_E_DEVICE_CHALLENGE_REQUIRED";
        case 0x8015DC20: return L"XO_E_EXPIRED_DEVICE_TOKEN";
        case 0x8015DC21: return L"XO_E_EXPIRED_TITLE_TOKEN";
        case 0x8015DC22: return L"XO_E_EXPIRED_USER_TOKEN";
        case 0x8015DC23: return L"XO_E_INVALID_DEVICE_TOKEN";
        case 0x8015DC24: return L"XO_E_INVALID_TITLE_TOKEN";
        case 0x8015DC25: return L"XO_E_INVALID_USER_TOKEN";

        // HTTP specific errors
        case WEB_E_UNSUPPORTED_FORMAT: return L"WEB_E_UNSUPPORTED_FORMAT";
        case WEB_E_INVALID_XML: return L"WEB_E_INVALID_XML";
        case WEB_E_MISSING_REQUIRED_ELEMENT: return L"WEB_E_MISSING_REQUIRED_ELEMENT";
        case WEB_E_MISSING_REQUIRED_ATTRIBUTE: return L"WEB_E_MISSING_REQUIRED_ATTRIBUTE";
        case WEB_E_UNEXPECTED_CONTENT: return L"WEB_E_UNEXPECTED_CONTENT";
        case WEB_E_RESOURCE_TOO_LARGE: return L"WEB_E_RESOURCE_TOO_LARGE";
        case WEB_E_INVALID_JSON_STRING: return L"WEB_E_INVALID_JSON_STRING";
        case WEB_E_INVALID_JSON_NUMBER: return L"WEB_E_INVALID_JSON_NUMBER";
        case WEB_E_JSON_VALUE_NOT_FOUND: return L"WEB_E_JSON_VALUE_NOT_FOUND";
        case HTTP_E_STATUS_UNEXPECTED: return L"HTTP_E_STATUS_UNEXPECTED";
        case HTTP_E_STATUS_UNEXPECTED_REDIRECTION: return L"HTTP_E_STATUS_UNEXPECTED_REDIRECTION";
        case HTTP_E_STATUS_UNEXPECTED_CLIENT_ERROR: return L"HTTP_E_STATUS_UNEXPECTED_CLIENT_ERROR";
        case HTTP_E_STATUS_UNEXPECTED_SERVER_ERROR: return L"HTTP_E_STATUS_UNEXPECTED_SERVER_ERROR";
        case HTTP_E_STATUS_AMBIGUOUS: return L"HTTP_E_STATUS_AMBIGUOUS";
        case HTTP_E_STATUS_MOVED: return L"HTTP_E_STATUS_MOVED";
        case HTTP_E_STATUS_REDIRECT: return L"HTTP_E_STATUS_REDIRECT";
        case HTTP_E_STATUS_REDIRECT_METHOD: return L"HTTP_E_STATUS_REDIRECT_METHOD";
        case HTTP_E_STATUS_NOT_MODIFIED: return L"HTTP_E_STATUS_NOT_MODIFIED";
        case HTTP_E_STATUS_USE_PROXY: return L"HTTP_E_STATUS_USE_PROXY";
        case HTTP_E_STATUS_REDIRECT_KEEP_VERB: return L"HTTP_E_STATUS_REDIRECT_KEEP_VERB";
        case HTTP_E_STATUS_BAD_REQUEST: return L"HTTP_E_STATUS_BAD_REQUEST";
        case HTTP_E_STATUS_DENIED: return L"HTTP_E_STATUS_DENIED";
        case HTTP_E_STATUS_PAYMENT_REQ: return L"HTTP_E_STATUS_PAYMENT_REQ";
        case HTTP_E_STATUS_FORBIDDEN: return L"HTTP_E_STATUS_FORBIDDEN";
        case HTTP_E_STATUS_NOT_FOUND: return L"HTTP_E_STATUS_NOT_FOUND";
        case HTTP_E_STATUS_BAD_METHOD: return L"HTTP_E_STATUS_BAD_METHOD";
        case HTTP_E_STATUS_NONE_ACCEPTABLE: return L"HTTP_E_STATUS_NONE_ACCEPTABLE";
        case HTTP_E_STATUS_PROXY_AUTH_REQ: return L"HTTP_E_STATUS_PROXY_AUTH_REQ";
        case HTTP_E_STATUS_REQUEST_TIMEOUT: return L"HTTP_E_STATUS_REQUEST_TIMEOUT";
        case HTTP_E_STATUS_CONFLICT: return L"HTTP_E_STATUS_CONFLICT";
        case HTTP_E_STATUS_GONE: return L"HTTP_E_STATUS_GONE";
        case HTTP_E_STATUS_LENGTH_REQUIRED: return L"HTTP_E_STATUS_LENGTH_REQUIRED";
        case HTTP_E_STATUS_PRECOND_FAILED: return L"HTTP_E_STATUS_PRECOND_FAILED";
        case HTTP_E_STATUS_REQUEST_TOO_LARGE: return L"HTTP_E_STATUS_REQUEST_TOO_LARGE";
        case HTTP_E_STATUS_URI_TOO_LONG: return L"HTTP_E_STATUS_URI_TOO_LONG";
        case HTTP_E_STATUS_UNSUPPORTED_MEDIA: return L"HTTP_E_STATUS_UNSUPPORTED_MEDIA";
        case HTTP_E_STATUS_RANGE_NOT_SATISFIABLE: return L"HTTP_E_STATUS_RANGE_NOT_SATISFIABLE";
        case HTTP_E_STATUS_EXPECTATION_FAILED: return L"HTTP_E_STATUS_EXPECTATION_FAILED";
        case HTTP_E_STATUS_SERVER_ERROR: return L"HTTP_E_STATUS_SERVER_ERROR";
        case HTTP_E_STATUS_NOT_SUPPORTED: return L"HTTP_E_STATUS_NOT_SUPPORTED";
        case HTTP_E_STATUS_BAD_GATEWAY: return L"HTTP_E_STATUS_BAD_GATEWAY";
        case HTTP_E_STATUS_SERVICE_UNAVAIL: return L"HTTP_E_STATUS_SERVICE_UNAVAIL";
        case HTTP_E_STATUS_GATEWAY_TIMEOUT: return L"HTTP_E_STATUS_GATEWAY_TIMEOUT";
        case HTTP_E_STATUS_VERSION_NOT_SUP: return L"HTTP_E_STATUS_VERSION_NOT_SUP";

        // WinINet specific errors
        case INET_E_INVALID_URL: return L"INET_E_INVALID_URL";
        case INET_E_NO_SESSION: return L"INET_E_NO_SESSION";
        case INET_E_CANNOT_CONNECT: return L"INET_E_CANNOT_CONNECT";
        case INET_E_RESOURCE_NOT_FOUND: return L"INET_E_RESOURCE_NOT_FOUND";
        case INET_E_OBJECT_NOT_FOUND: return L"INET_E_OBJECT_NOT_FOUND";
        case INET_E_DATA_NOT_AVAILABLE: return L"INET_E_DATA_NOT_AVAILABLE";
        case INET_E_DOWNLOAD_FAILURE: return L"INET_E_DOWNLOAD_FAILURE";
        case INET_E_AUTHENTICATION_REQUIRED: return L"INET_E_AUTHENTICATION_REQUIRED";
        case INET_E_NO_VALID_MEDIA: return L"INET_E_NO_VALID_MEDIA";
        case INET_E_CONNECTION_TIMEOUT: return L"INET_E_CONNECTION_TIMEOUT";
        case INET_E_INVALID_REQUEST: return L"INET_E_INVALID_REQUEST";
        case INET_E_UNKNOWN_PROTOCOL: return L"INET_E_UNKNOWN_PROTOCOL";
        case INET_E_SECURITY_PROBLEM: return L"INET_E_SECURITY_PROBLEM";
        case INET_E_CANNOT_LOAD_DATA: return L"INET_E_CANNOT_LOAD_DATA";
        case INET_E_CANNOT_INSTANTIATE_OBJECT: return L"INET_E_CANNOT_INSTANTIATE_OBJECT";
        case INET_E_INVALID_CERTIFICATE: return L"INET_E_INVALID_CERTIFICATE";
        case INET_E_REDIRECT_FAILED: return L"INET_E_REDIRECT_FAILED";
        case INET_E_REDIRECT_TO_DIR: return L"INET_E_REDIRECT_TO_DIR";
    }

    return L"Unknown error";
}

void
Utils::LogExceptionDebugInfo(
    __in HRESULT hr,
    __in_opt PCWSTR pwszFunction,
    __in_opt PCWSTR pwszFile,
    __in uint32 uLine
    )
{
    if( Configuration::IsAtDiagnosticsTraceLevel(XboxNetworkMeshDiagnosticsTraceLevel::Error) )
    {
        std::wstring info = L"[Exception]: HRESULT: ";
        info += Utils::ConvertHResultToString(hr)->Data();
        info += L"\n";
        if( pwszFunction != nullptr )
        {
            info += L"\t\tFunction: ";
            info += pwszFunction;
            info += L"\n";
        }
        if( pwszFile != nullptr )
        {
            info += L"\t\tFile:";
            info += pwszFile;
            info += L"(";
            info += uLine.ToString()->Data();
            info += L")\n";
        }

        OutputDebugString( info.c_str() );
        Configuration::RaiseDebugOutput( ref new Platform::String(info.c_str()) );
    }
}

Platform::String^ Utils::FormatString( LPCWSTR strMsg, ... )
{
    WCHAR strBuffer[2048];

    va_list args;
    va_start(args, strMsg);
    _vsnwprintf_s( strBuffer, 2048, _TRUNCATE, strMsg, args );
    strBuffer[2047] = L'\0';

    va_end(args);

    Platform::String^ str = ref new Platform::String(strBuffer);
    return str;
}

Platform::String^ Utils::GetErrorString( HRESULT hr )
{
    Platform::String^ str = Utils::FormatString(L" %s [0x%0.8x]", ConvertHResultToErrorName(hr)->Data(), hr );
    return str;
}


void Utils::GetBufferBytes( __in Windows::Storage::Streams::IBuffer^ buffer, __out byte** ppOut )
{
    if( ppOut == nullptr || buffer == nullptr )
    {
        throw ref new Platform::InvalidArgumentException();
    }
    *ppOut = nullptr;

    Microsoft::WRL::ComPtr<IInspectable> srcBufferInspectable(reinterpret_cast<IInspectable*>( buffer ));
    Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> srcBufferByteAccess;
    srcBufferInspectable.As(&srcBufferByteAccess);
    srcBufferByteAccess->Buffer(ppOut);
}


Platform::String^ Utils::PrintSocketAddress(
    _In_ UINT32 sockaddrSize,
    _In_ const SOCKADDR* sockaddr
    )
{
    int result;
    char hostname[256] = {0};
    char port[64] = {0};

    ZeroMemory(hostname, sizeof(hostname));
    result = getnameinfo(sockaddr,
        sockaddrSize,
        hostname,
        sizeof(hostname),
        port,
        sizeof( port ),
        (NI_NUMERICHOST | NI_NUMERICSERV));

    if (result == 0)
    {
        return Utils::FormatString(
            L"[%hs]:%hs",
            hostname,
            port
            );
    }
    else
    {
        result = WSAGetLastError();
        return Utils::FormatString( L"PrintSocketAddress: %d", result );
    }
}

Platform::String^ Utils::PrintSecureDeviceAssociation(
#ifdef _XBOX_ONE
    Windows::Xbox::Networking::SecureDeviceAssociation^ association,
#else
    Windows::Networking::XboxLive::XboxLiveEndpointPair^ association,
#endif
    bool includeLocal,
    bool includeRemote
    )
{
    if( association == nullptr )
    {
        return L"null";
    }

    try
    {
        Platform::String^ textLocal;
        if( includeLocal )
        {
            SOCKADDR_STORAGE localSocketAddress;
            Platform::ArrayReference<BYTE> localSocketAddressBytesFromAssociationInTemplate(
                (BYTE*) &localSocketAddress,
                sizeof(localSocketAddress));
            association->GetLocalSocketAddressBytes(localSocketAddressBytesFromAssociationInTemplate);

            textLocal = PrintSocketAddress( sizeof(localSocketAddress), (SOCKADDR*) &localSocketAddress );
        }

        Platform::String^ textRemote;
        if( includeRemote )
        {
            SOCKADDR_STORAGE remoteSocketAddress;
            Platform::ArrayReference<BYTE> remoteSocketAddressBytesFromAssociationInTemplate(
                (BYTE*) &remoteSocketAddress,
                sizeof(remoteSocketAddress));
            association->GetRemoteSocketAddressBytes(remoteSocketAddressBytesFromAssociationInTemplate);

            textRemote = PrintSocketAddress( sizeof(remoteSocketAddress), (SOCKADDR*) &remoteSocketAddress );
        }

        if( includeLocal && includeRemote )
        {
            return Utils::FormatString( L"Local:%s Remote:%s", textLocal->Data(), textRemote->Data());
        }
        else
        {
            return Utils::FormatString( L"%s%s", textLocal->Data(), textRemote->Data());
        }
    }
    catch (...)
    {
        return L"Failure printing address";
    }
}

Platform::String^ Utils::GetThreadDescription( Platform::String^ desc )
{
    Platform::String^ apartmentType = GetApartmentTypeString();
    DWORD dwThreadId = GetCurrentThreadId();
    return Utils::FormatString( L"%s: ThreadID:%d [%s]", desc->Data(), dwThreadId, apartmentType->Data() );
}

Platform::String^ Utils::GetApartmentTypeString()
{
    APTTYPE at;
    APTTYPEQUALIFIER atq;
    ::CoGetApartmentType(&at, &atq);

    Platform::String^ desc;
    switch (at)
    {
    case APTTYPE_CURRENT: desc = L"CURRENT"; break;
    case APTTYPE_STA: desc = L"STA"; break;
    case APTTYPE_MTA: desc = L"MTA"; break;
    case APTTYPE_NA: desc = L"NA"; break;
    case APTTYPE_MAINSTA: desc = L"MAINSTA"; break;
    default: desc = L"UNKNOWN"; break;
    }

    switch (atq)
    {
    case APTTYPEQUALIFIER_NONE: desc += L" NONE"; break;
    case APTTYPEQUALIFIER_IMPLICIT_MTA: desc += L" IMPLICIT_MTA"; break;
    case APTTYPEQUALIFIER_NA_ON_MTA: desc += L" NA_ON_MTA"; break;
    case APTTYPEQUALIFIER_NA_ON_STA: desc += L" NA_ON_STA"; break;
    case APTTYPEQUALIFIER_NA_ON_IMPLICIT_MTA: desc += L" NA_ON_IMPLICIT_MTA"; break;
    case APTTYPEQUALIFIER_NA_ON_MAINSTA: desc += L" NA_ON_MAINSTA"; break;
    default: desc += L" UNKNOWN"; break;
    }

    return desc;
}
