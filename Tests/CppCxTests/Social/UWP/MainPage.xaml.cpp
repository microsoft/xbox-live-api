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

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Leaderboard;
using namespace Microsoft::Xbox::Services::Statistics;
using namespace Microsoft::Xbox::Services::Statistics::Manager;

struct ScenarioDescriptionItem
{
    int tag;
    Platform::String^ name;
};

enum ScenarioItemTag
{
    Scenario_GetUserProfileAsync = 1,
    Scenario_GetSocialRelationshipsAsync,
    Scenario_WriteStat,
    Scenario_GetStat,
    Scenario_GetLeaderboard,
    Scenario_DeleteStat,
    Scenario_VerifyStringAsync
    //Scenario_GetUserProfilesForSocialGroupAsync
};

ScenarioDescriptionItem ScenarioDescriptions[] =
{
    { Scenario_GetUserProfileAsync, L"Get user profile" },
    { Scenario_GetSocialRelationshipsAsync, L"Get social list" },
    { Scenario_WriteStat, L"Write Stat" },
    { Scenario_GetStat, L"Get Stat" },
    { Scenario_GetLeaderboard, L"Get Leaderboard" },
    { Scenario_DeleteStat, L"Delete Stat" },
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
    case Scenario_WriteStat: m_scenarios.Scenario_WriteStat(this, m_xboxLiveContext); break;
    case Scenario_GetStat: m_scenarios.Scenario_GetStat(this, m_xboxLiveContext); break;
    case Scenario_GetLeaderboard: m_scenarios.Scenario_GetLeaderboard(this, m_xboxLiveContext); break;
    case Scenario_DeleteStat: m_scenarios.Scenario_DeleteStat(this, m_xboxLiveContext); break;
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

    StartTimerAndRegisterHandler();

    this->ScenarioListBox->SelectedIndex = 0;
    SignInSilently();
}

void MainPage::StartTimerAndRegisterHandler()
{
    auto timer = ref new Windows::UI::Xaml::DispatcherTimer();

    TimeSpan ts;
    ts.Duration = 500;
    timer->Interval = ts;
    timer->Start();
    auto registrationToken = timer->Tick += ref new EventHandler<Object^>(this, &MainPage::OnTick);
}

void MainPage::OnTick(Object^ sender, Object^ e)
{
    StatisticManager^ mgr = Microsoft::Xbox::Services::Statistics::Manager::StatisticManager::SingletonInstance;
    if (mgr != nullptr)
    {
        auto EventList = mgr->DoWork();
        for (StatisticEvent^ Event : EventList)
        {
            if (Event->ErrorCode != 0)
            {
                LogFormat(L"DoWork error: %s", Event->ErrorMessage->Data());
            }

            StatisticEventArgs^ args;

            switch (Event->EventType)
            {
            case StatisticEventType::LocalUserAdded:
                LogFormat(L"DoWork LocalUserAdded: %s", Event->User->Gamertag->Data());
                break;

            case StatisticEventType::LocalUserRemoved:
                LogFormat(L"DoWork LocalUserRemoved: %s", Event->User->Gamertag->Data());
                break;

            case StatisticEventType::GetLeaderboardComplete:
            {
                Log(L"DoWork GetLeaderboardComplete:");

                LeaderboardResultEventArgs^ LbResultEventArgs = safe_cast<LeaderboardResultEventArgs^>(Event->EventArgs);
                LeaderboardResult^ LbResult = LbResultEventArgs->Result;

                uint64_t t = LbResult->GetNextQuery()->CallbackId;

                Platform::String^ displayName = LbResult->DisplayName;
                LogFormat(L"Leaderboard displayName: %s\n", displayName->Data());

                for (unsigned int i = 0; i < LbResult->Columns->Size; i++)
                {
                    LeaderboardColumn^ col = LbResult->Columns->GetAt(i);
                    LogFormat(L"Column %d: DisplayName=%s StatName=%s\n", i, col->DisplayName->Data(), col->StatisticName->Data());
                }

                for (LeaderboardRow^ row : LbResult->Rows)
                {
                    String^ colValues;
                    for( unsigned int i = 0; i<row->Values->Size; i++)
                    {
                        Platform::String^ columnValue = row->Values->GetAt(i);
                        
                        colValues = colValues + L" ";
                        colValues = colValues + columnValue;
                    }
                    LogFormat(L"Gametag=%16s Rank=%s Percentile=%s colValues=%s\n", row->Gamertag->Data(), row->Rank.ToString()->Data(), row->Percentile.ToString()->Data(), colValues->Data());
                }
            }
            break;

            case StatisticEventType::StatisticUpdateComplete:
                LogFormat(L"DoWork StatisticUpdateComplete: %s", Event->User->Gamertag->ToString());

                IVectorView<String^>^ stats = mgr->GetStatisticNames(Event->User);
                for (String^ stat : stats)
                {
                    LogFormat(L"DoWork Stat: %s", stat->Data());
                }

                break;
            }
        }
    }
}

void MainPage::ScenarioListBox_DoubleTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs^ e)
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
        //ClearLogs();
        ListBoxItem^ selectedItem = safe_cast<ListBoxItem^>(this->ScenarioListBox->SelectedItem);
        int selectedTag = safe_cast<int>(selectedItem->Tag);
        RunScenario(selectedTag);
    }
}

void MainPage::RunAllButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    //ClearLogs();
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
    create_task(asyncOp).then([this](task<Microsoft::Xbox::Services::System::SignInResult^> t)
    {
        try
        {
            auto result = t.get();
            if (result->Status == Microsoft::Xbox::Services::System::SignInStatus::Success)
            {
                m_xboxLiveContext = ref new Microsoft::Xbox::Services::XboxLiveContext(m_user);

                StatisticManager^ mgr = StatisticManager::SingletonInstance;
                if (mgr == nullptr) return t;
                mgr->AddLocalUser(m_xboxLiveContext->User);

                m_xboxLiveContext->Settings->ServiceCallRouted += 
                    ref new EventHandler<XboxServiceCallRoutedEventArgs^>(
                        [this](Object^, XboxServiceCallRoutedEventArgs^ args)
                        {
                            this->LogFormat(L"[URL]: %s %s", args->HttpMethod->Data(), args->Url->AbsoluteUri->Data());
                            this->LogFormat(L"[Response]: %s %s", args->HttpStatus.ToString()->Data(), args->ResponseBody->Data());
                        });

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
    create_task(asyncOp).then([this](task<Microsoft::Xbox::Services::System::SignInResult^> t)
    {
        try
        {
            auto result = t.get();
            if (result->Status == Microsoft::Xbox::Services::System::SignInStatus::Success)
            {
                m_xboxLiveContext = ref new Microsoft::Xbox::Services::XboxLiveContext(m_user);

                StatisticManager^ mgr = StatisticManager::SingletonInstance;
                if (mgr == nullptr) return t;
                mgr->AddLocalUser(m_xboxLiveContext->User);

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