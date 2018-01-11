// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "privacy_permission_deny_reason_impl.h"

XSAPI_PRIVACY_PERMISSION_DENY_REASON_IMPL::XSAPI_PRIVACY_PERMISSION_DENY_REASON_IMPL(
    const xbox::services::privacy::permission_deny_reason& cppObj,
    XSAPI_PRIVACY_PERMISSION_DENY_REASON* cObj
    )
{
    m_reason = utils_c::to_utf8string(cppObj.reason());
    m_restrictedSetting = utils_c::to_utf8string(cppObj.restricted_setting());

    cObj->reason = m_reason.data();
    cObj->restrictedSetting = m_restrictedSetting.data();
}
