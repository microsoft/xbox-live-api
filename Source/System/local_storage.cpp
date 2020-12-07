// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "local_storage.h"
#include <iostream>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

XblLocalStorageWriteHandler g_localStorageWriteHandler{ nullptr };
XblLocalStorageReadHandler g_localStorageReadHandler{ nullptr };
XblLocalStorageClearHandler g_localStorageClearHandler{ nullptr };
XTaskQueueHandle g_localStorageTaskQueue{ nullptr };
void* g_localStorageClientContext{ nullptr };

LocalStorage::LocalStorage(
    const TaskQueue& queue
) :
    m_writeHandler{ g_localStorageWriteHandler },
    m_readHandler{ g_localStorageReadHandler },
    m_clearHandler{ g_localStorageClearHandler },
    m_context{ g_localStorageClientContext }
{
    // If the storage hooks were not configured, fall back to defaults
    if (!m_writeHandler || !m_readHandler || !m_clearHandler)
    {
        m_writeHandler = DefaultWrite;
        m_readHandler = DefaultRead;
        m_clearHandler = DefaultClear;
        m_context = this;
#if HC_PLATFORM == HC_PLATFORM_IOS || HC_PLATFORM == HC_PLATFORM_ANDROID
        m_path = GetDefaultStoragePath();
#endif
    }

    // If title configured a queue for local storage, use that. Otherwise use provided queue
    if (g_localStorageTaskQueue)
    {
        m_queue = TaskQueue::DeriveWorkerQueue(g_localStorageTaskQueue);
    }
    else
    {
        m_queue = queue.DeriveWorkerQueue();
    }
}

HRESULT LocalStorage::WriteAsync(
    const User& user,
    XblLocalStorageWriteMode mode,
    String key,
    Vector<uint8_t> data,
    Callback<Result<size_t>> callback
) noexcept
{
    auto copyUserResult{ user.Copy() };
    RETURN_HR_IF_FAILED(copyUserResult.Hresult());

    WriteOperation::OperationLauncher launcher{
        [
            sharedThis{ shared_from_this() },
            user = MakeShared<User>(copyUserResult.ExtractPayload()),
            mode,
            key{ std::move(key) },
            data{ std::move(data) }
        ]
    (XblClientOperationHandle op)
    {
        sharedThis->m_writeHandler(
            sharedThis->m_context,
            op,
            user->Handle(),
            mode,
            key.data(),
            data.size(),
            data.data()
        );
    }};

    auto op = MakeShared<WriteOperation>(
        std::move(launcher),
        AsyncContext<Result<size_t>>{ m_queue,
        [
            sharedThis{ shared_from_this() },
            callback{ std::move(callback) }
        ]
    (Result<size_t> result)
    {
        sharedThis->OperationComplete();
        callback(std::move(result));
    }
    });

    QueueOperation(op);

    return S_OK;
}

HRESULT LocalStorage::ReadAsync(
    const User& user,
    String key,
    Callback<Result<Vector<uint8_t>>> callback
) noexcept
{
    auto copyUserResult{ user.Copy() };
    RETURN_HR_IF_FAILED(copyUserResult.Hresult());

    ReadOperation::OperationLauncher launcher{
        [
            sharedThis{ shared_from_this() },
            user = MakeShared<User>(copyUserResult.ExtractPayload()),
            key{ std::move(key) }
        ]
    (XblClientOperationHandle op)
    {
        sharedThis->m_readHandler(sharedThis->m_context, op, user->Handle(), key.data());
    }};

    auto op = MakeShared<ReadOperation>(
        std::move(launcher),
        AsyncContext<Result<Vector<uint8_t>>>{ m_queue,
        [
            sharedThis{ shared_from_this() },
            callback{ std::move(callback) }
        ]
    (Result<Vector<uint8_t>> result)
    {
        sharedThis->OperationComplete();
        callback(std::move(result));
    }
    });

    QueueOperation(op);

    return S_OK;
}

HRESULT LocalStorage::ClearAsync(
    const User& user,
    String key,
    Callback<HRESULT> callback
) noexcept
{
    auto copyUserResult{ user.Copy() };
    RETURN_HR_IF_FAILED(copyUserResult.Hresult());

    ClearOperation::OperationLauncher launcher {
        [
            sharedThis{ shared_from_this() },
            user = MakeShared<User>(copyUserResult.ExtractPayload()),
            key{ std::move(key) }
        ]
    (XblClientOperationHandle op)
    {
        sharedThis->m_clearHandler(sharedThis->m_context, op, user->Handle(), key.data());
    }};

    auto op = MakeShared<ClearOperation>(
        std::move(launcher),
        AsyncContext<HRESULT>{ m_queue,
        [
            sharedThis{ shared_from_this() },
            callback{ std::move(callback) }
        ]
    (HRESULT result)
    {
        sharedThis->OperationComplete();
        callback(result);
    }
    });

    QueueOperation(op);

    return S_OK;
}

void LocalStorage::QueueOperation(
    std::shared_ptr<XblClientOperation> op
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_operationQueue.push(op);
    RunNextOperation();
}

void LocalStorage::RunNextOperation() noexcept
{
    // m_mutex must be held when calling this function
    if (!m_currentOperation && !m_operationQueue.empty())
    {
        m_currentOperation = m_operationQueue.front();
        m_operationQueue.pop();

        // Because operations aren't even kicked off synchronously, always raise errors through the
        // AsyncContext. If begin succeeds, client will complete operation later and the operation will
        // manage its own lifetime.
        HRESULT hr = m_currentOperation->Begin();
        if (FAILED(hr))
        {
            m_currentOperation->Fail(hr);
        }
    }
}

void LocalStorage::OperationComplete() noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_currentOperation = nullptr;
    RunNextOperation();
}

void LocalStorage::DefaultWrite(
    _In_opt_ void* context,
    _In_ XblClientOperationHandle op,
    _In_ XblUserHandle user,
    _In_ XblLocalStorageWriteMode mode,
    _In_z_ char const* key,
    _In_ size_t dataSize,
    _In_reads_bytes_(dataSize) void const* data
)
{
    assert(context);
    UNREFERENCED_PARAMETER(user);

    auto pThis{ static_cast<LocalStorage*>(context) };
    auto writeOp{ static_cast<LocalStorage::WriteOperation*>(op) };

    String fullPath = pThis->m_path + key;

    std::ios_base::openmode openMode{};
    switch (mode)
    {
    case XblLocalStorageWriteMode::Append:
    {
        openMode = std::ios_base::app;
        break;
    }
    case XblLocalStorageWriteMode::Truncate:
    {
        openMode = std::ios_base::trunc;
        break;
    }
    }

    std::ofstream file{ fullPath.data(), std::ios::binary | openMode };
    if (!file.is_open())
    {
        LOGS_DEBUG << "Failed to open file during LocalStorageService::WriteAsync, errno = " << errno;
        writeOp->Complete(E_FAIL);
        return;
    }

    file.write(reinterpret_cast<const char*>(data), dataSize);
    if (!file.good())
    {
        LOGS_DEBUG << "Write failed during LocalStorageService::WriteAsync, errno = " << errno;
        writeOp->Complete(E_FAIL);
        return;
    }

    uint64_t filesize = file.tellp();
    writeOp->Complete(static_cast<size_t>(filesize));
}

void LocalStorage::DefaultRead(
    _In_opt_ void* context,
    _In_ XblClientOperationHandle op,
    _In_ XblUserHandle user,
    _In_z_ const char* key
)
{
    assert(context);
    UNREFERENCED_PARAMETER(user);

    auto pThis{ static_cast<LocalStorage*>(context) };
    auto readOp{ static_cast<LocalStorage::ReadOperation*>(op) };

    String fullPath = pThis->m_path + key;

    std::ifstream file{ fullPath.data(), std::ios::binary | std::ios::ate };
    if (!file.is_open())
    {
        LOGS_DEBUG << "Failed to open file during LocalStorageService::ReadAsync, errno = " << errno;
        readOp->Complete(E_FAIL);
        return;
    }

    int64_t size = file.tellg();
    if (size < 0)
    {
        LOGS_DEBUG << "Failed to read file length during LocalStorageService::ReadAsync, errno = " << errno;
        readOp->Complete(E_FAIL);
        return;
    }

    file.seekg(0);

    Vector<uint8_t> data(static_cast<size_t>(size));

    file.read(reinterpret_cast<char*>(data.data()), data.size());
    if (!file.good())
    {
        LOGS_DEBUG << "Failed to read file during LocalStorageService::ReadAsync, errno = " << errno;
        readOp->Complete(E_FAIL);
        return;
    }

    assert(size == file.gcount());
    readOp->Complete(std::move(data));
}

void LocalStorage::DefaultClear(
    _In_opt_ void* context,
    _In_ XblClientOperationHandle op,
    _In_ XblUserHandle user,
    _In_z_ const char* key
)
{
    assert(context);
    UNREFERENCED_PARAMETER(user);

    auto pThis{ static_cast<LocalStorage*>(context) };
    auto clearOp{ static_cast<LocalStorage::ClearOperation*>(op) };

    String fullPath = pThis->m_path + key;

    int res = std::remove(fullPath.data());
    if (res != 0)
    {
        LOGS_DEBUG << "Failed to delete file in LocalStorageService::DeleteAsync, errno = " << errno;
        clearOp->Complete(E_FAIL);
    }
    else
    {
        clearOp->Complete(S_OK);
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
