//--------------------------------------------------------------------------------------
// File: LiveResources.h
//
// Handles Users signing in and out and the related Xbox Live Contexts
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once

#include <functional>
#include <string>

#include <stdint.h>

#include <xsapi-c/services_c.h>

#include <XUser.h>
#include <XTaskQueue.h>

namespace ATG
{
    class LiveResources : public std::enable_shared_from_this<LiveResources>
    {
    public:
        // Constructor Paramters:
        //   autoManageUser:
        //     1) sets current user to the first user it finds upon calling Initialize()
        //     2) switches to new users as they sign in
        //     3) attempts to find another signed in user when current user signs out
        //   isGuestUserAllowed: when true, guest users are valid users to use in auto user management or when resuming from suspend
        explicit LiveResources(_In_opt_ XTaskQueueHandle queue = nullptr, bool autoManageUser = true, bool isGuestUserAllowed = false) noexcept(false);

        LiveResources(LiveResources&&) = default;
        LiveResources& operator= (LiveResources&&) = default;

        LiveResources(LiveResources const&) = delete;
        LiveResources& operator= (LiveResources const&) = delete;

        ~LiveResources();

        void Initialize();
        void Refresh(); // call when resuming from suspend

        bool               IsNetworkAvailable() const { return m_isNetworkAvailable; }
        const std::string& GetGamertag()        const { return m_gamertag; }
        XblContextHandle   GetLiveContext()     const { return m_xboxLiveContext; }
        const std::string& GetSandbox()         const { return m_sandbox; }
        const std::string& GetServiceConfigId() const { return m_scid; }
        uint32_t           GetTitleId()         const { return m_titleId; }
        const std::string& GetTitleIdHex()      const { return m_titleIdHex; }
        XalUserHandle      GetUser()            const { return m_xboxLiveUser; }
        uint64_t           GetXuid()            const { return m_xuid; }
        bool               IsUserSignedIn()     const { return m_xboxLiveUser && IsUserSignedIn(m_xboxLiveUser); }

        XTaskQueueHandle GetAsyncQueue()        const { return m_asyncQueue; }

        void SetCurrentUser(XUserHandle user); // typically not needed when using auto user management
        void SetUserChangedCallback(std::function<void(XUserHandle)> callback) { m_onUserChangedCallback = callback; }
        void SetUserSignOutCompletedCallback(std::function<void(XUserHandle)> callback) { m_onUserSignOutCompletedCallback = callback; }
        void SetErrorHandler(std::function<void(HRESULT)> callback) { m_errorHandler = callback; }

        static bool IsUserSignedIn (XUserHandle user);
        bool        IsGuestAllowed() const { return m_isGuestUserAllowed; }

        void SignInSilently();
        void SignInWithUI();
    private:
        // Event Handlers
        void OnSignOutCompleted(const XUserLocalId user);
        void OnSignIn(const XUserLocalId user);


        void HandleError(HRESULT error);
        void InitializeXboxServices();

#ifdef _GAMING_XBOX
        HANDLE                             m_networkConnectivityChangedHandle;
#endif
        bool                               m_isNetworkAvailable;

        bool                               m_autoManageUser;
        bool                               m_isGuestUserAllowed;

        XTaskQueueHandle                   m_asyncQueue;
        XTaskQueueRegistrationToken        m_userChangedEventToken;

        // User Info
        std::string                        m_gamertag;
        XblContextHandle                   m_xboxLiveContext;
        XUserHandle                        m_xboxLiveUser;
        uint64_t                           m_xuid;

        // Title Info
        std::string                        m_sandbox;
        std::string                        m_scid;
        uint32_t                           m_titleId;
        std::string                        m_titleIdHex;

        // Callbacks
        std::function<void(XUserHandle)>   m_onUserChangedCallback;
        std::function<void(XUserHandle)>   m_onUserSignOutCompletedCallback;
        std::function<void(HRESULT)>       m_errorHandler;
    };
}