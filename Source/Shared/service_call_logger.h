// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#pragma once
#include "system_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class service_call_logger
{
public:

    /// <summary>
    /// Gets the service_call_logger singleton instance
    /// </summary>
    static std::shared_ptr<service_call_logger> get_singleton_instance();

    /// <summary>
    /// Enables the tracking of service calls
    /// </summary>
    void enable();

    /// <summary>
    /// Disables the tracking of service calls
    /// </summary>
    void disable();

    /// <summary>
    /// Returns whether or not the loggger will accept log calls
    /// </summary>
    bool is_enabled();

    ~service_call_logger();

    void log(_In_ const xsapi_internal_string& item);

    xsapi_internal_string file_location();

private:

    void create_log_file();
    void add_data_to_file(_In_ const xsapi_internal_string& data);

    service_call_logger();
    service_call_logger(const service_call_logger&);
    void operator=(const service_call_logger&);

    std::ofstream m_fileStream;
    xsapi_internal_string m_fileLocation;
    bool m_isEnabled;
    bool m_firstWrite;

    xbox::services::system::xbox_live_mutex m_writeLock;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END