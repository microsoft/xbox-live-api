//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "xsapi/title_storage.h"
#include "TitleStorageType_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_BEGIN

/// <summary>
/// Returns the amount of storage space allocated and used.
/// </summary>
public ref class TitleStorageQuota sealed
{
public:
    /// <summary>
    /// The service configuration ID associated with the quota.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ServiceConfigurationId, service_configuration_id);

    /// <summary>
    /// The type of storage counted in the quota.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(StorageType, storage_type, TitleStorageType);

    /// <summary>
    /// The Xbox User ID associated with the quota if StorageType is TrustedPlatformStorage or JsonStorage, otherwise null.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// The multiplayer session template name associated with the quota if StorageType is SessionStorage, otherwise null.
    /// </summary>
#if _MSC_VER >= 1800
    [Windows::Foundation::Metadata::Deprecated("Title Storage for sessions has been deprecated. Use universal instead.", Windows::Foundation::Metadata::DeprecationType::Deprecate, 0x0)]
#endif
    DEFINE_PROP_GET_STR_OBJ(MultiplayerSessionTemplateName, multiplayer_session_template_name);

    /// <summary>
    /// The multiplayer session name associated with the quota if StorageType is SessionStorage, otherwise null.
    /// </summary>
#if _MSC_VER >= 1800
    [Windows::Foundation::Metadata::Deprecated("Title Storage for sessions has been deprecated. Use universal instead.", Windows::Foundation::Metadata::DeprecationType::Deprecate, 0x0)]
#endif
    DEFINE_PROP_GET_STR_OBJ(MultiplayerSessionName, multiplayer_session_name);

    /// <summary>
    /// The number of bytes used in title storage of type StorageType.
    /// </summary>
    DEFINE_PROP_GET_OBJ(UsedBytes, used_bytes, uint64);

    /// <summary>
    /// The maximum number of bytes that can be used in title storage of type StorageType.
    /// Note that this is a soft limit and the used bytes can actually exceed this value.
    /// </summary>
    DEFINE_PROP_GET_OBJ(QuotaBytes, quota_bytes, uint64);

internal:
    TitleStorageQuota(_In_ xbox::services::title_storage::title_storage_quota cppObj);

private:
    xbox::services::title_storage::title_storage_quota m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_END