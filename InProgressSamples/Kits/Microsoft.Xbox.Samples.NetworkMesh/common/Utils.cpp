//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
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

Platform::String^ Utils::s_locale;

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

std::wstring&
Utils::UriReplace(
    __inout std::wstring& strSource,
    __in PCWSTR pwszPattern,
    __in_opt PCWSTR pwszReplacement,
    __out_opt size_t* pnOccurrencesReplaced
    )
{
    return Utils::Replace(
        strSource,
        pwszPattern,
        ( ( pwszReplacement != nullptr ) ? Utils::UriEncode(pwszReplacement).c_str() : nullptr ),
        pnOccurrencesReplaced
        );
}

std::wstring
Utils::UriEncode(
    __in std::wstring wstrUriToEncode
    )
{
    std::wstring wstrUrlEncoded;

    std::wstring::iterator it = wstrUriToEncode.begin();
    for ( ; it != wstrUriToEncode.end(); it++ )
    {
        wchar_t chr = *it;

        // We do not handle non-ASCII characters
        THROW_WIN32_IF( chr > 0x7F, ERROR_INVALID_DATA);

        // Do not encode Unreserved Characters 
        // reference: http://en.wikipedia.org/wiki/Percent-encoding
        bool fUnreserved   = ( ( chr >= L'0' && chr <= L'9' )
                            || ( chr >= L'a' && chr <= L'z' )
                            || ( chr >= L'A' && chr <= L'Z' )
                            || chr == L'-' || chr == L'_'
                            || chr == L'.' || chr == L'~' );

        if ( !fUnreserved )
        {
            // encode character
            wchar_t wszHex[] = L"FF";
            THROW_WIN32_IF(
                0 != _ultow_s( chr, wszHex, ARRAYSIZE(wszHex), 16 ),
                ERROR_INVALID_DATA
                );

            wstrUrlEncoded.append( 1, L'%' );
            wstrUrlEncoded.append( wszHex );
        }
        else
        {
            wstrUrlEncoded.append( 1, chr );
        }
    }

    return wstrUrlEncoded;
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

Microsoft::WRL::ComPtr<IStream>
Utils::StringToStream(
    __in Platform::String^ source,
    __out uint32* streamSize
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(source);
    THROW_E_POINTER_IF_NULL(streamSize);
    *streamSize = 0;

    Array<byte>^ buffer = StringToArray(source);
    return ArrayToStream(buffer, streamSize);
}

Platform::Array<byte>^ 
Utils::BufferToArray(
    __in Windows::Storage::Streams::IBuffer^ buffer
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(buffer);
    if (buffer->Length > 0)
    {
        Array<byte>^ byteArray = ref new Array<byte>(buffer->Length);
        DataReader^ dataReader = DataReader::FromBuffer(buffer);
        dataReader->ReadBytes(byteArray);
        return byteArray;
    }

    return nullptr;
}

Platform::Array<byte>^ 
Utils::StringToArray(
    __in Platform::String^ source
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(source);

    if (source->Length() > 0)
    {
        // Calculate the required buffer size first for the conversion of the
        // wide string into narrow string for the request body
        int bufferSize = WideCharToMultiByte(
            CP_UTF8,
            WC_ERR_INVALID_CHARS,
            source->Data(),
            -1,
            nullptr,
            0,
            nullptr,
            nullptr
            );

        THROW_WIN32_IF(bufferSize == 0, GetLastError());

        // Now perform the real conversion
        Array<byte>^ buffer = ref new Array<byte>(bufferSize);

        int result = WideCharToMultiByte(
            CP_UTF8,
            WC_ERR_INVALID_CHARS,
            source->Data(),
            -1,
            reinterpret_cast<PSTR>(buffer->Data),
            buffer->Length,
            nullptr,
            nullptr
            );

        THROW_WIN32_IF(result == 0, GetLastError());
        return buffer;
    }

    return nullptr;
}

Microsoft::WRL::ComPtr<IStream>
Utils::ArrayToStream(
    __in Platform::Array<byte>^ buffer,
    __out uint32* streamSize
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(buffer);
    THROW_INVALIDARGUMENT_IF_NULL(streamSize);

    // Create the stream which will contain the result
    Microsoft::WRL::ComPtr<IStream> stream;
    THROW_IF_HR_FAILED(
        CreateStreamOnHGlobal(nullptr, TRUE, stream.GetAddressOf())
        );
    
    // We have string converted, so write it to a stream
    ULONG totalBytesWritten = 0;
    THROW_IF_HR_FAILED(
        stream->Write(buffer->Data, buffer->Length, &totalBytesWritten)
        );

    // Need to set the stream back to the beginning. Otherwise,
    // the stream is at the end and the caller can't consume it.
    LARGE_INTEGER pos = {0};
    THROW_IF_HR_FAILED(
        stream->Seek(
            pos,
            STREAM_SEEK_SET,
            nullptr
            )
        );

    THROW_HR_IF(totalBytesWritten != buffer->Length, E_FAIL);
    *streamSize = totalBytesWritten;

    return stream;
}

Platform::String^
Utils::StreamToString(
    __in ISequentialStream* source
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(source);

    // Create the empty string which will be appended with the result
    Platform::String^ resultString = "";
    
    // Keep reading until the buffer is full
    CHAR tempBuffer[8192];
    Array<byte>^ byteBuffer;
    ULONG read = 0;
    ULONG contentSize = 0;
    do
    {
        THROW_IF_HR_FAILED(
            source->Read(
                tempBuffer,
                ARRAYSIZE(tempBuffer),
                &read
                )
            );

        if (read > 0)
        {
            // Allocate a new and bigger buffer. Always +1 to leave room for the
            // null terminator
            Array<byte>^ newBuffer = ref new Array<byte>(contentSize + read + 1);

            // Copy the existing buffer content to the new buffer first
            if (contentSize > 0)
            {
                CopyMemory(newBuffer->Data, byteBuffer->Data, contentSize);
            }

            // Copy the new content
            CopyMemory(&newBuffer->Data[contentSize], tempBuffer, read);

            // Set the new buffer as the buffer and update relevant info
            contentSize += read;
            byteBuffer = newBuffer;
        }
    }
    while (read > 0 );
    
    // If the stream was non-empty, then we need to convert to wide char string.
    if (contentSize > 0)
    {
        // First, null-terminate the buffer
        byteBuffer->Data[contentSize] = 0;

        // Calculate the required buffer size first for the conversion of the
        // wide string into narrow string for the request body
        int size = MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            (PSTR)byteBuffer->Data,
            -1,
            nullptr,
            0
            );

        THROW_WIN32_IF(size == 0, GetLastError());

        // Now perform the real conversion. The buffer needs to have +1 for the
        // null-terminator
        Array<WCHAR>^ wcharBuffer = ref new Array<WCHAR>(size + 1);

        int result = MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            (PSTR)byteBuffer->Data,
            -1,
            wcharBuffer->Data,
            wcharBuffer->Length
            );

        THROW_WIN32_IF(result == 0, GetLastError());

        // We have string converted, so copy it to our String^ result
        resultString = ref new String(wcharBuffer->Data);
    }

    return resultString;
}

HRESULT
Utils::ConvertHttpStatusCodeToHR(
    __in uint32 httpStatusCode
    )
{
    HRESULT hr = HTTP_E_STATUS_UNEXPECTED;
    // 2xx are http success codes
    if ((httpStatusCode >= 200) && (httpStatusCode < 300))
    {
        hr = S_OK;
    }
    // MSXML XHR bug: get_status() returns HTTP/1223 for HTTP/204:
    // http://blogs.msdn.com/b/ieinternals/archive/2009/07/23/the-ie8-native-xmlhttprequest-object.aspx
    // treat it as success code as well
    else if ( httpStatusCode == 1223 )
    {
        hr = S_OK;
    }
    else
    {
        switch(httpStatusCode)
        {
        //
        // 101 server has switched protocols in upgrade header. Not suppose to happen in product.
        //
        case 101:
            hr = INET_E_UNKNOWN_PROTOCOL;
            break;

        //
        // 300 Multiple Choices
        //
        case WebErrorStatus::WebErrorStatus_MultipleChoices:
            hr = HTTP_E_STATUS_AMBIGUOUS;
            break;

        //
        // 301 Moved Permanently
        //
        case WebErrorStatus::WebErrorStatus_MovedPermanently:
            hr = HTTP_E_STATUS_MOVED;
            break;

        //
        // 302 Found
        //
        case WebErrorStatus::WebErrorStatus_Found:
            hr = HTTP_E_STATUS_REDIRECT;
            break;

        //
        // 303 See Other
        //
        case WebErrorStatus::WebErrorStatus_SeeOther:
            hr = HTTP_E_STATUS_REDIRECT_METHOD;
            break;

        //
        // 304 Not Modified
        //
        case WebErrorStatus::WebErrorStatus_NotModified:
            hr = HTTP_E_STATUS_NOT_MODIFIED;
            break;

        //
        // 305 Use Proxy
        //
        case WebErrorStatus::WebErrorStatus_UseProxy:
            hr = HTTP_E_STATUS_USE_PROXY;
            break;

        //
        // 307 Temporary Redirect
        //
        case WebErrorStatus::WebErrorStatus_TemporaryRedirect:
            hr = HTTP_E_STATUS_REDIRECT_KEEP_VERB;
            break;

        //
        // 400 Bad Request
        //
        case WebErrorStatus::WebErrorStatus_BadRequest:
            hr = HTTP_E_STATUS_BAD_REQUEST;
            break;

        //
        // 401 Unauthorized
        //
        case WebErrorStatus::WebErrorStatus_Unauthorized:
            hr = HTTP_E_STATUS_DENIED;
            break;

        //
        // 402 Payment Required
        //
        case WebErrorStatus::WebErrorStatus_PaymentRequired:
            hr = HTTP_E_STATUS_PAYMENT_REQ;
            break;

        //
        // 403 Forbidden.
        //
        case WebErrorStatus::WebErrorStatus_Forbidden:
            hr = HTTP_E_STATUS_FORBIDDEN;
            break;

        //
        // 404 Not Found.
        //
        case WebErrorStatus::WebErrorStatus_NotFound:
            hr = HTTP_E_STATUS_NOT_FOUND;
            break;

        //
        // 405 Method Not Allowed
        //
        case WebErrorStatus::WebErrorStatus_MethodNotAllowed:
            hr = HTTP_E_STATUS_BAD_METHOD;
            break;

        //
        // 406 Not Acceptable
        //
        case WebErrorStatus::WebErrorStatus_NotAcceptable:
            hr = HTTP_E_STATUS_NONE_ACCEPTABLE;
            break;
        //
        // 407 Proxy Authentication Required
        //
        case WebErrorStatus::WebErrorStatus_ProxyAuthenticationRequired:
            hr = HTTP_E_STATUS_PROXY_AUTH_REQ;
            break;
        //
        // 408 Request Timeout
        //
        case WebErrorStatus::WebErrorStatus_RequestTimeout:
            hr = HTTP_E_STATUS_REQUEST_TIMEOUT;
            break;

        //
        // 409 Conflict
        //
        case WebErrorStatus::WebErrorStatus_Conflict:
            hr = HTTP_E_STATUS_CONFLICT;
            break;

        //
        // 410 Gone
        //
        case WebErrorStatus::WebErrorStatus_Gone:
            hr = HTTP_E_STATUS_GONE;
            break;

        //
        // 411 Length Required
        //
        case WebErrorStatus::WebErrorStatus_LengthRequired:
            hr = HTTP_E_STATUS_LENGTH_REQUIRED;
            break;

        //
        // 412 Precondition Failed
        //
        case WebErrorStatus::WebErrorStatus_PreconditionFailed:
            hr = HTTP_E_STATUS_PRECOND_FAILED;
            break;

        //
        // 413 Request Entity Too Large
        //
        case WebErrorStatus::WebErrorStatus_RequestEntityTooLarge:
            hr = HTTP_E_STATUS_REQUEST_TOO_LARGE;
            break;

        //
        // 414 Request URI Too Long
        //
        case WebErrorStatus::WebErrorStatus_RequestUriTooLong:
            hr = HTTP_E_STATUS_URI_TOO_LONG;
            break;

        //
        // 415 Unsupported Media Type
        //
        case WebErrorStatus::WebErrorStatus_UnsupportedMediaType:
            hr = HTTP_E_STATUS_UNSUPPORTED_MEDIA;
            break;

        //
        // 416 Requested Range Not Satisfiable
        //
        case WebErrorStatus::WebErrorStatus_RequestedRangeNotSatisfiable:
            hr = HTTP_E_STATUS_RANGE_NOT_SATISFIABLE;
            break;

        //
        // 417 Expectation Failed
        //
        case WebErrorStatus::WebErrorStatus_ExpectationFailed:
            hr = HTTP_E_STATUS_EXPECTATION_FAILED;
            break;

        //
        // 449 Retry after doing the appropriate action.
        //
        case 449:
            hr = HTTP_E_STATUS_BAD_REQUEST;
            break;

        //
        // 500 Internal Server Error
        //
        case WebErrorStatus::WebErrorStatus_InternalServerError:
            hr = HTTP_E_STATUS_SERVER_ERROR;
            break;

        //
        // 501 Not Implemented
        //
        case WebErrorStatus::WebErrorStatus_NotImplemented:
            hr = HTTP_E_STATUS_NOT_SUPPORTED;
            break;

        //
        // 502 Bad Gateway
        //
        case WebErrorStatus::WebErrorStatus_BadGateway:
            hr = HTTP_E_STATUS_BAD_GATEWAY;
            break;

        //
        // 503 Service Unavailable
        //
        case WebErrorStatus::WebErrorStatus_ServiceUnavailable:
            hr = HTTP_E_STATUS_SERVICE_UNAVAIL;
            break;

        //
        // 504 Gateway Timeout
        //
        case WebErrorStatus::WebErrorStatus_GatewayTimeout:
            hr = HTTP_E_STATUS_GATEWAY_TIMEOUT;
            break;

        //
        // 505 HTTP Version Not Supported.
        //
        case WebErrorStatus::WebErrorStatus_HttpVersionNotSupported:
            hr = HTTP_E_STATUS_VERSION_NOT_SUP;
            break;

        default:
            hr = HTTP_E_STATUS_UNEXPECTED;
            break;
        }
    }

    return hr;
}

PropertyType
Utils::ConvertStringToPropertyType(
    __in Platform::String^ typeName
    )
{
    if( typeName == nullptr )
    {
        return PropertyType::OtherType;
    }
    else if( _wcsicmp(typeName->Data(), L"Integer") == 0 )
    {
        return PropertyType::Int64;
    }
    else if( _wcsicmp(typeName->Data(), L"Double") == 0 )
    {
        return PropertyType::Double;
    }
    else if( _wcsicmp(typeName->Data(), L"String") == 0 )
    {
        return PropertyType::String;
    }
    else if( _wcsicmp(typeName->Data(), L"DateTime") == 0 )
    {
        return PropertyType::DateTime;
    }

    return PropertyType::OtherType;
}

HRESULT
Utils::ConvertExceptionToHRESULT()
{
    // Default value, if there is no exception appears, return S_OK
    HRESULT hr = S_OK;

    try
    {
        throw;
    }

    // std exceptions
    catch( const std::bad_alloc& ) // is an exception
    {
        hr = E_OUTOFMEMORY;
    }
    catch( const std::invalid_argument& ) // is a logic_error
    {
        hr = E_INVALIDARG;
    }
    catch( const std::domain_error& ) // is a logic_error
    {
        hr = E_INVALIDARG;
    }
    catch( const std::out_of_range& ) // is a logic_error
    {
        hr = E_INVALIDARG;
    }
    catch( const std::length_error& ) // is a logic_error
    {
        hr = __HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW );
    }
    catch( const std::overflow_error& ) // is a runtime_error
    {
        hr = __HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW );
    }
    catch( const std::underflow_error& ) // is a runtime_error
    {
        hr = RPC_S_FP_UNDERFLOW;
    }
    catch( const std::range_error& ) // is a runtime_error
    {
        hr = E_INVALIDARG;
    }
    catch ( const std::logic_error& ) // is an exception
    {
        hr = E_UNEXPECTED;
    }
    catch ( const std::runtime_error& ) // is an exception
    {
        hr = E_FAIL;
    }
    catch ( const std::exception& ) // base class for standard C++ exceptions
    {
        hr = E_FAIL;
    }
    catch ( Platform::Exception^ exception )
    {
        hr = (HRESULT)exception->HResult;
    }
    catch ( HRESULT exceptionHR )
    {
        hr = exceptionHR;
    }
    catch (...) // everything else
    {
        hr = E_FAIL;
    }

    return hr;
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

Microsoft::WRL::ComPtr<IStream>
Utils::BufferToStream(
    __in IBuffer^ source,
    __out uint32* streamSize
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(source);
    THROW_E_POINTER_IF_NULL(streamSize);
    *streamSize = 0;

    // Create the stream which will contain the result
    Microsoft::WRL::ComPtr<IStream> stream;
    THROW_IF_HR_FAILED(
        CreateStreamOnHGlobal(nullptr, TRUE, stream.GetAddressOf())
        );
    
    if (source->Length > 0)
    {
        ULONG totalBytesWritten = 0;
        DataReader^ dataReader = DataReader::FromBuffer(source);
        while(dataReader->UnconsumedBufferLength > 0)
        {
            uint32 dataBufferSize = min(1024*1024, dataReader->UnconsumedBufferLength);
            auto dataBuffer = ref new Array<uint8>(dataBufferSize);
            dataReader->ReadBytes(dataBuffer);

            ULONG bytesWritten = 0;
            THROW_IF_HR_FAILED(
                stream->Write(dataBuffer->Data, dataBufferSize, &bytesWritten)
                );

            THROW_HR_IF(bytesWritten != dataBufferSize, E_FAIL);

            totalBytesWritten += bytesWritten;
        }

        // Need to set the stream back to the beginning. Otherwise,
        // the stream is at the end and the caller can't consume it.
        LARGE_INTEGER pos = {0};
        THROW_IF_HR_FAILED(
            stream->Seek(pos, STREAM_SEEK_SET, nullptr)
            );

        *streamSize = totalBytesWritten;
    }

    return stream;
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


Windows::Data::Json::IJsonValue^ Utils::GetNullJsonValue() 
{
    Windows::Data::Json::JsonObject^ json = Windows::Data::Json::JsonObject::Parse("{ \"0\": null }");
    return json->GetNamedValue("0");
}


void
Utils::BufferCopy(
    __in Windows::Storage::Streams::IBuffer^ source,
    __in Windows::Storage::Streams::IBuffer^ destination,
    __in bool append
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(source);
    THROW_INVALIDARGUMENT_IF_NULL(destination);

    // check the response buffer length to see if it's large enough to hold the response
    THROW_WIN32_IF( source->Length > destination->Capacity, ERROR_INSUFFICIENT_BUFFER);

    Object^ srcObj = source;
    Microsoft::WRL::ComPtr<IInspectable> srcBufferInspectable(reinterpret_cast<IInspectable*>(srcObj));
    Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> srcBufferByteAccess;
    srcBufferInspectable.As(&srcBufferByteAccess);
    byte* srcBufferBytes = nullptr;
    srcBufferByteAccess->Buffer(&srcBufferBytes);

    Object^ destObj = destination;
    Microsoft::WRL::ComPtr<IInspectable> destBufferInspectable(reinterpret_cast<IInspectable*>(destObj));
    Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> destBufferByteAccess;
    destBufferInspectable.As(&destBufferByteAccess);
    byte* destBufferBytes = nullptr;
    destBufferByteAccess->Buffer(&destBufferBytes);

    uint32 startByteIndex = append ? destination->Length : 0;

    errno_t err = memcpy_s(
        &destBufferBytes[startByteIndex],
        destination->Capacity - startByteIndex,
        srcBufferBytes,
        source->Length
        );

    // Params were validated prior to the memcpy_s call, so this should never happen
    THROW_HR_IF(err != 0, E_FAIL);

    destination->Length = append ? (destination->Length + source->Length) : source->Length;
}

Platform::String^ Utils::ConvertHResultToString( HRESULT hr )
{
    WCHAR tmp[ 1024 ];
    _snwprintf_s( tmp, _countof( tmp ), _TRUNCATE, L"0x%0.8x", hr);
    return ref new Platform::String(tmp);
}

Platform::String^ Utils::GetLocale()
{
    if( s_locale->IsEmpty() )
    {
        WCHAR locale[LOCALE_NAME_MAX_LENGTH] = {0};
        GetUserDefaultLocaleName(locale, ARRAYSIZE(locale) );
        return ref new Platform::String(locale);
    }
    else
    {
        return s_locale;
    }
}

void Utils::SetLocaleMock(Platform::String^ locale)
{
    s_locale = locale;
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

std::wstring
Utils::UriEncodeSubPathComponents(
    __in std::wstring wstrSubPath
    )
{
    // The individual components of wstrSubPath need to be UriEncoded
    std::vector<std::wstring> values = Utils::StringSplit( wstrSubPath, L'/' );
    size_t cItems = values.size();
    if( cItems > 0 )
    {
        wstrSubPath.clear();

        size_t i = 0;
        while ( i < cItems )
        {
            wstrSubPath += Utils::UriEncode(values[i]);

            if( ++i < cItems )
            {
                wstrSubPath += L"/";
            }
        }
    }

    return wstrSubPath;
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
