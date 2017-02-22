// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Utils_WinRT.h"
#include "SocialManagerPresenceRecord_WinRT.h"

using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

SocialManagerPresenceRecord::SocialManagerPresenceRecord(
    _In_ xbox::services::social::manager::social_manager_presence_record cppObj
    ) : m_cppObj(std::move(cppObj))
{
}

Windows::Foundation::Collections::IVectorView<SocialManagerPresenceTitleRecord^>^
SocialManagerPresenceRecord::PresenceTitleRecords::get()
{
    auto socialManagerPresenceTitleRecordVec = ref new Platform::Collections::Vector<SocialManagerPresenceTitleRecord^>();
    for (auto& presenceTitleRecord : m_cppObj.presence_title_records())
    {
        if (!presenceTitleRecord._Is_null())
        {
            socialManagerPresenceTitleRecordVec->Append(ref new SocialManagerPresenceTitleRecord(presenceTitleRecord));
        }
    }
    return socialManagerPresenceTitleRecordVec->GetView();
}

bool
SocialManagerPresenceRecord::IsUserPlayingTitle(_In_ uint32_t titleId)
{
    return m_cppObj.is_user_playing_title(titleId);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END