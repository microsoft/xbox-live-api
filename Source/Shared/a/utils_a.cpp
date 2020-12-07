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
#include "utils_a.h"
#include <android/log.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "XSAPI.Android", __VA_ARGS__))
#define LOGCAT_BUFFER_SIZE 746
void utils_a::log_output(
    _In_ const string_t& logMessage
    )
{
    std::vector<string_t> logMessageList;
    auto logMessageSize = logMessage.size();
    if (logMessageSize > LOGCAT_BUFFER_SIZE)
    {
        uint32_t arraySize = logMessageSize / LOGCAT_BUFFER_SIZE;
        for (uint32_t i = 0; i <= arraySize; ++i)
        {
            string_t logSplitMessage;
            logSplitMessage = logMessage.substr(i * LOGCAT_BUFFER_SIZE, LOGCAT_BUFFER_SIZE);
            logMessageList.push_back(logSplitMessage);
        }
    }
    else
    {
        logMessageList.push_back(logMessage);
    }

    for (const auto& entry : logMessageList)
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
        LOGI(entry.c_str());
#pragma clang diagnostic pop
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END