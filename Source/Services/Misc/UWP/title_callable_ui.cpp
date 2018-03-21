// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#if !UNIT_TEST_SERVICES

#if defined(_APISET_TARGET_VERSION_WIN10_RS3)
    #define RS3_TCUI
    #define INVITE_WITH_CONTEXT
    #define _APISET_TARGET_VERSION _APISET_TARGET_VERSION_WIN10_RS3
#elif defined(_APISET_TARGET_VERSION_WIN10_RS2)
    #define INVITE_WITH_CONTEXT
    #define _APISET_TARGET_VERSION _APISET_TARGET_VERSION_WIN10_RS2
#else
    #define _APISET_TARGET_VERSION _APISET_TARGET_VERSION_WIN10_RS1
#endif

#if UWP_API || TV_API || UNIT_TEST_SERVICES
#include <gamingtcui.h>
#include <windows.system.h>
#endif
#include "xbox_system_factory.h"
#include "xsapi/title_callable_ui.h"


#define XBOX_APP_PFN _T("Microsoft.XboxApp_8wekyb3d8bbwe")
#define XBOX_DEEPLINK_FRIEND_FINDER _T("xbox-friendfinder:facebook")
#define XBOX_DEEPLINK_TITLE_HUB _T("xbox-gamehub:?titleId=")
#define XBOX_DEEPLINK_USER_SETTINGS _T("xbox-settings:general")
// customize profile isn't implemented yet on win10
#define XBOX_DEEPLINK_CUSTOMIZE_PROFILE _T("xbox-profile:customize")

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

#if UWP_API || TV_API || UNIT_TEST_SERVICES

class tcui_context
{
public:
    tcui_context() : m_returnCode(S_OK)
    {
    }

    HRESULT wait()
    {
        m_event.wait();
        return m_returnCode;
    }

    void set( HRESULT hr )
    {
        m_returnCode = hr;
        m_event.set();
    }

    void set(HRESULT hr, const std::vector<string_t>& payload)
    {
        m_payload = payload;
        m_returnCode = hr;
        m_event.set();
    }

    std::vector<string_t> payload()
    {
        return m_payload;
    }

private:
    HRESULT m_returnCode;
    std::vector<string_t> m_payload;

#if TV_API
    pplx::details::event_impl m_event;
#else
    pplx::event m_event;
#endif
};

// Detect if app is MUA and API is supported
bool IsMultiUserAPISupported()
{
    // Save the result in memory, as we only need to check once;
    static int isSupported = -1;

    // Only RS1 sdk will have this check.
#ifdef NTDDI_WIN10_RS1
    if (isSupported == -1)
    {
        // all RS1 based TCUI calls are based around multi-user
        isSupported = Windows::Foundation::Metadata::ApiInformation::IsMethodPresent("Windows.System.UserPicker", "IsSupported");
    }
#endif
    return isSupported == 1;
}

void WINAPI UICompletionRoutine(
    _In_ HRESULT returnCode,
    _In_ void* context
    )
{
    tcui_context* uiContext = (tcui_context*)context;
    if (uiContext != nullptr)
    {
        try
        {
            uiContext->set(returnCode);
        }
        catch (Platform::Exception^ exception)
        {
            HRESULT hr = (HRESULT)exception->HResult;
            uiContext->set(hr);
        }
        catch (...) // everything else
        {
            HRESULT hr = xbox::services::utils::convert_exception_to_hresult();
            uiContext->set(hr);
        }
    }
}

VOID WINAPI UIPlayerPickerUICompletionRoutine(
    _In_ HRESULT returnCode,
    _In_ void* context,
    _In_reads_(selectedXuidsCount) const HSTRING* selectedXuids,
    _In_ size_t selectedXuidsCount
    )
{
    tcui_context* uiContext = (tcui_context*)context;
    if (uiContext != nullptr)
    {
        try
        {
            std::vector<string_t> payload;
            for (size_t i = 0; i < selectedXuidsCount; i++)
            {
                Platform::String^ str = ref new Platform::String(selectedXuids[i]);
                payload.push_back(str->Data());
            }

            uiContext->set(returnCode, payload);
        }
        catch (Platform::Exception^ exception)
        {
            HRESULT hr = (HRESULT)exception->HResult;
            uiContext->set(hr);
        }
        catch (...) // everything else
        {
            HRESULT hr = xbox::services::utils::convert_exception_to_hresult();
            uiContext->set(hr);
        }
    }
}

pplx::task<xbox::services::xbox_live_result<std::vector<string_t>>>
title_callable_ui::show_player_picker_ui(
    _In_ const string_t& promptDisplayText,
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const std::vector<string_t>& preselectedXboxUserIds,
    _In_ uint32_t minSelectionCount,
    _In_ uint32_t maxSelectionCount
#if UWP_API
    , _In_opt_ Windows::System::User^ user
#endif
    )
{
    auto task = pplx::create_task([
        promptDisplayText, 
        xboxUserIds, 
        preselectedXboxUserIds, 
        minSelectionCount, 
        maxSelectionCount
#if UWP_API
        , user
#endif
        ]()
    {
        tcui_context context;
        Platform::String^ strPromptDisplayText = ref new Platform::String(promptDisplayText.c_str());
        std::vector<Platform::String^> strXboxUserIds;
        std::vector<Platform::String^> strPreselectedXboxUserIds;
        std::vector<HSTRING> hXboxUserIds;
        std::vector<HSTRING> hPreselectedXboxUserIds;

        for (const auto& s : xboxUserIds)
        {
            Platform::String^ str = ref new Platform::String(s.c_str());
            strXboxUserIds.push_back(str);
            hXboxUserIds.push_back( reinterpret_cast<HSTRING>(str) );
        }

        for (const auto& s : preselectedXboxUserIds)
        {
            Platform::String^ str = ref new Platform::String(s.c_str());
            strPreselectedXboxUserIds.push_back(str);
            hPreselectedXboxUserIds.push_back(reinterpret_cast<HSTRING>(str));
        }

        HRESULT hr = S_OK;
        if (hXboxUserIds.empty())
        {
            hr = E_INVALIDARG;
        }
        else
        {
#if UWP_API
            if (user != nullptr && IsMultiUserAPISupported())
            {
                ABI::Windows::System::IUser* userAbi = reinterpret_cast<ABI::Windows::System::IUser*>(user);
                hr = ShowPlayerPickerUIForUser(
                    userAbi,
                    reinterpret_cast<HSTRING>(strPromptDisplayText),
                    &hXboxUserIds[0],
                    hXboxUserIds.size(),
                    hPreselectedXboxUserIds.empty() ? nullptr : &hPreselectedXboxUserIds[0],
                    hPreselectedXboxUserIds.size(),
                    minSelectionCount,
                    maxSelectionCount,
                    UIPlayerPickerUICompletionRoutine,
                    static_cast<void*>(&context)
                    );
            }
            else
#endif
            {
                hr = ShowPlayerPickerUI(
                    reinterpret_cast<HSTRING>(strPromptDisplayText),
                    &hXboxUserIds[0],
                    hXboxUserIds.size(),
                    hPreselectedXboxUserIds.empty() ? nullptr : &hPreselectedXboxUserIds[0],
                    hPreselectedXboxUserIds.size(),
                    minSelectionCount,
                    maxSelectionCount,
                    UIPlayerPickerUICompletionRoutine,
                    static_cast<void*>(&context)
                    );
            }
        }
        if (SUCCEEDED(hr) || hr == E_PENDING)
        {
            hr = ProcessPendingGameUI(true);
            if (SUCCEEDED(hr))
            {
                hr = context.wait();
            }
        }

        std::error_code errcode = std::make_error_code(static_cast<xbox_live_error_code>(hr));

        std::vector<string_t> payload = context.payload();
        return xbox_live_result<std::vector<string_t>>(payload, errcode);
    });

    return utils::create_exception_free_task<std::vector<string_t>>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_game_invite_ui(
    _In_ const xbox::services::multiplayer::multiplayer_session_reference& sessionReference,
    _In_ const string_t& invitationDisplayText,
    _In_ const string_t& contextStringId
#if UWP_API
    , _In_opt_ Windows::System::User^ user
#endif
    )
{
    auto task = pplx::create_task([
        sessionReference,
        invitationDisplayText,
        contextStringId
#if UWP_API
        , user
#endif
        ]()
    {
        tcui_context context;

        Platform::String^ serviceConfigurationId = ref new Platform::String(sessionReference.service_configuration_id().c_str());
        Platform::String^ sessionTemplateName = ref new Platform::String(sessionReference.session_template_name().c_str());
        Platform::String^ sessionName = ref new Platform::String(sessionReference.session_name().c_str());
        Platform::String^ strInvitationDisplayText = ref new Platform::String(invitationDisplayText.c_str());
        Platform::String^ strContextStringId = ref new Platform::String(contextStringId.c_str());

        HRESULT hr = S_OK;
#if UWP_API
        if (user != nullptr && IsMultiUserAPISupported())
        {
            ABI::Windows::System::IUser* userAbi = reinterpret_cast<ABI::Windows::System::IUser*>(user);
#ifdef INVITE_WITH_CONTEXT
            if (strContextStringId != nullptr)
            {
                hr = ShowGameInviteUIWithContextForUser(
                    userAbi,
                    reinterpret_cast<HSTRING>(serviceConfigurationId),
                    reinterpret_cast<HSTRING>(sessionTemplateName),
                    reinterpret_cast<HSTRING>(sessionName),
                    reinterpret_cast<HSTRING>(strInvitationDisplayText),
                    reinterpret_cast<HSTRING>(strContextStringId),
                    UICompletionRoutine,
                    static_cast<void*>(&context)
                    );
            }
            else
#endif
            {
                hr = ShowGameInviteUIForUser(
                    userAbi,
                    reinterpret_cast<HSTRING>(serviceConfigurationId),
                    reinterpret_cast<HSTRING>(sessionTemplateName),
                    reinterpret_cast<HSTRING>(sessionName),
                    reinterpret_cast<HSTRING>(strInvitationDisplayText),
                    UICompletionRoutine,
                    static_cast<void*>(&context)
                    );
            }
        }
        else
#endif
        {
#ifdef INVITE_WITH_CONTEXT
            if (strContextStringId != nullptr)
            {
                hr = ShowGameInviteUIWithContext(
                    reinterpret_cast<HSTRING>(serviceConfigurationId),
                    reinterpret_cast<HSTRING>(sessionTemplateName),
                    reinterpret_cast<HSTRING>(sessionName),
                    reinterpret_cast<HSTRING>(strInvitationDisplayText),
                    reinterpret_cast<HSTRING>(strContextStringId),
                    UICompletionRoutine,
                    static_cast<void*>(&context)
                    );
            }
            else
#endif
            {
                hr = ShowGameInviteUI(
                    reinterpret_cast<HSTRING>(serviceConfigurationId),
                    reinterpret_cast<HSTRING>(sessionTemplateName),
                    reinterpret_cast<HSTRING>(sessionName),
                    reinterpret_cast<HSTRING>(strInvitationDisplayText),
                    UICompletionRoutine,
                    static_cast<void*>(&context)
                    );
            }
        }

        if (SUCCEEDED(hr) || hr == E_PENDING)
        {
            hr = ProcessPendingGameUI(true);
            if (SUCCEEDED(hr))
            {
                hr = context.wait();
            }
        }

        std::error_code errcode = std::make_error_code(static_cast<xbox_live_error_code>(hr));
        return xbox_live_result<void>(errcode);
    });

    return utils::create_exception_free_task<void>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_profile_card_ui(
    _In_ const string_t& targetXboxUserId
#if UWP_API
    , _In_opt_ Windows::System::User^ user
#endif
    )
{
    auto task = pplx::create_task([targetXboxUserId
#if UWP_API
        , user
#endif
        ]()
    {
        tcui_context context;
        Platform::String^ strTargetXboxUserId = ref new Platform::String(targetXboxUserId.c_str());

        HRESULT hr = S_OK;
#if UWP_API
        if (user != nullptr && IsMultiUserAPISupported())
        {
            ABI::Windows::System::IUser* userAbi = reinterpret_cast<ABI::Windows::System::IUser*>(user);
            hr = ShowProfileCardUIForUser(
                userAbi,
                reinterpret_cast<HSTRING>(strTargetXboxUserId),
                UICompletionRoutine,
                static_cast<void*>(&context)
                );
        }
        else
#endif
        {
            hr = ShowProfileCardUI(
                reinterpret_cast<HSTRING>(strTargetXboxUserId),
                UICompletionRoutine,
                static_cast<void*>(&context)
                );
        }

        if (SUCCEEDED(hr) || hr == E_PENDING)
        {
            hr = ProcessPendingGameUI(true);
            if (SUCCEEDED(hr))
            {
                hr = context.wait();
            }
        }

        std::error_code errcode = std::make_error_code(static_cast<xbox_live_error_code>(hr));
        return xbox_live_result<void>(errcode);
    });

    return utils::create_exception_free_task<void>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_change_friend_relationship_ui(
    _In_ const string_t& targetXboxUserId
#if UWP_API
    , _In_opt_ Windows::System::User^ user
#endif
    )
{
    auto task = pplx::create_task([targetXboxUserId
#if UWP_API
        , user
#endif
        ]()
    {
        tcui_context context;
        Platform::String^ strTargetXboxUserId = ref new Platform::String(targetXboxUserId.c_str());

        HRESULT hr = S_OK;
#if UWP_API
        if (user != nullptr && IsMultiUserAPISupported())
        {
            ABI::Windows::System::IUser* userAbi = reinterpret_cast<ABI::Windows::System::IUser*>(user);
            hr = ShowChangeFriendRelationshipUIForUser(
                userAbi,
                reinterpret_cast<HSTRING>(strTargetXboxUserId),
                UICompletionRoutine,
                static_cast<void*>(&context)
                );
        }
        else
#endif
        {
            hr = ShowChangeFriendRelationshipUI(
                reinterpret_cast<HSTRING>(strTargetXboxUserId),
                UICompletionRoutine,
                static_cast<void*>(&context)
                );
        }

        if (SUCCEEDED(hr) || hr == E_PENDING)
        {
            hr = ProcessPendingGameUI(true);
            if (SUCCEEDED(hr))
            {
                hr = context.wait();
            }
        }

        std::error_code errcode = std::make_error_code(static_cast<xbox_live_error_code>(hr));
        return xbox_live_result<void>(errcode);
    });

    return utils::create_exception_free_task<void>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_title_achievements_ui(
    _In_ uint32_t titleId
#if UWP_API
    , _In_opt_ Windows::System::User^ user
#endif
    )
{
    auto task = pplx::create_task([titleId
#if UWP_API
        , user
#endif
        ]()
    {
        tcui_context context;

        HRESULT hr = S_OK;
#if UWP_API
        if (user != nullptr && IsMultiUserAPISupported())
        {
            ABI::Windows::System::IUser* userAbi = reinterpret_cast<ABI::Windows::System::IUser*>(user);
            hr = ShowTitleAchievementsUIForUser(
                userAbi,
                titleId,
                UICompletionRoutine,
                static_cast<void*>(&context)
                );
        }
        else
#endif
        {
            hr = ShowTitleAchievementsUI(
                titleId,
                UICompletionRoutine,
                static_cast<void*>(&context)
                );
        }

        if (SUCCEEDED(hr) || hr == E_PENDING)
        {
            hr = ProcessPendingGameUI(true);
            if (SUCCEEDED(hr))
            {
                hr = context.wait();
            }
        }

        std::error_code errcode = std::make_error_code(static_cast<xbox_live_error_code>(hr));
        return xbox_live_result<void>(errcode);
    });

    return utils::create_exception_free_task<void>(
        task
        );
}

void
title_callable_ui::_Get_gaming_privilege_scope_policy(
    _Out_ Platform::String^& scope,
    _Out_ Platform::String^& policy
    )
{
    auto localConfig = xbox_system_factory::get_factory()->create_local_config();
    auth_config authConfig(
        localConfig->sandbox(),
        localConfig->environment_prefix(),
        localConfig->environment(),
        localConfig->use_first_party_token(),
        localConfig->is_creators_title(),
        localConfig->scope()
        );

    scope = PLATFORM_STRING_FROM_INTERNAL_STRING(authConfig.rps_ticket_service());
    policy = PLATFORM_STRING_FROM_INTERNAL_STRING(authConfig.rps_ticket_policy());
}

xbox::services::xbox_live_result<bool>
title_callable_ui::check_gaming_privilege_silently(
    _In_ gaming_privilege privilege
#if UWP_API
    , _In_opt_ Windows::System::User^ user
#endif
    )
{
    Platform::String^ scope;
    Platform::String^ policy;
    _Get_gaming_privilege_scope_policy(scope, policy);

    BOOL hasPrivilege = FALSE;

    HRESULT hr = S_OK;
#if UWP_API
    if (user != nullptr && IsMultiUserAPISupported())
    {
        ABI::Windows::System::IUser* userAbi = reinterpret_cast<ABI::Windows::System::IUser*>(user);
        hr = CheckGamingPrivilegeSilentlyForUser(
            userAbi,
            static_cast<UINT32>(privilege),
            reinterpret_cast<HSTRING>(scope),
            reinterpret_cast<HSTRING>(policy),
            &hasPrivilege
            );
    }
    else
#endif
    {
        hr = CheckGamingPrivilegeSilently(
            static_cast<UINT32>(privilege),
            reinterpret_cast<HSTRING>(scope),
            reinterpret_cast<HSTRING>(policy),
            &hasPrivilege
            );
    }

    std::error_code errcode = std::make_error_code(static_cast<xbox_live_error_code>(hr));
    return xbox_live_result<bool>(hasPrivilege == TRUE, errcode);
}

pplx::task<xbox::services::xbox_live_result<bool>>
title_callable_ui::check_gaming_privilege_with_ui(
    _In_ gaming_privilege privilege,
    _In_ string_t friendlyMessage
#if UWP_API
    , _In_opt_ Windows::System::User^ user
#endif
    )
{
    auto task = pplx::create_task([privilege, friendlyMessage
#if UWP_API
        , user
#endif
    ]()
    {
        tcui_context context;

        Platform::String^ scope;
        Platform::String^ policy;
        _Get_gaming_privilege_scope_policy(scope, policy);
        Platform::String^ friendlyMessageStr = ref new Platform::String(friendlyMessage.c_str());

        BOOL hasPrivilege = FALSE;
        HRESULT hr = S_OK;
#if UWP_API
        if (user != nullptr && IsMultiUserAPISupported())
        {
            ABI::Windows::System::IUser* userAbi = reinterpret_cast<ABI::Windows::System::IUser*>(user);
            hr = CheckGamingPrivilegeWithUIForUser(
                userAbi,
                static_cast<UINT32>(privilege),
                reinterpret_cast<HSTRING>(scope),
                reinterpret_cast<HSTRING>(policy),
                reinterpret_cast<HSTRING>(friendlyMessageStr),
                UICompletionRoutine,
                static_cast<void*>(&context)
                );
        }
        else
#endif
        {
            hr = CheckGamingPrivilegeWithUI(
                static_cast<UINT32>(privilege),
                reinterpret_cast<HSTRING>(scope),
                reinterpret_cast<HSTRING>(policy),
                reinterpret_cast<HSTRING>(friendlyMessageStr),
                UICompletionRoutine,
                static_cast<void*>(&context)
                );
        }
        if (SUCCEEDED(hr) || hr == E_PENDING)
        {
            hr = ProcessPendingGameUI(true);
            if (SUCCEEDED(hr))
            {
                hr = context.wait();
                if (SUCCEEDED(hr))
                {
                    hr = CheckGamingPrivilegeSilently(
                        static_cast<UINT32>(privilege),
                        reinterpret_cast<HSTRING>(scope),
                        reinterpret_cast<HSTRING>(policy),
                        &hasPrivilege
                        );
                }
            }
        }

        std::error_code errcode = std::make_error_code(static_cast<xbox_live_error_code>(hr));
        return xbox_live_result<bool>(hasPrivilege == TRUE, errcode);
    });

    return utils::create_exception_free_task<bool>(
        task
        );
}
#endif // UWP_API || TV_API

#if defined(RS3_TCUI)
pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_friend_finder_ui(
#if UWP_API
    _In_opt_ Windows::System::User^ user
#endif
    )
{
    auto task = pplx::create_task([
#if UWP_API
        user
#endif
    ]()
    {
        tcui_context context;

        HRESULT hr = S_OK;
#if UWP_API
        if (user != nullptr && IsMultiUserAPISupported())
        {
            ABI::Windows::System::IUser* userAbi = reinterpret_cast<ABI::Windows::System::IUser*>(user);
            hr = ShowFindFriendsUIForUser(
                userAbi,
                UICompletionRoutine,
                static_cast<void*>(&context)
            );
        }
        else
#endif
        {
            hr = ShowFindFriendsUI(
                UICompletionRoutine,
                static_cast<void*>(&context)
            );
        }

        if (SUCCEEDED(hr) || hr == E_PENDING)
        {
            hr = ProcessPendingGameUI(true);
            if (SUCCEEDED(hr))
            {
                hr = context.wait();
            }
        }

        std::error_code errcode = std::make_error_code(static_cast<xbox_live_error_code>(hr));
        return xbox_live_result<void>(errcode);
    });

    return utils::create_exception_free_task<void>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_user_profile_ui(
    _In_ const string_t& targetXboxUserId
    )
{
    // Show the profile card tcui
    return show_profile_card_ui(targetXboxUserId);
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_title_hub_ui(
#if UWP_API
    _In_opt_ Windows::System::User^ user
#endif
    )
{
    auto localConfig = xbox_system_factory::get_factory()->create_local_config();
    uint32_t titleId = localConfig->title_id();

    auto task = pplx::create_task([titleId
#if UWP_API
        , user
#endif
    ]()
    {
        tcui_context context;

        HRESULT hr = S_OK;
#if UWP_API
        if (user != nullptr && IsMultiUserAPISupported())
        {
            ABI::Windows::System::IUser* userAbi = reinterpret_cast<ABI::Windows::System::IUser*>(user);
            hr = ShowGameInfoUIForUser(
                userAbi,
                titleId,
                UICompletionRoutine,
                static_cast<void*>(&context)
            );
        }
        else
#endif
        {
            hr = ShowGameInfoUI(
                titleId,
                UICompletionRoutine,
                static_cast<void*>(&context)
            );
        }

        if (SUCCEEDED(hr) || hr == E_PENDING)
        {
            hr = ProcessPendingGameUI(true);
            if (SUCCEEDED(hr))
            {
                hr = context.wait();
            }
        }

        std::error_code errcode = std::make_error_code(static_cast<xbox_live_error_code>(hr));
        return xbox_live_result<void>(errcode);
    });

    return utils::create_exception_free_task<void>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_user_settings_ui(
#if UWP_API
    _In_opt_ Windows::System::User^ user
#endif
    )
{
    auto task = pplx::create_task([
#if UWP_API
        user
#endif
    ]()
    {
        tcui_context context;

        HRESULT hr = S_OK;
#if UWP_API
        if (user != nullptr && IsMultiUserAPISupported())
        {
            ABI::Windows::System::IUser* userAbi = reinterpret_cast<ABI::Windows::System::IUser*>(user);
            hr = ShowUserSettingsUIForUser(
                userAbi,
                UICompletionRoutine,
                static_cast<void*>(&context)
            );
        }
        else
#endif
        {
            hr = ShowUserSettingsUI(
                UICompletionRoutine,
                static_cast<void*>(&context)
            );
        }

        if (SUCCEEDED(hr) || hr == E_PENDING)
        {
            hr = ProcessPendingGameUI(true);
            if (SUCCEEDED(hr))
            {
                hr = context.wait();
            }
        }

        std::error_code errcode = std::make_error_code(static_cast<xbox_live_error_code>(hr));
        return xbox_live_result<void>(errcode);
    });

    return utils::create_exception_free_task<void>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_customize_user_profile_ui(
#if UWP_API
    _In_opt_ Windows::System::User^ user
#endif
    )
{
    auto task = pplx::create_task([
#if UWP_API
        user
#endif
    ]()
    {
        tcui_context context;

        HRESULT hr = S_OK;
#if UWP_API
        if (user != nullptr && IsMultiUserAPISupported())
        {
            ABI::Windows::System::IUser* userAbi = reinterpret_cast<ABI::Windows::System::IUser*>(user);
            hr = ShowCustomizeUserProfileUIForUser(
                userAbi,
                UICompletionRoutine,
                static_cast<void*>(&context)
            );
        }
        else
#endif
        {
            hr = ShowCustomizeUserProfileUI(
                UICompletionRoutine,
                static_cast<void*>(&context)
            );
        }

        if (SUCCEEDED(hr) || hr == E_PENDING)
        {
            hr = ProcessPendingGameUI(true);
            if (SUCCEEDED(hr))
            {
                hr = context.wait();
            }
        }

        std::error_code errcode = std::make_error_code(static_cast<xbox_live_error_code>(hr));
        return xbox_live_result<void>(errcode);
    });

    return utils::create_exception_free_task<void>(
        task
        );
}
#elif defined(XSAPI_U)
pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_friend_finder_ui()
{
    Platform::String^ urlStr = ref new Platform::String(XBOX_DEEPLINK_FRIEND_FINDER);

    Windows::Foundation::Uri^ uri = ref new Windows::Foundation::Uri(urlStr);

    Windows::System::LauncherOptions^ options = ref new Windows::System::LauncherOptions();
    options->TargetApplicationPackageFamilyName = XBOX_APP_PFN;

    auto task = pplx::create_task(Windows::System::Launcher::LaunchUriAsync(uri, options))
    .then([](bool result)
    {
        if (result)
        {
            return xbox_live_result<void>();
        }
        else
        {
            return xbox_live_result<void>(xbox_live_error_code::runtime_error, "The Xbox App failed to launch");
        }
    });

    return utils::create_exception_free_task<void>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_user_profile_ui(
    _In_ const string_t& targetXboxUserId
    )
{
    // Show the profile card tcui
    return show_profile_card_ui(targetXboxUserId);
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_title_hub_ui()
{
    auto localConfig = xbox_system_factory::get_factory()->create_local_config();
    uint32_t titleId = localConfig->title_id();

    stringstream_t ss;
    ss << XBOX_DEEPLINK_TITLE_HUB;
    ss << titleId;
    string_t link = ss.str();
    Platform::String^ urlStr = ref new Platform::String(link.c_str());

    Windows::Foundation::Uri^ uri = ref new Windows::Foundation::Uri(urlStr);

    Windows::System::LauncherOptions^ options = ref new Windows::System::LauncherOptions();
    options->TargetApplicationPackageFamilyName = XBOX_APP_PFN;

    auto task = pplx::create_task(Windows::System::Launcher::LaunchUriAsync(uri, options))
    .then([](bool result)
    {
        if (result)
        {
            return xbox_live_result<void>();
        }
        else
        {
            return xbox_live_result<void>(xbox_live_error_code::runtime_error, "The Xbox App failed to launch");
        }
    });

    return utils::create_exception_free_task<void>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_user_settings_ui()
{
    Platform::String^ urlStr = ref new Platform::String(XBOX_DEEPLINK_USER_SETTINGS);

    Windows::Foundation::Uri^ uri = ref new Windows::Foundation::Uri(urlStr);

    Windows::System::LauncherOptions^ options = ref new Windows::System::LauncherOptions();
    options->TargetApplicationPackageFamilyName = XBOX_APP_PFN;

    auto task = pplx::create_task(Windows::System::Launcher::LaunchUriAsync(uri, options))
    .then([](bool result)
    {
        if (result)
        {
            return xbox_live_result<void>();
        }
        else
        {
            return xbox_live_result<void>(xbox_live_error_code::runtime_error, "The Xbox App failed to launch");
        }
    });

    return utils::create_exception_free_task<void>(
        task
        );
}
#endif

#if defined(XSAPI_U)
pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_add_friends_ui()
{
    return pplx::task_from_result<xbox::services::xbox_live_result<void>>(xbox::services::xbox_live_result<void>(xbox_live_error_code::unsupported));
}
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif