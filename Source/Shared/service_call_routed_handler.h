// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "http_call_request_message_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class ServiceCallRoutedHandler
{
public:
    ServiceCallRoutedHandler(
        _In_ XblCallRoutedHandler handler,
        _In_opt_ void* context
    ) noexcept;

    ~ServiceCallRoutedHandler() noexcept;

private:
    static void HCCallRoutedHandler(
        _In_ HCCallHandle call,
        _In_ void* context
    );

    String GetFormattedResponse(
        HCCallHandle call
    ) const noexcept;

    XblCallRoutedHandler m_clientHandler{ nullptr };
    void* m_clientContext{ nullptr };
    int32_t m_hcToken{ 0 };
    static std::atomic<uint64_t> s_nextResponseNumber;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END