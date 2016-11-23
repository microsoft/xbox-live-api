//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once

namespace xbox { namespace services { namespace real_time_activity {

struct real_time_activity_service_factory_counter
{
    real_time_activity_service_factory_counter() : counter(1) {}

    uint32_t counter;
    std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> rtaService;
};

class real_time_activity_service_factory
{
public:
    real_time_activity_service_factory();

    static std::shared_ptr<real_time_activity_service_factory> get_singleton_instance();

    const std::shared_ptr<real_time_activity_service>& get_rta_instance(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    void remove_user_from_rta_map(
        _In_ std::shared_ptr<xbox::services::user_context> userContext
        );

private:
    std::unordered_map <string_t, real_time_activity_service_factory_counter> m_xuidToRTAMap;
};

}}}