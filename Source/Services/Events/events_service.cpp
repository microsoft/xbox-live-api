// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#if !UNIT_TEST_SERVICES
#include "xsapi/events.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"
#if XSAPI_A
#include "a/user_impl_a.h"
#include "a/java_interop.h"
#elif XSAPI_I
#include "xbox_cll.h"
#endif

#if UWP_API
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
#endif

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
    m_playSession = utils::create_guid(true).c_str();
#if UWP_API
    m_loggingOptions = ref new LoggingOptions(XBOX_LIVE_LOGGING_OPTIONS);
    m_loggingOptions->Tags = XBOX_LIVE_LOGGING_TAGS;


    stringstream_t channelId;
    channelId << L"Microsoft.XboxLive.T" << m_appConfig->title_id();

    m_loggingChannel = ref new LoggingChannel(
        ref new String(channelId.str().c_str()),
        ref new LoggingChannelOptions(Guid(XSAPI_TELEMETRY_GROUP)),
        Guid(GUID_LOGGING_CHANNEL));

    m_appInsightsKey = load_app_insights_key();
#endif
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
#if XSAPI_CPP
        if (!m_userContext->user() || !m_userContext->user()->is_signed_in())
#else
        if (!m_userContext->user() || !m_userContext->user()->IsSignedIn)
#endif
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

#ifdef _WIN32
        //Log service call
        if (xbox::services::service_call_logger::get_singleton_instance()->is_enabled())
        {
            std::shared_ptr<service_call_logger> tracker = service_call_logger::get_singleton_instance();

            service_call_logger_data logData(
                m_userContext->xbox_user_id(),
                eventName,
                m_playSession,
                dimensions.serialize(),
                measurements.serialize(),
                chrono_clock_t::now()
            );

            tracker->log(logData.to_string());
        }
#endif
#if UWP_API

        auto fields = create_logging_field(eventName, dimensions, measurements);

        m_loggingChannel->LogEvent(ref new String(eventName.c_str()), fields, LoggingLevel::Critical, m_loggingOptions);
#elif XSAPI_U
        stringstream_t ss;
        ss << m_appConfig->title_id();
        web::json::value eventData;
        eventData[_T("baseType")] = web::json::value::string(_T("Microsoft.XboxLive.InGame"));

        web::json::value baseData;
        baseData[_T("name")] = web::json::value::string(eventName.c_str());
        baseData[_T("serviceConfigId")] = web::json::value::string(m_appConfig->scid().c_str());
        baseData[_T("playerSessionId")] = web::json::value::string(m_playSession.c_str());
        baseData[_T("titleId")] = web::json::value::string(ss.str());
        baseData[_T("userId")] = web::json::value::string(m_userContext->xbox_user_id().c_str());
        baseData[_T("ver")] = web::json::value::number(1);
        baseData[_T("properties")] = dimensions;
        baseData[_T("measurements")] = measurements;

        eventData[_T("baseData")] = baseData;

        stringstream_t eventNameStream;
        eventNameStream << "Microsoft.XboxLive.T" << ss.str() << "." << eventName;
#if XSAPI_A
        std::shared_ptr<java_interop> interop = java_interop::get_java_interop_singleton();
        if (interop)
        {
            interop->log_cll(m_userContext->xbox_user_id(), eventNameStream.str(), eventData.serialize());
        }
#elif XSAPI_I
        std::shared_ptr<xbox_cll> cll = xbox_cll::get_xbox_cll_singleton();
        std::vector<string_t> ids = { m_userContext->xbox_user_id() };
        iOSCll* iCll = static_cast<iOSCll*>(cll->raw_cll().get());
        if (iCll)
        {
            iCll->log(eventNameStream.str(),
                eventData.serialize(),
                cll::Latency::LatencyRealtime,
                cll::Persistence::PersistenceCritical,
                cll::Sensitivity::SensitivityNone,
                cll::SampleRate_NoSampling,
                ids);
        }
#endif
#endif

    }
    catch (const std::exception& e)
    {
        xbox_live_error_code err = utils::convert_exception_to_xbox_live_error_code();
        return xbox_live_result<void>(err, e.what());
    }
#if UWP_API
    catch (Platform::Exception^ e)
    {
        xbox_live_error_code errc = static_cast<xbox_live_error_code>(e->HResult);
        return xbox_live_result<void>(errc, utility::conversions::to_utf8string(e->Message->Data()));
    }
#endif
    return xbox_live_result<void>();
}

#if UWP_API
void events_service::add_common_logging_field(_In_ Windows::Foundation::Diagnostics::LoggingFields^ fields)
{
    fields->AddString("serviceConfigId", ref new String(m_appConfig->scid().c_str()));
    fields->AddString("playerSessionId", ref new String(m_playSession.c_str()));
    fields->AddString("titleId", m_appConfig->title_id().ToString());
    fields->AddString("userId", ref new String(m_userContext->xbox_user_id().c_str()));
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
        if (localConfig->get_value_from_config(_T("SendXboxEventsToAppInsights"), false, _T("1")) != _T("0"))
        {
            // 2. check if an alternate app insight key is provided
            result = localConfig->get_value_from_config(_T("AlternateAppInsightsInstrumentationKey"), false, string_t());

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
#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END
#endif