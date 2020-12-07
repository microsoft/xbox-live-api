// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "public_utils.h"
#include "xsapi-c/events_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_BEGIN

events_service::events_service(_In_ XblContextHandle contextHandle)
{
    XblContextDuplicateHandle(contextHandle, &m_xblContext);
}

events_service::events_service(const events_service& other)
{
    XblContextDuplicateHandle(other.m_xblContext, &m_xblContext);
}

events_service& events_service::operator=(events_service other)
{
    std::swap(m_xblContext, other.m_xblContext);
    return *this;
}

events_service::~events_service()
{
    XblContextCloseHandle(m_xblContext);
}

xbox_live_result<void> events_service::write_in_game_event(
    _In_ const string_t& eventName
)
{
    return xbox_live_result<void>(Utils::ConvertHr(XblEventsWriteInGameEvent(m_xblContext, Utils::StringFromStringT(eventName).data(), nullptr, nullptr)));
}

xbox_live_result<void> events_service::write_in_game_event(
    _In_ const string_t& eventName,
    _In_ const web::json::value& dimensionsJson,
    _In_ const web::json::value& measurementJson
)
{
    return xbox_live_result<void>(Utils::ConvertHr(XblEventsWriteInGameEvent(
        m_xblContext,
        Utils::StringFromStringT(eventName).data(),
        Utils::StringFromStringT(dimensionsJson.serialize()).data(),
        Utils::StringFromStringT(measurementJson.serialize()).data()
    )));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END