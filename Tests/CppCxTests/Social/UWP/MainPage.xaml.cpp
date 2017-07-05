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
#include "MainPage.xaml.h"

using namespace concurrency;
using namespace Social_CppCx_140;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

struct ScenarioDescriptionItem
{
    int tag;
    Platform::String^ name;
};

enum ScenarioItemTag
{
    Scenario_GetUserProfileAsync = 1,
    Scenario_GetSocialRelationshipsAsync,
    Scenario_VerifyStringAsync
    //Scenario_GetUserProfilesForSocialGroupAsync
};

ScenarioDescriptionItem ScenarioDescriptions[] =
{
    { Scenario_GetUserProfileAsync, L"Get user profile" },
    { Scenario_GetSocialRelationshipsAsync, L"Get social list" },
};

bool MainPage::RunScenario(int selectedTag)
{
    if (m_xboxLiveContext == nullptr || !m_xboxLiveContext->User->IsSignedIn)
    {
        Log(L"No user signed in");
        return false;
    }

    switch (selectedTag)
    {
        case Scenario_GetUserProfileAsync: m_scenarios.Scenario_GetUserProfileAsync(this, m_xboxLiveContext); break;
        case Scenario_GetSocialRelationshipsAsync: m_scenarios.Scenario_GetSocialRelationshipsAsync(this, m_xboxLiveContext); break;
        default: return false;
    }

    return true;
}

MainPage::MainPage()
{
    InitializeComponent();
    this->CoreDispatcher = Windows::UI::Xaml::Window::Current->CoreWindow->Dispatcher;
    m_user = ref new Microsoft::Xbox::Services::System::XboxLiveUser();
    m_user->SignOutCompleted += ref new EventHandler<Microsoft::Xbox::Services::System::SignOutCompletedEventArgs^ >
        ([this](Platform::Object^, Microsoft::Xbox::Services::System::SignOutCompletedEventArgs^ args)
        {
            CoreDispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([this, args]()
            {
                this->UserInfoLabel->Text = L"user signed out";
                Log(L"----------------");
                LogFormat(L"User %s signed out", args->User->Gamertag->Data());
            }));
        });


    for (ScenarioDescriptionItem scenario : ScenarioDescriptions)
    {
        ListBoxItem^ listBoxItem = ref new ListBoxItem();
        listBoxItem->Content = scenario.name;
        listBoxItem->Tag = (Platform::Object^)scenario.tag;
        this->ScenarioListBox->Items->Append(listBoxItem);
    }

    this->ScenarioListBox->SelectedIndex = 0;
    SignInSilently();
}

void MainPage::ScenarioListBox_DoubleTapped(
    Platform::Object^ sender, 
    Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs^ e
    )
{
    RunSelectedScenario();
}

void MainPage::ClearLogs()
{
    this->OutputStackPanel->Children->Clear();
}

void MainPage::LogFormat(LPCWSTR strMsg, ...)
{
    WCHAR strBuffer[2048];

    va_list args;
    va_start(args, strMsg);
    _vsnwprintf_s(strBuffer, 2048, _TRUNCATE, strMsg, args);
    strBuffer[2047] = L'\0';

    va_end(args);

    Platform::String^ str = ref new Platform::String(strBuffer);
    Log(str);
}

void MainPage::Log(Platform::String^ logLine)
{
    TextBlock^ uiElement = ref new TextBlock();
    uiElement->FontSize = 14;
    uiElement->Text = logLine;
    this->OutputStackPanel->Children->Append(uiElement);
}

void MainPage::RunButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    RunSelectedScenario();
}

void MainPage::RunSelectedScenario()
{
    if (this->ScenarioListBox->SelectedItems->Size == 1)
    {
        ClearLogs();
        ListBoxItem^ selectedItem = safe_cast<ListBoxItem^>(this->ScenarioListBox->SelectedItem);
        int selectedTag = safe_cast<int>(selectedItem->Tag);
        RunScenario(selectedTag);
    }
}

void MainPage::RunAllButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ClearLogs();
    int scenarioTag = 1;
    for (;;)
    {
        bool success = RunScenario(scenarioTag);
        if (!success)
        {
            break;
        }

        scenarioTag++;
    }
}


void MainPage::SignInButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    SignIn();
}

void MainPage::SignIn()
{
    this->UserInfoLabel->Text = L"Trying to sign in...";
    Log(this->UserInfoLabel->Text);

    auto asyncOp = m_user->SignInAsync(this->CoreDispatcher);
    create_task(asyncOp)
        .then([this](task<Microsoft::Xbox::Services::System::SignInResult^> t)
    {
        try
        {
            auto result = t.get();
            if (result->Status == Microsoft::Xbox::Services::System::SignInStatus::Success)
            {
                m_xboxLiveContext = ref new Microsoft::Xbox::Services::XboxLiveContext(m_user);
                this->UserInfoLabel->Text = L"Sign in succeeded";
            }
            else if (result->Status == Microsoft::Xbox::Services::System::SignInStatus::UserCancel)
            {
                this->UserInfoLabel->Text = L"User Cancel";
            }

            Log(this->UserInfoLabel->Text);
        }
        catch (Platform::Exception^ e)
        {
            this->UserInfoLabel->Text = L"Sign in failed";
            Log(L"SignInAsync failed.  Exception: " + e->ToString());
        }

        return t;
    });
}

void MainPage::SignInSilently()
{
    this->UserInfoLabel->Text = L"Trying to SignIn Silent...";
    Log(this->UserInfoLabel->Text);

    auto asyncOp = m_user->SignInSilentlyAsync(nullptr);
    create_task(asyncOp)
    .then([this](task<Microsoft::Xbox::Services::System::SignInResult^> t) 
    {
        try
        {
            auto result = t.get();
            if (result->Status == Microsoft::Xbox::Services::System::SignInStatus::Success)
            {
                m_xboxLiveContext = ref new Microsoft::Xbox::Services::XboxLiveContext(m_user);
                this->UserInfoLabel->Text = L"SignIn Silent succeeded";
            }
            else if (result->Status == Microsoft::Xbox::Services::System::SignInStatus::UserInteractionRequired)
            {
                this->UserInfoLabel->Text = L"User Interaction Required";
            }
            Log(this->UserInfoLabel->Text);
        }
        catch (Platform::Exception^ e)
        {
            this->UserInfoLabel->Text = L"SignIn Silent failed";
            Log(L"SignIn Silent failed.  Exception: " + e->ToString());
        }

        return t;
    });
}

void MainPage::SwitchAccountButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    this->UserInfoLabel->Text = L"Trying to Switch Account...";
    Log(this->UserInfoLabel->Text);
}