// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "utils.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_activity_data::presence_activity_data() : 
    m_shouldSerialize(false)
{
}

presence_activity_data::presence_activity_data(
    _In_ presence_data_internal presenceData,
    _In_ media_presence_data mediaPresenceData
    ) :
    m_presenceData(std::move(presenceData)),
    m_mediaPresenceData(std::move(mediaPresenceData))
{
    m_shouldSerialize = (m_presenceData.should_serialize() || m_mediaPresenceData.should_serialize());
}

bool
presence_activity_data::should_serialize() const
{
    return m_shouldSerialize;
}

web::json::value
presence_activity_data::serialize() const
{
    web::json::value serializedObject;

    if (m_presenceData.should_serialize())
    {
        serializedObject[_T("richPresence")] = m_presenceData.serialize();
    }
    if (m_mediaPresenceData.should_serialize())
    {
        serializedObject[_T("media")] = m_mediaPresenceData.serialize();
    }

    return serializedObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END