// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/multiplayer_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

user_added_event_args::user_added_event_args(
    _In_ string_t xboxUserId
    ):
    m_xboxUserid(std::move(xboxUserId))
{
}

const string_t&
user_added_event_args::xbox_user_id() const
{
    return m_xboxUserid;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END