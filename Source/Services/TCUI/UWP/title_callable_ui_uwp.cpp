// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

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

#include <gamingtcui.h>
#include <windows.system.h>
#include "xsapi-cpp/title_callable_ui.h"
#include "local_config.h"

#define XBOX_APP_PFN _T("Microsoft.XboxApp_8wekyb3d8bbwe")
#define XBOX_DEEPLINK_FRIEND_FINDER _T("xbox-friendfinder:facebook")
#define XBOX_DEEPLINK_TITLE_HUB _T("xbox-gamehub:?titleId=")
#define XBOX_DEEPLINK_USER_SETTINGS _T("xbox-settings:general")
// customize profile isn't implemented yet on win10
#define XBOX_DEEPLINK_CUSTOMIZE_PROFILE _T("xbox-profile:customize")

namespace
{
    xbox_live_error_code
    convert_exception_to_xbox_live_error_code()
    {
        // Default value, if there is no exception appears, return no_error
        xbox::services::xbox_live_error_code errCode = xbox_live_error_code::no_error;

        try
        {
            throw;
        }
        // std exceptions
        catch (const std::bad_alloc&) // is an exception
        {
            errCode = xbox_live_error_code::bad_alloc;
        }
        catch (const std::bad_cast&) // is an exception
        {
            errCode = xbox_live_error_code::bad_cast;
        }
        catch (const std::invalid_argument&) // is a logic_error
        {
            errCode = xbox_live_error_code::invalid_argument;
        }
        catch (const std::out_of_range&) // is a logic_error
        {
            errCode = xbox_live_error_code::out_of_range;
        }
        catch (const std::length_error&) // is a logic_error
        {
            errCode = xbox_live_error_code::length_error;
        }
        catch (const std::range_error&) // is a runtime_error
        {
            errCode = xbox_live_error_code::range_error;
        }
        catch (const std::system_error& ex) // is a runtime_error
        {
            errCode = static_cast<xbox_live_error_code>(ex.code().value());
        }
        catch (const std::logic_error&) // is an exception
        {
            errCode = xbox_live_error_code::logic_error;
        }
        catch (const std::runtime_error&) // is an exception
        {
            errCode = xbox_live_error_code::runtime_error;
        }
#if !XSAPI_NO_PPL
        catch (const web::http::http_exception& ex) // is an exception
        {
            errCode = static_cast<xbox_live_error_code>(ex.error_code().value());
        }
#endif // !XSAPI_NO_PPL
        catch (const xbox::services::uri_exception&) // is an exception
        {
            errCode = xbox_live_error_code::uri_error;
        }
        catch (const std::exception&) // base class for standard C++ exceptions
        {
            errCode = xbox_live_error_code::generic_error;
        }
#if HC_PLATFORM_IS_MICROSOFT
        catch (HRESULT exceptionHR)
        {
            errCode = static_cast<xbox_live_error_code>(exceptionHR);
        }
#endif
        catch (...) // everything else
        {
            errCode = xbox_live_error_code::generic_error;
        }

        return errCode;
    }

#if !XSAPI_NO_PPL

template<typename T>
pplx::task <xbox_live_result<T>> create_exception_free_task(
    _In_ const pplx::task <xbox_live_result<T>>& t
)
{
    return t.then([](pplx::task <xbox::services::xbox_live_result<T>> result)
        {
            try
            {
                return result.get();
            }
            catch (const std::exception& e)
            {
                xbox_live_error_code err = convert_exception_to_xbox_live_error_code();
                return xbox_live_result<T>(err, e.what());
            }
#ifdef __cplusplus_winrt
            catch (Platform::Exception^ e)
            {
                xbox_live_error_code errc = static_cast<xbox_live_error_code>(e->HResult);
                return xbox_live_result<T>(errc, xbox::services::convert::utf16_to_utf8(e->Message->Data()));
            }
#endif
        });
}

#endif // !XSAPI_NO_PPL

}



NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

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
    pplx::event m_event;
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

pplx::task<xbox::services::xbox_live_result<std::vector<string_t>>> title_callable_ui::show_player_picker_ui(
    _In_ const string_t& promptDisplayText,
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const std::vector<string_t>& preselectedXboxUserIds,
    _In_ uint32_t minSelectionCount,
    _In_ uint32_t maxSelectionCount,
    _In_opt_ Windows::System::User^ user
)
{
    auto task = pplx::create_task([
        promptDisplayText, 
        xboxUserIds, 
        preselectedXboxUserIds, 
        minSelectionCount, 
        maxSelectionCount, 
        user]()
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

    return create_exception_free_task<std::vector<string_t>>(
        task
    );
}

pplx::task<xbox::services::xbox_live_result<void>> title_callable_ui::show_game_invite_ui(
    _In_ const xbox::services::multiplayer::multiplayer_session_reference& sessionReference,
    _In_ const string_t& invitationDisplayText,
    _In_ const string_t& contextStringId,
    _In_opt_ Windows::System::User^ user
)
{
    auto task = pplx::create_task([
        sessionReference,
        invitationDisplayText,
        contextStringId,
        user]()
    {
        tcui_context context;

        Platform::String^ serviceConfigurationId = ref new Platform::String(sessionReference.service_configuration_id().c_str());
        Platform::String^ sessionTemplateName = ref new Platform::String(sessionReference.session_template_name().c_str());
        Platform::String^ sessionName = ref new Platform::String(sessionReference.session_name().c_str());
        Platform::String^ strInvitationDisplayText = ref new Platform::String(invitationDisplayText.c_str());
        Platform::String^ strContextStringId = ref new Platform::String(contextStringId.c_str());

        HRESULT hr = S_OK;
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

    return create_exception_free_task<void>(
        task
    );
}

pplx::task<xbox::services::xbox_live_result<void>> title_callable_ui::show_profile_card_ui(
    _In_ const string_t& targetXboxUserId,
    _In_opt_ Windows::System::User^ user
)
{
    auto task = pplx::create_task([targetXboxUserId, user]()
    {
        tcui_context context;
        Platform::String^ strTargetXboxUserId = ref new Platform::String(targetXboxUserId.c_str());

        HRESULT hr = S_OK;
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

    return create_exception_free_task<void>(
        task
    );
}

pplx::task<xbox::services::xbox_live_result<void>> title_callable_ui::show_change_friend_relationship_ui(
    _In_ const string_t& targetXboxUserId,
    _In_opt_ Windows::System::User^ user
)
{
    auto task = pplx::create_task([targetXboxUserId, user]()
    {
        tcui_context context;
        Platform::String^ strTargetXboxUserId = ref new Platform::String(targetXboxUserId.c_str());

        HRESULT hr = S_OK;
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

    return create_exception_free_task<void>(
        task
    );
}

pplx::task<xbox::services::xbox_live_result<void>> title_callable_ui::show_title_achievements_ui(
    _In_ uint32_t titleId,
    _In_opt_ Windows::System::User^ user

)
{
    auto task = pplx::create_task([titleId, user]()
    {
        tcui_context context;

        HRESULT hr = S_OK;
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

    return create_exception_free_task<void>(
        task
    );
}

void title_callable_ui::_Get_gaming_privilege_scope_policy(
    _Out_ Platform::String^& scope,
    _Out_ Platform::String^& policy
    )
{
    auto localConfig{ AppConfig::Instance()->LocalConfig() };

    String rpsTicketService{ localConfig->scope() };
    if (!localConfig->is_creators_title() && localConfig->use_first_party_token())
    {
        Stringstream ss;
        ss << "open-user.auth" << localConfig->environment() << ".xboxlive.com";
        rpsTicketService = ss.str();
    }

    String rpsTicketPolicy{ localConfig->use_first_party_token() ? "MBI_SSL" : "DELEGATION" };

    scope = PLATFORM_STRING_FROM_INTERNAL_STRING(rpsTicketService);
    policy = PLATFORM_STRING_FROM_INTERNAL_STRING(rpsTicketPolicy);
}

xbox::services::xbox_live_result<bool> title_callable_ui::check_gaming_privilege_silently(
    _In_ gaming_privilege privilege, 
    _In_opt_ Windows::System::User^ user
)
{
    Platform::String^ scope;
    Platform::String^ policy;
    _Get_gaming_privilege_scope_policy(scope, policy);

    BOOL hasPrivilege = FALSE;

    HRESULT hr = S_OK;
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

pplx::task<xbox::services::xbox_live_result<bool>> title_callable_ui::check_gaming_privilege_with_ui(
    _In_ gaming_privilege privilege,
    _In_ string_t friendlyMessage,
    _In_opt_ Windows::System::User^ user
)
{
    auto task = pplx::create_task([privilege, friendlyMessage, user]()
    {
        tcui_context context;

        Platform::String^ scope;
        Platform::String^ policy;
        _Get_gaming_privilege_scope_policy(scope, policy);
        Platform::String^ friendlyMessageStr = ref new Platform::String(friendlyMessage.c_str());

        BOOL hasPrivilege = FALSE;
        HRESULT hr = S_OK;
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

    return create_exception_free_task<bool>(
        task
        );
}

#if defined(RS3_TCUI)
pplx::task<xbox::services::xbox_live_result<void>> title_callable_ui::show_friend_finder_ui(
    _In_opt_ Windows::System::User^ user
)
{
    auto task = pplx::create_task([user]()
    {
        tcui_context context;

        HRESULT hr = S_OK;
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

pplx::task<xbox::services::xbox_live_result<void>> title_callable_ui::show_user_profile_ui(
    _In_ const string_t& targetXboxUserId
)
{
    // Show the profile card tcui
    return show_profile_card_ui(targetXboxUserId);
}

pplx::task<xbox::services::xbox_live_result<void>> title_callable_ui::show_title_hub_ui(
    _In_opt_ Windows::System::User^ user
)
{
    auto localConfig = xbox_system_factory::get_factory()->create_local_config();
    uint32_t titleId = localConfig->title_id();

    auto task = pplx::create_task([titleId, user]()
    {
        tcui_context context;

        HRESULT hr = S_OK;
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

pplx::task<xbox::services::xbox_live_result<void>> title_callable_ui::show_user_settings_ui(
    _In_opt_ Windows::System::User^ user
)
{
    auto task = pplx::create_task([user]()
    {
        tcui_context context;

        HRESULT hr = S_OK;
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

pplx::task<xbox::services::xbox_live_result<void>> title_callable_ui::show_customize_user_profile_ui(
    _In_opt_ Windows::System::User^ user
)
{
    auto task = pplx::create_task([user]()
    {
        tcui_context context;

        HRESULT hr = S_OK;
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
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END