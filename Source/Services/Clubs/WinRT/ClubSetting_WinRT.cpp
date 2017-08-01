// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ClubSetting_WinRT.h"
#include "xsapi/clubs.h"
#include "Utils_WinRT.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace xbox::services::clubs;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

ClubStringSetting::ClubStringSetting(_In_ xbox::services::clubs::club_setting<string_t> cppObj)
    : m_cppObj(std::move(cppObj))
{
    if (m_cppObj.allowed_values().size() > 0)
    {
        m_allowedValues = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.allowed_values());
    }
}

IVectorView<String^>^ ClubStringSetting::AllowedValues::get()
{
    return m_allowedValues ? m_allowedValues->GetView() : nullptr;
}

ClubStringMultiSetting::ClubStringMultiSetting(_In_ xbox::services::clubs::club_multi_setting<string_t> cppObj)
    : m_cppObj(std::move(cppObj))
{
    m_values = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.values());
    if (m_cppObj.allowed_values().size() > 0)
    {
        m_allowedValues = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.allowed_values());
    }
}

IVectorView<String^>^ ClubStringMultiSetting::Values::get()
{
    return m_values->GetView();
}

IVectorView<String^>^ ClubStringMultiSetting::AllowedValues::get()
{
    return m_allowedValues ? m_allowedValues->GetView() : nullptr;
}

ClubBooleanSetting::ClubBooleanSetting(_In_ xbox::services::clubs::club_setting<bool> cppObj)
    : m_cppObj(std::move(cppObj))
{
}

ClubActionSetting::ClubActionSetting(_In_ xbox::services::clubs::club_action_setting cppObj)
    : m_cppObj(std::move(cppObj))
{
    if (m_cppObj.allowed_values().size() > 0)
    {
        m_allowedValues = UtilsWinRT::CreatePlatformVectorFromStdVectorEnum<ClubRole, club_role>(m_cppObj.allowed_values());
    }
}

IVectorView<ClubRole>^ ClubActionSetting::AllowedValues::get()
{
    return m_allowedValues ? m_allowedValues->GetView() : nullptr;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END