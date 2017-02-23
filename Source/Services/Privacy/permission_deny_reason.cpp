// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/privacy.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_BEGIN

permission_deny_reason::permission_deny_reason()
{
}

const string_t&
permission_deny_reason::reason() const
{
    return m_reason;
}

const string_t&
permission_deny_reason::restricted_setting() const
{
    return m_restrictedSetting;
}

xbox_live_result<permission_deny_reason>
permission_deny_reason::_Deserializer(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<permission_deny_reason>();

    permission_deny_reason result;

    std::error_code errc = xbox_live_error_code::no_error;
    result.m_reason = utils::extract_json_string(json, _T("reason"), errc, true);
    result.m_restrictedSetting = utils::extract_json_string(json, _T("restrictedSetting"), errc, false);

    return xbox_live_result<permission_deny_reason>(result, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_END