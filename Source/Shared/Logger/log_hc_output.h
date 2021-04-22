// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "log.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class log_hc_output : public log_output
{
public:
    log_hc_output() : log_output() {}

    void write(_In_ HCTraceLevel level, _In_ const xsapi_internal_string& msg) override;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
