// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

SocialRelationshipChangeSubscription::SocialRelationshipChangeSubscription(
    _In_ uint64_t xuid
) noexcept
    : m_xuid{ xuid }
{
    Stringstream uri;
    uri << "http://social.xboxlive.com/users/xuid(" << m_xuid << ")/friends";
    m_resourceUri = uri.str();
}

XblFunctionContext SocialRelationshipChangeSubscription::AddHandler(
    SocialRelationshipChangedHandler handler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_lock };
    m_handlers[m_nextHandlerToken] = std::move(handler);
    return m_nextHandlerToken++;
}

XblFunctionContext SocialRelationshipChangeSubscription::AddHandler(
    FriendRequestCountChangedHandler handler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_lock };
    m_friendRequestCountChangedHandlers[m_nextHandlerToken] = std::move(handler);
    return m_nextHandlerToken++;
}

size_t SocialRelationshipChangeSubscription::RemoveHandler(
    XblFunctionContext token
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_lock };
    m_handlers.erase(token);
    m_friendRequestCountChangedHandlers.erase(token);
    return m_handlers.size() + m_friendRequestCountChangedHandlers.size();
}

void SocialRelationshipChangeSubscription::OnEvent(
    const JsonValue& data
) noexcept
{
    // Payload format http://xboxwiki/wiki/RTA%3AEVENT#People
    // [<API_ID>, <SUB_ID>, {"NotificationType":"Added","Xuids":["2533274964271787"]}]

    String notificationTypeString;
    XblSocialNotificationType notificationType;
    Vector<uint64_t> xuids;

    HRESULT hr = JsonUtils::ExtractJsonString(data, "NotificationType", notificationTypeString);
    if (SUCCEEDED(hr))
    {
        notificationType = EnumValue<XblSocialNotificationType>(notificationTypeString.data());

        switch (notificationType)
        {
        // Special handling for IncomingFriendRequestCountChanged as that payload is structured differently
        // and results in a different event handler firing
        case XblSocialNotificationType::IncomingFriendRequestCountChanged:
        {

            size_t incomingFriendRequestCount{};
            hr = JsonUtils::ExtractJsonSizeT(data, "Count", incomingFriendRequestCount, true);
            if (SUCCEEDED(hr))
            {
                XblSocialFriendRequestCountChangedEventArgs args
                {
                    m_xuid, 
                    incomingFriendRequestCount 
                };

                std::unique_lock<std::mutex> lock{ m_lock };
                auto handlers{ m_friendRequestCountChangedHandlers };
                lock.unlock();

                for (auto& handler : handlers)
                {
                    handler.second(args);
                }
            }
            else
            {
                LOGS_DEBUG << __FUNCTION__ << ": Ignoring malformed event";
            }
            break;
        }
        // Everything else can be handled the same
        case XblSocialNotificationType::Added:
        case XblSocialNotificationType::Changed:
        case XblSocialNotificationType::Removed:
        {
            hr = JsonUtils::ExtractJsonVector<uint64_t>(
                JsonUtils::JsonXuidExtractor,
                data,
                "Xuids",
                xuids,
                true
            );

            if (SUCCEEDED(hr))
            {
                XblSocialRelationshipChangeEventArgs args
                {
                    m_xuid,
                    notificationType,
                    xuids.data(),
                    xuids.size()
                };

                std::unique_lock<std::mutex> lock{ m_lock };
                auto handlers{ m_handlers };
                lock.unlock();

                for (auto& handler : handlers)
                {
                    handler.second(args);
                }
            }
            else
            {
                LOGS_DEBUG << __FUNCTION__ << ": Ignoring malformed event";
            }
            break;
        }
        case XblSocialNotificationType::Unknown:
        default:
        {
            LOGS_DEBUG << __FUNCTION__ << ": Ignoring malformed event";
            break;
        }
        }
    }
    else
    {
        LOGS_DEBUG << __FUNCTION__ << ": Ignoring malformed event";
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END
