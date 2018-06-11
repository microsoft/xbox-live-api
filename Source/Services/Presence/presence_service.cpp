// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "utils.h"
#include "presence_internal.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_service::presence_service(
    _In_ std::shared_ptr<presence_service_internal> internalService
    ) :
    m_presenceServiceInternal(internalService)
{
}

task<xbox_live_result<void>>
presence_service::set_presence(
    _In_ bool isUserActiveInTitle
    )
{
    return set_presence(isUserActiveInTitle, presence_data());
}

task<xbox_live_result<void>>
presence_service::set_presence(
    _In_ bool isUserActiveInTitle,
    _In_ presence_data presenceData
    )
{
    task_completion_event<xbox_live_result<void>> tce;

    auto result = m_presenceServiceInternal->set_presence(
        isUserActiveInTitle,
        presence_data_internal(presenceData),
        get_xsapi_singleton()->m_asyncQueue, 
        [tce](xbox_live_result<uint32_t> result)
    {
        tce.set(xbox_live_result<void>(result.err(), result.err_message()));
    });

    if (result.err())
    {
        return pplx::task_from_result(result);
    }
    return pplx::task<xbox_live_result<void>>(tce);
}

std::shared_ptr<xbox_live_context_settings>
presence_service::_Xbox_live_context_settings()
{
    return m_presenceServiceInternal->xbox_live_context_settings();
}

task<xbox_live_result<presence_record>> 
presence_service::get_presence(
    _In_ const string_t& xboxUserId
    )
{
    task_completion_event<xbox_live_result<presence_record>> tce;

    auto result = m_presenceServiceInternal->get_presence(
        utils::internal_string_from_string_t(xboxUserId),
        get_xsapi_singleton()->m_asyncQueue,
        [tce](xbox_live_result<std::shared_ptr<presence_record_internal>> result)
    {
        tce.set(CREATE_EXTERNAL_XBOX_LIVE_RESULT(presence_record, result));
    });

    if (result.err())
    {
        return pplx::task_from_result(xbox_live_result<presence_record>(result.err(), result.err_message()));
    }
    return pplx::task<xbox_live_result<presence_record>>(tce);
}

task<xbox_live_result<std::vector<presence_record>>>
presence_service::get_presence_for_multiple_users(
    _In_ const std::vector<string_t>& xboxUserIds
    )
{
    return get_presence_for_multiple_users(
        xboxUserIds,
        std::vector<presence_device_type>(),
        std::vector<uint32_t>(),
        presence_detail_level::default_level,
        false,
        false
        );
}

xbox_live_result<std::vector<presence_record>> create_external_presence_records_result(
    _In_ xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>> result
)
{
    if (result.err())
    {
        return xbox_live_result<std::vector<presence_record>>(result.err(), result.err_message());
    }

    auto vector = utils::std_vector_external_from_internal_vector<presence_record, std::shared_ptr<presence_record_internal>>(result.payload());
    return xbox_live_result<std::vector<presence_record>>(
        vector,
        result.err(),
        result.err_message()
        );
}

task<xbox_live_result<std::vector<presence_record>>>
presence_service::get_presence_for_multiple_users(
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const std::vector<presence_device_type>& deviceTypes,
    _In_ const std::vector<uint32_t>& titleIds,
    _In_ presence_detail_level presenceDetailLevel,
    _In_ bool onlineOnly,
    _In_ bool broadcastingOnly
    )
{
    task_completion_event<xbox_live_result<std::vector<presence_record>>> tce;

    auto result = m_presenceServiceInternal->get_presence_for_multiple_users(
        utils::internal_string_vector_from_std_string_vector(xboxUserIds),
        utils::internal_vector_from_std_vector(deviceTypes),
        utils::internal_vector_from_std_vector(titleIds),
        presenceDetailLevel,
        onlineOnly,
        broadcastingOnly,
        get_xsapi_singleton()->m_asyncQueue,
        [tce](xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>> result)
    {
        tce.set(create_external_presence_records_result(result));
    });

    if (result.err())
    {
        return pplx::task_from_result(xbox_live_result<std::vector<presence_record>>(result.err(), result.err_message()));
    }
    return pplx::task<xbox_live_result<std::vector<presence_record>>>(tce);
}

task<xbox_live_result<std::vector<presence_record>>> 
presence_service::get_presence_for_social_group(
    _In_ const string_t& socialGroup
    )
{
    task_completion_event<xbox_live_result<std::vector<presence_record>>> tce;

    auto result = m_presenceServiceInternal->get_presence_for_social_group(
        utils::internal_string_from_string_t(socialGroup),
        get_xsapi_singleton()->m_asyncQueue,
        [tce](xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>> result)
    {
        tce.set(create_external_presence_records_result(result));
    });

    if (result.err())
    {
        return pplx::task_from_result(xbox_live_result<std::vector<presence_record>>(result.err(), result.err_message()));
    }
    return pplx::task<xbox_live_result<std::vector<presence_record>>>(tce);
}

task<xbox_live_result<std::vector<presence_record>>>
presence_service::get_presence_for_social_group(
    _In_ const string_t& socialGroup,
    _In_ const string_t& socialGroupOwnerXboxUserId,
    _In_ const std::vector<presence_device_type>& deviceTypes,
    _In_ const std::vector<uint32_t>& titleIds,
    _In_ presence_detail_level peoplehubDetailLevel,
    _In_ bool onlineOnly,
    _In_ bool broadcastingOnly
    )
{
    task_completion_event<xbox_live_result<std::vector<presence_record>>> tce;

    auto result = m_presenceServiceInternal->get_presence_for_social_group(
        utils::internal_string_from_string_t(socialGroup),
        utils::internal_string_from_string_t(socialGroupOwnerXboxUserId),
        utils::internal_vector_from_std_vector(deviceTypes),
        utils::internal_vector_from_std_vector(titleIds),
        peoplehubDetailLevel,
        onlineOnly,
        broadcastingOnly,
        get_xsapi_singleton()->m_asyncQueue,
        [tce](xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>> result)
    {
        tce.set(create_external_presence_records_result(result));
    });

    if (result.err())
    {
        return pplx::task_from_result(xbox_live_result<std::vector<presence_record>>(result.err(), result.err_message()));
    }
    return pplx::task<xbox_live_result<std::vector<presence_record>>>(tce);
}

function_context
presence_service::add_device_presence_changed_handler(
    _In_ std::function<void(const device_presence_change_event_args&)> handler
    )
{
    return m_presenceServiceInternal->add_device_presence_changed_handler(
        [handler](std::shared_ptr<device_presence_change_event_args_internal> args)
    {
        handler(device_presence_change_event_args(args));
    });
}

void
presence_service::remove_device_presence_changed_handler(
    _In_ function_context context
    )
{
    return m_presenceServiceInternal->remove_device_presence_changed_handler(
        context
        );
}

function_context
presence_service::add_title_presence_changed_handler(
    _In_ std::function<void(const title_presence_change_event_args&)> handler
    )
{
    return m_presenceServiceInternal->add_title_presence_changed_handler(
        [handler](std::shared_ptr<title_presence_change_event_args_internal> args)
    {
        handler(title_presence_change_event_args(args));
    });
}

void
presence_service::remove_title_presence_changed_handler(
    _In_ function_context context
    )
{
    m_presenceServiceInternal->remove_title_presence_changed_handler(
        context
        );
}

xbox_live_result<std::shared_ptr<device_presence_change_subscription>>
presence_service::subscribe_to_device_presence_change(
    _In_ const string_t& xboxUserId
    )
{
    auto result =  m_presenceServiceInternal->subscribe_to_device_presence_change(
        utils::internal_string_from_string_t(xboxUserId)
        );

    if (result.err())
    {
        return xbox_live_result<std::shared_ptr<device_presence_change_subscription>>(result.err(), result.err_message());
    }
    return xbox_live_result<std::shared_ptr<device_presence_change_subscription>>(
        xsapi_allocate_shared<device_presence_change_subscription>(result.payload())
        );
}

xbox_live_result<void>
presence_service::unsubscribe_from_device_presence_change(
    _In_ std::shared_ptr<device_presence_change_subscription> subscription
    )
{
    return m_presenceServiceInternal->unsubscribe_from_device_presence_change(
        subscription->m_internalObj
        );
}

xbox_live_result<std::shared_ptr<title_presence_change_subscription>>
presence_service::subscribe_to_title_presence_change(
    _In_ const string_t& xboxUserId,
    _In_ uint32_t titleId
    )
{
    auto result = m_presenceServiceInternal->subscribe_to_title_presence_change(
        utils::internal_string_from_string_t(xboxUserId),
        titleId
        );

    if (result.err())
    {
        return xbox_live_result<std::shared_ptr<title_presence_change_subscription>>(result.err(), result.err_message());
    }
    return xbox_live_result<std::shared_ptr<title_presence_change_subscription>>(
        xsapi_allocate_shared<title_presence_change_subscription>(result.payload())
        );
}

xbox_live_result<void>
presence_service::unsubscribe_from_title_presence_change(
    _In_ std::shared_ptr<title_presence_change_subscription> subscription
    )
{
    return m_presenceServiceInternal->unsubscribe_from_title_presence_change(
        subscription->m_internalObj
        );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END