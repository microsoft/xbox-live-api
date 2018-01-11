// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

#include "SampleGUI.h"
#include "LiveResources.h"

#include <robuffer.h>

using namespace Windows::Storage::Streams;
using Microsoft::WRL::ComPtr;

void ATG::LiveResources::Initialize(std::shared_ptr<ATG::UIManager> &ui, ATG::IPanel *userDependentPanel, ATG::IPanel *nouserDependentPanel)
{
    ui->LoadLayout(L".\\Assets\\LiveInfoHUD.csv", L".\\Assets");
    m_gamertag = ui->FindControl<ATG::Legend>(1000, 1002);
    m_gamerPic = ui->FindControl<ATG::Image>(1000, 1001);
    auto userPickerHint = ui->FindControl<ATG::Legend>(1000, 1011);
    if (!IsMultiUserApplication())
    {
        userPickerHint->SetVisible(false);
    }

    m_userDependentPanel = userDependentPanel;
    m_nouserDependentPanel = nouserDependentPanel;

    xbox::services::system::xbox_live_user::add_sign_out_completed_handler(
        [this](const xbox::services::system::sign_out_completed_event_args&)
        {
            UpdateCurrentUser();
        });

    m_sandboxLabel = ui->FindControl<ATG::TextLabel>(1000, 1004);
    m_titleIdLabel = ui->FindControl<ATG::TextLabel>(1000, 1006);
    m_scidLabel = ui->FindControl<ATG::TextLabel>(1000, 1008);
    m_signInErrorLabel = ui->FindControl<ATG::TextLabel>(1000, 1010);
    m_signInErrorLabel->SetVisible(false);

    ui->FindPanel<ATG::HUD>(1000)->Show();

    Refresh();
}

void ATG::LiveResources::Refresh()
{
    auto appConfig = xbox::services::xbox_live_app_config::get_app_config_singleton();

    m_titleId = appConfig->title_id();
    m_scid = appConfig->scid();

    if (!appConfig->sandbox().empty())
    {
        m_sandboxLabel->SetText(appConfig->sandbox().c_str());
    }

    wchar_t hexTitleId[16] = {};
    swprintf_s(hexTitleId, L"0x%08X", m_titleId);

    m_titleIdLabel->SetText(hexTitleId);
    m_scidLabel->SetText(m_scid.c_str());

    UpdateCurrentUser();
}

void ATG::LiveResources::HandleSignInResult(
    _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
    xbox::services::xbox_live_result<xbox::services::system::sign_in_result>& signInResult
    )
{
    if (!signInResult.err())
    {
        auto result = signInResult.payload();
        OnUserSignInEvent(user, result.status());
        switch (result.status())
        {
        case xbox::services::system::sign_in_status::success:
            LogLine(L"Signing in current user success");
            Refresh();
            break;

        case xbox::services::system::sign_in_status::user_cancel:
            LogLine(L"Signing in result: user cancel");
            m_signInErrorLabel->SetText(L"Error: User canceled");
            m_signInErrorLabel->SetVisible(true);
            UpdateCurrentUser();
            break;

        case xbox::services::system::sign_in_status::user_interaction_required:
            LogLine(L"Signing in result: User interaction required");
            m_signInErrorLabel->SetText(L"Error: User interaction required");
            m_signInErrorLabel->SetVisible(true);
            UpdateCurrentUser();
            break;
        }
    }
    else
    {
        string_t errorStr = L"Sign in failed:" + utility::conversions::utf8_to_utf16(signInResult.err_message());
        LogLine(errorStr.c_str());
        m_signInErrorLabel->SetText(errorStr.c_str());
        m_signInErrorLabel->SetVisible(true);
        UpdateCurrentUser();
    }
}

void ATG::LiveResources::UpdateCurrentUser()
{
    if (m_currentUser != nullptr && m_currentUser->is_signed_in())
    {
        if (m_nouserDependentPanel != nullptr)
        {
            m_nouserDependentPanel->Close();
        }

        if (m_userDependentPanel != nullptr)
        {
            m_userDependentPanel->Show();
        }

        m_gamertag->SetText(m_currentUser->gamertag().c_str());
        m_signInErrorLabel->SetVisible(false);
    }
    else
    {
        if (m_userDependentPanel != nullptr)
        {
            m_userDependentPanel->Close();
        }

        if (m_nouserDependentPanel != nullptr)
        {
            m_nouserDependentPanel->Show();
        }

        m_gamertag->SetText(L"Press [A] to sign in");
    }
}

function_context ATG::LiveResources::add_signin_handler(
    _In_ std::function<void(std::shared_ptr<xbox::services::system::xbox_live_user>, xbox::services::system::sign_in_status)> handler
    )
{
    std::lock_guard<std::mutex> lock(m_writeLock);

    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_signinRoutedHandlersCounter;
        m_signinRoutedHandlers[m_signinRoutedHandlersCounter] = std::move(handler);
    }

    return context;
}

void ATG::LiveResources::remove_signin_handler(
    _In_ function_context context
    )
{
    std::lock_guard<std::mutex> lock(m_writeLock);
    m_signinRoutedHandlers.erase(context);
}

void ATG::LiveResources::OnUserSignInEvent(
    _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
    _In_ xbox::services::system::sign_in_status result
)
{
    std::lock_guard<std::mutex> lock(m_writeLock);

    for (auto& handler : m_signinRoutedHandlers)
    {
        if (handler.second != nullptr)
        {
            try
            {
                handler.second(user, result);
            }
            catch (...)
            {
            }
        }
    }
}


void ATG::LiveResources::SetCurrentGamepad(int gamepadIndex)
{
    if (gamepadIndex != -1)
    {
        std::wstringstream ss;
        ss << L"Current gamepad index set to " << gamepadIndex;
        LogLine(ss.str());
        m_currentGamepadIndex = gamepadIndex;
    }
}

void ATG::LiveResources::SetCurrentGamepadAndUser(int gamepadIndex, std::wstring systemUserId)
{
    SetCurrentGamepad(gamepadIndex);

    Windows::System::User^ systemUser = Windows::System::User::GetFromId(ref new Platform::String(systemUserId.c_str()));
    if (systemUser != nullptr)
    {
        SetupCurrentUser(systemUser);
    }
    else
    {
        LogLine(L"No user found bind to current gamepad");
    }
}

void ATG::LiveResources::SetCurrentGamepadAndUser(int gamepadIndex, Windows::System::User^ systemUser)
{
    SetCurrentGamepad(gamepadIndex);

    if (systemUser != nullptr)
    {
        SetupCurrentUser(systemUser);
    }
    else
    {
        LogLine(L"No user found bind to current gamepad");
    }
}

void ATG::LiveResources::SetupCurrentUser(Windows::System::User^ systemUser)
{
    if (m_currentUser == nullptr ||
        (m_currentUser != nullptr && m_currentUser->windows_system_user()->NonRoamableId != systemUser->NonRoamableId))
    {
        //std::shared_ptr<xbox::services::system::xbox_live_user> xblUser;
        auto xblUser = GetXboxLiveUser(systemUser);

        if (xblUser == nullptr)
        {
            xblUser = std::make_shared<xbox::services::system::xbox_live_user>(systemUser);
            if (systemUser != nullptr)
            {
                m_users.emplace(systemUser->NonRoamableId->Data(), xblUser);
            }
            else
            {
                m_users.emplace(L"", xblUser);
            }
        }

        std::wstringstream ss;
        ss << L"Setting current user to " << (systemUser == nullptr? L"default": systemUser->NonRoamableId->Data());
        LogLine(ss.str());
        m_currentUser = xblUser;

        UpdateCurrentUser();
    }
}

void ATG::LiveResources::TrySignInCurrentUser()
{
    if (m_currentUser != nullptr && !m_currentUser->is_signed_in())
    {
        LogLine(L"Signing in current user");

        m_currentUser->signin(Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher)
        .then([this](xbox::services::xbox_live_result<xbox::services::system::sign_in_result> result) // use task_continuation_context::use_current() to make the continuation task running in current apartment 
        {
            HandleSignInResult(m_currentUser, result);

        }, concurrency::task_continuation_context::use_current());
    }
}

void ATG::LiveResources::TrySignInCurrentUserSilently()
{
    if (m_currentUser != nullptr && !m_currentUser->is_signed_in())
    {
        LogLine(L"Signing in current user silently");

        m_currentUser->signin_silently(Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher)
            .then([this](xbox::services::xbox_live_result<xbox::services::system::sign_in_result> result) // use task_continuation_context::use_current() to make the continuation task running in current apartment 
        {
            HandleSignInResult(m_currentUser, result);

        }, concurrency::task_continuation_context::use_current());
    }
}

bool ATG::LiveResources::IsMultiUserApplication()
{
    // Save the result in memory, as we only need to check once;
    static int isSupported = -1;

    if (isSupported == -1)
    {
        try
        {
            bool APIExist = Windows::Foundation::Metadata::ApiInformation::IsMethodPresent("Windows.System.UserPicker", "IsSupported");
            isSupported = (APIExist && Windows::System::UserPicker::IsSupported()) ? 1 : 0;
        }
        catch (...)
        {
            isSupported = 0;
        }
    }
    return isSupported == 1;
}

std::shared_ptr<xbox::services::system::xbox_live_user> ATG::LiveResources::GetXboxLiveUser(Windows::System::User^ systemUser)
{
    if (!IsMultiUserApplication())
    {
        return m_currentUser;
    }
    else
    {
        auto itor = m_users.find(systemUser->NonRoamableId->Data());
        if (itor != m_users.end())
        {
            return itor->second;
        }
        return nullptr;
    }
}

void ATG::LiveResources::SetLogCallback(_In_ const std::function<void(const std::wstring&)>& callback)
{
    m_logCallback = callback;
}

void ATG::LiveResources::LogLine(const std::wstring& log)
{
    if (m_logCallback)
    {
        m_logCallback(log);
    }
}