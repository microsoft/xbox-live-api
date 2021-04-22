// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class MockWebsocket;

class MockRealTimeActivityService
{
public:
    static MockRealTimeActivityService& Instance() noexcept;

    MockRealTimeActivityService(const MockRealTimeActivityService&) = delete;
    MockRealTimeActivityService& operator=(MockRealTimeActivityService) = delete;
    ~MockRealTimeActivityService() = default;

    void HandleClientMessage(
        std::shared_ptr<MockWebsocket> socket,
        const char* message
    ) noexcept;

    enum class ErrorCode : uint32_t
    {
        Success = 0,
        UnknownResource = 1,
        SubscriptionLimitReached = 2,
        NoResourceData = 3,
        Throttled = 1001,
        ServiceUnavailable = 1002
    };

    using SubscribeHandler = std::function<void(uint32_t subId, xsapi_internal_string uri)>;
    void SetSubscribeHandler(
        SubscribeHandler handler
    ) noexcept;

    void CompleteSubscribeHandshake(
        uint32_t subId,
        const rapidjson::Value& payload = rapidjson::Document{ rapidjson::kNullType },
        ErrorCode errorCode = ErrorCode::Success
    ) noexcept;

    void CompleteSubscribeHandshake(
        uint32_t subId,
        const char* payload,
        ErrorCode errorCode = ErrorCode::Success
    ) noexcept;

    void RaiseEvent(
        const xsapi_internal_string& uri,
        const rapidjson::Value& payload
    ) noexcept;

    void RaiseEvent(
        const xsapi_internal_string& uri,
        const char* payload
    ) noexcept;

    void RaiseResync() noexcept;

    void DisconnectClient(
        uint64_t xuid
    ) noexcept;

private:
    MockRealTimeActivityService() = default;

    uint32_t m_nextSubId{ 1 };
    SubscribeHandler m_subscribeHandler{ nullptr };

    struct Subscription
    {
        xsapi_internal_string uri;
        uint32_t clientSequenceNumber{ 0 };
        bool active{ false };
    };

    std::map<std::weak_ptr<MockWebsocket>, std::map<uint32_t, Subscription>, std::owner_less<std::weak_ptr<MockWebsocket>>> m_clients;

    mutable std::mutex m_mutex;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END