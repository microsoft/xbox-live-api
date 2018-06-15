// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UserController.h"
#include "time.h"
#include "..\Support\Game.h"
#include "Utils.h"
#include <memory>

using namespace xbox::services;
using namespace xbox::services::multiplayer;
using namespace xbox::services::multiplayer::manager;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage;
using namespace Windows::System;
using namespace Windows::UI::Core;
using namespace Windows::Xbox::Multiplayer;
using namespace Windows::Xbox::Networking;
using namespace Windows::Xbox::System;

void UserController::Initialize()
{
    m_currentUser = Windows::Xbox::ApplicationModel::Core::CoreApplicationContext::CurrentUser;
    SetupEventHandlers();
    RefreshUserList();
}

void UserController::RefreshUserList()
{
    User^ currentUser = nullptr;
    User^ previousCurrentUser = nullptr;
    {
        Concurrency::critical_section::scoped_lock lock(m_lock);
        previousCurrentUser = m_currentUser;
        m_currentUser = nullptr;

        try
        {
            m_userList = User::Users;
            for (User^ user : m_userList)
            {
                std::wstring logLine = L"RefreshUserList: Found user:";
                logLine += user->DisplayInfo->Gamertag->Data();
                g_sampleInstance->Log(logLine);

                // Use the first signed-in account we can find.
                if( user->IsSignedIn )
                {
                    if( previousCurrentUser != nullptr )
                    {
                        // Try to keep the same user as we had 
                        if( Utils::IsStringEqualCaseInsenstive(previousCurrentUser->XboxUserId, user->XboxUserId) )
                        {
                            m_currentUser = previousCurrentUser;
                        }
                    }
                    else
                    {
                        // Use the first we find
                        if (m_currentUser == nullptr)
                        {
                            m_currentUser = user;
                        }
                    }
                }
            }

            if (m_currentUser == nullptr)
            {
                g_sampleInstance->Log(L"RefreshUserList: No IsSignedIn & !IsGuest user found");
            }
        }
        catch (Platform::Exception^ ex)
        {
            // Handle errors when calling User::Users 
            std::wstringstream str;
            str << L"System::User call failed";
            str << ex->HResult;

            g_sampleInstance->Log(str.str());

            m_currentUser = nullptr;
            m_userList = nullptr;
            m_xboxLiveContrextMap.clear();
        }
   
        currentUser = m_currentUser;
    }
}

void UserController::SetupEventHandlers()
{
    std::weak_ptr<UserController> weakPtrToThis = shared_from_this();

    // Register user sign-in event
    m_signInCompletedToken = User::SignInCompleted += ref new EventHandler<SignInCompletedEventArgs^>(
        [weakPtrToThis] (Platform::Object^, SignInCompletedEventArgs^ eventArgs)
    {
        // Using a std::weak_ptr instead of 'this' to avoid dangling pointer if caller class is released.
        // Simply unregistering the callback in the destructor isn't enough to prevent a dangling pointer
        std::shared_ptr<UserController> sharedPtrToThis(weakPtrToThis.lock());
        if( sharedPtrToThis != nullptr )
        {
            sharedPtrToThis->OnSignInCompleted( eventArgs );
        }
    });

    // Register user sign-out event
    m_signOutCompletedToken = User::SignOutCompleted += ref new EventHandler<SignOutCompletedEventArgs^>(
        [weakPtrToThis] (Platform::Object^, SignOutCompletedEventArgs^ eventArgs)
    {
        // Using a std::weak_ptr instead of 'this' to avoid dangling pointer if caller class is released.
        // Simply unregistering the callback in the destructor isn't enough to prevent a dangling pointer
        std::shared_ptr<UserController> sharedPtrToThis(weakPtrToThis.lock());
        if( sharedPtrToThis != nullptr )
        {
            sharedPtrToThis->OnSignOutCompleted( eventArgs );
        }
    });

    m_userAddedToken = User::UserAdded += ref new EventHandler<UserAddedEventArgs^>(
        [weakPtrToThis] (Platform::Object^, UserAddedEventArgs^ eventArgs)
    {
        // Using a std::weak_ptr instead of 'this' to avoid dangling pointer if caller class is released.
        // Simply unregistering the callback in the destructor isn't enough to prevent a dangling pointer
        std::shared_ptr<UserController> sharedPtrToThis(weakPtrToThis.lock());
        if( sharedPtrToThis != nullptr )
        {
            sharedPtrToThis->OnUserAdded(eventArgs);
        }
    });

    m_userRemovedToken = User::UserRemoved += ref new EventHandler<UserRemovedEventArgs^>(
        [weakPtrToThis] (Platform::Object^, UserRemovedEventArgs^ eventArgs)
    {
        // Using a std::weak_ptr instead of 'this' to avoid dangling pointer if caller class is released.
        // Simply unregistering the callback in the destructor isn't enough to prevent a dangling pointer
        std::shared_ptr<UserController> sharedPtrToThis(weakPtrToThis.lock());
        if( sharedPtrToThis != nullptr )
        {
            sharedPtrToThis->OnUserRemoved(eventArgs);
        }
    });
}

void UserController::OnUserAdded(
    UserAddedEventArgs^ eventArgs
    )
{
    User^ user = eventArgs->User;
    if( user != nullptr )
    {
        g_sampleInstance->Log(std::wstring(L"OnUserAdded: ") + user->DisplayInfo->Gamertag->Data());
    }
}

void UserController::OnUserRemoved(
    UserRemovedEventArgs^ eventArgs
    )
{
    User^ user = eventArgs->User;
    if( user != nullptr )
    {
        g_sampleInstance->Log(std::wstring(L"OnUserRemoved: ") + user->DisplayInfo->Gamertag->Data());
    }
}

void UserController::OnSignInCompleted( 
    SignInCompletedEventArgs^ eventArgs 
    )
{
    User^ user = eventArgs->User;
    if( user != nullptr && user->IsSignedIn )
    {
        g_sampleInstance->Log(std::wstring(L"OnSignInCompleted: ") + user->DisplayInfo->Gamertag->Data());
    }

    HandleUserChange();
}

void UserController::OnSignOutCompleted( 
    SignOutCompletedEventArgs^ eventArgs 
    )
{
    HandleUserChange();
}

void UserController::HandleUserChange()
{
    RefreshUserList();
    DebugDumpUserList();

    if( m_currentUser == nullptr )
    {
        g_sampleInstance->Log( L"A user must be signed in" );
    }
}

void UserController::OnShutdown()
{
    try
    {
        User::UserAdded -= m_userAddedToken;
        User::UserRemoved -= m_userRemovedToken;
        User::SignInCompleted -= m_signInCompletedToken;
        User::SignOutCompleted -= m_signOutCompletedToken;
    }
    catch(Platform::Exception^ )
    {
        OutputDebugString(L"User Exception onShutdown");
    }
}

bool UserController::IsLocalUser(Platform::String^ xboxUserId)
{
    Concurrency::critical_section::scoped_lock lock(m_lock);
    
    for (User^ user : m_userList)
    {
        if( Utils::IsStringEqualCaseInsenstive(user->XboxUserId, xboxUserId) )
        {
            return true;
        }
    }

    return false;
}

Windows::Xbox::System::User^ UserController::GetCurrentUser()
{
    Concurrency::critical_section::scoped_lock lock(m_lock);
    return m_currentUser;
}

Windows::Foundation::Collections::IVectorView<User^>^ UserController::GetUserList() 
{ 
    Concurrency::critical_section::scoped_lock lock(m_lock);
    return m_userList; 
}

Windows::Foundation::Collections::IVectorView<User^>^ UserController::GetLocallySignedInUsers() 
{ 
    Concurrency::critical_section::scoped_lock lock(m_lock);

    Platform::Collections::Vector<User^>^ multiplayerUsers = ref new Platform::Collections::Vector<User^>();

    if( m_userList != nullptr ) 
    {
        for (User^ user : m_userList)
        {
            if (user->IsSignedIn)
            {
                multiplayerUsers->Append(user);
            }
        }
    }

    return multiplayerUsers->GetView();
}

bool UserController::IsXboxUserIdInsideXboxLiveContextMap(Platform::String^ xboxUserId)
{
    if (xboxUserId == nullptr)
    {
        return false;
    }

    Concurrency::critical_section::scoped_lock lock(m_lock);
    for(auto iter1 = m_xboxLiveContrextMap.begin(); iter1 != m_xboxLiveContrextMap.end(); ++iter1)
    {
        if(Utils::IsStringEqualCaseInsenstive(iter1->first, xboxUserId))
        {
            return true;
        }
    }

    return false;
}

std::shared_ptr<xbox::services::xbox_live_context> UserController::GetXboxLiveContext(Platform::String^ xboxUserId)
{
    if (xboxUserId == nullptr)
    {
        return nullptr;
    }

    Concurrency::critical_section::scoped_lock lock(m_lock);
    return m_xboxLiveContrextMap[xboxUserId];
}

std::shared_ptr<xbox::services::xbox_live_context> UserController::GetXboxLiveContext(
    User^ user, 
    bool addIfMissing
    )
{
    if (user == nullptr)
    {
        return nullptr;
    }

    std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext = GetXboxLiveContext(user->XboxUserId);
    if (xboxLiveContext == nullptr && addIfMissing)
    {
        xboxLiveContext = std::make_shared<xbox_live_context>(user);
        {
            Concurrency::critical_section::scoped_lock lock(m_lock);
            Platform::String^ xboxUserId = user->XboxUserId;
            m_xboxLiveContrextMap[xboxUserId] = xboxLiveContext;
            EnableDiagnostics(xboxLiveContext);
        }
    }

    return xboxLiveContext;
}

void UserController::EnableDiagnostics(
    _In_ std::shared_ptr<xbox_live_context> xboxLiveContext
    )
{
    if( xboxLiveContext != nullptr &&
        !xboxLiveContext->settings()->enable_service_call_routed_events())
    {
        xboxLiveContext->settings()->set_enable_service_call_routed_events(true);

        std::weak_ptr<UserController> weakPtrToThis = shared_from_this();
        xboxLiveContext->settings()->add_service_call_routed_handler( 
            [weakPtrToThis](xbox_service_call_routed_event_args args )
        {
            // Using a std::weak_ptr instead of 'this' to avoid dangling pointer if caller class is released.
            // Simply unregistering the callback in the destructor isn't enough to prevent a dangling pointer
            std::shared_ptr<UserController> sharedPtrToThis(weakPtrToThis.lock());
            if( sharedPtrToThis != nullptr && args.http_status() >= 300 )
            {
                std::wstringstream stream;
                stream << L"[URL]: ";
                stream << args.http_method();
                stream << " ";
                stream << args.uri();

                g_sampleInstance->Log( stream.str().data() );
                g_sampleInstance->Log( L"" );

                stream.clear();
                stream << L"[Response]: ";
                stream << args.http_status();
                stream << " ";
                stream << args.response_body();
                g_sampleInstance->Log( stream.str().data() );
            }
        });
    }
}

void UserController::DisableDiagnostics(
    _In_ std::shared_ptr<xbox_live_context> xboxLiveContext
    )
{
    if( xboxLiveContext != nullptr &&
        xboxLiveContext->settings()->enable_service_call_routed_events())
    {
        // Disable debug tracing of the Xbox Service API traffic to the game UI
        xboxLiveContext->settings()->set_enable_service_call_routed_events(false);
    }
}

bool UserController::HasSignedInUser()
{
    User^ user = GetCurrentUser();
    return (user != nullptr) && (user->IsSignedIn);
}

Platform::String^ UserController::GetCurrentXboxLiveId()
{
    User^ user = GetCurrentUser();
    return (user != nullptr) ? user->XboxUserId : "n/a";
}

std::shared_ptr<xbox::services::xbox_live_context> UserController::GetCurrentXboxLiveContext()
{
    User^ user = GetCurrentUser();
    return (user != nullptr) ? GetXboxLiveContext(user, true) : nullptr;
}

void UserController::DebugDumpUserList()
{
    g_sampleInstance->Log(L"[User] Dumping User State");

    User^ user = GetCurrentUser();
    if (user == nullptr)
    {
        g_sampleInstance->Log(L"\tCurrent User: NONE");
    }
    else
    {
        std::wstringstream str;
        str << L"\tCurrent User: ";
        str << user->DisplayInfo->Gamertag->Data();
        str << L" [";
        str << user->XboxUserId->Data();
        str << L"]";
        g_sampleInstance->Log(str.str());
    }

    Windows::Foundation::Collections::IVectorView<User^>^ userList = GetUserList();
    for (User^ userInList : userList)
    {
        std::wstringstream str;
        str << L"User: ";
        str << userInList->DisplayInfo->Gamertag->Data();
        str << L" [";
        str << userInList->XboxUserId->Data();
        str << ((userInList->IsSignedIn) ? L"signed-in]" : L"not signed-in]");
        g_sampleInstance->Log(str.str());
    }
}
