// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

using namespace xbox::services::real_time_activity;

TitlePresenceChangeSubscription::TitlePresenceChangeSubscription(
    _In_ uint64_t xuid,
    _In_ uint32_t titleId,
    _In_ std::shared_ptr<PresenceService> presenceService
) noexcept
    : m_xuid{ xuid },
    m_titleId{ titleId },
    m_presenceService{ presenceService }
{
    Stringstream uri;
    uri << "https://userpresence.xboxlive.com/users/xuid(" << m_xuid << ")/titles/" << m_titleId;
    m_resourceUri = uri.str();
}

void TitlePresenceChangeSubscription::OnSubscribe(
    const JsonValue& data
) noexcept
{
    if (data.IsNull())
    {
        LOGS_ERROR << __FUNCTION__ << ": RTA payload unexpectedly null";
        return;
    }

    auto presenceService{ m_presenceService.lock() };
    if (presenceService)
    {
        auto deserializationResult = XblPresenceRecord::Deserialize(data);
        if (Succeeded(deserializationResult))
        {
            bool isPlaying{ deserializationResult.Payload()->IsUserPlayingTitle(m_titleId) };
            presenceService->HandleTitlePresenceChanged(
                m_xuid,
                m_titleId,
                isPlaying ? XblPresenceTitleState::Started : XblPresenceTitleState::Ended
            );
        }
    }
}

void TitlePresenceChangeSubscription::OnEvent(
    const JsonValue& data
) noexcept
{
    auto presenceService{ m_presenceService.lock() };
    if (presenceService && data.IsString())
    {
        presenceService->HandleTitlePresenceChanged(m_xuid, m_titleId, EnumValue<XblPresenceTitleState>(data.GetString()));
    }
}

void TitlePresenceChangeSubscription::OnResync() noexcept
{
    auto presenceService{ m_presenceService.lock() };
    if (presenceService)
    {
        presenceService->GetPresence(m_xuid, {
            [
                sharedThis{ shared_from_this() },
                presenceService
            ]
        (Result<std::shared_ptr<XblPresenceRecord>> result)
            {
                if (Succeeded(result))
                {
                    bool isPlaying{ result.Payload()->IsUserPlayingTitle(sharedThis->m_titleId) };
                    presenceService->HandleTitlePresenceChanged(
                        sharedThis->m_xuid,
                        sharedThis->m_titleId,
                        isPlaying ? XblPresenceTitleState::Started : XblPresenceTitleState::Ended
                    );
                }
            }}
        );
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END