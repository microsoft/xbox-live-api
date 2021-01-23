// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/privacy_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_BEGIN

class PermissionCheckResult : public XblPermissionCheckResult
{
public:
    PermissionCheckResult() noexcept = default;
    PermissionCheckResult(const PermissionCheckResult& other) noexcept;
    PermissionCheckResult(PermissionCheckResult&& other) noexcept;
    PermissionCheckResult& operator=(const PermissionCheckResult& other) noexcept;
    ~PermissionCheckResult() noexcept = default;

    static Result<PermissionCheckResult> Deserialize(
        _In_ const JsonValue& json,
        _In_ XblPermission requestedPermission
    ) noexcept;

    static Result<xsapi_internal_vector<PermissionCheckResult>> BatchDeserialize(
        _In_ const JsonValue& json,
        _In_ const xsapi_internal_vector<XblPermission>& permissionsRequested
    ) noexcept;

    size_t SizeOf() const noexcept;

private:
    xsapi_internal_vector<XblPermissionDenyReasonDetails> m_reasons;
};

class PrivacyService : public std::enable_shared_from_this<PrivacyService>
{
public:
    PrivacyService(
        _In_ User&& user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings
    ) noexcept;

    HRESULT GetAvoidList(
        _In_ AsyncContext<Result<xsapi_internal_vector<uint64_t>>> async
    ) const noexcept;

    HRESULT GetMuteList(
        _In_ AsyncContext<Result<xsapi_internal_vector<uint64_t>>> async
    ) const noexcept;

    HRESULT CheckPermission(
        _In_ XblPermission permission,
        _In_ uint64_t targetXuid,
        _In_ AsyncContext<Result<PermissionCheckResult>> async
    ) const noexcept;

    HRESULT CheckPermission(
        _In_ XblPermission permissionToCheck,
        _In_ XblAnonymousUserType userType,
        _In_ AsyncContext<Result<PermissionCheckResult>> async
    ) const noexcept;

    HRESULT BatchCheckPermission(
        _In_ xsapi_internal_vector<XblPermission> permissionsToCheck,
        _In_ const xsapi_internal_vector<uint64_t>& targetXuids,
        _In_ const xsapi_internal_vector<XblAnonymousUserType>& userTypes,
        _In_ AsyncContext<Result<xsapi_internal_vector<PermissionCheckResult>>> async
    ) const noexcept;

private:
    enum class PrivacyListType
    {
        Avoid,
        Mute
    };

    HRESULT GetUserList(
        _In_ PrivacyListType listType,
        _In_ AsyncContext<Result<xsapi_internal_vector<uint64_t>>> async
    ) const noexcept;

    static Result<xsapi_internal_vector<uint64_t>> DeserializeUserList(
        _In_ const JsonValue& json
    ) noexcept;

    HRESULT CheckPermission(
        _In_ XblPermission permissionToCheck,
        _In_ const String& targetQuery,
        _In_ AsyncContext<Result<PermissionCheckResult>> async
    ) const noexcept;

    User m_user;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_contextSettings;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_END