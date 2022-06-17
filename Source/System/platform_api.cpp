#include "pch.h"
#include "client_operation.h"
#include "local_storage.h"

using namespace xbox::services;
using namespace xbox::services::system;

STDAPI XblLocalStorageWriteComplete(
    _In_ XblClientOperationHandle operation,
    _In_ XblClientOperationResult result,
    _In_ size_t dataSize
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(operation);

    auto writeOp{ static_cast<LocalStorage::WriteOperation*>(operation) };
    return writeOp->Complete({ dataSize, XblClientOperation::HresultFromResult(result) });
}
CATCH_RETURN()

STDAPI XblLocalStorageReadComplete(
    _In_ XblClientOperationHandle operation,
    _In_ XblClientOperationResult result,
    _In_ size_t dataSize,
    _In_reads_bytes_opt_(dataSize) void const* data
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(operation);
    RETURN_HR_INVALIDARGUMENT_IF(dataSize && !data);

    auto readOp{ static_cast<LocalStorage::ReadOperation*>(operation) };

    Vector<uint8_t> dataVector(dataSize);
    memcpy(dataVector.data(), data, dataSize);

    return readOp->Complete(Result<Vector<uint8_t>>{ std::move(dataVector), XblClientOperation::HresultFromResult(result) });
}
CATCH_RETURN()

STDAPI XblLocalStorageClearComplete(
    _In_ XblClientOperationHandle operation,
    _In_ XblClientOperationResult result
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(operation);

    auto clearOp{ static_cast<LocalStorage::ClearOperation*>(operation) };
    return clearOp->Complete(XblClientOperation::HresultFromResult(result));
}
CATCH_RETURN()

STDAPI XblLocalStorageSetHandlers(
    _In_opt_ XTaskQueueHandle queue,
    _In_ XblLocalStorageWriteHandler writeHandler,
    _In_ XblLocalStorageReadHandler readHandler,
    _In_ XblLocalStorageClearHandler clearHandler,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    if (GlobalState::Get())
    {
        return E_XBL_ALREADY_INITIALIZED;
    }

    if ((writeHandler && readHandler && clearHandler) || 
       (!writeHandler && !readHandler && !clearHandler))
    {
        g_localStorageWriteHandler = writeHandler;
        g_localStorageReadHandler = readHandler;
        g_localStorageClearHandler = clearHandler;
        g_localStorageClientContext = context;
        g_localStorageTaskQueue = queue; // Store a non-owning pointer until XblInitialize is called
    }
    else
    {
        // Require that the hooks be set together
        return E_INVALIDARG;
    }

    return S_OK;
}
CATCH_RETURN()
