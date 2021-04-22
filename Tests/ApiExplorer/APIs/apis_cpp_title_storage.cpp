// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

#if CPP_TESTS_ENABLED
xbox::services::title_storage::title_storage_type ConvertStringToCppTitleStorageType(const char* str)
{
    xbox::services::title_storage::title_storage_type storageType = xbox::services::title_storage::title_storage_type::trusted_platform_storage;

    if (pal::stricmp(str, "title_storage_type::trusted_platform_storage") == 0) storageType = xbox::services::title_storage::title_storage_type::trusted_platform_storage;
    else if (pal::stricmp(str, "title_storage_type::global_storage") == 0) storageType = xbox::services::title_storage::title_storage_type::global_storage;
    else if (pal::stricmp(str, "title_storage_type::universal") == 0) storageType = xbox::services::title_storage::title_storage_type::universal;

    return storageType;
}

xbox::services::title_storage::title_storage_blob_type ConvertStringToCppTitleStorageBlobType(const char* str)
{
    xbox::services::title_storage::title_storage_blob_type blobType = xbox::services::title_storage::title_storage_blob_type::unknown;

    if (pal::stricmp(str, "title_storage_blob_type::binary") == 0) blobType = xbox::services::title_storage::title_storage_blob_type::binary;
    else if(pal::stricmp(str, "title_storage_blob_type::json") == 0) blobType = xbox::services::title_storage::title_storage_blob_type::json;
    else if (pal::stricmp(str, "title_storage_blob_type::config") == 0) blobType = xbox::services::title_storage::title_storage_blob_type::config;

    return blobType;
}

xbox::services::title_storage::title_storage_e_tag_match_condition ConvertStringToCppETagMatchCondition(const char* str)
{
    xbox::services::title_storage::title_storage_e_tag_match_condition matchCondition = xbox::services::title_storage::title_storage_e_tag_match_condition::not_used;

    if (pal::stricmp(str, "title_storage_e_tag_match_condition::if_match") == 0) matchCondition = xbox::services::title_storage::title_storage_e_tag_match_condition::if_match;
    else if (pal::stricmp(str, "title_storage_e_tag_match_condition::if_not_match") == 0) matchCondition = xbox::services::title_storage::title_storage_e_tag_match_condition::if_not_match;

    return matchCondition;
}
#endif

int TitleStorageServiceGetQuota_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    xbox::services::title_storage::title_storage_type storageType = ConvertStringToCppTitleStorageType(GetStringFromLua(L, 1, "title_storage_type::universal").c_str());

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->title_storage_service().get_quota(
        xbox::services::Utils::StringTFromUtf8(Data()->scid),
        storageType
    ).then(
        [](xbox::services::xbox_live_result < xbox::services::title_storage::title_storage_quota> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("TitleStorageServiceGetQuota: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                xbox::services::title_storage::title_storage_quota storageQuota = result.payload();
                LogToScreen("XblTitleStorageGetQuotaResult: usedBytes=%u quotaBytes=%u", storageQuota.used_bytes(), storageQuota.quota_bytes());
            }

            CallLuaFunctionWithHr(hr, "OnTitleStorageServiceGetQuota");
        });
#else
    LogToFile("TitleStorageServiceGetQuota is disabled for this platform.");
    CallLuaFunctionWithHr(S_OK, "OnTitleStorageServiceGetQuota");
#endif

    return LuaReturnHR(L, S_OK);
}

int TitleStorageServiceGetBlobMetadata_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    xbox::services::title_storage::title_storage_type storageType = ConvertStringToCppTitleStorageType(GetStringFromLua(L, 1, "title_storage_type::universal").c_str());
    string_t blobPath = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "").c_str());
    string_t xboxUserId = xbox::services::Utils::StringTFromUint64(GetUint64FromLua(L, 3, 0));
    uint32_t skipItems = GetUint32FromLua(L, 4, 0);
    uint32_t maxItems = GetUint32FromLua(L, 5, 0);

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->title_storage_service().get_blob_metadata(
        xbox::services::Utils::StringTFromUtf8(Data()->scid),
        storageType,
        blobPath,
        xboxUserId,
        skipItems,
        maxItems
    ).then(
        [xblc](xbox::services::xbox_live_result<xbox::services::title_storage::title_storage_blob_metadata_result> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("TitleStorageServiceGetBlobMetadata: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                xbox::services::title_storage::title_storage_blob_metadata_result metadataResult = result.payload();
                Data()->blobMetadataResultCpp = metadataResult;
                LogToScreen("TitleStorageServiceGetBlobMetadata result count: %d", metadataResult.items().size());
                Data()->blobMetadataCpp = metadataResult.items()[0];
            }

            CallLuaFunctionWithHr(hr, "OnTitleStorageServiceGetBlobMetadata");
        });
#else
    LogToFile("TitleStorageServiceGetBlobMetadata is disabled for this platform.");
    CallLuaFunctionWithHr(S_OK, "OnTitleStorageServiceGetBlobMetadata");
#endif
    return LuaReturnHR(L, S_OK);
}

int TitleStorageBlobMetadataResultHasNextCpp_Lua(lua_State* L)
{
    bool hasNext = false;
#if CPP_TESTS_ENABLED
    hasNext = Data()->blobMetadataResultCpp.has_next();
    LogToFile("TitleStorageBlobMetadataResultHasNextCpp: hasNext=%s", hasNext ? "true" : "false");
#else
    LogToFile("TitleStorageBlobMetadataResultHasNextCpp is disabled form this platform.");
#endif
    lua_pushnumber(L, (int)hasNext);
    return LuaReturnHR(L, S_OK, 1);
}

int TitleStorageBlobMetadataResultGetNextCpp_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    uint32_t maxItems = GetUint32FromLua(L, 1, 100);
    LogToFile("TitleStorageBlobMetadataResultGetNextCpp: MaxItems: %d", maxItems);

    Data()->blobMetadataResultCpp.get_next(maxItems).then(
        [](xbox::services::xbox_live_result<xbox::services::title_storage::title_storage_blob_metadata_result> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("TitleStorageBlobMetadataResultGetNextCpp: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                xbox::services::title_storage::title_storage_blob_metadata_result metadataResult = result.payload();
                Data()->blobMetadataResultCpp = metadataResult;
                LogToScreen("TitleStorageBlobMetadataResultGetNext count: %d", metadataResult.items().size());
            }

            CallLuaFunctionWithHr(hr, "OnTitleStorageBlobMetadataResultGetNextCpp");
        });
#else
    LogToFile("TitleStorageBlobMetadataResultGetNextCpp is disabled for this platform.");
    CallLuaFunctionWithHr(S_OK, "OnTitleStorageBlobMetadataResultGetNextCpp");
#endif

    return LuaReturnHR(L, S_OK);
}

int TitleStorageServiceDeleteBlob_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    bool deleteOnlyIfEtagMatches = GetBoolFromLua(L, 1, false);

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->title_storage_service().delete_blob(
        Data()->blobMetadataCpp,
        deleteOnlyIfEtagMatches
    ).then(
        [](xbox::services::xbox_live_result<void> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToScreen("TitleStorageServiceDeleteBlob: hr=%s", ConvertHR(hr).c_str());

            CallLuaFunctionWithHr(hr, "OnTitleStorageServiceDeleteBlob");
        });
#else
    LogToFile("TitleStorageServiceDeleteBlob is disabled for this platform.");
    CallLuaFunctionWithHr(S_OK, "OnTitleStorageServiceDeleteBlob");
#endif
    return LuaReturnHR(L, S_OK);
}

int TitleStorageServiceDownloadBlob_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    xbox::services::title_storage::title_storage_e_tag_match_condition eTagMatchCondition{ xbox::services::title_storage::title_storage_e_tag_match_condition::not_used };
    string_t selectQuery = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "").c_str());
    uint32_t preferredDownloadBlockSize = GetUint32FromLua(L, 2, 1024 * 256);

    std::shared_ptr<std::vector<unsigned char>> downloadBlobBuffer = std::make_shared<std::vector<unsigned char>>((unsigned int)(Data()->blobMetadataCpp.length()));

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->title_storage_service().download_blob(
        Data()->blobMetadataCpp,
        downloadBlobBuffer,
        eTagMatchCondition,
        selectQuery,
        preferredDownloadBlockSize
    ).then(
        [downloadBlobBuffer](xbox::services::xbox_live_result<xbox::services::title_storage::title_storage_blob_result> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToScreen("TitleStorageServiceDownloadBlob: hr=%s", ConvertHR(hr).c_str());

            CallLuaFunctionWithHr(hr, "OnTitleStorageServiceDownloadBlob");
        });
#else
    LogToFile("TitleStorageServiceDownloadBlob is disabled for this platform.");
    CallLuaFunctionWithHr(S_OK, "OnTitleStorageServiceDownloadBlob");
#endif
    return LuaReturnHR(L, S_OK);
}

int TitleStorageServiceUploadBlob_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    xbox::services::title_storage::title_storage_type storageType = ConvertStringToCppTitleStorageType(GetStringFromLua(L, 1, "title_storage_type::universal").c_str());
    string_t blobPath = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "apirunner/test/path.txt").c_str());
    xbox::services::title_storage::title_storage_blob_type blobType = ConvertStringToCppTitleStorageBlobType(GetStringFromLua(L, 3, "title_storage_blob_type::binary").c_str());
    string_t displayName = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 4, "Test Binary Data").c_str());
    string_t eTag = _T("TestETag");

    xbox::services::title_storage::title_storage_blob_metadata blobMetadata(
        xbox::services::Utils::StringTFromUtf8(Data()->scid),
        storageType,
        blobPath,
        blobType,
        xbox::services::Utils::StringTFromUint64(Data()->xboxUserId),
        displayName,
        eTag
    );

    std::shared_ptr<std::vector<unsigned char>> blobBuffer = std::make_shared<std::vector<unsigned char>>();

    xbox::services::title_storage::title_storage_e_tag_match_condition eTagMatchCondition = ConvertStringToCppETagMatchCondition(GetStringFromLua(L, 5, "title_storage_e_tag_match_condition::not_used").c_str());

    uint32_t preferredUploadBlockSize = 1024 * 256;
    size_t blobBufferSize = 1024 * 600; // requires 3 chunk uploads
    blobBuffer->resize(blobBufferSize);
    char zero = '0';
    for (size_t i = 0; i < blobBufferSize; i++)
    {
        (*blobBuffer)[i] = static_cast<unsigned char>(zero + i % 10);
    }

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->title_storage_service().upload_blob(
        blobMetadata,
        blobBuffer,
        eTagMatchCondition,
        preferredUploadBlockSize
    ).then(
        [blobBuffer](xbox::services::xbox_live_result<xbox::services::title_storage::title_storage_blob_metadata> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToScreen("TitleStorageServiceUploadBlob: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                Data()->blobMetadataCpp = result.payload();
            }

            CallLuaFunctionWithHr(hr, "OnTitleStorageServiceUploadBlob");
        });
#else
    LogToFile("TitleStorageServiceUploadBlob is disabled for this platform.");
    CallLuaFunctionWithHr(S_OK, "OnTitleStorageServiceUploadBlob");
#endif
    return LuaReturnHR(L, S_OK);
}

void SetupAPIs_CppTitleStorage()
{
    lua_register(Data()->L, "TitleStorageServiceGetQuota", TitleStorageServiceGetQuota_Lua);
    lua_register(Data()->L, "TitleStorageServiceGetBlobMetadata", TitleStorageServiceGetBlobMetadata_Lua);
    lua_register(Data()->L, "TitleStorageBlobMetadataResultHasNextCpp", TitleStorageBlobMetadataResultHasNextCpp_Lua);
    lua_register(Data()->L, "TitleStorageBlobMetadataResultGetNextCpp", TitleStorageBlobMetadataResultGetNextCpp_Lua);
    lua_register(Data()->L, "TitleStorageServiceDeleteBlob", TitleStorageServiceDeleteBlob_Lua);
    lua_register(Data()->L, "TitleStorageServiceDownloadBlob", TitleStorageServiceDownloadBlob_Lua);
    lua_register(Data()->L, "TitleStorageServiceUploadBlob", TitleStorageServiceUploadBlob_Lua);
}