// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

#include "SampleGUI.h"
#include "LiveResources.h"

#include <robuffer.h>

using namespace Windows::Storage::Streams;
using Microsoft::WRL::ComPtr;

ATG::LiveResources::LiveResources() :
    m_user(nullptr),
    m_xboxLiveContext(nullptr),
    m_gamertag(nullptr),
    m_userDependentPanel(nullptr)
{
}

void ATG::LiveResources::Initialize(std::unique_ptr<ATG::UIManager> &ui, ATG::IPanel *userDependentPanel, ATG::IPanel *nouserDependentPanel)
{
    ui->LoadLayout(L".\\Assets\\LiveInfoHUD.csv", L".\\Assets");
    m_gamertag = ui->FindControl<ATG::Legend>(1000, 1002);
    m_gamerPic = ui->FindControl<ATG::Image>(1000, 1001);

    m_userDependentPanel = userDependentPanel;
    m_nouserDependentPanel = nouserDependentPanel;

    m_user = std::make_shared<xbox::services::system::xbox_live_user>();
    m_user->add_sign_out_completed_handler(
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

    if (!m_user->is_signed_in())
    {
        m_user->signin_silently()
        .then([this](xbox::services::xbox_live_result<xbox::services::system::sign_in_result> result)
        {
            HandleSignInResult(result);
        });
    }
    else
    {
        UpdateCurrentUser();
    }
}

void ATG::LiveResources::SignIn()
{
    m_user->signin(Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher)
    .then([this](xbox::services::xbox_live_result<xbox::services::system::sign_in_result> result) // use task_continuation_context::use_current() to make the continuation task running in current apartment 
    {
        HandleSignInResult(result);

    }, concurrency::task_continuation_context::use_current());
}

void ATG::LiveResources::HandleSignInResult(
    xbox::services::xbox_live_result<xbox::services::system::sign_in_result>& signInResult
    )
{
    if (!signInResult.err())
    {
        auto result = signInResult.payload();
        switch (result.status())
        {
        case xbox::services::system::sign_in_status::success:
            Refresh();
            break;

        case xbox::services::system::sign_in_status::user_cancel:
            m_signInErrorLabel->SetText(L"Error: User canceled");
            m_signInErrorLabel->SetVisible(true);
            UpdateCurrentUser();
            break;

        case xbox::services::system::sign_in_status::user_interaction_required:
            m_signInErrorLabel->SetText(L"Error: User interaction required");
            m_signInErrorLabel->SetVisible(true);
            UpdateCurrentUser();
            break;
        }
    }
    else
    {
        string_t errorStr = L"Sign in failed:" + utility::conversions::utf8_to_utf16(signInResult.err_message());
        m_signInErrorLabel->SetText(errorStr.c_str());
        m_signInErrorLabel->SetVisible(true);
        UpdateCurrentUser();
    }
}

void ATG::LiveResources::UpdateCurrentUser()
{
    if (m_user->is_signed_in())
    {
        if (m_nouserDependentPanel != nullptr)
        {
            m_nouserDependentPanel->Close();
        }

        if (m_userDependentPanel != nullptr)
        {
            m_userDependentPanel->Show();
        }

        m_xboxLiveContext = std::make_shared<xbox::services::xbox_live_context>(m_user);
        m_gamertag->SetText(m_user->gamertag().c_str());
        m_signInErrorLabel->SetVisible(false);
    }
    else
    {
        m_xboxLiveContext.reset();

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