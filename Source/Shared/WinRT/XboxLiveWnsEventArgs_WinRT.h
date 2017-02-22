// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "Macros_WinRT.h"
#include "xsapi/system.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

public ref class XboxLiveWnsEventArgs sealed
{
public:
    /// <summary>
    /// Returns the xbox user id for the WNS event
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// Returns the notification type
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(NotificationType, notification_type);


internal:
    XboxLiveWnsEventArgs(
        _In_ xbox::services::system::xbox_live_wns_event_args cppObj
    ) : m_cppObj(std::move(cppObj))
    {}

private:
    xbox::services::system::xbox_live_wns_event_args m_cppObj;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_END
