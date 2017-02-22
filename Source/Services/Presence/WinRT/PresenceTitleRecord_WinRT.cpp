// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "PresenceTitleRecord_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

PresenceTitleRecord::PresenceTitleRecord(
    _In_ xbox::services::presence::presence_title_record cppObj
    ) :
    m_cppObj(cppObj)
{
    m_broadcastRecord = ref new PresenceBroadcastRecord(m_cppObj.broadcast_record());
}

PresenceBroadcastRecord^ 
PresenceTitleRecord::BroadcastRecord::get()
{
    return m_broadcastRecord;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END