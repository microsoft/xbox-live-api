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
