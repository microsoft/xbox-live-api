// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "user_context.h"
#include "xsapi/real_time_activity.h"
#include "real_time_activity_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_BEGIN

std::shared_ptr<real_time_activity_service_factory>
real_time_activity_service_factory::get_singleton_instance()
{
    auto xsapiSingleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> guard(xsapiSingleton->m_singletonLock);
    if (xsapiSingleton->m_rtaFactoryInstance == nullptr)
    {
        xsapiSingleton->m_rtaFactoryInstance = std::make_shared<real_time_activity_service_factory>();
    }

    return xsapiSingleton->m_rtaFactoryInstance;
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
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_singletonLock);
    XSAPI_ASSERT(userContext != nullptr);

    auto xboxUserId = utils::string_t_from_internal_string(userContext->xbox_user_id());
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
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_singletonLock);
    XSAPI_ASSERT(userContext != nullptr);
    auto xuid = utils::string_t_from_internal_string(userContext->xbox_user_id());
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