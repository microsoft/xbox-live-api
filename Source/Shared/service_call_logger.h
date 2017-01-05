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

    void log(_In_ const string_t& item);

    string_t file_location();

private:

    void create_log_file();
    void add_data_to_file(_In_ const string_t& data);

    service_call_logger();
    service_call_logger(const service_call_logger&);
    void operator=(const service_call_logger&);

    std::ofstream m_fileStream;
    string_t m_fileLocation;
    bool m_isEnabled;
    bool m_firstWrite;

#if BEAM_API
	xbox::services::beam::system::xbox_live_mutex m_writeLock;
#else
    xbox::services::system::xbox_live_mutex m_writeLock;
#endif
};

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
#endif