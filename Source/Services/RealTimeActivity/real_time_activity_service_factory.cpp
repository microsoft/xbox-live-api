//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "user_context.h"
#include "xsapi/real_time_activity.h"
#include "real_time_activity_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_BEGIN
static std::mutex s_singletonLock;

std::shared_ptr<real_time_activity_service_factory>
real_time_activity_service_factory::get_singleton_instance()
{
    std::lock_guard<std::mutex> guard(s_singletonLock);
    static std::shared_ptr<real_time_activity_service_factory> instance;
    if (instance == nullptr)
    {
        instance = std::make_shared<real_time_activity_service_factory>();
    }

    return instance;
}

real_time_activity_service_factory::real_time_activity_service_factory()
{
}

const std::shared_ptr<real_time_activity_service>&
real_time_activity_service_factory::get_rta_instance(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    )
{
    std::lock_guard<std::mutex> guard(s_singletonLock);
    XSAPI_ASSERT(userContext != nullptr);

    auto xboxUserId = userContext->xbox_user_id();
    auto iter = m_xuidToRTAMap.find(xboxUserId);
    if (iter == m_xuidToRTAMap.end())
    {
        auto rtaService = std::shared_ptr<real_time_activity_service>(
            new real_time_activity_service(
                userContext,
                xboxLiveContextSettings,
                appConfig
            ));

        real_time_activity_service_factory_counter rtaImplCounter;
        rtaImplCounter.rtaService = rtaService;
        m_xuidToRTAMap[xboxUserId] = rtaImplCounter;
        iter = m_xuidToRTAMap.find(xboxUserId);
    }
    else
    {
        ++iter->second.counter;
    }

    return iter->second.rtaService;
}

void
real_time_activity_service_factory::remove_user_from_rta_map(
    _In_ std::shared_ptr<xbox::services::user_context> userContext
    )
{
    std::lock_guard<std::mutex> guard(s_singletonLock);
    XSAPI_ASSERT(userContext != nullptr);
    auto& xuid = userContext->xbox_user_id();
    if (!xuid.empty())
    {
        auto iter = m_xuidToRTAMap.find(xuid);
        if (iter == m_xuidToRTAMap.end())
        {
            return;
        }

        --iter->second.counter;
        if (iter->second.counter == 0)
        {
            m_xuidToRTAMap.erase(xuid);
        }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_END