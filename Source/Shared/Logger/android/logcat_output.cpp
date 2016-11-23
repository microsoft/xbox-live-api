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
#include "Logger/android/logcat_output.h"
#include <android/log.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

void logcat_output::add_log(_In_ const log_entry& entry)
{
    std::string category = entry.category();
    log_level level = entry.get_log_level();
    std::string tag = (category.empty()) ? "XSAPI.Android" : category;

    switch (level)
    {
    case log_level::debug:
        __android_log_print(ANDROID_LOG_DEBUG, tag.c_str(), entry.msg_stream().str().c_str());
        break;
    case log_level::info:
        __android_log_print(ANDROID_LOG_INFO, tag.c_str(), entry.msg_stream().str().c_str());
        break;
    case log_level::warn:
        __android_log_print(ANDROID_LOG_WARN, tag.c_str(), entry.msg_stream().str().c_str());
        break;
    case log_level::error:
        __android_log_print(ANDROID_LOG_ERROR, tag.c_str(), entry.msg_stream().str().c_str());
        break;
    default:
        break;

    }

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
