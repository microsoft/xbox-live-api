// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "SampleGUI.h"
#include "LiveResources.h"
#include <robuffer.h>

using namespace Windows::Storage::Streams;
using namespace Microsoft::WRL;

namespace
{
    void* GetPointerToBufferData(IBuffer^ buffer)
    {
        // Obtain IBufferByteAccess from IBuffer
        ComPtr<IUnknown> pBuffer((IUnknown*)buffer);
        ComPtr<IBufferByteAccess> pBufferByteAccess;

        pBuffer.As(&pBufferByteAccess);

        // Get pointer to data
        byte* pData = nullptr;
        if (pBufferByteAccess->Buffer(&pData) < 0)
        {
            // Buffer is not annotated with _COM_Outpr, so if it were to fail, then the value of pData is undefined
            pData = nullptr;
        }

        return pData;
    }

    const int c_maxImageSize = 1000000;
}

ATG::LiveResources::LiveResources() :
    m_gamertag(nullptr),
    m_userDependentPanel(nullptr)
{

}

void ATG::LiveResources::Initialize(std::unique_ptr<ATG::UIManager> &ui, ATG::IPanel *userDependentPanel)
{
    ui->LoadLayout(L".\\Assets\\LiveInfoHUD.csv", L".\\Assets");
    m_gamertag = ui->FindControl<ATG::Legend>(1000, 1002);

    m_userDependentPanel = userDependentPanel;

    UpdateFirstSignedInUser();

    Windows::Xbox::ApplicationModel::Core::CoreApplicationContext::CurrentUserChanged += 
        ref new Windows::Foundation::EventHandler<Platform::Object^>(
            [this](Platform::Object^ sender, Platform::Object^ args)
        {
            OnCurrentUserChanged();
        });

    Windows::Xbox::System::User::SignInCompleted += 
        ref new Windows::Foundation::EventHandler<Windows::Xbox::System::SignInCompletedEventArgs^>(
            [this](Platform::Object^ sender, Windows::Xbox::System::SignInCompletedEventArgs^ args)
        {
            SetCurrentUser(args->User);
        });

    Windows::Xbox::System::User::SignOutCompleted += 
        ref new Windows::Foundation::EventHandler<Windows::Xbox::System::SignOutCompletedEventArgs^>(
            [this](Platform::Object^ sender, Windows::Xbox::System::SignOutCompletedEventArgs^ args)
        {
            if (m_user == args->User)
            {
                m_user = nullptr;
                m_xboxLiveContext.reset();

                // If there is another user still signed in, set them as the current user
                UpdateFirstSignedInUser();

                if (m_user == nullptr)
                {
                    if (m_userDependentPanel != nullptr)
                    {
                        m_userDependentPanel->Close();
                    }
                    m_gamertag->SetText(L"Press [Menu] to sign in");
                }
            }
        });

    auto appConfig = xbox::services::xbox_live_app_config::get_app_config_singleton();

    m_titleId = appConfig->title_id();
    m_scid = appConfig->scid();

    ui->FindPanel<ATG::HUD>(1000)->Show();
    ui->FindControl<ATG::TextLabel>(1000, 1004)->SetText(appConfig->sandbox().c_str());

    wchar_t hexTitleId[16] = {};
    swprintf_s(hexTitleId,L"0x%08X", m_titleId);
    ui->FindControl<ATG::TextLabel>(1000, 1006)->SetText(hexTitleId);
    ui->FindControl<ATG::TextLabel>(1000, 1008)->SetText(m_scid.c_str());
}

void ATG::LiveResources::Refresh()
{
    auto user = Windows::Xbox::ApplicationModel::Core::CoreApplicationContext::CurrentUser;
    if (user != nullptr && user->IsSignedIn && !user->IsGuest)
    {
        SetCurrentUser(user);
    }
}

void ATG::LiveResources::OnCurrentUserChanged()
{
    m_user = nullptr;
    m_xboxLiveContext.reset();

    auto user = Windows::Xbox::ApplicationModel::Core::CoreApplicationContext::CurrentUser;
    if (user != nullptr && user->IsSignedIn && !user->IsGuest)
    {
        SetCurrentUser(user);
    }
}

void ATG::LiveResources::SetCurrentUser(Windows::Xbox::System::User^ user)
{
    if (m_userDependentPanel != nullptr)
    {
        m_userDependentPanel->Show();
    }

    m_user = user;
    m_xboxLiveContext = std::make_shared<xbox::services::xbox_live_context>(m_user);
    m_gamertag->SetText(m_user->DisplayInfo->GameDisplayName->Data());

    m_userXUID = m_user->XboxUserId->Data();
}

void ATG::LiveResources::UpdateFirstSignedInUser()
{
    auto userList = Windows::Xbox::System::User::Users;
    if (userList != nullptr && userList->Size > 0)
    {
        auto userIter = userList->First();
        do
        {
            auto user = userIter->Current;
            if (user != nullptr && user->IsSignedIn && !user->IsGuest)
            {
                SetCurrentUser(user);
                break;
            }
        } while (userIter->MoveNext());
    }
}
