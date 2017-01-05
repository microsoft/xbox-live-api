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
#include "system_internal.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
#endif

class service_call_logger_protocol : public std::enable_shared_from_this<service_call_logger_protocol>
{
public:

    /// <summary>
    /// Gets the service_call_logger singleton instance
    /// </summary>
    static std::shared_ptr<service_call_logger_protocol> get_singleton_instance();

    /// <summary>
    /// Enables Logs to be enabled/disabled through protocal activation
    /// </summary>
    void register_for_protocol_activation();

private:
    void process_service_call_tracking_activation_uri(_In_ Windows::Foundation::Uri^ activationUri);
    void set_state_bread_crumb(_In_ bool isTracking);
    Windows::Foundation::EventRegistrationToken m_onActivatedToken;
    
    service_call_logger_protocol();
    service_call_logger_protocol(const service_call_logger_protocol&);
    void operator=(const service_call_logger_protocol&);
};

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
#endif
