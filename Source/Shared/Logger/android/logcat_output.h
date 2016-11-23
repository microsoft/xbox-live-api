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
#include "Logger/log.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class logcat_output : public log_output
{
public:
    logcat_output() : log_output(log_output_level_setting::use_own_setting, log_level::debug) {}
    void add_log(_In_ const log_entry& entry) override;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
