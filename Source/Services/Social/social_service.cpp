// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_internal.h"
#include "real_time_activity_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

SocialService::SocialService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> rtaManager
) noexcept :
    m_user{ std::move(user) },
    m_xboxLiveContextSettings{ std::move(xboxLiveContextSettings) },
    m_rtaManager{ std::move(rtaManager) }
{
}

SocialService::~SocialService() noexcept
{
    if (m_socialRelationshipChangedSubscription)
    {
        m_rtaManager->RemoveSubscription(m_user, m_socialRelationshipChangedSubscription);
    }
}

XblFunctionContext SocialService::AddSocialRelationshipChangedHandler(
    _In_ SocialRelationshipChangedHandler handler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_lock };

    if (!m_socialRelationshipChangedSubscription)
    {
        m_socialRelationshipChangedSubscription = MakeShared<SocialRelationshipChangeSubscription>(m_user.Xuid());
        m_rtaManager->AddSubscription(m_user, m_socialRelationshipChangedSubscription);
    }
    return m_socialRelationshipChangedSubscription->AddHandler(std::move(handler));
}

void SocialService::RemoveSocialRelationshipChangedHandler(
    _In_ XblFunctionContext token
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_lock };

    if (m_socialRelationshipChangedSubscription)
    {
        if (m_socialRelationshipChangedSubscription->RemoveHandler(token) == 0)
        {
            m_rtaManager->RemoveSubscription(m_user, m_socialRelationshipChangedSubscription);
            m_socialRelationshipChangedSubscription.reset();
        }
    }
}

HRESULT SocialService::GetSocialRelationships(
    _In_ uint64_t xuid,
    _In_ XblSocialRelationshipFilter filter,
    _In_ size_t startIndex,
    _In_ size_t maxItems,
    _In_ AsyncContext<Result<std::shared_ptr<XblSocialRelationshipResult>>> async
) const noexcept
{
    bool includeViewFilter = filter != XblSocialRelationshipFilter::All;

    xsapi_internal_stringstream subpath;
    subpath << "/users/xuid(";
    subpath << xuid;
    subpath << ")/people";

    xsapi_internal_string nextDelimiter = "?";

    if (includeViewFilter)
    {
        subpath << nextDelimiter;
        subpath << "view=";
        subpath << SocialRelationshipFilterToString(filter);
        nextDelimiter = "&";
    }

    if (startIndex > 0)
    {
        subpath << nextDelimiter;
        subpath << "startIndex=";
        subpath << startIndex;
        nextDelimiter = "&";
    }

    if (maxItems > 0)
    {
        subpath << nextDelimiter;
        subpath << "maxItems=";
        subpath << maxItems;
        nextDelimiter = "&";
    }

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "GET",
        XblHttpCall::BuildUrl("social", subpath.str()),
        xbox_live_api::get_social_relationships
    ));

    return httpCall->Perform({
        async.Queue(),
        [
            async,
            filter,
            startIndex
        ]
    (HttpResult httpResult)
    {
        HRESULT hr{ Failed(httpResult) ? httpResult.Hresult() : httpResult.Payload()->Result() };
        if (FAILED(hr))
        {
            return async.Complete(hr);
        }
        else
        {
            auto result{ XblSocialRelationshipResult::Deserialize(httpResult.Payload()->GetResponseBodyJson()) };

            if (Succeeded(result))
            {
                result.Payload()->ContinuationSkip = startIndex + result.Payload()->SocialRelationships().size();
                result.Payload()->Filter = filter;
            }

            return async.Complete(result);
        }
    } });
}

xsapi_internal_string SocialService::SocialRelationshipFilterToString(
    _In_ XblSocialRelationshipFilter filter
) noexcept
{
    switch (filter)
    {
    case XblSocialRelationshipFilter::Favorite:
    {
        return "Favorite";
    }
    case XblSocialRelationshipFilter::LegacyXboxLiveFriends:
    {
        return "LegacyXboxLiveFriends";
    }

    case XblSocialRelationshipFilter::All:
    default:
    {
        return xsapi_internal_string{};
    }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END