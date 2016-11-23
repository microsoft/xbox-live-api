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
