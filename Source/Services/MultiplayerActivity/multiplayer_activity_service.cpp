// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_activity_internal.h"
#if HC_PLATFORM == HC_PLATFORM_GDK
#include <XSystem.h>
#endif

#define RECENT_PLAYERS_UPLOAD_INTERVAL_MS 5000
#define MPA_SERVICE_NAME "multiplayeractivity"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

namespace multiplayer_activity
{

constexpr auto PlatformName = EnumName<XblMultiplayerActivityPlatform, 0, static_cast<uint32_t>(XblMultiplayerActivityPlatform::All)>;

MultiplayerActivityService::MultiplayerActivityService(
    _In_ User&& user,
    _In_ const TaskQueue& queue,
    _In_ std::shared_ptr<XboxLiveContextSettings> settings
) noexcept
    : m_user{ std::move(user) },
    m_queue{ queue.DeriveWorkerQueue() },
    m_xboxLiveContextSettings{ std::move(settings) }
{
}

MultiplayerActivityService::~MultiplayerActivityService() noexcept
{
    FlushRecentPlayers(m_queue);

    // Terminating m_queue to cancel next scheduled flush. FlushRecentPlayers call above will not be canceled
    // since the work will happen on a queue derived from m_queue.
    m_queue.Terminate(false);
}

HRESULT MultiplayerActivityService::UpdateRecentPlayers(
    _In_reads_(updatesCount) const XblMultiplayerActivityRecentPlayerUpdate* updates,
    _In_ size_t updatesCount
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(updates == nullptr || updatesCount == 0);

    std::lock_guard<std::mutex> lock{ m_mutex };

    time_t now{ time(nullptr) };
    for (size_t i = 0; i < updatesCount; ++i)
    {
        m_pendingRecentPlayerUpdates[updates[i].xuid] = RecentPlayerUpdateMetadata{ now, updates[i].encounterType };
    }

    if (!m_recentPlayersUpdateScheduled)
    {
        m_recentPlayersUpdateScheduled = true;
        ScheduleRecentPlayersUpdate();
    }

    return S_OK;
}

HRESULT MultiplayerActivityService::FlushRecentPlayers(
    _In_ AsyncContext<Result<void>>&& async
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    if (m_pendingRecentPlayerUpdates.empty())
    {
        async.Complete(S_OK);
        return S_OK;
    }

    // Build HTTP request
    class ServiceCall : public XblHttpCall
    {
    public:
        ServiceCall(User&& user) noexcept
            : XblHttpCall{ std::move(user) }
        {
        }

        HRESULT Init(
            _In_ std::shared_ptr<XboxLiveContextSettings> contextSettings,
            _In_ uint32_t titleId
        ) noexcept
        {
            m_globalState = GlobalState::Get();
            if (!m_globalState)
            {
                return E_XBL_NOT_INITIALIZED;
            }

            Stringstream path{};
            path << "/titles/" << titleId << "/recentplayers";

            RETURN_HR_IF_FAILED(XblHttpCall::Init(
                contextSettings,
                "POST",
                XblHttpCall::BuildUrl(MPA_SERVICE_NAME, path.str()),
                xbox_live_api::post_recent_players
            ));
            RETURN_HR_IF_FAILED(XblHttpCall::SetUserAgent(HttpCallAgent::MultiplayerActivity));

            return S_OK;
        }

        HRESULT PerformWithRetry(
            AsyncContext<xbox::services::Result<void>>&& async
        )
        {
            return XblHttpCall::Perform(AsyncContext<HttpResult>{ async.Queue().DeriveWorkerQueue(),
                [async](HttpResult httpResult) mutable
                {
                    HandleHttpResult(std::move(httpResult), std::move(async));
                }
            });
        }

    private:
        static void HandleHttpResult(
            HttpResult result,
            AsyncContext<xbox::services::Result<void>>&& async
        ) noexcept
        {
            auto sharedThis{ std::dynamic_pointer_cast<ServiceCall>(result.Payload()) };

            if (Failed(result))
            {
                return async.Complete(result);
            }

            auto httpResult{ result.Payload()->Result() };
            switch (httpResult)
            {
            case HTTP_E_STATUS_BAD_REQUEST:
                // Retrying 400 unlikely to resolve the issue so just treat that the same as
                // a success and complete the async operation
            case HTTP_E_STATUS_DENIED:
                // XblHttpCall already retries 401 so don't do it again here.
            case S_OK:
            {
                return async.Complete(S_OK);
            }
            default:
            {
                // For other failures, backoff and retry
                auto backoff{ __min(std::pow(2, ++sharedThis->m_iteration), 60) * 1000 };

                async.Queue().RunWork([ sharedThis, async ]
                {
                    HRESULT hr = sharedThis->ResetAndCopyForRetry();
                    if (FAILED(hr))
                    {
                        return async.Complete(hr);
                    }

                    hr = sharedThis->XblHttpCall::Perform(AsyncContext<HttpResult>{ async.Queue().DeriveWorkerQueue(),
                        [async](HttpResult httpResult) mutable
                        {
                            HandleHttpResult(std::move(httpResult), std::move(async));
                        }
                    });

                    if (FAILED(hr))
                    {
                        return async.Complete(hr);
                    }
                },
                backoff);
            }
            }
        }

        uint32_t m_iteration{ 0 };
        std::shared_ptr<GlobalState> m_globalState{ nullptr };
    };

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto serviceCall = MakeShared<ServiceCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(serviceCall->Init(m_xboxLiveContextSettings, m_titleId));

    JsonDocument requestBody{ rapidjson::kObjectType };
    auto& a{ requestBody.GetAllocator() };

    JsonValue recentPlayers{ rapidjson::kArrayType };
    for (auto& update : m_pendingRecentPlayerUpdates)
    {
        JsonValue player{ rapidjson::kObjectType };
        player.AddMember("id", JsonValue{ utils::uint64_to_internal_string(update.first).data(), a }.Move(), a);
        player.AddMember("timestamp", JsonUtils::SerializeTime(update.second.timestamp, a).Move(), a);
        player.AddMember("encounterType", JsonValue{ EnumName(update.second.encounterType).data(), a }.Move(), a);
        recentPlayers.PushBack(player.Move(), a);
    }

    requestBody.AddMember("recentPlayers", recentPlayers.Move(), a);

    RETURN_HR_IF_FAILED(serviceCall->SetRequestBody(requestBody));
    RETURN_HR_IF_FAILED(serviceCall->PerformWithRetry(std::move(async)));

    m_pendingRecentPlayerUpdates.clear();
    return S_OK;
}

uint64_t MultiplayerActivityService::GetSequenceNumber()
{
    uint64_t dateTime = xbox::services::datetime::utc_now().to_interval(); // eg. 131472330440000000
    const uint64_t dateTimeFromJan1st2015 = 130645440000000000;
    if (dateTime < dateTimeFromJan1st2015)
    {
        return static_cast<int64_t>(time(nullptr)); // Clock is wrong and is not yet sync'd with internet time so just revert to old logic
    }
    else
    {
        uint64_t dateTimeSince2015 = dateTime - dateTimeFromJan1st2015; // eg. 826888900000000
        uint64_t dateTimeTrimmed = dateTimeSince2015 >> 16; // divide by 2^16 to get it to sub second range.  eg. 12617323303
        return dateTimeTrimmed;
    }
}

HRESULT MultiplayerActivityService::SetActivity(
    _In_ const ActivityInfo& info,
    _In_ bool allowCrossPlatformJoin,
    _In_ AsyncContext<HRESULT> async
) const noexcept
{
    Stringstream path{};
    path << "/titles/" << m_titleId << "/users/" << m_user.Xuid() << "/activities";

    JsonDocument requestBody{ rapidjson::kObjectType };
    auto& a{ requestBody.GetAllocator() };

    requestBody.AddMember("sequenceNumber", GetSequenceNumber(), a);
    requestBody.AddMember("connectionString", JsonValue{ info.connectionString.data(), a }, a);
    requestBody.AddMember("joinRestriction", JsonValue{ EnumName(info.joinRestriction).data(), a }, a);
    if (info.maxPlayers)
    {
        requestBody.AddMember("maxPlayers", static_cast<uint64_t>(info.maxPlayers), a);
    }
    if (info.currentPlayers)
    {
        requestBody.AddMember("currentPlayers", static_cast<uint64_t>(info.currentPlayers), a);
    }
    if (!info.groupId.empty())
    {
        requestBody.AddMember("groupId", JsonValue{ info.groupId.data(), a }, a);
    }
    if (!allowCrossPlatformJoin)
    {
        requestBody.AddMember("platform", JsonValue{ PlatformName(GetLocalPlatform()).data(), a }, a);
    }

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "PUT",
        XblHttpCall::BuildUrl(MPA_SERVICE_NAME, path.str()),
        xbox_live_api::set_activity
    ));

    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(requestBody));

    return httpCall->Perform({
        async.Queue().DeriveWorkerQueue(),
        [
            async
        ]
    (HttpResult httpResult)
        {
            HRESULT hr{ Failed(httpResult) ? httpResult.Hresult() : httpResult.Payload()->Result() };
            async.Complete(hr);
        }
        });
}

HRESULT MultiplayerActivityService::GetActivity(
    _In_ const Vector<uint64_t>& xuids,
    _In_ AsyncContext<Result<Vector<ActivityInfo>>> async
) const noexcept
{
    Stringstream path{};
    path << "/titles/" << m_titleId << "/activities/query";

    JsonDocument requestBody{ rapidjson::kObjectType };
    auto& a{ requestBody.GetAllocator() };

    JsonValue userList;
    JsonUtils::SerializeVector(JsonUtils::JsonXuidSerializer, xuids, userList, a);
    requestBody.AddMember("users", userList.Move(), a);

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "POST",
        XblHttpCall::BuildUrl(MPA_SERVICE_NAME, path.str()),
        xbox_live_api::get_activity_batch
    ));

    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(requestBody));

    return httpCall->Perform({
        async.Queue().DeriveWorkerQueue(),
        [
            async,
            titleId{ m_titleId }
        ]
    (HttpResult httpResult)
        {
            HRESULT hr{ Failed(httpResult) ? httpResult.Hresult() : httpResult.Payload()->Result() };
            if (SUCCEEDED(hr))
            {
                return async.Complete(ActivityInfo::Deserialize(httpResult.Payload()->GetResponseBodyJson(), titleId));
            }
            return async.Complete(hr);
        }
        });
}

HRESULT MultiplayerActivityService::DeleteActivity(
    _In_ AsyncContext<HRESULT> async
) const noexcept
{
    Stringstream path;
    path << "/titles/" << m_titleId << "/users/" << m_user.Xuid() << "/activities";

    JsonDocument requestBody{ rapidjson::kObjectType };
    auto& a{ requestBody.GetAllocator() };

    requestBody.AddMember("sequenceNumber", GetSequenceNumber(), a);

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "DELETE",
        XblHttpCall::BuildUrl(MPA_SERVICE_NAME, path.str()),
        xbox_live_api::delete_activity
    ));

    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(requestBody));

    return httpCall->Perform({
        async.Queue().DeriveWorkerQueue(),
        [
            async
        ]
    (HttpResult httpResult)
        {
            HRESULT hr{ Failed(httpResult) ? httpResult.Hresult() : httpResult.Payload()->Result() };
            async.Complete(hr);
        }
        });
}

HRESULT MultiplayerActivityService::SendInvites(
    _In_ const Vector<uint64_t>& xuids,
    _In_ bool allowCrossPlatformJoin,
    _In_ const String& connectionString,
    _In_ AsyncContext<HRESULT> async
) const noexcept
{
    Stringstream path;
    path << "/titles/" << m_titleId << "/invites";

    JsonDocument requestBody{ rapidjson::kObjectType };
    auto& a{ requestBody.GetAllocator() };

    if (!allowCrossPlatformJoin)
    {
        requestBody.AddMember("platform", JsonValue{ PlatformName(GetLocalPlatform()).data(), a }, a);
    }
    if (!connectionString.empty())
    {
        requestBody.AddMember("connectionString", JsonValue{ connectionString.data(), a }, a);
    }
    JsonValue invitedUsersArray{ rapidjson::kArrayType };
    for (auto xuid : xuids)
    {
        auto xuidString{ utils::uint64_to_internal_string(xuid) };
        invitedUsersArray.PushBack(JsonValue{ xuidString.data(), a }, a);
    }
    requestBody.AddMember("invitedUsers", invitedUsersArray.Move(), a);

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "POST",
        XblHttpCall::BuildUrl(MPA_SERVICE_NAME, path.str()),
        xbox_live_api::mpa_send_invites
    ));

    RETURN_HR_IF_FAILED(httpCall->SetRequestBody(requestBody));

    return httpCall->Perform({
        async.Queue().DeriveWorkerQueue(),
        [
            async
        ]
    (HttpResult httpResult)
        {
            HRESULT hr{ Failed(httpResult) ? httpResult.Hresult() : httpResult.Payload()->Result() };
            async.Complete(hr);
        }
        });
}

void MultiplayerActivityService::ScheduleRecentPlayersUpdate() noexcept
{
    auto hr = m_queue.RunWork([weakThis = std::weak_ptr<MultiplayerActivityService>{ shared_from_this() }]
        {
            auto pThis{ weakThis.lock() };
            if (pThis)
            {
                pThis->FlushRecentPlayers({ pThis->m_queue, [ pThis ] (Result<void> result)
                {
                    // FlushRecentPlayers already has retry logic. If we get to this point, just log the error
                    // and schedule the next upload.
                    if (Failed(result))
                    {
                        LOGS_ERROR << "MultiplayerActivity::FlushRecentPlayers failed with HRESULT " << result.Hresult();
                    }
                    pThis->ScheduleRecentPlayersUpdate();
                } });
            }
        },
        RECENT_PLAYERS_UPLOAD_INTERVAL_MS
    );

    if (FAILED(hr))
    {
        // Not much we can do if RunWork fails so just log the error and return
        LOGS_ERROR << __FUNCTION__ << " failed with HRESULT " << hr;
    }
}

XblMultiplayerActivityPlatform MultiplayerActivityService::GetLocalPlatform() noexcept
{
#if HC_PLATFORM == HC_PLATFORM_GDK
    auto GDKDeviceType{ XSystemGetDeviceType() };
    switch (GDKDeviceType)
    {
    case XSystemDeviceType::Pc:
    {
        return XblMultiplayerActivityPlatform::WindowsOneCore;
    }
    case XSystemDeviceType::XboxOne:
    case XSystemDeviceType::XboxOneS:
    case XSystemDeviceType::XboxOneX:
    case XSystemDeviceType::XboxOneXDevkit:
    {
        return XblMultiplayerActivityPlatform::XboxOne;
    }
    case XSystemDeviceType::XboxScarlettLockhart:
    case XSystemDeviceType::XboxScarlettAnaconda:
    case XSystemDeviceType::XboxScarlettDevkit:
    {
        return XblMultiplayerActivityPlatform::Scarlett;
    }
    case XSystemDeviceType::Unknown:
    default:
    {
        LOGS_DEBUG << "Unable to detect GDK device type";
        assert(false);
        return XblMultiplayerActivityPlatform::Unknown;
    }
    }
#else
    constexpr XblMultiplayerActivityPlatform localPlatform
    {
    #if HC_PLATFORM == HC_PLATFORM_WIN32
        XblMultiplayerActivityPlatform::Win32
    #elif HC_PLATFORM == HC_PLATFORM_UWP
        XblMultiplayerActivityPlatform::WindowsOneCore
    #elif HC_PLATFORM == HC_PLATFORM_XDK
        XblMultiplayerActivityPlatform::XboxOne
    #elif HC_PLATFORM == HC_PLATFORM_ANDROID
        XblMultiplayerActivityPlatform::Android
    #elif HC_PLATFORM == HC_PLATFORM_IOS
        XblMultiplayerActivityPlatform::iOS
    #elif HC_PLATFORM == HC_PLATFORM_MAC
        XblMultiplayerActivityPlatform::MacOS
    #elif HC_PLATFORM == HC_PLATFORM_NINTENDO_SWITCH
        XblMultiplayerActivityPlatform::Nintendo
    #elif HC_PLATFORM == HC_PLATFORM_SONY_PLAYSTATION_4
        XblMultiplayerActivityPlatform::PlayStation
    #else
        XblMultiplayerActivityPlatform::Unknown
    #endif
    };

    static_assert(localPlatform != XblMultiplayerActivityPlatform::Unknown, "Unable to detect platform");

    return localPlatform;
#endif
}

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
