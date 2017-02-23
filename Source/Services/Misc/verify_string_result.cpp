// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/system.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

verify_string_result::verify_string_result()
{
}

verify_string_result::verify_string_result(
    verify_string_result_code resultCode,
    string_t firstOffendingSubstring
    ):
    m_resultCode(resultCode),
    m_firstOffendingSubstring(std::move(firstOffendingSubstring))
{
}

verify_string_result_code 
verify_string_result::result_code() const
{
    return m_resultCode;
}

const string_t&
verify_string_result::first_offending_substring() const
{
    return m_firstOffendingSubstring;
}

xbox_live_result<verify_string_result>
verify_string_result::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<verify_string_result>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto result = verify_string_result(
        static_cast<verify_string_result_code>(utils::extract_json_int(json, _T("resultCode"), errc)),
        utils::extract_json_string(json, _T("offendingString"), errc)
        );

    return xbox_live_result<verify_string_result>(result, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
