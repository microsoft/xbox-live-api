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