// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "system_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class service_call_logger_protocol : public std::enable_shared_from_this<service_call_logger_protocol>
{
public:

    /// <summary>
    /// Gets the service_call_logger singleton instance
    /// </summary>
    static std::shared_ptr<service_call_logger_protocol> get_singleton_instance();

    /// <summary>
    /// Enables Logs to be enabled/disabled through protocol activation
    /// </summary>
    void register_for_protocol_activation();

private:
#if UWP_API || TV_API || UNIT_TEST_SERVICES
    void process_service_call_tracking_activation_uri(_In_ Windows::Foundation::Uri^ activationUri);
    Windows::Foundation::EventRegistrationToken m_onActivatedToken;
#endif
    void set_state_bread_crumb(_In_ bool isTracking);
    
    service_call_logger_protocol();
    service_call_logger_protocol(const service_call_logger_protocol&);
    void operator=(const service_call_logger_protocol&);
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
