// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "Logger\log.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class unittest_output: public log_output
{
public:
    unittest_output() : log_output(log_output_level_setting::use_logger_setting, log_level::off) {}

    void write(_In_ const std::string& msg) override;
    std::string format_log(_In_ const log_entry& entry) override;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
