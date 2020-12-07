// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#include "string_service_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
VerifyStringResult::VerifyStringResult()
{

}

VerifyStringResult::VerifyStringResult(
    XblVerifyStringResultCode resultCode,
    xsapi_internal_string firstOffendingSubstring
) :
    m_resultCode(resultCode),
    m_firstOffendingSubstring(firstOffendingSubstring)
{
}

const XblVerifyStringResultCode VerifyStringResult::ResultCode() const
{
    return m_resultCode;
}

const xsapi_internal_string& VerifyStringResult::FirstOffendingSubstring() const
{
    return m_firstOffendingSubstring;
}

size_t VerifyStringResult::SizeOf() const
{
    size_t size = sizeof(XblVerifyStringResult);
    if (m_resultCode != XblVerifyStringResultCode::Success)
    {
        size += m_firstOffendingSubstring.length() + 1;
    }
    return size;
}

/*static*/ Result<xsapi_internal_vector<VerifyStringResult>> VerifyStringResult::DeserializeVerifyStringsResult(
    _In_ const JsonValue& json
)
{
    if (json.IsNull())
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    HRESULT errc = S_OK;
    xsapi_internal_vector<VerifyStringResult> resultVector;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<VerifyStringResult>(
        VerifyStringResult::DeserializeVerifyStringResult,
        json,
        "verifyStringResult",
        resultVector,
        true
        ));

    return Result<xsapi_internal_vector<VerifyStringResult>> {resultVector, errc};
}

/*static*/ Result<VerifyStringResult> VerifyStringResult::DeserializeVerifyStringResult(
    _In_ const JsonValue& json
)
{
    VerifyStringResult returnResult;
    if (json.IsNull())
    {
        return returnResult;
    }

	int32_t resultCode = 0;
    xsapi_internal_string offendingString;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(json, "resultCode", resultCode));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "offendingString", offendingString));
    returnResult = VerifyStringResult(
        static_cast<XblVerifyStringResultCode>(resultCode),
        offendingString
    );

    return Result<VerifyStringResult>(returnResult, S_OK);
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
