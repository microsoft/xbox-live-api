// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/title_storage.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_BEGIN

title_storage_blob_metadata_result::title_storage_blob_metadata_result()
{
}

bool
title_storage_blob_metadata_result::has_next() const
{
    return !m_continuationToken.empty();
}

const std::vector<title_storage_blob_metadata>& 
title_storage_blob_metadata_result::items() const
{
    return m_items;
}

pplx::task<xbox_live_result<title_storage_blob_metadata_result>>
title_storage_blob_metadata_result::get_next(
    _In_ uint32_t maxItems
    ) const
{
    if (m_continuationToken.empty())
    {
        return pplx::task_from_result(xbox_live_result<title_storage_blob_metadata_result>(xbox_live_error_code::runtime_error, "No more data availible"));
    }

    title_storage_service titleStorageService(
        m_userContext,
        m_xboxLiveContextSettings,
        m_appConfig
        );

    return titleStorageService.internal_get_blob_metadata(
        m_serviceConfigurationId,
        m_storageType,
        m_blobPath,
        m_xboxUserId,
        m_multiplayerSessionTemplateName,
        m_multiplayerSessionName,
        0,
        maxItems,
        m_continuationToken
        );
}

void
title_storage_blob_metadata_result::_Initialize(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
    _In_ string_t serviceConfigurationId,
    _In_ title_storage_type storageType,
    _In_ string_t blobPath,
    _In_ string_t xboxUserId,
    _In_ string_t multiplayerSessionTemplateName,
    _In_ string_t multiplayerSessionName
    )
{
    m_userContext = std::move(userContext);
    m_xboxLiveContextSettings = std::move(xboxLiveContextSettings);
    m_appConfig = std::move(appConfig);
    m_serviceConfigurationId = std::move(serviceConfigurationId);
    m_storageType = storageType;
    m_blobPath = std::move(blobPath);
    m_xboxUserId = std::move(xboxUserId);
    m_multiplayerSessionTemplateName = std::move(multiplayerSessionTemplateName);
    m_multiplayerSessionName = std::move(multiplayerSessionName);

    for (auto& item : m_items)
    {
        item._Initialize(
            m_xboxUserId,
            m_multiplayerSessionTemplateName,
            m_multiplayerSessionName,
            m_serviceConfigurationId,
            m_storageType
            );
    }
}

xbox_live_result<title_storage_blob_metadata_result>
title_storage_blob_metadata_result::_Deserialize(
    _In_ const web::json::value& json
    )
{
    title_storage_blob_metadata_result returnObject;

    if (json.is_null()) return xbox_live_result<title_storage_blob_metadata_result>(returnObject);

    std::error_code errc = xbox_live_error_code::no_error;

    web::json::value blobs = utils::extract_json_field(json, _T("blobs"), true);
    returnObject.m_items = utils::extract_json_vector<title_storage_blob_metadata>(
        title_storage_blob_metadata::_Deserialize,
        blobs,
        errc,
        true
        );

    web::json::value pagingInfoJson = utils::extract_json_field(json, _T("pagingInfo"), errc, false);

    if (!pagingInfoJson.is_null())
    {
        returnObject.m_continuationToken = utils::extract_json_string(
            pagingInfoJson,
            _T("continuationToken"),
            errc
            );
    }

    return xbox_live_result<title_storage_blob_metadata_result>(returnObject, errc);
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_END