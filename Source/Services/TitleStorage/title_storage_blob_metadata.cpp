// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/title_storage.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_BEGIN

title_storage_blob_metadata::title_storage_blob_metadata() :
    m_storageType(title_storage_type::trusted_platform_storage),
    m_blobType(title_storage_blob_type::unknown),
    m_lengthBytes(0)
{
}

title_storage_blob_metadata::title_storage_blob_metadata(
    _In_ string_t serviceConfigurationId,
    _In_ title_storage_type storageType,
    _In_ string_t blobPath,
    _In_ title_storage_blob_type blobType,
    _In_ string_t xboxUserId,
    _In_ string_t multiplayerSessionTemplateName,
    _In_ string_t multiplayerSessionName,
    _In_ string_t displayName,
    _In_ string_t eTag
    ) :
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_storageType(storageType),
    m_blobPath(std::move(blobPath)),
    m_blobType(blobType),
    m_xboxUserId(std::move(xboxUserId)),
    m_multiplayerSessionTemplateName(std::move(multiplayerSessionTemplateName)),
    m_multiplayerSessionName(std::move(multiplayerSessionName)),
    m_displayName(std::move(displayName)),
    m_eTag(std::move(eTag))
{
    XSAPI_ASSERT(!m_serviceConfigurationId.empty());
    XSAPI_ASSERT(!m_blobPath.empty());
}

title_storage_blob_metadata::title_storage_blob_metadata(
    _In_ string_t serviceConfigurationId,
    _In_ title_storage_type storageType,
    _In_ string_t blobPath,
    _In_ title_storage_blob_type blobType,
    _In_ string_t xboxUserId
    ) :
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_storageType(storageType),
    m_blobPath(std::move(blobPath)),
    m_blobType(blobType),
    m_xboxUserId(std::move(xboxUserId)),
    m_lengthBytes(0)
{
    XSAPI_ASSERT(!m_serviceConfigurationId.empty());
    XSAPI_ASSERT(!m_blobPath.empty());
}

title_storage_blob_metadata::title_storage_blob_metadata(
    _In_ string_t serviceConfigurationId,
    _In_ title_storage_type storageType,
    _In_ string_t blobPath,
    _In_ title_storage_blob_type blobType,
    _In_ string_t xboxUserId,
    _In_ string_t displayName,
    _In_ string_t eTag
    ) :
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_storageType(storageType),
    m_blobPath(std::move(blobPath)),
    m_blobType(blobType),
    m_xboxUserId(std::move(xboxUserId)),
    m_displayName(std::move(displayName)),
    m_eTag(std::move(eTag))
{
    XSAPI_ASSERT(!m_serviceConfigurationId.empty());
    XSAPI_ASSERT(!m_blobPath.empty());
}

title_storage_blob_metadata::title_storage_blob_metadata(
    _In_ string_t serviceConfigurationId,
    _In_ title_storage_type storageType,
    _In_ string_t blobPath,
    _In_ title_storage_blob_type blobType,
    _In_ string_t xboxUserId,
    _In_ string_t displayName,
    _In_ string_t eTag,
    _In_ utility::datetime clientTimestamp
    ) :
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_storageType(storageType),
    m_blobPath(std::move(blobPath)),
    m_blobType(blobType),
    m_xboxUserId(std::move(xboxUserId)),
    m_displayName(std::move(displayName)),
    m_eTag(std::move(eTag)),
    m_clientTimestamp(std::move(clientTimestamp))
{
    XSAPI_ASSERT(!m_serviceConfigurationId.empty());
    XSAPI_ASSERT(!m_blobPath.empty());
}

void
title_storage_blob_metadata::_Initialize(
    _In_ string_t xboxUserId,
    _In_ string_t multiplayerSessionTemplateName,
    _In_ string_t multiplayerSessionName,
    _In_ string_t serviceConfigurationId,
    _In_ title_storage_type storageType
    )
{
    m_xboxUserId = std::move(xboxUserId);
    m_multiplayerSessionTemplateName = std::move(multiplayerSessionTemplateName);
    m_multiplayerSessionName = std::move(multiplayerSessionName);
    m_serviceConfigurationId = std::move(serviceConfigurationId);
    m_storageType = storageType;
}

title_storage_blob_metadata
title_storage_blob_metadata::create_title_storage_blob_metadata_for_session_storage(
    _In_ string_t serviceConfigurationId,
    _In_ string_t blobPath,
    _In_ title_storage_blob_type blobType,
    _In_ string_t multiplayerSessionTemplateName,
    _In_ string_t multiplayerSessionName,
    _In_ string_t displayName,
    _In_ string_t eTag
    )
{
    return title_storage_blob_metadata(
        std::move(serviceConfigurationId),
        title_storage_type::session_storage,
        std::move(blobPath),
        std::move(blobType),
        string_t(),
        std::move(multiplayerSessionTemplateName),
        std::move(multiplayerSessionName),
        std::move(displayName),
        std::move(eTag)
        );
}

const string_t&
title_storage_blob_metadata::blob_path() const
{
    return m_blobPath;
}

title_storage_blob_type
title_storage_blob_metadata::blob_type() const
{
    return m_blobType;
}

title_storage_type
title_storage_blob_metadata::storage_type() const
{
    return m_storageType;
}

const string_t&
title_storage_blob_metadata::display_name() const
{
    return m_displayName;
}

const string_t&
title_storage_blob_metadata::e_tag() const
{
    return m_eTag;
}

const utility::datetime&
title_storage_blob_metadata::client_timestamp() const
{
    return m_clientTimestamp;
}

void
title_storage_blob_metadata::set_client_timestamp(
    _In_ utility::datetime value
    )
{
    m_clientTimestamp = std::move(value);
}

uint64_t
title_storage_blob_metadata::length() const
{
    return m_lengthBytes;
}

const string_t&
title_storage_blob_metadata::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

const string_t&
title_storage_blob_metadata::xbox_user_id() const
{
    return m_xboxUserId;
}

const string_t&
title_storage_blob_metadata::multiplayer_session_template_name() const
{
    return m_multiplayerSessionTemplateName;
}

const string_t&
title_storage_blob_metadata::multiplayer_session_name() const
{
    return m_multiplayerSessionName;
}

void
title_storage_blob_metadata::_Set_e_tag_and_length(
    _In_ string_t etag,
    _In_ uint64_t length
    )
{
    m_eTag = std::move(etag);
    m_lengthBytes = length;
}

title_storage_blob_type
title_storage_blob_metadata::_Convert_string_to_title_storage_blob_type(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("binary")) == 0)
    {
        return title_storage_blob_type::binary;
    }
    else if (utils::str_icmp(value, _T("json")) == 0)
    {
        return title_storage_blob_type::json;
    }
    else if (utils::str_icmp(value, _T("config")) == 0)
    {
        return title_storage_blob_type::config;
    }

    return title_storage_blob_type::unknown;
}

const xbox_live_result<string_t>
title_storage_blob_metadata::_Convert_title_storage_blob_type_to_string(
    _In_ title_storage_blob_type blobType
    )
{
    switch (blobType)
    {
        case title_storage_blob_type::binary: return xbox_live_result<string_t>(_T("binary"));
        case title_storage_blob_type::json: return xbox_live_result<string_t>(_T("json"));
        case title_storage_blob_type::config: return xbox_live_result<string_t>(_T("config"));
        default: return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "Enum out of range");
    }
}

xbox_live_result<title_storage_blob_metadata>
title_storage_blob_metadata::_Deserialize(
    _In_ const web::json::value& json
    )
{
    title_storage_blob_metadata returnObject;
    if (json.is_null()) return xbox_live_result<title_storage_blob_metadata>(returnObject);

    std::error_code errc = xbox_live_error_code::no_error;

    returnObject.m_clientTimestamp = utils::extract_json_time(json, _T("clientFileTime"), errc);
    returnObject.m_displayName = utils::extract_json_string(json, _T("displayName"), errc);
    returnObject.m_eTag = utils::extract_json_string(json, _T("etag"), errc);
    returnObject.m_lengthBytes = utils::extract_json_uint52(json, "size", errc);

    string_t fileName = utils::extract_json_string(json, _T("fileName"), errc);
    if (!fileName.empty())
    {
        auto nPos = fileName.find(',');
        if (nPos == std::string::npos)
        {
            return xbox_live_result<title_storage_blob_metadata>(returnObject, xbox_live_error_code::json_error, "Blob metadata deserialization failed due to file type not found");
        }

        string_t smartBlobType = fileName.substr(nPos + 1);
        returnObject.m_blobType = _Convert_string_to_title_storage_blob_type(smartBlobType);
        fileName.resize(nPos);
        returnObject.m_blobPath = std::move(fileName);
        
    }

    return xbox_live_result<title_storage_blob_metadata>(returnObject, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_END