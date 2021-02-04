//--------------------------------------------------------------------------------------
// File: LiveResources.cpp
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright(c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "pch.h"

#include "LiveResources.h"

#include <XGame.h>
#include <XGameRuntimeFeature.h>
#include <XGameUI.h>
#include <XSystem.h>

#ifdef __clang__
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wsign-conversion"
#endif

#pragma warning(disable : 4061 4365)

#ifdef _GAMING_XBOX
#include <ws2def.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#endif

using namespace ATG;

namespace
{
    // This is a rare error that is not exposed externally because it doesn't happen in retail scenarios
    constexpr long XO_E_CONTENT_ISOLATION = 0x8015DC12;
}

_Use_decl_annotations_
LiveResources::LiveResources(XTaskQueueHandle queue, bool autoManageUser, bool isGuestUserAllowed) noexcept(false):
#ifdef _GAMING_XBOX
    m_networkConnectivityChangedHandle{},
#endif
    m_isNetworkAvailable(false),
    m_autoManageUser(autoManageUser),
    m_isGuestUserAllowed(isGuestUserAllowed),
    m_asyncQueue{},
    m_userChangedEventToken{},
    m_xboxLiveContext{},
    m_xboxLiveUser{},
    m_xuid{},
    m_titleId{}
{
    HRESULT hr;

    if (queue)
    {
        XTaskQueueDuplicateHandle(queue, &m_asyncQueue);
    }
    else
    {
        hr = XTaskQueueCreate(XTaskQueueDispatchMode::ThreadPool, XTaskQueueDispatchMode::ThreadPool, &m_asyncQueue);
        DX::ThrowIfFailed(hr);
    }

    XGameGetXboxTitleId(&m_titleId);

    char hexTitleId[16] = {};
    sprintf_s(hexTitleId, "0x%08X", m_titleId);
    m_titleIdHex.assign(hexTitleId);

    char scidBuffer[64] = {};
    sprintf_s(scidBuffer, "00000000-0000-0000-0000-0000%08x", m_titleId);
    m_scid = scidBuffer;

#ifdef _GAMING_XBOX
    if (XGameRuntimeIsFeatureAvailable(XGameRuntimeFeature::XNetworking))
    {
        // Listen for network connectivity changes
        NotifyNetworkConnectivityHintChange(
            [](void* context, NL_NETWORK_CONNECTIVITY_HINT connectivityHint)
            {
                auto liveResources = static_cast<LiveResources*>(context);
            
                liveResources->m_isNetworkAvailable =
                    connectivityHint.ConnectivityLevel != NL_NETWORK_CONNECTIVITY_LEVEL_HINT::NetworkConnectivityLevelHintUnknown;
            }, // Callback function
            this,                                   // Context object
            true,                                   // Notify immediately with the current status
            &m_networkConnectivityChangedHandle     // Notification handle
        );
    }
    else
#endif
    {
        // Assume network stack is ready on desktop
        m_isNetworkAvailable = true;
    }
}

LiveResources::~LiveResources()
{
#ifdef _GAMING_XBOX
    if (m_networkConnectivityChangedHandle)
    {
        CancelMibChangeNotify2(m_networkConnectivityChangedHandle);
    }
#endif

    if (m_asyncQueue)
    {
        XTaskQueueCloseHandle(m_asyncQueue);
        m_asyncQueue = nullptr;
    }
}

void LiveResources::Initialize()
{
    XUserRegisterForChangeEvent(m_asyncQueue, this, [](void *context, const XUserLocalId userLocalId, XUserChangeEvent event) 
    {
        auto pThis = reinterpret_cast<LiveResources*>(context);

        switch (event)
        {
        case XUserChangeEvent::SignedInAgain:
            pThis->OnSignIn(userLocalId);
            break;
        case XUserChangeEvent::SignedOut:
            pThis->OnSignOutCompleted(userLocalId);
            break;
        default:
            break;
        }

    }, &m_userChangedEventToken);

    if (m_autoManageUser)
    {
        SignInSilently();
    }
}

void LiveResources::Refresh()
{
}

void LiveResources::SetCurrentUser(XUserHandle user)
{
    if (!user)
    {
        if (m_xboxLiveContext)
        {
            XblContextCloseHandle(m_xboxLiveContext);
            m_xboxLiveContext = nullptr;
        }
        if (m_xboxLiveUser)
        {
            XUserCloseHandle(m_xboxLiveUser);
            m_xboxLiveUser = nullptr;
        }
        m_gamertag.clear();
        m_xuid = 0;
    }
    else if ((user && !m_xboxLiveUser) || !XUserCompare(user, m_xboxLiveUser))
    {
        if (m_xboxLiveUser)
        {
            XUserCloseHandle(m_xboxLiveUser);
            m_xboxLiveUser = nullptr;
        }

        m_xboxLiveUser = user;

        char gamertag[XUserGamertagComponentClassicMaxBytes] = {};

        auto result = XUserGetGamertag(user, XUserGamertagComponent::Classic, XUserGamertagComponentClassicMaxBytes, gamertag, nullptr);

        if (SUCCEEDED(result))
        {
            m_gamertag = gamertag;
        }
        else
        {
            HandleError(result);
        }

        result = XUserGetId(user, &m_xuid);

        if (FAILED(result))
        {
            HandleError(result);
        }

        InitializeXboxServices();

        if (m_onUserChangedCallback)
        {
            m_onUserChangedCallback(user);
        }
    }
    else
    {
        XUserCloseHandle(user);
    }
}

void LiveResources::OnSignOutCompleted(const XUserLocalId localId)
{
    XUserHandle user = nullptr;
    XUserFindUserByLocalId(localId, &user);

    if (!XUserCompare(user, m_xboxLiveUser))
    {
        SetCurrentUser(nullptr);

        if (m_onUserSignOutCompletedCallback)
        {
            m_onUserSignOutCompletedCallback(user);
        }

        if (m_autoManageUser)
        {
            SignInSilently();
        }
    }
}

void LiveResources::OnSignIn(const XUserLocalId localId)
{
    XUserHandle user = nullptr;
    XUserFindUserByLocalId(localId, &user);

    if ((user && !m_xboxLiveUser) || !XUserCompare(user, m_xboxLiveUser))
    {
        SetCurrentUser(user);

        if (m_onUserChangedCallback)
        {
            m_onUserChangedCallback(user);
        }
    }
}

void LiveResources::SignInSilently()
{
    auto async = new XAsyncBlock{};
    async->queue = m_asyncQueue;
    async->context = this;
    async->callback = [](XAsyncBlock* async)
    {
        auto pThis = reinterpret_cast<LiveResources*>(async->context);

        XUserHandle user = nullptr;

        HRESULT result = XUserAddResult(async, &user);
        if (SUCCEEDED(result))
        {
            pThis->SetCurrentUser(user);
        }
        else
        {
            if (result == XO_E_CONTENT_ISOLATION)
            {
                // The user doesn't exist in current sandbox and cannot be logged in.
                // This is a case that really only exists when running samples on PC where
                // the user might already be logged in as a user in the RETAIL sandbox then
                // switches to a sample-specific sandbox without logging in a different user
                // Propagate an error which is handled by samples
                pThis->HandleError(E_GAMEUSER_RESOLVE_USER_ISSUE_REQUIRED);
            }
            else
            {
                pThis->HandleError(result);
            }
        }
        
        delete async;
    };

    HRESULT hr = XUserAddAsync(XUserAddOptions::AddDefaultUserSilently, async);

    if (FAILED(hr))
    {
        delete async;
        HandleError(hr);
    }
}

void LiveResources::SignInWithUI()
{
    auto async = new XAsyncBlock{};
    async->queue = m_asyncQueue;
    async->context = this;
    async->callback = [](XAsyncBlock* async)
    {
        auto pThis = reinterpret_cast<LiveResources*>(async->context);

        XUserHandle user = nullptr;
        auto result = XUserAddResult(async, &user);

        if (SUCCEEDED(result))
        {
            pThis->SetCurrentUser(user);
        }
        else
        {
            pThis->HandleError(result);
        }

        delete async;
    };

    HRESULT hr = XUserAddAsync(XUserAddOptions::AllowGuests, async);

    if (FAILED(hr))
    {
        delete async;
        HandleError(hr);
    }
}

void LiveResources::HandleError(HRESULT error)
{
    if (m_errorHandler)
    {
        m_errorHandler(error);
    }
}

void LiveResources::InitializeXboxServices()
{
    if (m_xboxLiveUser)
    {
        if (m_xboxLiveContext)
        {
            XblContextCloseHandle(m_xboxLiveContext);
        }

        auto result = XblContextCreateHandle(m_xboxLiveUser, &m_xboxLiveContext);

        if (FAILED(result))
        {
            HandleError(result);
            m_xboxLiveContext = nullptr;
        }
    }
    else if (m_xboxLiveContext)
    {
        XblContextCloseHandle(m_xboxLiveContext);
        m_xboxLiveContext = nullptr;
    }
}

bool LiveResources::IsUserSignedIn(XUserHandle user)
{
    XUserState state = XUserState::SignedOut;
    XUserGetState(user, &state);
    return state == XUserState::SignedIn;
}
