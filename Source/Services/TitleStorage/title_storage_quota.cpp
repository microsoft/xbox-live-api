// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/title_storage.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_BEGIN

title_storage_quota::title_storage_quota() :
    m_storageType(title_storage_type::trusted_platform_storage),
    m_usedBytes(0),
    m_quotaBytes(0)
{
}

void
title_storage_quota::_Initialize(
    _In_ string_t serviceConfigurationId,
    _In_ title_storage_type storageType,
    _In_ string_t xboxUserId,
    _In_ string_t multiplayerSessionTemplateName,
    _In_ string_t multiplayerSessionName
    )
{
    m_serviceConfigurationId = std::move(serviceConfigurationId);
    m_storageType = storageType;
    m_xboxUserId = std::move(xboxUserId);
    m_multiplayerSessionTemplateName = std::move(multiplayerSessionTemplateName);
    m_multiplayerSessionName = std::move(multiplayerSessionName);
}

const string_t&
title_storage_quota::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

title_storage_type
title_storage_quota::storage_type() const
{
    return m_storageType;
}

const string_t&
title_storage_quota::xbox_user_id() const
{
    return m_xboxUserId;
}

const string_t&
title_storage_quota::multiplayer_session_template_name() const
{
    return m_multiplayerSessionTemplateName;
}

const string_t&
title_storage_quota::multiplayer_session_name() const
{
    return m_multiplayerSessionName;
}

uint64_t
title_storage_quota::used_bytes() const
{
    return m_usedBytes;
}

uint64_t
title_storage_quota::quota_bytes() const
{
    return m_quotaBytes;
}

xbox_live_result<title_storage_quota>
title_storage_quota::_Deserialize(
    _In_ const web::json::value& json
    )
{
    title_storage_quota returnObject;
    if (json.is_null()) return xbox_live_result<title_storage_quota>(returnObject);
    
    std::error_code errc = xbox_live_error_code::no_error;
    web::json::value quotaInfoJson = utils::extract_json_field(json, _T("quotaInfo"), errc, false);

    if (!quotaInfoJson.is_null())
    {
        returnObject.m_usedBytes = utils::extract_json_uint52(quotaInfoJson, "usedBytes", errc);
        returnObject.m_quotaBytes = utils::extract_json_uint52(quotaInfoJson, "quotaBytes", errc);
    }

    return xbox_live_result<title_storage_quota>(returnObject, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_END