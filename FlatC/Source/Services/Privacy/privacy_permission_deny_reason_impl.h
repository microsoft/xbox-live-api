// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/privacy_c.h"

class XSAPI_PRIVACY_PERMISSION_DENY_REASON_IMPL
{
public:
    XSAPI_PRIVACY_PERMISSION_DENY_REASON_IMPL(
        const xbox::services::privacy::permission_deny_reason& cppObj, 
        XSAPI_PRIVACY_PERMISSION_DENY_REASON* cObj);

private:
    std::string m_reason;
    std::string m_restrictedSetting;
};