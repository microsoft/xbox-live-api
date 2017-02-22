// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/privacy.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_BEGIN

permission_check_result::permission_check_result() :
    m_isAllowed(false)
{
}

void permission_check_result::initialize(
    _In_ const string_t& permissionIdRequested
    )
{
    m_permissionRequested = permissionIdRequested;
}

bool
permission_check_result::is_allowed() const
{
    return m_isAllowed;
}

const string_t&
permission_check_result::permission_requested() const
{
    return m_permissionRequested;
}

const std::vector<permission_deny_reason>&
permission_check_result::deny_reasons() const
{
    return m_denyReasons;
}

xbox_live_result<permission_check_result>
permission_check_result::_Deserializer(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<permission_check_result>();

    permission_check_result result;

    std::error_code errc = xbox_live_error_code::no_error;
    result.m_isAllowed = utils::extract_json_bool(json, _T("isAllowed"), errc, true);
    result.m_denyReasons = utils::extract_json_vector<permission_deny_reason>(permission_deny_reason::_Deserializer, std::move(json), _T("reasons"), errc, false);

    return xbox_live_result<permission_check_result>(result, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_END