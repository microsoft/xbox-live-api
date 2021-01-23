// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/platform_c.h"
#include "client_operation.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

extern XblLocalStorageWriteHandler g_localStorageWriteHandler;
extern XblLocalStorageReadHandler g_localStorageReadHandler;
extern XblLocalStorageClearHandler g_localStorageClearHandler;
extern XTaskQueueHandle g_localStorageTaskQueue;
extern void* g_localStorageClientContext;

class LocalStorage : public std::enable_shared_from_this<LocalStorage>
{
public:
    LocalStorage(const TaskQueue& queue);
#if HC_PLATFORM == HC_PLATFORM_WIN32
    HRESULT SetStoragePath(_In_opt_z_ const char* path);
#endif

    using WriteOperation = ClientOperation<Result<size_t>>;
    using ReadOperation = ClientOperation<Result<Vector<uint8_t>>>;
    using ClearOperation = ClientOperation<HRESULT>;

    HRESULT WriteAsync(
        const User& user,
        XblLocalStorageWriteMode mode,
        String key,
        Vector<uint8_t> data,
        Callback<Result<size_t>> callback
    ) noexcept;

    HRESULT ReadAsync(
        const User& user,
        String key,
        Callback<Result<Vector<uint8_t>>> callback
    ) noexcept;

    HRESULT ClearAsync(
        const User& user,
        String key,
        Callback<HRESULT> callback
    ) noexcept;

private:
    void QueueOperation(std::shared_ptr<XblClientOperation> op) noexcept;
    void RunNextOperation() noexcept;
    void OperationComplete() noexcept;

    static void DefaultWrite(
        _In_opt_ void* context,
        _In_ XblClientOperationHandle operation,
        _In_ XblUserHandle user,
        _In_ XblLocalStorageWriteMode mode,
        _In_z_ char const* key,
        _In_ size_t dataSize,
        _In_reads_bytes_(dataSize) void const* data
    );

    static void DefaultRead(
        _In_opt_ void* context,
        _In_ XblClientOperationHandle operation,
        _In_ XblUserHandle user,
        _In_z_ const char* key
    );

    static void DefaultClear(
        _In_opt_ void* context,
        _In_ XblClientOperationHandle operation,
        _In_ XblUserHandle user,
        _In_z_ const char* key
    );

    std::mutex m_mutex;
    TaskQueue m_queue;
    Queue<std::shared_ptr<XblClientOperation>> m_operationQueue;
    std::shared_ptr<XblClientOperation> m_currentOperation;

    XblLocalStorageWriteHandler m_writeHandler;
    XblLocalStorageReadHandler m_readHandler;
    XblLocalStorageClearHandler m_clearHandler;
    void* m_context;

    // State for default handlers
    String m_path;

#if HC_PLATFORM == HC_PLATFORM_IOS || HC_PLATFORM == HC_PLATFORM_ANDROID
    static String GetDefaultStoragePath();
#endif

    friend class StorageOperationLauncher;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
