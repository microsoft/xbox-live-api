// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "pch.h"

//
// Manages the set of game users
//
class UserController : public std::enable_shared_from_this<UserController>
{
public:
    void Initialize();
    void RefreshUserList();
    void SetupEventHandlers();
    void OnShutdown();

    // Returns true if the user identified by the xboxUserId is from the local console
    bool IsLocalUser(Platform::String^ xboxUserId);

    // Returns all known users, including guest
    Windows::Foundation::Collections::IVectorView<Windows::Xbox::System::User^>^ GetUserList();

    // Returns only signed in users (and thus not guest accounts)
    Windows::Foundation::Collections::IVectorView<Windows::Xbox::System::User^>^ GetLocallySignedInUsers();

    std::shared_ptr<xbox::services::xbox_live_context> GetXboxLiveContext(Platform::String^ xboxUserId);
    std::shared_ptr<xbox::services::xbox_live_context> GetXboxLiveContext(Windows::Xbox::System::User^ user, bool addIfMissing);
    std::shared_ptr<xbox::services::xbox_live_context> GetCurrentXboxLiveContext();
    void EnableDiagnostics(std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
    void DisableDiagnostics(std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
    bool IsXboxUserIdInsideXboxLiveContextMap(Platform::String^ xboxUserId);

    void DebugDumpUserList();

    Windows::Xbox::System::User^ GetCurrentUser();
    bool HasSignedInUser();
    Platform::String^ GetCurrentXboxLiveId();

private:
    void OnUserAdded(Windows::Xbox::System::UserAddedEventArgs^ eventArgs);
    void HandleUserChange();
    void OnUserRemoved(Windows::Xbox::System::UserRemovedEventArgs^ eventArgs);
    void OnSignInCompleted( Windows::Xbox::System::SignInCompletedEventArgs^ eventArgs );
    void OnSignOutCompleted( Windows::Xbox::System::SignOutCompletedEventArgs^ eventArgs );

    Concurrency::critical_section m_lock;

    std::map<Platform::String^, std::shared_ptr<xbox::services::xbox_live_context>> m_xboxLiveContrextMap;
    Windows::Xbox::System::User^ m_currentUser;
    Windows::Foundation::EventRegistrationToken m_userAddedToken;
    Windows::Foundation::EventRegistrationToken m_userRemovedToken;
    Windows::Foundation::EventRegistrationToken m_audioDeviceAddedToken;
    Windows::Foundation::EventRegistrationToken m_signInCompletedToken;
    Windows::Foundation::EventRegistrationToken m_signOutCompletedToken;
    Windows::Foundation::Collections::IVectorView<Windows::Xbox::System::User^>^ m_userList;
};


