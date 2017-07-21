//--------------------------------------------------------------------------------------
// File: LiveResourcesXDK.cpp
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright(c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "pch.h"

#include "LiveResourcesXDK.h"
#include <collection.h>
#include <ppltasks.h>

using namespace Concurrency;
using namespace xbox::services;
using namespace xbox::services::system;

ATG::LiveResources::LiveResources(bool autoManageUser, bool isGuestUserAllowed) :
    m_autoManageUser(autoManageUser),
    m_isGuestUserAllowed(isGuestUserAllowed)
{
    auto appConfig = xbox_live_app_config::get_app_config_singleton();

    m_sandbox = appConfig->sandbox();
    m_scid = appConfig->scid();
    m_titleId = appConfig->title_id();

    wchar_t hexTitleId[16] = {};
    swprintf_s(hexTitleId, L"0x%08X", m_titleId);
    m_titleIdHex.assign(hexTitleId);
}

void ATG::LiveResources::Initialize()
{
    std::weak_ptr<LiveResources> thisWeakPtr = shared_from_this();

    if (m_autoManageUser)
    {
        Windows::Xbox::ApplicationModel::Core::CoreApplicationContext::CurrentUserChanged +=
            ref new Windows::Foundation::EventHandler<Platform::Object^>(
                [thisWeakPtr](Platform::Object^ sender, Platform::Object^ args)
        {
            std::shared_ptr<LiveResources> thisSharedPtr(thisWeakPtr.lock());
            if (thisSharedPtr)
            {
                thisSharedPtr->OnCurrentUserChanged();
            }
        });

        Windows::Xbox::System::User::SignInCompleted +=
            ref new Windows::Foundation::EventHandler<Windows::Xbox::System::SignInCompletedEventArgs^>(
                [thisWeakPtr](Platform::Object^ sender, Windows::Xbox::System::SignInCompletedEventArgs^ args)
        {
            std::shared_ptr<LiveResources> thisSharedPtr(thisWeakPtr.lock());
            if (thisSharedPtr)
            {
                thisSharedPtr->SetCurrentUser(args->User, true);
            }
        });
    }

    Windows::Xbox::System::User::SignOutCompleted +=
        ref new Windows::Foundation::EventHandler<Windows::Xbox::System::SignOutCompletedEventArgs^>(
            [thisWeakPtr](Platform::Object^ sender, Windows::Xbox::System::SignOutCompletedEventArgs^ args)
    {
        std::shared_ptr<LiveResources> thisSharedPtr(thisWeakPtr.lock());
        if (thisSharedPtr)
        {
            thisSharedPtr->OnSignOutCompleted(args);
        }
    });

    Windows::Xbox::System::User::SignOutStarted +=
        ref new Windows::Foundation::EventHandler<Windows::Xbox::System::SignOutStartedEventArgs^>(
            [thisWeakPtr](Platform::Object^ sender, Windows::Xbox::System::SignOutStartedEventArgs^ args)
    {
        std::shared_ptr<LiveResources> thisSharedPtr(thisWeakPtr.lock());
        if (thisSharedPtr)
        {
            thisSharedPtr->OnSignOutStarted(args);
        }
    });

    if (m_autoManageUser)
    {
        Refresh();
        UpdateFirstSignedInUser();
    }
    else
    {
        UpdateUserInfo();
    }
}

void ATG::LiveResources::Refresh()
{
    auto currentUser = Windows::Xbox::ApplicationModel::Core::CoreApplicationContext::CurrentUser;

    if (currentUser && currentUser->IsSignedIn && (!currentUser->IsGuest || m_isGuestUserAllowed))
    {
        SetCurrentUser(currentUser, true);
    }
    else
    {
        SetCurrentUser(nullptr, true);

        if (m_autoManageUser)
        {
            UpdateFirstSignedInUser();
        }
    }
}

void ATG::LiveResources::SetCurrentUser(XboxLiveUser user, bool callUserChangedCallback)
{
    auto newXuid = user ? user->XboxUserId : nullptr;
    bool userChanged = _wcsicmp(newXuid->Data(), m_xuid.c_str()) != 0;

    m_xboxLiveUser = user;
    UpdateUserInfo();

    if (userChanged && callUserChangedCallback && m_onUserChangedCallback)
    {
        m_onUserChangedCallback(m_xboxLiveUser);
    }
}

void ATG::LiveResources::OnCurrentUserChanged()
{
    auto user = Windows::Xbox::ApplicationModel::Core::CoreApplicationContext::CurrentUser;
    if (user && user != m_xboxLiveUser && user->IsSignedIn && (!user->IsGuest || m_isGuestUserAllowed))
    {
        SetCurrentUser(user, true);
    }
}

void ATG::LiveResources::OnSignOutCompleted(Windows::Xbox::System::SignOutCompletedEventArgs^ args)
{
    if (m_xboxLiveUser == args->User)
    {
        if (m_onUserSignOutCompletedCallback)
        {
            m_onUserSignOutCompletedCallback(args->User);
        }

        SetCurrentUser(nullptr, false);

        if (m_autoManageUser)
        {
            // If there is another user still signed in, set them as the current user
            UpdateFirstSignedInUser();
        }
    }
}

void ATG::LiveResources::OnSignOutStarted(Windows::Xbox::System::SignOutStartedEventArgs^ args)
{
    if (m_onUserSignOutStartedCallback)
    {
        m_onUserSignOutStartedCallback(args);
    }
}

void ATG::LiveResources::UpdateFirstSignedInUser()
{
    if (m_xboxLiveUser)
        return;

    for (auto user : Windows::Xbox::System::User::Users)
    {
        if (user && user->IsSignedIn && (!user->IsGuest || m_isGuestUserAllowed))
        {
            SetCurrentUser(user, true);
            break;
        }
    }
}

void ATG::LiveResources::UpdateUserInfo()
{
    if (m_xboxLiveUser)
    {
        m_gamertag.assign(m_xboxLiveUser->DisplayInfo->GameDisplayName->Data());
        m_xuid.assign(m_xboxLiveUser->XboxUserId->Data());

        if (m_xboxLiveUser->IsSignedIn)
        {
            m_xboxLiveContext = std::make_shared<xbox_live_context>(m_xboxLiveUser);
        }
        else
        {
            m_xboxLiveContext.reset();
        }
    }
    else
    {
        m_gamertag.clear();
        m_xuid.clear();
        m_xboxLiveContext.reset();
    }
}
