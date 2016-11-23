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

#include "pch.h"
#include "xsapi/multiplayer_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

member_property_changed_event_args::member_property_changed_event_args(
    _In_ std::shared_ptr<multiplayer_member> member,
    _In_ web::json::value jsonProperties
    ):
    m_member(std::move(member)),
    m_properties(std::move(jsonProperties))
{
}

std::shared_ptr<multiplayer_member>
member_property_changed_event_args::member()
{
    return m_member;
}

web::json::value
member_property_changed_event_args::properties()
{
    return m_properties;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END