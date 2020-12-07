// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/platform_c.h"

struct XblClientOperation
{
public:
    static HRESULT HresultFromResult(XblClientOperationResult result) noexcept
    {
        switch (result)
        {
        case XblClientOperationResult::Success:
        {
            return S_OK;
        }
        case XblClientOperationResult::Failure:
        {
            return E_FAIL;
        }
        default:
        {
            return S_OK;
        }
        }
    }

    virtual HRESULT Begin() noexcept = 0;
    virtual HRESULT Fail(HRESULT result) noexcept = 0;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

template<typename ResultT>
class ClientOperation :
    public XblClientOperation,
    public RefCounter,
    public std::enable_shared_from_this<ClientOperation<ResultT>>
{
public:
    using OperationLauncher = Function<void(XblClientOperationHandle)>;

    ClientOperation(
        OperationLauncher launcher,
        AsyncContext<ResultT> async
    ) noexcept;

    ClientOperation() = delete;
    ClientOperation(const ClientOperation&) = delete;
    ClientOperation(ClientOperation&&) = delete;
    ClientOperation& operator=(ClientOperation) = delete;

    virtual HRESULT Begin() noexcept override;
    virtual HRESULT Fail(HRESULT result) noexcept override;
    virtual HRESULT Complete(ResultT args) noexcept;

private:
    // RefCounter
    std::shared_ptr<RefCounter> GetSharedThis() override;

    OperationLauncher m_launcher;
    AsyncContext<ResultT> m_asyncContext;
};

template<typename ResultT>
ClientOperation<ResultT>::ClientOperation(
    OperationLauncher launcher,
    AsyncContext<ResultT> async
) noexcept 
    : m_launcher{ std::move(launcher) },
    m_asyncContext{ std::move(async) }
{
}

template<typename ResultT>
HRESULT ClientOperation<ResultT>::Begin() noexcept
{
    // Leak a ref until client calls Complete
    AddRef();

    // Marshall the launcher to the correct task queue
    return m_asyncContext.Queue().RunWork([this]
        {
            m_launcher(this);
        });
}

template<typename ResultT>
HRESULT ClientOperation<ResultT>::Fail(
    HRESULT result
) noexcept
{
    return Complete(ResultT{ result });
}

template<typename ResultT>
HRESULT ClientOperation<ResultT>::Complete(
    ResultT result
) noexcept
{
    // Marshall the completion to the completion port
    return m_asyncContext.Queue().RunCompletion(
        [
            this,
            result{ std::move(result) }
        ]
    () mutable
    {
        m_asyncContext.Complete(std::move(result));

        // Reclaim the ref leaked in Run
        DecRef();
    });
}

template<typename ResultT>
std::shared_ptr<RefCounter> ClientOperation<ResultT>::GetSharedThis()
{
    return this->shared_from_this();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END