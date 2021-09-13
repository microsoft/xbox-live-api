// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "log_hc_output.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

void log_hc_output::write(_In_ HCTraceLevel level, _In_ const String& msg)
{
    constexpr char escapedFormatSpecifier[]{ "%%" };

    String const* msgPtr{ &msg };
    String escapedMsg{};

    size_t index = msg.find('%');
    if (index != std::string::npos)
    {
        // Escape format string before passing to HC_TRACE
        escapedMsg = msg;

        while ((index = escapedMsg.find('%', index)) != std::string::npos)
        {
            escapedMsg.replace(index, 1, escapedFormatSpecifier);
            index += (sizeof(escapedFormatSpecifier) - 1);
        }

        msgPtr = &escapedMsg;
    }

    HC_TRACE_MESSAGE(XSAPI, level, msgPtr->data());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
