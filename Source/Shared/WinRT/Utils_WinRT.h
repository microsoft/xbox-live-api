// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

typedef std::chrono::duration<long long, std::ratio<1, 10000000>> timeSpanTicks;

private class UtilsWinRT
{
public:
    static Windows::Foundation::DateTime
    ConvertNativeDateTimeToDateTime(
        _In_ const utility::datetime& time
        );

    static utility::datetime
    ConvertDateTimeToNativeDateTime(
        _In_ const Windows::Foundation::DateTime& time
        );

    template<class T>
    static Windows::Foundation::TimeSpan
    ConvertSecondsToTimeSpan(
        _In_ const T& seconds
        )
    {
        Windows::Foundation::TimeSpan ts;
        ts.Duration = std::chrono::duration_cast<timeSpanTicks>(seconds).count();
        return ts;
    }

    template<class T>
    static T
    ConvertTimeSpanToSeconds(
        _In_ const Windows::Foundation::TimeSpan& timeSpan
        )
    {
        return std::chrono::duration_cast<T>(timeSpanTicks(timeSpan.Duration));
    }

    static HRESULT ConvertExceptionToHRESULT();

    static Platform::Exception^ ConvertHRESULTToException(HRESULT hr, Platform::String^ message);

    static byte* GetBufferBytes(
        _In_ Windows::Storage::Streams::IBuffer^ buffer);

    static Windows::Storage::Streams::IBuffer^ CreateBufferFromBytes(
        _In_reads_bytes_(size) byte* bytes,
        _In_ unsigned int size);

    static std::vector<string_t> CovertVectorViewToStdVectorString(
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ vec
        );

    static Platform::Collections::Vector<Platform::String^>^ CreatePlatformVectorFromStdVectorString(
        _In_ const std::vector<std::wstring>& cppVector
        )
    {
        auto vector = ref new Platform::Collections::Vector<Platform::String^>();
        for (const auto& item : cppVector)
        {
            vector->Append(ref new Platform::String(item.c_str()));
        }

        return vector;
    }

    static std::map<std::wstring, std::wstring> CreateStdMapStringFromMapObj(
        _In_opt_ Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ winrtMap
        )
    {
        std::map<std::wstring, std::wstring> cppMap;
        if (winrtMap != nullptr)
        {
            for (const auto& item : winrtMap)
            {
                cppMap[item->Key->Data()] = item->Value->Data();
            }
        }

        return cppMap;
    }

    static std::unordered_map<std::wstring, std::wstring> CreateStdUnorderedMapStringFromMapObj(
        _In_opt_ Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ winrtMap
        )
    {
        std::unordered_map<std::wstring, std::wstring> cppMap;
        if (winrtMap != nullptr)
        {
            for (const auto& item : winrtMap)
            {
                cppMap[item->Key->Data()] = item->Value->Data();
            }
        }

        return cppMap;
    }

    template<typename T, typename cppT>
    static Platform::Collections::Vector<T^>^ CreatePlatformVectorFromStdVectorObj(
        _In_ const std::vector<cppT>& cppVector
        )
    {
        auto vector = ref new Platform::Collections::Vector<T^>();
        for (const auto& item : cppVector)
        {
            vector->Append(ref new T(item));
        }

        return vector;
    }

    template<typename T, typename cppT>
    static Platform::Collections::Vector<T>^ CreatePlatformVectorFromStdVectorEnum(
        _In_ const std::vector<cppT>& cppVector
        )
    {
        auto vector = ref new Platform::Collections::Vector<T>();
        for (const auto& item : cppVector)
        {
            vector->Append(static_cast<T>(item));
        }

        return vector;
    }

    static std::vector<string_t> CreateStdVectorStringFromPlatformVectorObj(
        _In_opt_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ platformVector
        )
    {
        auto vector = std::vector<string_t>();

        if (platformVector == nullptr){ return vector; }
        for (const auto& item : platformVector)
        {
            if (item == nullptr)
            {
                vector.push_back(string_t());
            }
            else
            {
                vector.push_back(string_t(item->Data()));
            }
        }

        return vector;
    }

    template<typename cppT, typename T>
    static std::vector<cppT> CreateStdVectorObjectFromPlatformVectorObj(
        _In_opt_ Windows::Foundation::Collections::IVectorView<T^>^ platformVector
        )
    {
        auto vector = std::vector<cppT>();

        if (platformVector == nullptr){ return vector; }
        for (const auto& item : platformVector)
        {
            vector.push_back(cppT(item->GetCppObj()));
        }

        return vector;
    }

    template<typename cppT, typename T>
    static std::unordered_map<string_t, cppT> CreateStdMapObjectWithStringKeyFromPlatformMapObj(
        _In_opt_ Windows::Foundation::Collections::IMapView<Platform::String^, T^>^ platformMap
        )
    {
        auto map = std::unordered_map<string_t, cppT>();

        if (platformMap == nullptr) { return map; }
        for (const auto& item : platformMap)
        {
            map[item->Key->Data()] = item->Value->GetCppObj();
        }

        return map;
    }

    template<typename T>
    static std::unordered_map<string_t, T> CreateStdUnorderedMapObjectWithStringKeyFromPlatformMapObj(
        _In_opt_ Windows::Foundation::Collections::IMapView<Platform::String^, T>^ platformMap
        )
    {
        auto map = std::unordered_map<string_t, T>();

        if (platformMap == nullptr) { return map; }
        for (const auto& item : platformMap)
        {
            map[item->Key->Data()] = item->Value;
        }

        return map;
    }

    template<typename T, typename cppT>
    static Platform::Collections::Map<Platform::String^, T^>^ CreatePlatformMapObjectWithStringKeyFromStdMapObj(
        _In_ std::unordered_map<string_t, cppT> stdMap
        )
    {
        auto map = ref new Platform::Collections::Map<Platform::String^, T^>();

        if (stdMap.size() == 0) { return map; }
        for (const auto& item : stdMap)
        {
            std::wstring wStr = std::wstring(item.first.begin(), item.first.end());
            Platform::String^ str = ref new Platform::String(wStr.c_str());
            map->Insert(str, ref new T(item.second));
        }

        return map;
    }

    static Windows::Foundation::PropertyType ConvertStringToPropertyType(
        _In_ std::wstring typeName
        );

    static inline web::json::value JsonValueFromPlatformString(Platform::String^ x)
    {
        if (x->IsEmpty())
        {
            return web::json::value::null();
        }
        else
        {
            auto jsonString = string_t(x->Data());
            return xbox::services::utils::json_get_value_from_string(jsonString);
        }
    }

    static web::json::value JsonValueFromPropertySet(
        _In_ Windows::Foundation::Collections::PropertySet^ value
        );

    static inline Platform::String^ StringFromInternalString(const xsapi_internal_string& internalString)
    {
        return ref new Platform::String(xbox::services::utils::string_t_from_internal_string(internalString).data());
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
