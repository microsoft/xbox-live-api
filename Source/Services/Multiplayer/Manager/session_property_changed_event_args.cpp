// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/multiplayer_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

session_property_changed_event_args::session_property_changed_event_args(
    _In_ web::json::value jsonProperties
    ):
    m_properties(std::move(jsonProperties))
{
}

const web::json::value&
session_property_changed_event_args::properties()
{
    return m_properties;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END