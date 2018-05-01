// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#if !UNIT_TEST_SERVICES && UWP_API
#include "xsapi/events.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

#include "service_call_logger_data.h"
#include "service_call_logger.h"

#include <initguid.h>
#include <regex>
#include <debugapi.h>
#include "service_call_logger_data.h"
#include "service_call_logger.h"

using namespace Windows::Foundation::Diagnostics;
using namespace Platform;


DEFINE_GUID(XSAPI_TELEMETRY_GROUP,
    0x53b78fc6, 0xe359, 0x453e, 0x89, 0xfe, 0xa5, 0xf4, 0xe5, 0xff, 0x4a, 0xf3);

// {5E9EDC93-F04F-47EC-8DCB-0CF8F3442BDC}
DEFINE_GUID(GUID_LOGGING_CHANNEL,
    0x5e9edc93, 0xf04f, 0x47ec, 0x8d, 0xcb, 0xc, 0xf8, 0xf3, 0x44, 0x2b, 0xdc);

#define XBOX_LIVE_LOGGING_OPTIONS  0x0000800000000000
#define XBOX_LIVE_LOGGING_TAGS  0x00B00000

using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_BEGIN

// Events service
events_service::events_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
) :
    m_userContext(std::move(userContext)),
    m_appConfig(std::move(appConfig))
{
    m_playSession = utils::string_t_from_internal_string(utils::create_guid(true).c_str());
    m_loggingOptions = ref new LoggingOptions(XBOX_LIVE_LOGGING_OPTIONS);
    m_loggingOptions->Tags = XBOX_LIVE_LOGGING_TAGS;


    stringstream_t channelId;
    channelId << L"Microsoft.XboxLive.T" << m_appConfig->title_id();

    m_loggingChannel = ref new LoggingChannel(
        ref new String(channelId.str().c_str()),
        ref new LoggingChannelOptions(Guid(XSAPI_TELEMETRY_GROUP)),
        Guid(GUID_LOGGING_CHANNEL));

    m_appInsightsKey = load_app_insights_key();
}

xbox_live_result<void>
events_service::write_in_game_event(_In_ const string_t& eventName)
{
    return write_in_game_event(eventName, web::json::value::null(), web::json::value::null());
}

xbox_live_result<void>
events_service::write_in_game_event(
    _In_ const string_t& eventName,
    _In_ const web::json::value& dimensions,
    _In_ const web::json::value& measurements
    )
{
    try
    {
        if (!m_userContext->is_signed_in())
        {
            return xbox_live_result<void>(xbox_live_error_code::auth_user_not_signed_in, "User must be signed in to call this API");
        }

        // Check event name
        regex_t regex(_T("[A-Za-z]+[A-Za-z0-9_]*"));
        bool matchFound = std::regex_match(eventName, regex);
        if (!matchFound)
        {
            return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "Invalid event name");
        }

        //Log service call
        if (xbox::services::service_call_logger::get_singleton_instance()->is_enabled())
        {
            std::shared_ptr<service_call_logger> tracker = service_call_logger::get_singleton_instance();

            service_call_logger_data logData(
                m_userContext->xbox_user_id(),
                utils::internal_string_from_string_t(eventName),
                utils::internal_string_from_string_t(m_playSession),
                utils::internal_string_from_string_t(dimensions.serialize()),
                utils::internal_string_from_string_t(measurements.serialize()),
                chrono_clock_t::now()
            );

            tracker->log(logData.to_string());
        }

        auto fields = create_logging_field(eventName, dimensions, measurements);

        m_loggingChannel->LogEvent(ref new String(eventName.c_str()), fields, LoggingLevel::Critical, m_loggingOptions);
    }
    catch (const std::exception& e)
    {
        xbox_live_error_code err = utils::convert_exception_to_xbox_live_error_code();
        return xbox_live_result<void>(err, e.what());
    }
    catch (Platform::Exception^ e)
    {
        xbox_live_error_code errc = static_cast<xbox_live_error_code>(e->HResult);
        return xbox_live_result<void>(errc, utility::conversions::to_utf8string(e->Message->Data()));
    }

    return xbox_live_result<void>();
}

void events_service::add_common_logging_field(_In_ Windows::Foundation::Diagnostics::LoggingFields^ fields)
{
    fields->AddString("serviceConfigId", PLATFORM_STRING_FROM_STRING_T(m_appConfig->scid()));
    fields->AddString("playerSessionId", PLATFORM_STRING_FROM_STRING_T(m_playSession));
    fields->AddString("titleId", m_appConfig->title_id().ToString());
    fields->AddString("userId", PLATFORM_STRING_FROM_INTERNAL_STRING(m_userContext->xbox_user_id()));
    fields->AddUInt16("ver", 1);
}

Windows::Foundation::Diagnostics::LoggingFields^
events_service::create_logging_field(
    _In_ const string_t& eventName,
    _In_ const web::json::value& dimensions,
    _In_ const web::json::value& measurements
    )
{
    THROW_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(eventName);
    THROW_CPP_INVALIDARGUMENT_IF(!(dimensions.is_object() || dimensions.is_null()));
    THROW_CPP_INVALIDARGUMENT_IF(!(measurements.is_object() || measurements.is_null()));

    auto fields = ref new Windows::Foundation::Diagnostics::LoggingFields();
    if (!m_appInsightsKey.empty())
    {
        fields->AddString("PartA_iKey", "AIX-" + ref new String(m_appInsightsKey.c_str()));
    }

    fields->BeginStruct("PartB_Microsoft.XboxLive.InGame");
    fields->AddString("name", ref new String(eventName.c_str()));
    add_common_logging_field(fields);

    if (dimensions.is_object())
    {
        // Add properties
        fields->BeginStruct("properties");
        for (const auto& pair : dimensions.as_object())
        {
            add_value_pair(fields, pair);
        }
        fields->EndStruct();
    }

    if (measurements.is_object())
    {
        // Add measurements
        fields->BeginStruct("measurements");
        for (const auto& pair : measurements.as_object())
        {
            add_value_pair(fields, pair);
        }
        fields->EndStruct();
    }

    fields->EndStruct();

    return fields;
}

void
events_service::add_value_pair(
    _In_ LoggingFields^ fields, 
    _In_ const std::pair<utility::string_t, web::json::value>& pair
    )
{
    // check property name.
    const auto& name = pair.first;
    THROW_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(name);

    regex_t regex(_T("[A-Za-z]+[A-Za-z0-9]*"));
    bool matchFound = std::regex_match(name, regex);
    if (!matchFound)
    {
        throw std::invalid_argument("Invalid properties or measurements name");
    }

    String^ propertyName = ref new String(name.c_str());

    const auto& value = pair.second;
    switch (value.type())
    {
    case web::json::value::value_type::Number:
        // if value can fit into int64, add as int64
        if (value.as_number().is_int64())
        {
            fields->AddInt64(propertyName, value.as_number().to_int64());
        }
        else if (value.as_number().is_uint64())
        {
            fields->AddUInt64(propertyName, value.as_number().to_uint64());
        }
        else
        {
            fields->AddDouble(propertyName, value.as_number().to_double());
        }
        break;

    case web::json::value::value_type::Boolean:
        fields->AddBoolean(propertyName, value.as_bool());
        break;

    case web::json::value::value_type::Null:
        fields->AddEmpty(propertyName);
        break;

    case web::json::value::value_type::String:
        fields->AddString(propertyName, ref new String(value.as_string().c_str()));
        break;

    default:
        throw std::invalid_argument("Logging property type not supported");
    }
}

string_t events_service::load_app_insights_key()
{
    string_t result;
    try
    {
        auto localConfig = xbox_system_factory::get_factory()->create_local_config();

        // 1. Check if app insight is disabled.
        if (localConfig->get_value_from_config("SendXboxEventsToAppInsights", false, "1") != "0")
        {
            // 2. check if an alternate app insight key is provided
            result = utils::string_t_from_internal_string(localConfig->get_value_from_config("AlternateAppInsightsInstrumentationKey", false, xsapi_internal_string()));

            // 3. Load app insight key from ApplicationInsights.config
            if (result.empty())
            {
                Windows::ApplicationModel::Package^ package = Windows::ApplicationModel::Package::Current;
                Windows::Storage::StorageFolder^ installedLocation = package->InstalledLocation;
                string_t configPath = string_t(installedLocation->Path->Data()) + _T("\\ApplicationInsights.config");
                string_t fileData = utils::read_file_to_string(configPath);

                if(!fileData.empty())
                {
                    auto xml = ref new Windows::Data::Xml::Dom::XmlDocument();
                    xml->LoadXml(ref new String(fileData.c_str()));

                    auto node = xml->SelectSingleNodeNS("//ns:InstrumentationKey", "xmlns:ns='http://schemas.microsoft.com/ApplicationInsights/2013/Settings'");
                    if (node != nullptr)
                    {
                        result = node->InnerText->Data();
                    }
                }
            }
        }
    }
    catch (...)
    {
        LOG_ERROR("Xbox Services API couldn't load ApplicationInsights.config");
    }

    return result;
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END

#endif // !UNIT_TEST_SERVICES && UWP_API
