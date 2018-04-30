// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social_manager.h"
#include "social_manager_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

std::shared_ptr<social_manager>
social_manager::get_singleton_instance()
{
    auto xsapiSingleton = get_xsapi_singleton();
    std::lock_guard<std::recursive_mutex> lock(xsapiSingleton->m_socialManagerLock);
    if (xsapiSingleton->m_socialManagerInstance == nullptr)
    {
        auto buffer = xbox::services::system::xsapi_memory::mem_alloc(sizeof(social_manager));
        xsapiSingleton->m_socialManagerInstance = std::shared_ptr<social_manager>(
            new (buffer) social_manager(),
            xsapi_alloc_deleter<social_manager>()
            );
    }
    return xsapiSingleton->m_socialManagerInstance;
}

social_manager::social_manager()
{
    m_internalObj = social_manager_internal::get_singleton_instance();
}

xbox_live_result<std::shared_ptr<xbox_social_user_group>>
social_manager::create_social_user_group_from_filters(
    _In_ xbox_live_user_t user,
    _In_ presence_filter presenceFilterLevel,
    _In_ relationship_filter relationshipFilter
    )
{
    auto internalResult = m_internalObj->create_social_user_group_from_filters(user, presenceFilterLevel, relationshipFilter);

    return xbox_live_result<std::shared_ptr<xbox_social_user_group>>(
        xsapi_allocate_shared<xbox_social_user_group>(internalResult.payload()),
        internalResult.err(),
        internalResult.err_message()
        );
}

xbox_live_result<std::shared_ptr<xbox_social_user_group>>
social_manager::create_social_user_group_from_list(
    _In_ xbox_live_user_t user,
    _In_ std::vector<string_t> xboxUserIdList
)
{
    auto internalResult = m_internalObj->create_social_user_group_from_list(user, utils::internal_string_vector_from_std_string_vector(xboxUserIdList));

    return xbox_live_result<std::shared_ptr<xbox_social_user_group>>(
        xsapi_allocate_shared<xbox_social_user_group>(internalResult.payload()),
        internalResult.err(),
        internalResult.err_message()
        );
}

xbox_live_result<void>
social_manager::destroy_social_user_group(
    _In_ std::shared_ptr<xbox_social_user_group> socialGroup
    )
{
    return m_internalObj->destroy_social_user_group(socialGroup->m_internalObj);
}

xbox_live_result<void>
social_manager::add_local_user(
    _In_ xbox_live_user_t user,
    _In_ social_manager_extra_detail_level extraDetailLevel
    )
{
    return m_internalObj->add_local_user(user, extraDetailLevel);
}

xbox_live_result<void>
social_manager::remove_local_user(
    _In_ xbox_live_user_t user
    )
{
    return m_internalObj->remove_local_user(user);
}

std::vector<social_event>
social_manager::do_work()
{
    return utils::std_vector_external_from_internal_vector<social_event, std::shared_ptr<social_event_internal>>(
        m_internalObj->do_work()
        );
}

std::vector<xbox_live_user_t>
social_manager::local_users() const
{
    return utils::std_vector_from_internal_vector(m_internalObj->local_users());
}

xbox_live_result<void> social_manager::update_social_user_group(
    _In_ const std::shared_ptr<xbox_social_user_group>& socialGroup,
    _In_ const std::vector<string_t>& users
    )
{
    return m_internalObj->update_social_user_group(
        socialGroup->m_internalObj,
        utils::internal_string_vector_from_std_string_vector(users)
        );
}

xbox_live_result<void>
social_manager::set_rich_presence_polling_status(
    _In_ xbox_live_user_t user,
    _In_ bool shouldEnablePolling
    )
{
    return m_internalObj->set_rich_presence_polling_status(user, shouldEnablePolling);
}

void 
social_manager::set_diagnostics_trace_level(
    _In_ xbox_services_diagnostics_trace_level traceLevel
    )
{
    m_internalObj->set_diagnostics_trace_level(traceLevel);
}

void social_manager::_Log_state()
{
    m_internalObj->log_state();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END
