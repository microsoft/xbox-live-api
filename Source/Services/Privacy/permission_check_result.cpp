// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "privacy_service_internal.h"

constexpr auto XblPrivilegeValue = EnumValue<XblPrivilege, 205, 255>;

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_BEGIN

PermissionCheckResult::PermissionCheckResult(
    const PermissionCheckResult& other
) noexcept
{
    *this = other;
}

PermissionCheckResult::PermissionCheckResult(
    PermissionCheckResult&& other
) noexcept :
    m_reasons{ std::move(other.m_reasons) }
{
    isAllowed = other.isAllowed;
    targetXuid = other.targetXuid;
    targetUserType = other.targetUserType;
    permissionRequested = other.permissionRequested;
    reasons = m_reasons.data();
    reasonsCount = m_reasons.size();
}

PermissionCheckResult& PermissionCheckResult::operator=(
    const PermissionCheckResult& other
) noexcept
{
    m_reasons = other.m_reasons;
    isAllowed = other.isAllowed;
    targetXuid = other.targetXuid;
    targetUserType = other.targetUserType;
    permissionRequested = other.permissionRequested;
    reasons = m_reasons.data();
    reasonsCount = m_reasons.size();

    return *this;
}

Result<PermissionCheckResult> PermissionCheckResult::Deserialize(
    _In_ const JsonValue& json,
    _In_ XblPermission permissionRequested
) noexcept
{
    if (json.IsNull())
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    PermissionCheckResult result{};
    HRESULT errc = S_OK;

    result.permissionRequested = permissionRequested;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(json, "isAllowed", result.isAllowed, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<XblPermissionDenyReasonDetails>(
        [&errc](_In_ const JsonValue& json)
        {
            XblPermissionDenyReasonDetails result{};
            String reasonString;
            errc = JsonUtils::ExtractJsonString(json, "reason", reasonString, true);
            result.reason = EnumValue<XblPermissionDenyReason>(reasonString.data());

            if (SUCCEEDED(errc))
            {
                String restrictedSetting;
                errc = JsonUtils::ExtractJsonString(json, "restrictedSetting", restrictedSetting, false);
                switch (result.reason)
                {
                case XblPermissionDenyReason::MissingPrivilege: // intentional fallthrough
                case XblPermissionDenyReason::PrivilegeRestrictsTarget:
                {
                    result.restrictedPrivilege = XblPrivilegeValue(restrictedSetting.data());
                    break;
                }
                case XblPermissionDenyReason::PrivacySettingRestrictsTarget:
                {
                    result.restrictedPrivacySetting = EnumValue<XblPrivacySetting>(restrictedSetting.data());
                    break;
                }
                default: break;
                }
            }

            return Result<XblPermissionDenyReasonDetails>(result, errc);
        },
        json, "reasons", result.m_reasons, false
    ));

    result.reasons = result.m_reasons.data();
    result.reasonsCount = result.m_reasons.size();

    if (FAILED(errc))
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    return result;
}

Result<xsapi_internal_vector<PermissionCheckResult>> PermissionCheckResult::BatchDeserialize(
    _In_ const JsonValue& json,
    _In_ const xsapi_internal_vector<XblPermission>& permissionsRequested
) noexcept
{
    if (json.IsNull())
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    xsapi_internal_vector<PermissionCheckResult> result;

    if (json.IsObject() && json.HasMember("responses"))
    {
        const JsonValue& responsesJsonArray = json["responses"];
        if (responsesJsonArray.IsArray())
        {
            for (const JsonValue& userJson : responsesJsonArray.GetArray())
            {
                if (userJson.IsObject() && userJson.HasMember("user"))
                {
                    const JsonValue& userObj = userJson["user"];
                    uint64_t xuid = 0;
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonXuid(userObj, "xuid", xuid, false));
                    String anonymousUserTypeString;
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(userObj, "anonymousUser", anonymousUserTypeString, false));
                    auto userType = EnumValue<XblAnonymousUserType>(anonymousUserTypeString.data());

                    xsapi_internal_vector<PermissionCheckResult> userResults;
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<PermissionCheckResult>(
                        [&xuid, &userType](const JsonValue& json)
                        {
                            auto result { Deserialize(json, XblPermission::Unknown) };
                            auto& payload { result.Payload() };
                            payload.targetXuid = xuid;
                            payload.targetUserType = userType;
                            return Result<PermissionCheckResult>(payload, result.Hresult());
                        },
                        userJson,
                        "permissions",
                        userResults,
                        true
                        ));

                    if (userResults.size() != permissionsRequested.size())
                    {
                        LOG_DEBUG("The resulting number of items did not match the number of items requested!");
                        return WEB_E_INVALID_JSON_STRING;
                    }
                    for (size_t i = 0; i < userResults.size(); ++i)
                    {
                        userResults[i].permissionRequested = permissionsRequested[i];
                    }

                    result.insert(result.end(), userResults.begin(), userResults.end());
                }
                else
                {
                    //required
                    return WEB_E_INVALID_JSON_STRING;
                }
            }
        }
    }
    return result;
}

size_t PermissionCheckResult::SizeOf() const noexcept
{
    return sizeof(XblPermissionCheckResult) + m_reasons.size() * sizeof(XblPermissionDenyReasonDetails);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_END
