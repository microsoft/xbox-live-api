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
#include "xsapi/system.h"
#include "notification_service.h"
#include "xbox_system_factory.h"
#include "user_context.h"
#include "user_impl.h"
#import "XBLiOSGlobalState.h"

using namespace pplx;
using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN

pplx::task<xbox_live_result<void>>
notification_service_ios::subscribe_to_notifications()
{
    string_t endpointIdCache = xbox_system_factory::get_factory()->create_local_config()->get_value_from_local_storage(ENDPOINT_ID_CACHE_NAME);
    if (!endpointIdCache.empty())
    {
        unsubscribe_from_notifications(endpointIdCache).get();
    }
    string_t registrationToken = [XBLiOSGlobalState notificationRegistrationToken];
    if(registrationToken.empty())
    {
        LOG_INFO("No APNS device token");
        return pplx::task_from_result(xbox_live_result<void>(xbox_live_error_code::runtime_error, "No APNS device token"));
    }
    std::weak_ptr<notification_service_ios> thisWeak = std::dynamic_pointer_cast<notification_service_ios>(shared_from_this());
    return create_task([thisWeak, registrationToken]()
    {
        std::shared_ptr<notification_service_ios> pThis = thisWeak.lock();
        if (pThis != nullptr)
        {
            std::vector<notification_filter> notificationFilterList;
            notification_filter notificationFilter = { notification_filter_source_type::multiplayer, 1 };
            notificationFilterList.push_back(notificationFilter);
            
            return pThis->subscribe_to_notifications_helper(utils::create_guid(true),
                                                     registrationToken,
                                                     _T("iOS"),
                                                     _T("iOSDevice"),
                                                     _T("XSAPI_I"),
                                                     notificationFilterList
                                                     ).get();
        }
        else
        {
            return xbox_live_result<void>(xbox_live_error_code::runtime_error);
        }
    });
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END
