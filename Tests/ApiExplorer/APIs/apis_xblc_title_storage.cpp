// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#include "rapidjson/document.h"

#define CHECKHR(hr) if (FAILED(hr)) goto Cleanup;
// <summary>
/// Metadata about a blob.
/// </summary>
typedef struct JsonStorageBlobMetadata
{
    /// <summary>
    /// Blob path is a unique string that conforms to a SubPath\file format (example: "foo\bar\blob.txt").
    /// </summary>
    std::string blobPath;

    /// <summary>
    /// [optional] Friendly display name to show in app UI.
    /// </summary>
    std::string displayName;

    /// <summary>
    /// Gets the number of bytes of the blob data.
    /// </summary>
    size_t length;

    /// <summary>
    /// Gets the position in the list of blob metadata
    /// </summary>
    size_t positionInList;

} JsonStorageBlobMetadata;


struct JsonBlobMetadataResult : public std::enable_shared_from_this<JsonBlobMetadataResult>
{
    std::string m_scid;
    uint64_t m_xuid;
    std::string m_blobPath;

    std::vector<JsonStorageBlobMetadata> m_items;
    std::string m_continuationToken;
};

JsonBlobMetadataResult DeserializeResult(std::string blobPathRoot, std::string json)
{
    rapidjson::Document document;
    document.Parse(json.c_str());

    JsonBlobMetadataResult result;

    // Deserialize blobs
    std::vector<JsonStorageBlobMetadata> metadataList;

    size_t index = 0;
    for (const auto& blobJson : document["blobs"].GetArray())
    {
        JsonStorageBlobMetadata metadata;
        metadata.blobPath = blobPathRoot + blobJson["fileName"].GetString();
        if (blobJson.HasMember("displayName"))
        {
            metadata.displayName = blobJson["displayName"].GetString();
        }
        metadata.length = static_cast<size_t>(blobJson["size"].GetInt());
        metadata.positionInList = index;
        index++;
        result.m_items.push_back(metadata);
    }
    
    return result;
}

XblTitleStorageType ConvertStringToXblTitleStorageType(const char* str)
{
    XblTitleStorageType type = XblTitleStorageType::TrustedPlatformStorage;

    if (pal::stricmp(str, "XblTitleStorageType::TrustedPlatformStorage") == 0) type = XblTitleStorageType::TrustedPlatformStorage;
    else if (pal::stricmp(str, "XblTitleStorageType::GlobalStorage") == 0) type = XblTitleStorageType::GlobalStorage;
    else if (pal::stricmp(str, "XblTitleStorageType::Universal") == 0) type = XblTitleStorageType::Universal;

    return type;
}

XblTitleStorageBlobType ConvertStringToXblTitleStorageBlobType(const char* str)
{
    XblTitleStorageBlobType type = XblTitleStorageBlobType::Unknown;

    if (pal::stricmp(str, "XblTitleStorageBlobType::Binary") == 0) type = XblTitleStorageBlobType::Binary;
    else if (pal::stricmp(str, "XblTitleStorageBlobType::Json") == 0) type = XblTitleStorageBlobType::Json;
    else if (pal::stricmp(str, "XblTitleStorageBlobType::Config") == 0) type = XblTitleStorageBlobType::Config;

    return type;
}

XblTitleStorageETagMatchCondition ConvertStringToXblTitleStorageETagMatchCondition(const char* str)
{
    XblTitleStorageETagMatchCondition matchCondition = XblTitleStorageETagMatchCondition::NotUsed;

    if (pal::stricmp(str, "XblTitleStorageETagMatchCondition::IfMatch") == 0) matchCondition = XblTitleStorageETagMatchCondition::IfMatch;
    else if (pal::stricmp(str, "XblTitleStorageETagMatchCondition::IfNotMatch") == 0) matchCondition = XblTitleStorageETagMatchCondition::IfNotMatch;

    return matchCondition;
}

int XblTitleStorageBlobMetadataResultGetItems_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblTitleStorageBlobMetadataResultGetItems
    XblTitleStorageBlobMetadataResultHandle handle = Data()->blobMetadataResultHandle;

    const XblTitleStorageBlobMetadata* items;
    size_t itemsSize;

    HRESULT hr = XblTitleStorageBlobMetadataResultGetItems(handle, &items, &itemsSize);
    // CODE SNIPPET END
    LogToScreen("XblTitleStorageBlobMetadataResultGetItems: hr=%s", ConvertHR(hr).c_str());

    if (itemsSize > 0)
    {
        Data()->binaryBlobMetadata = items[0];
        Data()->jsonBlobMetadata = items[1];
    }

    return LuaReturnHR(L, hr);
}

int XblTitleStorageBlobMetadataResultHasNext_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblTitleStorageBlobMetadataResultHasNext
    XblTitleStorageBlobMetadataResultHandle handle = Data()->blobMetadataResultHandle;

    bool hasNext;
    HRESULT hr = XblTitleStorageBlobMetadataResultHasNext(handle, &hasNext);
    // CODE SNIPPET END
    LogToScreen("XblTitleStorageBlobMetadataResultHasNext: hr=%s hasNext=%s", ConvertHR(hr).c_str(), hasNext ? "true" : "false");
    
    return LuaReturnHR(L, hr);
}

int XblTitleStorageBlobMetadataResultGetNextAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    
    uint32_t maxItems = GetUint32FromLua(L, 1, 0);

    // CODE SNIPPET START: XblTitleStorageBlobMetadataResultGetNextAsync
    XblTitleStorageBlobMetadataResultHandle handle = Data()->blobMetadataResultHandle;

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        XblTitleStorageBlobMetadataResultHandle handle{ nullptr };
        
        HRESULT hr = XblTitleStorageBlobMetadataResultGetNextResult(asyncBlock, &handle);
        if (Data()->blobMetadataResultHandle != nullptr) // CODE SNIP SKIP
        { // CODE SNIP SKIP
            XblTitleStorageBlobMetadataResultCloseHandle(Data()->blobMetadataResultHandle); // CODE SNIP SKIP
        } // CODE SNIP SKIP
        Data()->blobMetadataResultHandle = handle; // CODE SNIP SKIP
        LogToScreen("XblTitleStorageBlobMetadataResultGetNextResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP

        CallLuaFunctionWithHr(hr, "OnXblTitleStorageBlobMetadataResultGetNextAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblTitleStorageBlobMetadataResultGetNextAsync(handle, maxItems, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToScreen("XblTitleStorageBlobMetadataResultGetNextAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblTitleStorageBlobMetadataResultDuplicateHandle_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblTitleStorageBlobMetadataResultDuplicateHandle
    XblTitleStorageBlobMetadataResultHandle handle = Data()->blobMetadataResultHandle;
    XblTitleStorageBlobMetadataResultHandle duplicatedHandle;

    HRESULT hr = XblTitleStorageBlobMetadataResultDuplicateHandle(handle, &duplicatedHandle);
    // CODE SNIPPET END
    LogToScreen("XblTitleStorageBlobMetadataResultDuplicateHandle: hr=%s", ConvertHR(hr).c_str());

    // Cleanup
    XblTitleStorageBlobMetadataResultCloseHandle(duplicatedHandle);

    return LuaReturnHR(L, hr);
}

int XblTitleStorageBlobMetadataResultCloseHandle_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblTitleStorageBlobMetadataResultCloseHandle
    XblTitleStorageBlobMetadataResultHandle handle = Data()->blobMetadataResultHandle;
    XblTitleStorageBlobMetadataResultCloseHandle(handle);
    Data()->blobMetadataResultHandle = nullptr;
    // CODE SNIPPET END
    LogToScreen("XblTitleStorageBlobMetadataResultCloseHandle done");

    return LuaReturnHR(L, S_OK);
}

int XblTitleStorageGetQuotaAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    XblTitleStorageType storageType = ConvertStringToXblTitleStorageType(GetStringFromLua(L, 1, "XblTitleStorageType::Universal").c_str());

    // CODE SNIPPET START: XblTitleStorageGetQuotaAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t usedBytes;
        size_t quotaBytes;
        HRESULT hr = XblTitleStorageGetQuotaResult(asyncBlock, &usedBytes, &quotaBytes);
        LogToScreen("XblTitleStorageGetQuotaResult: usedBytes=%u quotaBytes=%u", usedBytes, quotaBytes); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblTitleStorageGetQuotaAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblTitleStorageGetQuotaAsync(
        Data()->xboxLiveContext,
        Data()->scid,
        storageType,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToScreen("XblTitleStorageGetQuotaAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblTitleStorageGetBlobMetadataAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    XblTitleStorageType storageType = ConvertStringToXblTitleStorageType(GetStringFromLua(L, 1, "XblTitleStorageType::Universal").c_str());
    std::string blobPath = GetStringFromLua(L, 2, "");
    uint64_t xboxUserId = GetUint64FromLua(L, 3, 0);
    uint32_t skipItems = GetUint32FromLua(L, 4, 0);
    uint32_t maxItems = GetUint32FromLua(L, 5, 0);

    // CODE SNIPPET START: XblTitleStorageGetBlobMetadataAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        XblTitleStorageBlobMetadataResultHandle handle;
        HRESULT hr = XblTitleStorageGetBlobMetadataResult(asyncBlock, &handle);
        LogToScreen("XblTitleStorageGetBlobMetadataResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP

        if (SUCCEEDED(hr))
        {
            if (Data()->blobMetadataResultHandle != nullptr)
            {
                XblTitleStorageBlobMetadataResultCloseHandle(Data()->blobMetadataResultHandle);
            }

            Data()->blobMetadataResultHandle = handle;
        }

        CallLuaFunctionWithHr(hr, "OnXblTitleStorageGetBlobMetadataAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblTitleStorageGetBlobMetadataAsync(
        Data()->xboxLiveContext,
        Data()->scid,
        storageType,
        blobPath.c_str(),
        xboxUserId,
        skipItems,
        maxItems,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToScreen("XblTitleStorageGetBlobMetadataAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblTitleStorageDeleteBinaryBlobAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    bool deleteOnlyIfEtagMatches = GetBoolFromLua(L, 1, false);

    // CODE SNIPPET START: XblTitleStorageDeleteBlobAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);
        LogToScreen("XblTitleStorageDeleteBlobAsync result (BINARY): hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP

        CallLuaFunctionWithHr(hr, "OnXblTitleStorageDeleteBinaryBlobAsync"); // CODE SNIP SKIP
    };
    
    HRESULT hr = XblTitleStorageDeleteBlobAsync(
        Data()->xboxLiveContext,
        Data()->binaryBlobMetadata,
        deleteOnlyIfEtagMatches,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToScreen("XblTitleStorageDeleteBlobAsync (BINARY): hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}


int XblTitleStorageDeleteJsonBlobAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    bool deleteOnlyIfEtagMatches = GetBoolFromLua(L, 1, false);

    // CODE SNIPPET START: XblTitleStorageDeleteBlobAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);
        LogToScreen("XblTitleStorageDeleteBlobAsync result (JSON): hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblTitleStorageDeleteJsonBlobAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblTitleStorageDeleteBlobAsync(
        Data()->xboxLiveContext,
        Data()->jsonBlobMetadata,
        deleteOnlyIfEtagMatches,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToScreen("XblTitleStorageDeleteBlobAsync (JSON): hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblTitleStorageDownloadBinaryBlobAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    std::string selectQuery = GetStringFromLua(L, 1, "");
    uint32_t preferredDownloadBlockSize = GetUint32FromLua(L, 1, 1024*256);

    // CODE SNIPPET START: XblTitleStorageDownloadBlobAsync
    std::unique_ptr<std::vector<uint8_t>> downloadBlobBuffer = std::make_unique<std::vector<uint8_t>>(Data()->binaryBlobMetadata.length);

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = downloadBlobBuffer.get();
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        std::unique_ptr<std::vector<uint8_t>> downloadBlobBuffer{ static_cast<std::vector<uint8_t>*>(asyncBlock->context) };
        HRESULT hr = XblTitleStorageDownloadBlobResult(asyncBlock, &Data()->binaryBlobMetadata);
        LogToScreen("XblTitleStorageDownloadBlobResult (BINARY): hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblTitleStorageDownloadBinaryBlobAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblTitleStorageDownloadBlobAsync(
        Data()->xboxLiveContext,
        Data()->binaryBlobMetadata,
        downloadBlobBuffer->data(),
        Data()->binaryBlobMetadata.length,
        XblTitleStorageETagMatchCondition::NotUsed,
        selectQuery.c_str(), // optional
        preferredDownloadBlockSize, // optional
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
        downloadBlobBuffer.release();
    }
    // CODE SNIPPET END
    LogToScreen("XblTitleStorageDownloadBlobAsync (BINARY): hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblTitleStorageDownloadJsonBlobAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    std::string selectQuery = GetStringFromLua(L, 1, "");
    uint32_t preferredDownloadBlockSize = GetUint32FromLua(L, 1, 1024);
    size_t length = Data()->jsonBlobMetadata.length;
    std::unique_ptr<std::vector<uint8_t>> downloadBlobBuffer = std::make_unique<std::vector<uint8_t>>(length);

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = downloadBlobBuffer.get();
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock };
        std::unique_ptr<std::vector<uint8_t>> downloadBlobBuffer{ static_cast<std::vector<uint8_t>*>(asyncBlock->context) };
        HRESULT hr = XblTitleStorageDownloadBlobResult(asyncBlock, &Data()->jsonBlobMetadata);
        LogToScreen("XblTitleStorageDownloadBlobResult (JSON): hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblTitleStorageDownloadJsonBlobAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblTitleStorageDownloadBlobAsync(
        Data()->xboxLiveContext,
        Data()->jsonBlobMetadata,
        downloadBlobBuffer->data(),
        Data()->jsonBlobMetadata.length,
        XblTitleStorageETagMatchCondition::NotUsed,
        selectQuery.c_str(), // optional
        preferredDownloadBlockSize, // optional
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
        downloadBlobBuffer.release();
    }

    LogToScreen("XblTitleStorageDownloadBlobAsync (JSON): hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblTitleStorageUploadJsonBlobAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    std::string displayName = GetStringFromLua(L, 1, "Test Name 4");
    std::string blobPath = GetStringFromLua(L, 2, "apirunner/test/path/json.txt");
    std::string blobBufferParam = GetStringFromLua(L, 3, "{}");
    XblTitleStorageType storageType = ConvertStringToXblTitleStorageType(GetStringFromLua(L, 4, "XblTitleStorageType::Universal").c_str());
    XblTitleStorageBlobType blobType = ConvertStringToXblTitleStorageBlobType(GetStringFromLua(L, 5, "XblTitleStorageBlobType::Json").c_str());
    uint32_t preferredUploadBlockSize = GetUint32FromLua(L, 6, 1024);
    XblTitleStorageETagMatchCondition eTagMatchCondition = ConvertStringToXblTitleStorageETagMatchCondition(GetStringFromLua(L, 7, "XblTitleStorageETagMatchCondition::NotUsed").c_str());
    std::unique_ptr<std::vector<char>> blobBuffer = std::make_unique<std::vector<char>>();

    size_t blobBufferSize;
    blobBufferSize = blobBufferParam.size() + 1;
    blobBuffer->resize(blobBufferSize);
    pal::strcpy(blobBuffer->data(), blobBufferSize, blobBufferParam.c_str());

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = blobBuffer.get();
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; 
        HRESULT hr = XblTitleStorageUploadBlobResult(asyncBlock, &Data()->jsonBlobMetadata);
        LogToScreen("XblTitleStorageUploadBlobResult (JSON): hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblTitleStorageUploadJsonBlobAsync"); // CODE SNIP SKIP
    };

    XblTitleStorageBlobMetadata blobMetadata{};
    pal::strcpy(blobMetadata.displayName, displayName.size() + 1, displayName.c_str());
    pal::strcpy(blobMetadata.serviceConfigurationId, XBL_SCID_LENGTH, Data()->scid);
    pal::strcpy(blobMetadata.blobPath, blobPath.size() + 1, blobPath.c_str());
    blobMetadata.storageType = storageType;
    blobMetadata.blobType = blobType;
    time(&blobMetadata.clientTimestamp);

    HRESULT hr = XblTitleStorageUploadBlobAsync(
        Data()->xboxLiveContext,
        blobMetadata,
        reinterpret_cast<const uint8_t*>(blobBuffer->data()),
        blobBufferSize,
        eTagMatchCondition,
        preferredUploadBlockSize, // optional
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
        blobBuffer.release();
    }
    LogToScreen("XblTitleStorageUploadBlobAsync (JSON): hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblTitleStorageUploadBinaryBlobAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    std::string displayName = GetStringFromLua(L, 1, "Test Binary Data");
    std::string blobPath = GetStringFromLua(L, 2, "apirunner/test/path.txt");
    XblTitleStorageType storageType = ConvertStringToXblTitleStorageType(GetStringFromLua(L, 3, "XblTitleStorageType::Universal").c_str());
    XblTitleStorageBlobType blobType = ConvertStringToXblTitleStorageBlobType(GetStringFromLua(L, 4, "XblTitleStorageBlobType::Binary").c_str());
    XblTitleStorageETagMatchCondition eTagMatchCondition = ConvertStringToXblTitleStorageETagMatchCondition(GetStringFromLua(L, 5, "XblTitleStorageETagMatchCondition::NotUsed").c_str());
    std::unique_ptr<std::vector<char>> blobBuffer = std::make_unique<std::vector<char>>();

    size_t preferredUploadBlockSize = 1024 * 256;
    size_t blobBufferSize = 1024 * 600; // requires 3 chunk uploads
    blobBuffer->resize(blobBufferSize);
    char zero = '0';
    for (size_t i = 0; i < blobBufferSize; i++)
    {
        (*blobBuffer)[i] = (char)(zero + i % 10);
    }

    // CODE SNIPPET START: XblTitleStorageUploadBlobAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = blobBuffer.get();
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XblTitleStorageUploadBlobResult(asyncBlock, &Data()->binaryBlobMetadata);
        LogToScreen("XblTitleStorageUploadBlobResult (Binary): hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblTitleStorageUploadBinaryBlobAsync"); // CODE SNIP SKIP
    };

    XblTitleStorageBlobMetadata blobMetadata{};
    pal::strcpy(blobMetadata.displayName, displayName.size() + 1, displayName.c_str());
    pal::strcpy(blobMetadata.serviceConfigurationId, XBL_SCID_LENGTH, Data()->scid);
    pal::strcpy(blobMetadata.blobPath, blobPath.size() + 1, blobPath.c_str());
    blobMetadata.storageType = storageType;
    blobMetadata.blobType = blobType;
    time(&blobMetadata.clientTimestamp);
    
    HRESULT hr = XblTitleStorageUploadBlobAsync(
        Data()->xboxLiveContext,
        blobMetadata,
        reinterpret_cast<const uint8_t*>(blobBuffer->data()),
        blobBufferSize,
        eTagMatchCondition,
        preferredUploadBlockSize, // optional
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
        blobBuffer.release();
    }
    // CODE SNIPPET END
    LogToScreen("XblTitleStorageUploadBlobAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int RestCallToDownloadJsonBlob_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    auto response = Data()->metadataResponseString;
    HRESULT hr = S_OK;

    std::string methodName = GetStringFromLua(L, 1, "GET");
    char url[300];
    sprintf_s(url, "https://titlestorage.xboxlive.com/json/users/xuid(%" PRIu64 ")/scids/00000000-0000-0000-0000-000076029b4d/data/", Data()->xboxUserId);

    auto result = DeserializeResult(url, response);
    for (const auto& blobMetadata : result.m_items)
    {
        // Download the blob
        XblHttpCallHandle httpCallHandle;
        hr = XblHttpCallCreate(Data()->xboxLiveContext, "GET", blobMetadata.blobPath.c_str(), &httpCallHandle);
        XblHttpCallRequestSetHeader(httpCallHandle, "Content-Type", "application/json; charset=utf-8", true);
        XblHttpCallRequestSetHeader(httpCallHandle, "Accept-Language", "en-US,en", true);
        XblHttpCallRequestSetHeader(httpCallHandle, "x-xbl-contract-version", "2", true);
        Data()->titleStorageHttpCalls.push_back(httpCallHandle);

        auto asyncBlock = std::make_unique<XAsyncBlock>();
        asyncBlock->queue = Data()->queue;
        asyncBlock->context = Data()->titleStorageHttpCalls[blobMetadata.positionInList];
        asyncBlock->callback = [](XAsyncBlock* asyncBlock)
        {
            std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
            HRESULT hr = XAsyncGetStatus(asyncBlock, false);

            if (SUCCEEDED(hr))
            {
                auto httpCall = static_cast<XblHttpCallHandle>(asyncBlock->context);

                const char* responseString;
                hr = XblHttpCallGetResponseString(httpCall, &responseString);

                LogToFile("Response String: length %d, hr=%s", strlen(responseString), ConvertHR(hr).c_str());
                LogToFile("Response: %s", responseString);
                CHECKHR(hr);

                Data()->blobResponseStrings.push_back(responseString);

                uint32_t statusCode;
                hr = XblHttpCallGetStatusCode(httpCall, &statusCode);
                LogToScreen("Status Code: %d, hr=%s", statusCode, ConvertHR(hr).c_str());
                CHECKHR(hr);
            }

        Cleanup:
            Data()->titleStorageCompletedHttpCalls++;
            if (Data()->titleStorageCompletedHttpCalls == Data()->titleStorageHttpCalls.size())
            {
                CallLuaFunctionWithHr(hr, "OnDownloadBlobs");
            }

            LogToScreen("XblHttpCallPerformAsync Completion: hr=%s", ConvertHR(hr).c_str());
        };

        LogToScreen("Downloading %s", blobMetadata.blobPath.c_str());
        hr = XblHttpCallPerformAsync(httpCallHandle, XblHttpCallResponseBodyType::String, asyncBlock.get());
        if (SUCCEEDED(hr))
        {
            // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
            // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
            asyncBlock.release();
        }
    }

    return LuaReturnHR(L, hr);
}

int RestCallToUploadJsonBlob_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    auto response = Data()->metadataResponseString;
    HRESULT hr = S_OK;

    std::string methodName = GetStringFromLua(L, 1, "GET");
    char url[300];
    sprintf_s(url, "https://titlestorage.xboxlive.com/json/users/xuid(%" PRIu64 ")/scids/00000000-0000-0000-0000-000076029b4d/data/apirunner/test/json/file.json,json", Data()->xboxUserId);

    std::string blobContent = GetStringFromLua(L, 1, "{}");

    // Upload the blob
    XblHttpCallHandle httpCallHandle;
    hr = XblHttpCallCreate(Data()->xboxLiveContext, "PUT", url, &httpCallHandle);
    XblHttpCallRequestSetHeader(httpCallHandle, "Content-Type", "application/json; charset=utf-8", true);
    XblHttpCallRequestSetHeader(httpCallHandle, "Accept-Language", "en-US,en", true);
    XblHttpCallRequestSetHeader(httpCallHandle, "x-xbl-contract-version", "2", true);

    XblHttpCallRequestSetRequestBodyString(httpCallHandle, blobContent.c_str());

    Data()->xblHttpCall = httpCallHandle;

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = Data()->xblHttpCall;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);

        if (SUCCEEDED(hr))
        {
            auto httpCall = static_cast<XblHttpCallHandle>(asyncBlock->context);

            uint32_t statusCode;
            hr = XblHttpCallGetStatusCode(httpCall, &statusCode);
            LogToScreen("Status Code: %d, hr=%s", statusCode, ConvertHR(hr).c_str());
            CHECKHR(hr);
        }

    Cleanup:
        CallLuaFunctionWithHr(hr, "OnXblTitleStorageRestUpload");
        LogToScreen("XblHttpCallPerformAsync Completion: hr=%s", ConvertHR(hr).c_str());
    };

    LogToScreen("Uploading %s", url);
    hr = XblHttpCallPerformAsync(httpCallHandle, XblHttpCallResponseBodyType::String, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    return LuaReturnHR(L, hr);
}

int RestCallForJsonMetadata_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    std::string methodName = GetStringFromLua(L, 1, "GET");
    char url[300];
    sprintf_s(url, "https://titlestorage.xboxlive.com/json/users/xuid(%" PRIu64 ")/scids/00000000-0000-0000-0000-000076029b4d/data/apirunner/test/json?maxItems=100", Data()->xboxUserId);

    XblHttpCallHandle httpCallHandle;
    HRESULT hr = XblHttpCallCreate(Data()->xboxLiveContext, methodName.c_str(), url, &httpCallHandle);
    XblHttpCallRequestSetHeader(httpCallHandle, "Content-Type", "application/json; charset=utf-8", true);
    XblHttpCallRequestSetHeader(httpCallHandle, "Accept-Language", "en-US,en", true);
    XblHttpCallRequestSetHeader(httpCallHandle, "x-xbl-contract-version", "2", true);
    Data()->xblHttpCall = httpCallHandle;

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = Data()->xblHttpCall;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);
        LogToScreen("XblHttpCallPerformAsync result: hr=%s", ConvertHR(hr).c_str());

        if (SUCCEEDED(hr))
        {
            const char* responseString;
            hr = XblHttpCallGetResponseString(Data()->xblHttpCall, &responseString);
            Data()->metadataResponseString = responseString;

            LogToScreen("XblHttpCallResponseGetResponseString: length %d, hr=%s", strlen(responseString), ConvertHR(hr).c_str());
            CHECKHR(hr);

            uint32_t statusCode;
            hr = XblHttpCallGetStatusCode(Data()->xblHttpCall, &statusCode);
            LogToScreen("XblHttpCallResponseGetStatusCode: %d, hr=%s", statusCode, ConvertHR(hr).c_str());
            CHECKHR(hr);
        }

    Cleanup:
        LogToScreen("XblHttpCallPerformAsync Completion: hr=%s", ConvertHR(hr).c_str());
        CallLuaFunctionWithHr(hr, "OnDownloadMetadataBlobs");
    };

    hr = XblHttpCallPerformAsync(Data()->xblHttpCall, XblHttpCallResponseBodyType::String, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    return LuaReturnHR(L, hr);
}



void SetupAPIs_XblTitleStorage()
{
    lua_register(Data()->L, "RestCallForJsonMetadata", RestCallForJsonMetadata_Lua);
    lua_register(Data()->L, "RestCallToDownloadJsonBlob", RestCallToDownloadJsonBlob_Lua);
    lua_register(Data()->L, "RestCallToUploadJsonBlob", RestCallToUploadJsonBlob_Lua);
    lua_register(Data()->L, "XblTitleStorageBlobMetadataResultGetItems", XblTitleStorageBlobMetadataResultGetItems_Lua);
    lua_register(Data()->L, "XblTitleStorageBlobMetadataResultHasNext", XblTitleStorageBlobMetadataResultHasNext_Lua);
    lua_register(Data()->L, "XblTitleStorageBlobMetadataResultGetNextAsync", XblTitleStorageBlobMetadataResultGetNextAsync_Lua);
    lua_register(Data()->L, "XblTitleStorageBlobMetadataResultDuplicateHandle", XblTitleStorageBlobMetadataResultDuplicateHandle_Lua);
    lua_register(Data()->L, "XblTitleStorageBlobMetadataResultCloseHandle", XblTitleStorageBlobMetadataResultCloseHandle_Lua);
    lua_register(Data()->L, "XblTitleStorageUploadJsonBlobAsync", XblTitleStorageUploadJsonBlobAsync_Lua);
    lua_register(Data()->L, "XblTitleStorageGetQuotaAsync", XblTitleStorageGetQuotaAsync_Lua);
    lua_register(Data()->L, "XblTitleStorageGetBlobMetadataAsync", XblTitleStorageGetBlobMetadataAsync_Lua);
    lua_register(Data()->L, "XblTitleStorageDeleteBinaryBlobAsync", XblTitleStorageDeleteBinaryBlobAsync_Lua);
    lua_register(Data()->L, "XblTitleStorageDeleteJsonBlobAsync", XblTitleStorageDeleteJsonBlobAsync_Lua);
    lua_register(Data()->L, "XblTitleStorageDownloadBinaryBlobAsync", XblTitleStorageDownloadBinaryBlobAsync_Lua);
    lua_register(Data()->L, "XblTitleStorageDownloadJsonBlobAsync", XblTitleStorageDownloadJsonBlobAsync_Lua);
    lua_register(Data()->L, "XblTitleStorageUploadBinaryBlobAsync", XblTitleStorageUploadBinaryBlobAsync_Lua);
}

