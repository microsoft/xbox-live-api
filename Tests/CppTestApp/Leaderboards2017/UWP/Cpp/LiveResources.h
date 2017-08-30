// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// Handles Users signing in and out and the related Xbox Live Contexts
//
#pragma once

namespace ATG
{
    class UIManager;
    class IPanel;

    typedef int32_t function_context;

    class LiveResources
    {
    public:

        void Initialize(std::shared_ptr<ATG::UIManager> &ui, ATG::IPanel *userDependantPanel = nullptr, ATG::IPanel *nouserDependantPanel = nullptr);
        void Refresh();

        uint32_t                                           GetTitleId()         const { return m_titleId; }
        std::wstring                                       GetServiceConfigId() const { return m_scid; }

        static bool IsMultiUserApplication();

        std::shared_ptr<xbox::services::system::xbox_live_user> GetCurrentUser()       const { return m_currentUser; }
        int GetCurrentGamepadIndex()       const { return m_currentGamepadIndex; }

        std::shared_ptr<xbox::services::system::xbox_live_user> GetXboxLiveUser(Windows::System::User^ systemUser);

        void TrySignInCurrentUser();
        void TrySignInCurrentUserSilently();
        void SetCurrentGamepadAndUser(int gamepadIndex, Windows::System::User^ systemUser);
        void SetCurrentGamepadAndUser(int gamepadIndex, std::wstring systemUserId);
        void SetCurrentGamepad(int gamepadIndex);
        void SetupCurrentUser(Windows::System::User^ systemUser);

        function_context add_signin_handler(_In_ std::function<void(std::shared_ptr<xbox::services::system::xbox_live_user>, xbox::services::system::sign_in_status)> handler);
        void remove_signin_handler(_In_ function_context context);

        void SetLogCallback(_In_ const std::function<void(const std::wstring&)>& callback);
        void LogLine(const std::wstring& log);

    private:
        void OnUserSignInEvent(_In_ std::shared_ptr<xbox::services::system::xbox_live_user> user, _In_ xbox::services::system::sign_in_status result);
        void HandleSignInResult(_In_ std::shared_ptr<xbox::services::system::xbox_live_user> user, _In_ xbox::services::xbox_live_result<xbox::services::system::sign_in_result> &t);
        void UpdateCurrentUser();

        std::unordered_map<std::wstring, std::shared_ptr<xbox::services::system::xbox_live_user>> m_users;
        std::shared_ptr<xbox::services::system::xbox_live_user> m_currentUser;
        int m_currentGamepadIndex = -1;

        // Title Info
        uint32_t                                           m_titleId;
        std::wstring                                       m_scid;

        // UI Elements
        ATG::Legend*                                       m_gamertag = nullptr;
        ATG::Image*                                        m_gamerPic = nullptr;
        ATG::IPanel*                                       m_userDependentPanel = nullptr;
        ATG::IPanel*                                       m_nouserDependentPanel = nullptr;
        ATG::TextLabel*                                    m_sandboxLabel = nullptr;
        ATG::TextLabel*                                    m_titleIdLabel = nullptr;
        ATG::TextLabel*                                    m_scidLabel = nullptr;
        ATG::TextLabel*                                    m_signInErrorLabel = nullptr;

        std::mutex m_writeLock;
        std::unordered_map<function_context, std::function<void(std::shared_ptr<xbox::services::system::xbox_live_user>, xbox::services::system::sign_in_status)>> m_signinRoutedHandlers;
        function_context m_signinRoutedHandlersCounter;
        std::function<void(const std::wstring&)> m_logCallback;
    };
}