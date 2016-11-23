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
#include "SocialEvent_WinRT.h"
#include "Utils_WinRT.h"
#include "SocialUserGroupLoadedEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN
using namespace xbox::services;
using namespace xbox::services::social::manager;
using namespace Microsoft::Xbox::Services::System;

SocialEvent::SocialEvent(
    _In_ xbox::services::social::manager::social_event cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_user = user_context::user_convert(m_cppObj.user());
    auto trackedUsers = m_cppObj.users_affected();
    auto vector = ref new Platform::Collections::Vector<Platform::String^>();
    for (auto& user : trackedUsers)
    {
        vector->Append(ref new Platform::String(user.xbox_user_id()));
    }

    m_usersAffected = vector->GetView();

    if (m_cppObj.err())
    {
        m_errorCode = ref new Platform::Exception(xbox::services::utils::convert_xbox_live_error_code_to_hresult(m_cppObj.err()));
    }

    m_errorMessage = ref new Platform::String(utility::conversions::utf8_to_utf16(m_cppObj.err_message()).c_str());
}

XboxLiveUser_t
SocialEvent::User::get()
{
    return m_user;
}


Windows::Foundation::Collections::IVectorView<Platform::String^>^
SocialEvent::UsersAffected::get()
{
    return m_usersAffected;
}

int
SocialEvent::ErrorCode::get()
{
    if (m_errorCode == nullptr)
    {
        return 0;
    }

    return m_errorCode->HResult;
}

Platform::String^
SocialEvent::ErrorMessage::get()
{
    return m_errorMessage;
}

SocialEventArgs^ SocialEvent::EventArgs::get()
{
    switch (m_cppObj.event_type())
    {
    case social_event_type::social_user_group_loaded:
        return ref new SocialUserGroupLoadedEventArgs(std::dynamic_pointer_cast<social_user_group_loaded_event_args>(m_cppObj.event_args()));
    default:
        return nullptr;

    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END