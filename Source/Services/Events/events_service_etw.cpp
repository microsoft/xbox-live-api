// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

#if XSAPI_WRL_EVENTS_SERVICE

#include "events_service_etw.h"

#include <initguid.h>
#include <wrl.h>
#include <windows.foundation.diagnostics.h>

using namespace xbox::services::system;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Diagnostics;

DEFINE_GUID(XSAPI_TELEMETRY_GROUP,
    0x53b78fc6, 0xe359, 0x453e, 0x89, 0xfe, 0xa5, 0xf4, 0xe5, 0xff, 0x4a, 0xf3);

// {5E9EDC93-F04F-47EC-8DCB-0CF8F3442BDC}
DEFINE_GUID(GUID_LOGGING_CHANNEL,
    0x5e9edc93, 0xf04f, 0x47ec, 0x8d, 0xcb, 0xc, 0xf8, 0xf3, 0x44, 0x2b, 0xdc);

#define XBOX_LIVE_LOGGING_OPTIONS  0x0000800000000000
#define XBOX_LIVE_LOGGING_TAGS  0x00B00000

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_BEGIN

EventsService::EventsService(_In_ User&& user)
    : m_user{ std::move(user) }
{
    m_playSession = utils::create_guid(true);
}

EventsService::~EventsService()
{
#if HC_PLATFORM != HC_PLATFORM_UWP
    if (SUCCEEDED(m_hrCoIncrementMTAUsage))
    {
        CoDecrementMTAUsage(m_mtaUsageCookie);
    }
#endif
}

HRESULT EventsService::Initialize(
)
{

#if HC_PLATFORM != HC_PLATFORM_UWP
    m_hrCoIncrementMTAUsage = CoIncrementMTAUsage(&m_mtaUsageCookie);
    if (FAILED(m_hrCoIncrementMTAUsage))
    {
        LOG_DEBUG("CoIncrementMTAUsage failed with during EventsService::Initialize.");
        return m_hrCoIncrementMTAUsage;
    }
#endif

    ComPtr<ILoggingChannelOptionsFactory> loggingChannelOptionsFactory;
    HRESULT hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Foundation_Diagnostics_LoggingChannelOptions).Get(), &loggingChannelOptionsFactory);
    if (FAILED(hr))
    {
        LOG_DEBUG("GetActivationFactory for ILoggingChannelOptionsFactory failed during EventsService::Initialize.");
        return hr;
    }

    ComPtr<ILoggingChannelOptions> loggingChannelOptions;
    hr = loggingChannelOptionsFactory->Create(XSAPI_TELEMETRY_GROUP, &loggingChannelOptions);
    if (FAILED(hr))
    {
        LOG_DEBUG("ILoggingChannelOptions::Create failed during EventsService::Initialize.");
        return hr;
    }

    ComPtr<ILoggingChannelFactory2> loggingChannelFactory;
    hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Foundation_Diagnostics_LoggingChannel).Get(), &loggingChannelFactory);
    if (FAILED(hr))
    {
        LOG_DEBUG("GetActivationFactory for ILoggingChannelFactory2 failed during EventsService::Initialize.");
        return hr;
    }

    xsapi_internal_wstringstream ss;
    ss << L"Microsoft.XboxLive.T" << AppConfig::Instance()->TitleId();

    HString channelName;
    channelName.Set(ss.str().data());
    hr = loggingChannelFactory->CreateWithOptionsAndId(channelName.Get(), loggingChannelOptions.Get(), GUID_LOGGING_CHANNEL, &m_loggingChannel);
    if (FAILED(hr))
    {
        LOG_DEBUG("ILoggingChannelFactory2::CreateWithOptionsAndId failed during EventsService::Initialize.");
        return hr;
    }

    ComPtr<ILoggingOptionsFactory> loggingOptionsFactory;
    hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Foundation_Diagnostics_LoggingOptions).Get(), &loggingOptionsFactory);
    if (FAILED(hr))
    {
        LOG_DEBUG("GetActivationFactory for ILoggingOptionsFactory failed during EventsService::Initialize.");
        return hr;
    }

    hr = loggingOptionsFactory->CreateWithKeywords(XBOX_LIVE_LOGGING_OPTIONS, &m_loggingOptions);
    if (FAILED(hr))
    {
        LOG_DEBUG("ILoggingOptionsFactory::CreateWithKeywords failed during EventsService::Initialize.");
        return hr;
    }

    m_loggingOptions->put_Tags(XBOX_LIVE_LOGGING_TAGS);

    return S_OK;
}

HRESULT EventsService::WriteInGameEvent(
    _In_z_ const char* eventName,
    _In_opt_z_ const char* dimensions,
    _In_opt_z_ const char* measurements
)
{
    JsonDocument dimensionsJson;
    JsonDocument measurementsJson;

    std::regex regex("[A-Za-z]+[A-Za-z0-9_]*");
    bool matchFound = std::regex_match(eventName, regex);
    if (!matchFound)
    {
        LOG_DEBUG("Invalid event name");
        return E_INVALIDARG;
    }

    if (dimensions)
    {
        dimensionsJson.Parse(dimensions);
    }
    if (measurements)
    {
        measurementsJson.Parse(measurements);
    }

    if(dimensionsJson.HasParseError() || measurementsJson.HasParseError())
    {
        LOG_DEBUG("Unable to parse json string");
        return E_INVALIDARG;
    }

    return WriteInGameEventHelper(eventName, dimensionsJson, measurementsJson);
}


HRESULT EventsService::WriteInGameEventHelper(
    _In_ const xsapi_internal_string& eventName,
    _In_ const JsonValue& dimensions,
    _In_ const JsonValue& measurements
)
{
    try
    {
        auto fields = CreateLoggingFields(eventName, dimensions, measurements);

        //m_loggingChannel->loge
        ComPtr<ILoggingTarget> loggingTarget;
        m_loggingChannel->QueryInterface(__uuidof(ILoggingTarget), &loggingTarget);

        loggingTarget->LogEventWithFieldsAndOptions(utils::HStringFromUtf8(eventName.data()).Get(), fields.Get(), LoggingLevel_Critical, m_loggingOptions.Get());
    }
    catch (const std::exception&)
    {
        return utils::convert_exception_to_hresult();
    }
    catch (...)
    {
        return E_FAIL;
    }

    return S_OK;
}

ComPtr<ILoggingFields> EventsService::CreateLoggingFields(
    _In_ const xsapi_internal_string& eventName,
    _In_ const JsonValue& dimensions,
    _In_ const JsonValue& measurements
)
{
    THROW_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(eventName);
    THROW_CPP_INVALIDARGUMENT_IF(!(dimensions.IsObject() || dimensions.IsNull()));
    THROW_CPP_INVALIDARGUMENT_IF(!(measurements.IsObject() || measurements.IsNull()));

    ComPtr<ILoggingFields> fields;
    RoActivateInstance(HStringReference(RuntimeClass_Windows_Foundation_Diagnostics_LoggingFields).Get(), &fields);

    fields->BeginStruct(HStringReference(L"PartB_Microsoft.XboxLive.InGame").Get());
    fields->AddString(HStringReference(L"name").Get(), utils::HStringFromUtf8(eventName.data()).Get());

    fields->AddString(HStringReference(L"serviceConfigId").Get(), utils::HStringFromUtf8(AppConfig::Instance()->Scid().data()).Get());
    fields->AddString(HStringReference(L"playerSessionId").Get(), utils::HStringFromUtf8(m_playSession.data()).Get());
    fields->AddUInt32(HStringReference(L"titleId").Get(), AppConfig::Instance()->TitleId());
    fields->AddString(HStringReference(L"userId").Get(), utils::HStringFromUtf8(utils::uint64_to_internal_string(m_user.Xuid()).data()).Get());
    fields->AddUInt16(HStringReference(L"ver").Get(), 1);

    if (dimensions.IsObject())
    {
        // Add properties
        fields->BeginStruct(HStringReference(L"properties").Get());
        for (const auto& jsonPair : dimensions.GetObject())
        {
            std::pair<xsapi_internal_string, JsonDocument> pair;
            pair.first = jsonPair.name.GetString();
            JsonUtils::CopyFrom(pair.second, jsonPair.value);
            AddValuePair(fields, pair);
        }
        fields->EndStruct();
    }

    if (measurements.IsObject())
    {
        // Add measurements
        fields->BeginStruct(HStringReference(L"measurements").Get());
        for (const auto& jsonPair : measurements.GetObject())
        {
            std::pair<xsapi_internal_string, JsonDocument> pair;
            pair.first = jsonPair.name.GetString();
            JsonUtils::CopyFrom(pair.second, jsonPair.value);
            AddValuePair(fields, pair);
        }
        fields->EndStruct();
    }

    fields->EndStruct();

    return fields;
}

void EventsService::AddValuePair(
    _Inout_ Microsoft::WRL::ComPtr<ABI::Windows::Foundation::Diagnostics::ILoggingFields> fields,
    _In_ const std::pair<xsapi_internal_string, JsonDocument>& pair
)
{
    // check property name.
    const auto& name = pair.first;
    THROW_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(name);

    std::regex regex("[A-Za-z]+[A-Za-z0-9]*");
    bool matchFound = std::regex_match(name, regex);
    if (!matchFound)
    {
        throw std::invalid_argument("Invalid properties or measurements name");
    }

    auto s = utility::conversions::utf8_to_utf16(name.data());
    HStringReference propertyName{ s.c_str() };

    const auto& value = pair.second;
    switch (value.GetType())
    {
    case rapidjson::Type::kNumberType:
        // if value can fit into int64, add as int64
        if (value.IsInt64())
        {
            fields->AddInt64(propertyName.Get(), value.GetInt64());
        }
        else if (value.IsUint64())
        {
            fields->AddUInt64(propertyName.Get(), value.GetUint64());
        }
        else
        {
            fields->AddDouble(propertyName.Get(), value.GetDouble());
        }
        break;

    case rapidjson::Type::kTrueType:
    case rapidjson::Type::kFalseType:
        fields->AddBoolean(propertyName.Get(), value.GetBool());
        break;

    case rapidjson::Type::kNullType:
        fields->AddEmpty(propertyName.Get());
        break;

    case rapidjson::Type::kStringType:
        fields->AddString(propertyName.Get(), HStringReference(utils::string_t_from_internal_string(value.GetString()).data()).Get());
        break;

    default:
        throw std::invalid_argument("Logging property type not supported");
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END

#endif