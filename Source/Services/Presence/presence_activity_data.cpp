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
#include "xsapi/presence.h"
#include "utils.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_activity_data::presence_activity_data() : 
    m_shouldSerialize(false)
{
}

presence_activity_data::presence_activity_data(
    _In_ presence_data presenceData,
    _In_ media_presence_data mediaPresenceData
    ) :
    m_presenceData(std::move(presenceData)),
    m_mediaPresenceData(std::move(mediaPresenceData))
{
    m_shouldSerialize = (m_presenceData._Should_serialize() || m_mediaPresenceData.should_serialize());
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

    if (m_presenceData._Should_serialize())
    {
        serializedObject[_T("richPresence")] = m_presenceData._Serialize();
    }
    if (m_mediaPresenceData.should_serialize())
    {
        serializedObject[_T("media")] = m_mediaPresenceData.serialize();
    }

    return serializedObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END