// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Utils_WinRT.h"
#include "shared_macros.h"
#include "Macros_WinRT.h"
#include "Utils.h"
#include <time.h> 

#include <robuffer.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;

Windows::Foundation::DateTime
UtilsWinRT::ConvertNativeDateTimeToDateTime(
    _In_ const utility::datetime& time
    )
{
    Windows::Foundation::DateTime dateTime;
    dateTime.UniversalTime = time.to_interval();
    return dateTime;
}

utility::datetime
UtilsWinRT::ConvertDateTimeToNativeDateTime(
    _In_ const Windows::Foundation::DateTime& time
    )
{
    utility::datetime dateTime;
    dateTime = dateTime + time.UniversalTime;
    return dateTime;
}

HRESULT
UtilsWinRT::ConvertExceptionToHRESULT()
{
    // Default value, if there is no exception appears, return S_OK
    HRESULT hr = S_OK;

    try
    {
        throw;
    }

    catch (Platform::Exception^ exception)
    {
        hr = (HRESULT)exception->HResult;
    }
    catch (...) // everything else
    {
        hr = xbox::services::utils::convert_exception_to_hresult();
    }

    return hr;
}

Platform::Exception^
UtilsWinRT::ConvertHRESULTToException(HRESULT hr, Platform::String^ message)
{
    if( message == nullptr ) 
    {
        message = L"n/a";
    }

    Platform::Exception^ result;
    switch (hr)
    {
    case E_NOTIMPL:
        result = ref new Platform::NotImplementedException(message);
        break;

    case E_NOINTERFACE: 
        result = ref new Platform::InvalidCastException(message);
        break;

    case E_POINTER: // E_POINTER
        result = ref new Platform::NullReferenceException(message);
        break;

    case E_ABORT: // E_ABORT
        result = ref new Platform::NullReferenceException(message);
        break;

    case E_FAIL: // E_FAIL
        result = ref new Platform::FailureException(message);
        break;

    case E_ACCESSDENIED: // E_ACCESSDENIED
        result = ref new Platform::AccessDeniedException(message);
        break;

    case E_OUTOFMEMORY: // E_OUTOFMEMORY
        result = ref new Platform::OutOfMemoryException(message);
        break;

    case E_INVALIDARG: // E_INVALIDARG
        result = ref new Platform::InvalidArgumentException(message);
        break;

    case E_BOUNDS: // E_BOUNDS
        result = ref new Platform::OutOfBoundsException(message);
        break;

    case E_CHANGED_STATE: // E_CHANGED_STATE
        result = ref new Platform::ChangedStateException(message);
        break;

    case REGDB_E_CLASSNOTREG: // REGDB_E_CLASSNOTREG
        result = ref new Platform::ClassNotRegisteredException(message);
        break;

    case RPC_E_WRONG_THREAD: // RPC_E_WRONG_THREAD
        result = ref new Platform::WrongThreadException(message);
        break;

    case RPC_E_DISCONNECTED: // RPC_E_DISCONNECTED
        result = ref new Platform::DisconnectedException(message);
        break;

    case RO_E_CLOSED: // RO_E_CLOSED
        result = ref new Platform::ObjectDisposedException(message);
        break;

    default:
        result = ref new Platform::Exception(hr, message);
        break;
    }

    return result;
}

byte*
UtilsWinRT::GetBufferBytes(
    _In_ Windows::Storage::Streams::IBuffer^ buffer
)
{
    if (buffer == nullptr)
    {
        throw ref new Platform::InvalidArgumentException();
    }

    byte* ppOut = nullptr;

    Microsoft::WRL::ComPtr<IInspectable> srcBufferInspectable(reinterpret_cast<IInspectable*>(buffer));
    Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> srcBufferByteAccess;
    HRESULT hr = srcBufferInspectable.As(&srcBufferByteAccess);
    THROW_IF_HR_FAILED(hr);

    hr = srcBufferByteAccess->Buffer(&ppOut);
    THROW_IF_HR_FAILED(hr);

    return ppOut;
}

IBuffer^
UtilsWinRT::CreateBufferFromBytes(
    _In_reads_bytes_(size) byte* bytes,
    _In_ unsigned int size
)
{
    Buffer^ buffer = ref new Buffer(size);
    byte* bufferBytes = UtilsWinRT::GetBufferBytes(buffer);
    errno_t err = memcpy_s(bufferBytes, size, bytes, size);
    THROW_HR_IF(err != 0, E_FAIL)
    buffer->Length = size;
    return buffer;
}

std::vector<string_t> 
UtilsWinRT::CovertVectorViewToStdVectorString(
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ vec
    )
{
    std::vector<string_t> stdVec;
    if (vec != nullptr)
    {
        for (Platform::String^ s : vec)
        {
            stdVec.push_back(s->Data());
        }
    }
    return stdVec;
}

PropertyType
UtilsWinRT::ConvertStringToPropertyType(
    _In_ std::wstring typeName
)
{
    if (_wcsicmp(typeName.c_str(), L"Integer") == 0)
    {
        return PropertyType::Int64;
    }
    else if (_wcsicmp(typeName.c_str(), L"Double") == 0)
    {
        return PropertyType::Double;
    }
    else if (_wcsicmp(typeName.c_str(), L"String") == 0)
    {
        return PropertyType::String;
    }
    else if (_wcsicmp(typeName.c_str(), L"DateTime") == 0)
    {
        return PropertyType::DateTime;
    }

    return PropertyType::OtherType;
}

web::json::value 
UtilsWinRT::JsonValueFromPropertySet(
    _In_ Windows::Foundation::Collections::PropertySet^ value
    )
{
    if (value == nullptr || value->Size == 0)
    {
        return web::json::value::null();
    }

    web::json::value result = web::json::value::object();
    for (const auto& pair : value)
    {
        // We could assume it's a vaild key thus no empty check is needed.
        const auto& name = pair->Key;

        Windows::Foundation::IPropertyValue^ propertyValue;
        if (pair->Value == nullptr)
        {
            result[name->Data()] = web::json::value::null();
        }
        else
        {
            propertyValue = dynamic_cast<Windows::Foundation::IPropertyValue^>(pair->Value);
            auto valueType = propertyValue->Type;
            switch (valueType)
            {
            case Windows::Foundation::PropertyType::Empty:
                result[name->Data()] = web::json::value::null();
                break;

            case Windows::Foundation::PropertyType::Boolean:
            {
                bool boolValue = propertyValue->GetBoolean();
                result[name->Data()] = web::json::value::boolean(boolValue);
                break;
            }

            case Windows::Foundation::PropertyType::Int16:
            case Windows::Foundation::PropertyType::Int32:
            case Windows::Foundation::PropertyType::Int64:
            {
                // use the biggest range to take the value
                int64_t int64Value = propertyValue->GetInt64();
                result[name->Data()] = web::json::value(int64Value);
                break;
            }

            case Windows::Foundation::PropertyType::UInt8:
            case Windows::Foundation::PropertyType::UInt16:
            case Windows::Foundation::PropertyType::UInt32:
            case Windows::Foundation::PropertyType::UInt64:
            {
                // use the biggest range to take the value
                uint64_t numberValue = propertyValue->GetUInt64();
                result[name->Data()] = web::json::value(numberValue);
                break;
            }

            case Windows::Foundation::PropertyType::Single:
            case Windows::Foundation::PropertyType::Double:
            {
                // use the biggest range to take the value
                double numberValue = propertyValue->GetDouble();
                result[name->Data()] = web::json::value(numberValue);
                break;
            }

            case Windows::Foundation::PropertyType::Guid:
            {
                // use the biggest range to take the value
                Platform::Guid guidValue = propertyValue->GetGuid();
                result[name->Data()] = web::json::value::string(guidValue.ToString()->Data());
                break;
            }

            case Windows::Foundation::PropertyType::String:
            {
                Platform::String^ strValue = propertyValue->GetString();
                result[name->Data()] = web::json::value::string(strValue->Data());
                break;
            }

            default:
                throw ref new Platform::InvalidArgumentException("Property type is not supported");
            }
        }
    }

    return result;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
