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
#include "unittest_output.h"
#if USING_TAEF
#include "WexTestClass.h"
#else 
#include <CppUnitTestLogger.h>
#endif


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

void unittest_output::write(_In_ const std::string& msg)
{
#if USING_TAEF
    WEX::Logging::Log::Comment(utility::conversions::utf8_to_utf16(msg).c_str());
#else 
    Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(msg.c_str());
#endif
}

std::string
unittest_output::format_log(_In_ const log_entry& entry)
{
    std::stringstream stream;

    // format : "[<thread id>] <level> <category> - <msg>"
    stream << " [" << std::this_thread::get_id() << "] ";
    stream << entry.level_to_string() << " " << entry.category() << " - ";
    stream << entry.msg_stream().str();

    return stream.str();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
