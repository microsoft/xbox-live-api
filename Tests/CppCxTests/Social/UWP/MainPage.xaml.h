//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once

#include "MainPage.g.h"
#include "Scenarios.h"

namespace Social_CppCx_140
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public ref class MainPage sealed
    {
    public:
        MainPage();
        void OnTick(Object^ sender, Object^ e);

    internal:
        void ClearLogs();
        void LogFormat(LPCWSTR strMsg, ...);
        void Log(Platform::String^ logLine);
        property Windows::UI::Core::CoreDispatcher^ CoreDispatcher;

    private:
        void StartTimerAndRegisterHandler();

        void ScenarioListBox_DoubleTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs^ e);
        void RunButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void RunAllButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SignInButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SwitchAccountButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void SignIn();
        void SignInSilently();

        void RunSelectedScenario();
        bool RunScenario(int selectedTag);

        Microsoft::Xbox::Services::System::XboxLiveUser^ m_user;
        Microsoft::Xbox::Services::XboxLiveContext^ m_xboxLiveContext;
        Scenarios m_scenarios;
    };
}
