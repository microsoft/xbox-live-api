// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include <stdio.h>
#include "console_output.h"


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

void console_output::write(_In_ const std::string& msg)
{
    std::cout << msg;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
