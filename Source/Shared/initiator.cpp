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
#include "initiator.h"
#include "telemetry.h"
#include "Logger/Log.h"
#include "Logger/debug_output.h"
#include "Logger/custom_output.h"

#ifdef _WIN32
#include "Logger/etw_output.h"
#endif // _WIN32

#if UNIT_TEST_SERVICES
#include "unittest_output.h"
#endif

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
#endif

initiator::initiator()
{
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= _WIN32_WINNT_WIN10
    // Create telemetry provider
    TraceLoggingRegister(g_hTraceLoggingProvider);
#endif

    // Only enable logging on debug build by default, release version can 
    // turn it on at runtime
    logger::create_logger();
    
#if UNIT_TEST_SERVICES
    logger::get_logger()->set_log_level(log_level::debug);
    logger::get_logger()->add_log_output(std::make_shared<unittest_output>());

#else
    logger::get_logger()->set_log_level(log_level::error);
#if !XSAPI_U
    logger::get_logger()->add_log_output(std::make_shared<debug_output>());
#endif
#endif
    logger::get_logger()->add_log_output(std::make_shared<custom_output>());
#if !XSAPI_U
    logger::get_logger()->add_log_output(std::make_shared<etw_output>());
#endif
}

initiator::~initiator()
{
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= _WIN32_WINNT_WIN10
    // Create telemetry provider
    TraceLoggingUnregister(g_hTraceLoggingProvider);
#endif

    logger::release_logger();
}

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
#endif
