// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

const char mp_default_resourceUri[] = "https://sessiondirectory.xboxlive.com/connections/";

MultiplayerSubscription::MultiplayerSubscription() noexcept
{
    m_resourceUri = mp_default_resourceUri;
}

const String& MultiplayerSubscription::RtaConnectionId() const
{
    return m_connectionId;
}

XblFunctionContext MultiplayerSubscription::AddSessionChangedHandler(
    SessionChangedHandler handler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutexMultiplayerSubscription };
    m_sessionChangedHandlers[m_nextToken] = std::move(handler);
    return m_nextToken++;
}

size_t MultiplayerSubscription::RemoveSessionChangedHandler(
    XblFunctionContext token
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutexMultiplayerSubscription };
    m_sessionChangedHandlers.erase(token);
    return m_sessionChangedHandlers.size();
}

XblFunctionContext MultiplayerSubscription::AddConnectionIdChangedHandler(
    ConnectionIdChangedHandler handler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutexMultiplayerSubscription };
    m_connectionIdChangedHandlers[m_nextToken] = std::move(handler);
    return m_nextToken++;
}

size_t MultiplayerSubscription::RemoveConnectionIdChangedHandler(
    XblFunctionContext token
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutexMultiplayerSubscription };
    m_connectionIdChangedHandlers.erase(token);
    return m_connectionIdChangedHandlers.size();
}

void MultiplayerSubscription::OnSubscribe(
    _In_ const JsonValue& data
) noexcept
{
    std::unique_lock<std::mutex> lock{ m_mutexMultiplayerSubscription };

    HRESULT hr = JsonUtils::ExtractJsonString(data, "ConnectionId", m_connectionId, true);
    if (FAILED(hr))
    {
        LOGS_ERROR << __FUNCTION__ << ": Ignoring malformed payload";
        return;
    }

    auto handlers{ m_connectionIdChangedHandlers };
    lock.unlock();

    for (auto& handler : handlers)
    {
        handler.second(m_connectionId);
    }
}

void MultiplayerSubscription::OnEvent(
    _In_ const JsonValue& data
) noexcept
{
    if (!data.IsObject() || !data.HasMember("shoulderTaps"))
    {
        LOGS_ERROR << __FUNCTION__ << ": Ignoring malformed payload";
        return;
    }

    List<XblMultiplayerSessionChangeEventArgs> taps;

    const JsonValue& shoulderTaps = data["shoulderTaps"];
    if (shoulderTaps.IsArray())
    {
        for (const auto& tapValue : shoulderTaps.GetArray())
        {
            String resourceName;
            JsonUtils::ExtractJsonString(tapValue, "resource", resourceName, true);
            Vector<String> nameComponents = utils::string_split_internal(resourceName, '~');

            if (nameComponents.size() != 3)
            {
                LOGS_ERROR << __FUNCTION__ << ": Resource has too many values";
                continue;
            }

            taps.emplace_back();
            auto& tap{ taps.back() };

            tap.SessionReference = XblMultiplayerSessionReferenceCreate(
                nameComponents[0].data(),
                nameComponents[1].data(),
                nameComponents[2].data()
            );
            JsonUtils::ExtractJsonInt(tapValue, "changeNumber", tap.ChangeNumber, false);
            JsonUtils::ExtractJsonStringToCharArray(tapValue, "branch", tap.Branch, sizeof(tap.Branch));

            LOGS_DEBUG << __FUNCTION__ << ": Resource=" << resourceName;
        }
    }

    std::unique_lock<std::mutex> lock{ m_mutexMultiplayerSubscription };
    auto handlers{ m_sessionChangedHandlers };
    lock.unlock();

    for (auto& handler : handlers)
    {
        for (auto& tap : taps)
        {
            handler.second(tap);
        }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END