// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/multiplayer_activity_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

namespace multiplayer_activity
{

class ActivityInfo : public XblMultiplayerActivityInfo
{
public:
    ActivityInfo(const XblMultiplayerActivityInfo* info) noexcept;

    static Result<Vector<ActivityInfo>> Deserialize(
        const JsonValue& responseJson,
        uint32_t titleId
    ) noexcept;

    String connectionString;
    String groupId;

private:
    ActivityInfo(uint64_t xuid) noexcept;
};

class MultiplayerActivityService : public std::enable_shared_from_this<MultiplayerActivityService>
{
public:
    MultiplayerActivityService(
        _In_ User&& user,
        _In_ const TaskQueue& queue,
        _In_ std::shared_ptr<XboxLiveContextSettings> settings
    ) noexcept;

    ~MultiplayerActivityService() noexcept;

    HRESULT UpdateRecentPlayers(
        _In_reads_(updatesCount) const XblMultiplayerActivityRecentPlayerUpdate* updates,
        _In_ size_t updatesCount
    ) noexcept;

    HRESULT FlushRecentPlayers(
        _In_ AsyncContext<Result<void>>&& async
    ) noexcept;

    HRESULT SetActivity(
        _In_ const ActivityInfo& info,
        _In_ bool allowCrossPlatformJoin,
        _In_ AsyncContext<HRESULT> async
    ) const noexcept;

    HRESULT GetActivity(
        _In_ const Vector<uint64_t>& xuids,
        _In_ AsyncContext<Result<Vector<ActivityInfo>>> async
    ) const noexcept;

    HRESULT DeleteActivity(
        _In_ AsyncContext<HRESULT> async
    ) const noexcept;

    HRESULT SendInvites(
        _In_ const Vector<uint64_t>& xuids,
        _In_ bool allowCrossPlatformJoin,
        _In_ const String& connectionString,
        _In_ AsyncContext<HRESULT> async
    ) const noexcept;

private:
    struct RecentPlayerUpdateMetadata
    {
        time_t timestamp{};
        XblMultiplayerActivityEncounterType encounterType{};
    };

    UnorderedMap<uint64_t, RecentPlayerUpdateMetadata> m_pendingRecentPlayerUpdates{};
    bool m_recentPlayersUpdateScheduled{ false };

    static uint64_t GetSequenceNumber();
    void ScheduleRecentPlayersUpdate() noexcept;
    static XblMultiplayerActivityPlatform GetLocalPlatform() noexcept;

    User m_user;
    TaskQueue m_queue;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_xboxLiveContextSettings;
    uint32_t m_titleId{ AppConfig::Instance()->TitleId() };
    mutable std::mutex m_mutex{};
};

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
