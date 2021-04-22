// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "privacy_service_internal.h"
#include "xbox_live_context_internal.h"

using namespace xbox::services;
using namespace xbox::services::legacy;
using namespace xbox::services::privacy;

constexpr auto XblPermissionName = EnumName<XblPermission, 1000, 1025>;

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_BEGIN

PrivacyService::PrivacyService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings
) noexcept :
    m_user{ std::move(user) },
    m_contextSettings{ contextSettings }
{
}

HRESULT PrivacyService::GetAvoidList(
    _In_ AsyncContext<Result<xsapi_internal_vector<uint64_t>>> async
) const noexcept
{
    return GetUserList(PrivacyListType::Avoid, async);
}

HRESULT PrivacyService::GetMuteList(
    _In_ AsyncContext<Result<xsapi_internal_vector<uint64_t>>> async
) const noexcept
{
    return GetUserList(PrivacyListType::Mute, async);
}

HRESULT PrivacyService::GetUserList(
    _In_ PrivacyListType listType,
    _In_ AsyncContext<Result<xsapi_internal_vector<uint64_t>>> async
) const noexcept
{
    xsapi_internal_stringstream path;
    path << "/users/xuid(" << m_user.Xuid() << ")/people/" << (listType == PrivacyListType::Mute ? "mute" : "avoid");

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_contextSettings,
        "GET",
        XblHttpCall::BuildUrl("privacy", path.str()),
        xbox_live_api::get_avoid_or_mute_list
    ));

    return httpCall->Perform({
        async.Queue().DeriveWorkerQueue(),
        [
            async
        ]
    (HttpResult httpResult)
        {
            HRESULT hr{ Failed(httpResult) ? httpResult.Hresult() : httpResult.Payload()->Result() };
            if (SUCCEEDED(hr))
            {
                return async.Complete(DeserializeUserList(httpResult.Payload()->GetResponseBodyJson()));
            }
            return async.Complete(hr);
        }
        });
}

HRESULT PrivacyService::CheckPermission(
    _In_ XblPermission permission,
    _In_ uint64_t targetXuid,
    _In_ AsyncContext<Result<PermissionCheckResult>> async
) const noexcept
{
    // users/xuid({xuid})/permission/validate?setting={setting}&target=xuid({targetXuid})
    Stringstream targetQuery;
    targetQuery << "xuid(" << targetXuid << ")";

    return CheckPermission(permission, targetQuery.str(), { async.Queue(),
        [
            targetXuid,
            async
        ]
    (Result<PermissionCheckResult> result)
        {
            result.Payload().targetXuid = targetXuid;
            async.Complete(result);
        }
        });
}

HRESULT PrivacyService::CheckPermission(
    _In_ XblPermission permission,
    _In_ XblAnonymousUserType userType,
    _In_ AsyncContext<Result<PermissionCheckResult>> async
) const noexcept
{
    return CheckPermission(permission, EnumName(userType), { async.Queue(),
        [
            userType,
            async
        ]
    (Result<PermissionCheckResult> result)
        {
            result.Payload().targetUserType = userType;
            async.Complete(result);
        }
        });
}

HRESULT PrivacyService::CheckPermission(
    _In_ XblPermission permission,
    _In_ const String& targetQuery,
    _In_ AsyncContext<Result<PermissionCheckResult>> async
) const noexcept
{
    // users/xuid({xuid})/permission/validate?setting={setting}&target={target})
    xbox::services::uri_builder subPathBuilder;
    xsapi_internal_stringstream path;
    path << "/users/xuid(" << m_user.Xuid() << ")/permission/validate";

    subPathBuilder.append_path(path.str());
    subPathBuilder.append_query("setting", XblPermissionName(permission).data());
    subPathBuilder.append_query("target", targetQuery);

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_contextSettings,
        "GET",
        XblHttpCall::BuildUrl("privacy", subPathBuilder.to_string()),
        xbox_live_api::check_permission_with_target_user
    ));

    return httpCall->Perform({
        async.Queue().DeriveWorkerQueue(),
        [
            async,
            permission
        ]
    (HttpResult httpResult)
        {
            HRESULT hr{ Failed(httpResult) ? httpResult.Hresult() : httpResult.Payload()->Result() };
            if (SUCCEEDED(hr))
            {
                return async.Complete(PermissionCheckResult::Deserialize(httpResult.Payload()->GetResponseBodyJson(), permission));
            }
            return async.Complete(hr);
        }
        });
}

HRESULT PrivacyService::BatchCheckPermission(
    _In_ xsapi_internal_vector<XblPermission> permissions,
    _In_ const xsapi_internal_vector<uint64_t>& targetXuids,
    _In_ const xsapi_internal_vector<XblAnonymousUserType>& userTypes,
    _In_ AsyncContext<Result<xsapi_internal_vector<PermissionCheckResult>>> async
) const noexcept
{
    // users/xuid({xuid})/permission/validate
    xsapi_internal_stringstream path;
    path << "/users/xuid(" << m_user.Xuid() << ")/permission/validate";

    // Set request body to something like:
    //{
    //    "users":
    //    [
    //        {"xuid":"12345"},
    //        {"anonymousUser":"crossNetworkUser"}
    //    ],
    //    "permissions":
    //    [
    //        "ViewTargetGameHistory",
    //        "ViewTargetProfile"
    //    ]
    //}

    JsonDocument requestBody(rapidjson::kObjectType);
    JsonDocument::AllocatorType& allocator = requestBody.GetAllocator();

    JsonValue usersJson(rapidjson::kArrayType);
    for (auto xuid : targetXuids)
    {
        JsonValue userJson(rapidjson::kObjectType);
        userJson.AddMember("xuid", JsonValue(utils::uint64_to_internal_string(xuid).c_str(), allocator).Move(), allocator);
        usersJson.PushBack(userJson, allocator);
    }
    for (auto userType : userTypes)
    {
        JsonValue userJson(rapidjson::kObjectType);
        userJson.AddMember("anonymousUser", JsonValue(EnumName(userType).data(), allocator).Move(), allocator);
        usersJson.PushBack(userJson, allocator);
    }

    requestBody.AddMember("users", usersJson, allocator);
    JsonValue permissionsJson(rapidjson::kArrayType);
    JsonUtils::SerializeVector<XblPermission>(
        [](XblPermission permission, JsonValue& outObj, JsonDocument::AllocatorType& allocator)
        {
            outObj.SetString(XblPermissionName(permission).data(), allocator);
        },
        permissions,
        permissionsJson,
        allocator
    );

    requestBody.AddMember("permissions", permissionsJson, allocator);

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_contextSettings,
        "POST",
        XblHttpCall::BuildUrl("privacy", path.str()),
        xbox_live_api::check_multiple_permissions_with_multiple_target_users
    ));

    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(requestBody));

    return httpCall->Perform({
        async.Queue().DeriveWorkerQueue(),
        [
            async,
            permissionsRequested{ std::move(permissions) }
        ]
    (HttpResult httpResult)
        {
            HRESULT hr{ Failed(httpResult) ? httpResult.Hresult() : httpResult.Payload()->Result() };
            if (SUCCEEDED(hr))
            {
                return async.Complete(PermissionCheckResult::BatchDeserialize(httpResult.Payload()->GetResponseBodyJson(), permissionsRequested));
            }
            return async.Complete(hr);
        }
        });
}

Result<xsapi_internal_vector<uint64_t>> PrivacyService::DeserializeUserList(
    _In_ const JsonValue& json
) noexcept
{
    HRESULT errc = S_OK;
    xsapi_internal_vector<uint64_t> xuids;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<uint64_t>(
        [&errc](_In_ const JsonValue& json)
        {
            if (json.IsNull())
            {
                return Result<uint64_t>(WEB_E_INVALID_JSON_STRING);
            }

            uint64_t xuid = 0;
            HRESULT tempErr = JsonUtils::ExtractJsonXuid(json, "xuid", xuid, true);
            if (FAILED(tempErr))
            {
                errc = tempErr;
            }

            return Result<uint64_t>(xuid, errc);
        },
        json, ("users"), xuids, true
        ));

    if (FAILED(errc))
    {
        return WEB_E_INVALID_JSON_STRING;
    }
    
    return xuids;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_END
