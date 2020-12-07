// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

#include "service_call_logger_protocol.h"
#include "service_call_logger.h"
#include "service_call_logger_data.h"

#if HC_PLATFORM == HC_PLATFORM_UWP || HC_PLATFORM == HC_PLATFORM_XDK 
#include <collection.h>

using namespace pplx;
using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

std::shared_ptr<service_call_logger_protocol> service_call_logger_protocol::get_singleton_instance()
{
    auto xsapiSingleton = xbox::services::get_xsapi_singleton();
    std::lock_guard<std::mutex> guard(xsapiSingleton->m_serviceLoggerProtocolSingletonLock);
    if (xsapiSingleton->m_serviceLoggerProtocolSingleton == nullptr)
    {
        xsapiSingleton->m_serviceLoggerProtocolSingleton = std::shared_ptr<service_call_logger_protocol>(new service_call_logger_protocol());
    }
    return xsapiSingleton->m_serviceLoggerProtocolSingleton;
}

service_call_logger_protocol::service_call_logger_protocol()
{
#if HC_PLATFORM == HC_PLATFORM_UWP || HC_PLATFORM == HC_PLATFORM_XDK
    m_onActivatedToken.Value = 0;
#endif
}

void service_call_logger_protocol::register_for_protocol_activation()
{
#if HC_PLATFORM == HC_PLATFORM_UWP || HC_PLATFORM == HC_PLATFORM_XDK
    if (m_onActivatedToken.Value != 0)
    {
        return;
    }
    try
    {
        if (CoreApplication::GetCurrentView() == nullptr)
        {
            return;
        }
    }
    catch(Platform::Exception^ ex)
    {
        LOG_ERROR("Exception on CoreApplication::GetCurrentView()!");
        return;
    }

    std::weak_ptr<service_call_logger_protocol> thisWeakPtr = shared_from_this();
    TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>^ activatedEvent = ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(
        [thisWeakPtr](CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
    {
        std::shared_ptr<service_call_logger_protocol> pThis(thisWeakPtr.lock());
        // Only handling protocol activation here

        if (pThis == nullptr || args->Kind != Activation::ActivationKind::Protocol)
        {
            return;
        }

        IProtocolActivatedEventArgs^ protocolArgs = dynamic_cast< IProtocolActivatedEventArgs^>(args);
        Windows::Foundation::Uri^ url = ref new Windows::Foundation::Uri(protocolArgs->Uri->RawUri);

        if (utils::str_icmp(url->Host->Data(), _T("serviceCallTracking")) == 0)
        {
            pThis->process_service_call_tracking_activation_uri(url);
        }
    });

    try
    {
        m_onActivatedToken = CoreApplication::GetCurrentView()->Activated += activatedEvent;
    }
    catch (Platform::Exception^ ex)
    {
        std::string exMsg("Exception on CoreApplication::GetCurrentView()->Activated");
        exMsg += xbox::services::convert::to_utf8string(ex->Message->Data());
        LOG_ERROR(exMsg);
        return;
    }
#endif
}

#if HC_PLATFORM == HC_PLATFORM_UWP || HC_PLATFORM == HC_PLATFORM_XDK
void service_call_logger_protocol::process_service_call_tracking_activation_uri(_In_ Windows::Foundation::Uri^ activationUri)
{
    auto serviceCallLogger{ service_call_logger::get_singleton_instance() };
    if (serviceCallLogger)
    {
        WwwFormUrlDecoder^ decoder = activationUri->QueryParsed;
        if (decoder->Size == 0)
        {
            return;
        }

        // Check activationUri for relevant query arguments
        for (auto entry : decoder)
        {
            if ((utils::str_icmp(entry->Name->Data(), _T("state")) == 0))
            {
                if (utils::str_icmp(entry->Value->Data(), _T("start")) == 0)
                {
                    serviceCallLogger->enable();
                    set_state_bread_crumb(true);
                }
                else if (utils::str_icmp(entry->Value->Data(), _T("stop")) == 0)
                {
                    serviceCallLogger->disable();
                    set_state_bread_crumb(false);
                }
            }
        }
    }
    else
    {
        LOGS_WARN << "Got service call logger protocol activiation but XSAPI was not initializated.";
    }
}
#endif

void service_call_logger_protocol::set_state_bread_crumb(_In_ bool isTracking)
{
    // IMPORTANT: xbTrace app depends on this filename, so please talk to xbTrace owner before
    // considering any changes to this name
#if HC_PLATFORM == HC_PLATFORM_XDK || XSAPI_UNIT_TESTS
    string_t filePath = _T("d:\\callHistoryJson.tmp");
#else
    Windows::Storage::ApplicationData^ currentAppData = Windows::Storage::ApplicationData::Current;
    const string_t fileDir = currentAppData->TemporaryFolder->Path->Data();
    wchar_t filePathTemp[MAX_PATH];
    swprintf_s(filePathTemp, _T("%s\\callHistoryJson.tmp"), fileDir.c_str());
    string_t filePath = filePathTemp;
#endif

    // Try delete the old file no matter what. So the new created file will have latest timestamp.
    DeleteFile(filePath.c_str());

    if (isTracking)
    {
        std::ofstream file;
        file.open(filePath, std::ios_base::app | std::ios_base::out);

        if (!file.is_open())
        {
            LOGS_ERROR << "WriteFile failed: " <<filePath;
            return;
        }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END