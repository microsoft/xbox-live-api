// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "utils.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_title_request::presence_title_request(
    _In_ bool isUserActive,
    _In_ presence_data_internal presenceData,
    _In_ media_presence_data mediaPresenceData
    ) :
    m_isUserActive(isUserActive)
{
    m_presenceActivityData = presence_activity_data(
        std::move(presenceData),
        std::move(mediaPresenceData)
        );
}

web::json::value 
presence_title_request::serialize()
{
    web::json::value serializedObject;
    string_t state = m_isUserActive ? _T("active") : _T("inactive");
    serializedObject[_T("state")] = web::json::value::string(state);

    if (m_presenceActivityData.should_serialize())
    {
        serializedObject[_T("activity")] = m_presenceActivityData.serialize();
    }

    return serializedObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END