// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "privacy_list_change_subscription.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_BEGIN

// MuteListChangeSubscription

MuteListChangeSubscription::MuteListChangeSubscription(
    _In_ uint64_t xuid
) noexcept
    : m_xuid{ xuid }
{
    Stringstream uri;
    uri << "https://privacy.xboxlive.com/users/xuid(" << m_xuid << ")/mutelist";
    m_resourceUri = uri.str();
}

XblFunctionContext MuteListChangeSubscription::AddHandler(
    MuteListChangedHandler handler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_lock };
    m_handlers[m_nextHandlerToken] = std::move(handler);
    return m_nextHandlerToken++;
}

size_t MuteListChangeSubscription::RemoveHandler(
    XblFunctionContext token
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_lock };
    m_handlers.erase(token);
    return m_handlers.size();
}

void MuteListChangeSubscription::OnEvent(
    const JsonValue& data
) noexcept
{
    // Payload format: {"NotificationType":"Added","Xuids":["2533274964271787"]}

    String notificationTypeString;
    HRESULT hr = JsonUtils::ExtractJsonString(data, "NotificationType", notificationTypeString);
    if (FAILED(hr))
    {
        LOGS_DEBUG << __FUNCTION__ << ": Ignoring malformed event";
        return;
    }

    XblPrivacyListChangeType changeType;
    if (utils::str_icmp_internal(notificationTypeString, "Added") == 0)
    {
        changeType = XblPrivacyListChangeType::Added;
    }
    else if (utils::str_icmp_internal(notificationTypeString, "Removed") == 0)
    {
        changeType = XblPrivacyListChangeType::Removed;
    }
    else
    {
        LOGS_DEBUG << __FUNCTION__ << ": Ignoring unknown NotificationType";
        return;
    }

    Vector<uint64_t> xuids;
    hr = JsonUtils::ExtractJsonVector<uint64_t>(
        JsonUtils::JsonXuidExtractor,
        data,
        "Xuids",
        xuids,
        true
    );

    if (FAILED(hr))
    {
        LOGS_DEBUG << __FUNCTION__ << ": Ignoring malformed event";
        return;
    }

    XblPrivacyMuteListChangeEventArgs args
    {
        changeType,
        xuids.data(),
        xuids.size()
    };

    std::unique_lock<std::mutex> lock{ m_lock };
    auto handlers{ m_handlers };
    lock.unlock();

    for (auto& handler : handlers)
    {
        try
        {
            handler.second(args);
        }
        catch (const std::exception& e)
        {
            LOGS_DEBUG << __FUNCTION__ << ": Exception caught in handler: " << e.what();
        }
        catch (...)
        {
            LOGS_DEBUG << __FUNCTION__ << ": Unknown exception caught in handler.";
        }
    }
}

// BlockListChangeSubscription

BlockListChangeSubscription::BlockListChangeSubscription(
    _In_ uint64_t xuid
) noexcept
    : m_xuid{ xuid }
{
    Stringstream uri;
    uri << "https://privacy.xboxlive.com/users/xuid(" << m_xuid << ")/neverlist";
    m_resourceUri = uri.str();
}

XblFunctionContext BlockListChangeSubscription::AddHandler(
    BlockListChangedHandler handler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_lock };
    m_handlers[m_nextHandlerToken] = std::move(handler);
    return m_nextHandlerToken++;
}

size_t BlockListChangeSubscription::RemoveHandler(
    XblFunctionContext token
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_lock };
    m_handlers.erase(token);
    return m_handlers.size();
}

void BlockListChangeSubscription::OnEvent(
    const JsonValue& data
) noexcept
{
    // Payload format: {"NotificationType":"Added","Xuids":["2533274964271787"]}

    String notificationTypeString;
    HRESULT hr = JsonUtils::ExtractJsonString(data, "NotificationType", notificationTypeString);
    if (FAILED(hr))
    {
        LOGS_DEBUG << __FUNCTION__ << ": Ignoring malformed event";
        return;
    }

    XblPrivacyListChangeType changeType;
    if (utils::str_icmp_internal(notificationTypeString, "Added") == 0)
    {
        changeType = XblPrivacyListChangeType::Added;
    }
    else if (utils::str_icmp_internal(notificationTypeString, "Removed") == 0)
    {
        changeType = XblPrivacyListChangeType::Removed;
    }
    else
    {
        LOGS_DEBUG << __FUNCTION__ << ": Ignoring unknown NotificationType";
        return;
    }

    Vector<uint64_t> xuids;
    hr = JsonUtils::ExtractJsonVector<uint64_t>(
        JsonUtils::JsonXuidExtractor,
        data,
        "Xuids",
        xuids,
        true
    );

    if (FAILED(hr))
    {
        LOGS_DEBUG << __FUNCTION__ << ": Ignoring malformed event";
        return;
    }

    XblPrivacyBlockListChangeEventArgs args
    {
        changeType,
        xuids.data(),
        xuids.size()
    };

    std::unique_lock<std::mutex> lock{ m_lock };
    auto handlers{ m_handlers };
    lock.unlock();

    for (auto& handler : handlers)
    {
        try
        {
            handler.second(args);
        }
        catch (const std::exception& e)
        {
            LOGS_DEBUG << __FUNCTION__ << ": Exception caught in handler: " << e.what();
        }
        catch (...)
        {
            LOGS_DEBUG << __FUNCTION__ << ": Unknown exception caught in handler.";
        }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_END
