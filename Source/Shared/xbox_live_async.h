// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

template<typename T>
struct XblAsyncBlock : public AsyncBlock
{
    template<typename... Args>
    static std::shared_ptr<XblAsyncBlock<T>> alloc(Args... args)
    {
        return xsapi_allocate_shared<XblAsyncBlock<T>>(args...);
    }

    /// <summary>
    /// Creates an XblAsyncBlock whose typed callback will be invoked when the asyncronous operation completes.
    /// </summary>
    XblAsyncBlock(async_queue_t _queue, xbox_live_callback<T> _typedCallback)
        : typedCallback(std::move(_typedCallback))
    {
        queue = _queue;
        callback = [](_In_ struct AsyncBlock* asyncBlock)
        {
            size_t resultSize;
            auto hr = GetAsyncResultSize(asyncBlock, &resultSize);
            XSAPI_ASSERT(SUCCEEDED(hr) && resultSize == sizeof(T));

            auto pThis = reinterpret_cast<XblAsyncBlock<T>*>(asyncBlock);
            auto callback = pThis->typedCallback;

            T result;
            hr = GetAsyncResult(asyncBlock, nullptr, sizeof(T), &result);
            callback(result);
        };
    }

    XblAsyncBlock(xbox_live_callback<T> _typedCallback) 
        : XblAsyncBlock(nullptr, _typedCallback)
    {
    }

    xbox_live_callback<T> typedCallback;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END