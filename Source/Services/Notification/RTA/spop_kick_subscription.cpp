#include "pch.h"
#include "spop_kick_subscription.h"
#include "real_time_activity_manager.h"
#include "multiplayer_internal.h"

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN

SpopKickSubscription::SpopKickSubscription(
    _In_ User user,
    _In_ uint32_t titleId
) noexcept : 
    m_user(user)
{
    Stringstream ss;
    ss << "https://notify.xboxlive.com/users/xuid(" << m_user.Xuid() << ")/deviceId/current/titleId/" << titleId;
    m_resourceUri = ss.str();
}

const String& SpopKickSubscription::ResourceUri() const noexcept
{
    return m_resourceUri;
}

void SpopKickSubscription::OnEvent(
    _In_ const JsonValue& data
) noexcept
{
    auto datastr = JsonUtils::SerializeJson(data);
    UNREFERENCED_PARAMETER(datastr);
    if (data.IsObject() && data.HasMember("KickNotification"))
    {        
        auto args = std::make_unique<XalUserGetTokenAndSignatureArgs>();
        args->forceRefresh = true;
        args->url = "https://xboxlive.com/";
        args->method = "GET";

        auto async = std::make_unique<XAsyncBlock>();
        async->callback = [](XAsyncBlock* async)
        {
            size_t bufferSize{};
            auto h = XalUserGetTokenAndSignatureSilentlyResultSize(async, &bufferSize);
            UNREFERENCED_PARAMETER(h);
        };

        if (SUCCEEDED(XalUserGetTokenAndSignatureSilentlyAsync(m_user.Handle(), args.get(), async.get())))
        {
            args.release();
            async.release();
        }
    }
}

void SpopKickSubscription::OnResync() noexcept
{
    // Don't think there is much we can do here - just log an error 
    LOGS_ERROR << __FUNCTION__ << ": SPOP notifications event may have been discarded by RTA service";
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END
#endif