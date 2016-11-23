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
#include "XboxServiceCallRoutedEventArgs_WinRT.h"
#include "utils.h"

using namespace Concurrency;
using namespace Platform;
using namespace std;
using namespace Windows::Foundation;

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

XboxServiceCallRoutedEventArgs::XboxServiceCallRoutedEventArgs(
    _In_ xbox::services::xbox_service_call_routed_event_args cppObj 
    ) : 
    m_cppObj(std::move(cppObj))
{
    m_requestBody = ref new HttpCallRequestMessage(m_cppObj.request_body());
    m_uri = ref new Windows::Foundation::Uri(ref new String(m_cppObj.uri().c_str()));

    //Convert milliseconds to 100-nano seconds of a TimeSpan
    m_ellapsedCallTime.Duration = m_cppObj.elapsed_call_time().count() / 10000;

    //a DateTime.UniversalTime value is identical to a FILETIME value although it can only 
    //represent dates up to about 29000 C.E. A negative value represents the number of intervals 
    //prior to January 1, 1601 and can represent dates back to about 27,400 B.C.E.
    uint64_t requestTime = 0;
    uint64_t responseTime = 0;
    xbox::services::utils::convert_timepoint_to_filetime(m_cppObj.request_time(), requestTime);
    xbox::services::utils::convert_timepoint_to_filetime(m_cppObj.response_time(), responseTime);

    m_requestTimeUTC.UniversalTime = requestTime;
    m_responseTimeUTC.UniversalTime = responseTime;
}

Windows::Foundation::Uri^
XboxServiceCallRoutedEventArgs::Url::get()
{
    return m_uri;
}

HttpCallRequestMessage^
XboxServiceCallRoutedEventArgs::RequestBody::get()
{
    return m_requestBody;
}

DateTime XboxServiceCallRoutedEventArgs::RequestTimeUTC::get()
{
    return m_requestTimeUTC;
}

DateTime XboxServiceCallRoutedEventArgs::ResponseTimeUTC::get()
{
    return m_responseTimeUTC;
}

TimeSpan XboxServiceCallRoutedEventArgs::ElapsedCallTime::get()
{
    return m_ellapsedCallTime;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_END
