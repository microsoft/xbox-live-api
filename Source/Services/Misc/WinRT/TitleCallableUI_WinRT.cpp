// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#if !UNIT_TEST_SERVICES
#include "xsapi/title_callable_ui.h"
#include "xsapi/multiplayer.h"
#include "TitleCallableUI_WinRT.h"
#include "MultiplayerSessionReference_WinRT.h"

using namespace pplx;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace Platform::Collections;
using namespace xbox::services::system;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Platform::String^>^>^
TitleCallableUI::ShowPlayerPickerUI(
    _In_ Platform::String^ promptDisplayText,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ preselectedXboxUserIds,
    _In_ uint32_t minSelectionCount,
    _In_ uint32_t maxSelectionCount
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(xboxUserIds);
    THROW_INVALIDARGUMENT_IF_NULL(preselectedXboxUserIds);

    auto task = title_callable_ui::show_player_picker_ui(
        STRING_T_FROM_PLATFORM_STRING(promptDisplayText),
        UtilsWinRT::CovertVectorViewToStdVectorString(xboxUserIds),
        UtilsWinRT::CovertVectorViewToStdVectorString(preselectedXboxUserIds),
        minSelectionCount,
        maxSelectionCount
        )
    .then([](xbox_live_result<std::vector<string_t>> result)
    {
        THROW_IF_ERR(result);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorString(result.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncAction^ 
TitleCallableUI::ShowGameInviteUIAsync(
    _In_ Xbox::Services::Multiplayer::MultiplayerSessionReference^ sessionReference,
    _In_ Platform::String^ contextStringId
    )
{  
    THROW_INVALIDARGUMENT_IF_NULL(sessionReference);

    auto task = title_callable_ui::show_game_invite_ui(
        sessionReference->GetCppObj(),
        STRING_T_FROM_PLATFORM_STRING(contextStringId)
        )
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncAction^ 
TitleCallableUI::ShowProfileCardUIAsync(
    _In_ Platform::String^ targetXboxUserId
    )
{
    auto task = title_callable_ui::show_profile_card_ui(
        STRING_T_FROM_PLATFORM_STRING(targetXboxUserId)
        )
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncAction^ 
TitleCallableUI::ShowChangeFriendRelationshipUIAsync(
    _In_ Platform::String^ targetXboxUserId
    )
{
    auto task = title_callable_ui::show_change_friend_relationship_ui(
        STRING_T_FROM_PLATFORM_STRING(targetXboxUserId)
        )
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncAction^ 
TitleCallableUI::ShowTitleAchievementsUIAsync(
    _In_ uint32_t titleId
    )
{
    auto task = title_callable_ui::show_title_achievements_ui(
        titleId
        )
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

bool
TitleCallableUI::CheckGamingPrivilegeSilently(
    _In_ GamingPrivilege privilege
    )
{
    auto result = title_callable_ui::check_gaming_privilege_silently(
        static_cast<gaming_privilege>(privilege)
        );

    THROW_ON_ERR_CODE(result.err());
    return result.payload();
}

Windows::Foundation::IAsyncOperation<bool>^
TitleCallableUI::CheckGamingPrivilegeWithUI(
    _In_ GamingPrivilege privilege,
    _In_opt_ Platform::String^ friendlyMessage
    )
{
    auto task = title_callable_ui::check_gaming_privilege_with_ui(
        static_cast<gaming_privilege>(privilege),
        STRING_T_FROM_PLATFORM_STRING(friendlyMessage)
        )
    .then([](xbox_live_result<bool> result)
    {
        THROW_IF_ERR(result);
        return result.payload();
    });

    return ASYNC_FROM_TASK(task);
}


#if UWP_API
Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Platform::String^>^>^
TitleCallableUI::ShowPlayerPickerUIForUser(
    _In_ Platform::String^ promptDisplayText,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ preselectedXboxUserIds,
    _In_ uint32_t minSelectionCount,
    _In_ uint32_t maxSelectionCount,
    _In_ Windows::System::User^ user
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(xboxUserIds);
    THROW_INVALIDARGUMENT_IF_NULL(preselectedXboxUserIds);

    auto task = title_callable_ui::show_player_picker_ui(
        STRING_T_FROM_PLATFORM_STRING(promptDisplayText),
        UtilsWinRT::CovertVectorViewToStdVectorString(xboxUserIds),
        UtilsWinRT::CovertVectorViewToStdVectorString(preselectedXboxUserIds),
        minSelectionCount,
        maxSelectionCount,
        user
        )
    .then([](xbox_live_result<std::vector<string_t>> result)
    {
        THROW_IF_ERR(result);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorString(result.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncAction^ 
TitleCallableUI::ShowGameInviteUIForUserAsync(
    _In_ Xbox::Services::Multiplayer::MultiplayerSessionReference^ sessionReference,
    _In_ Platform::String^ invitationDisplayText,
    _In_ Platform::String^ contextStringId,
    _In_ Windows::System::User^ user
    )
{  
    THROW_INVALIDARGUMENT_IF_NULL(sessionReference);

    auto task = title_callable_ui::show_game_invite_ui(
        sessionReference->GetCppObj(),
        STRING_T_FROM_PLATFORM_STRING(invitationDisplayText),
        STRING_T_FROM_PLATFORM_STRING(contextStringId),
        user
        )
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncAction^
TitleCallableUI::ShowGameInviteUIForUserAsync(
    _In_ Xbox::Services::Multiplayer::MultiplayerSessionReference^ sessionReference,
    _In_ Platform::String^ contextStringId,
    _In_ Windows::System::User^ user
)
{
    THROW_INVALIDARGUMENT_IF_NULL(sessionReference);

    auto task = title_callable_ui::show_game_invite_ui(
        sessionReference->GetCppObj(),
        string_t(),
        STRING_T_FROM_PLATFORM_STRING(contextStringId),
        user
    )
        .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncAction^
TitleCallableUI::ShowProfileCardUIForUserAsync(
    _In_ Platform::String^ targetXboxUserId,
    _In_ Windows::System::User^ user
    )
{
    auto task = title_callable_ui::show_profile_card_ui(
        STRING_T_FROM_PLATFORM_STRING(targetXboxUserId),
        user
        )
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncAction^ 
TitleCallableUI::ShowChangeFriendRelationshipUIForUserAsync(
    _In_ Platform::String^ targetXboxUserId,
    _In_ Windows::System::User^ user
    )
{
    auto task = title_callable_ui::show_change_friend_relationship_ui(
        STRING_T_FROM_PLATFORM_STRING(targetXboxUserId),
        user
        )
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncAction^ 
TitleCallableUI::ShowTitleAchievementsUIForUserAsync(
    _In_ uint32_t titleId,
    _In_ Windows::System::User^ user
    )
{
    auto task = title_callable_ui::show_title_achievements_ui(
        titleId,
        user
        )
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

bool
TitleCallableUI::CheckGamingPrivilegeSilentlyForUser(
    _In_ GamingPrivilege privilege,
    _In_ Windows::System::User^ user
    )
{
    auto result = title_callable_ui::check_gaming_privilege_silently(
        static_cast<gaming_privilege>(privilege),
        user
        );

    THROW_ON_ERR_CODE(result.err());
    return result.payload();
}

Windows::Foundation::IAsyncOperation<bool>^
TitleCallableUI::CheckGamingPrivilegeWithUIForUser(
    _In_ GamingPrivilege privilege,
    _In_opt_ Platform::String^ friendlyMessage,
    _In_ Windows::System::User^ user
    )
{
    auto task = title_callable_ui::check_gaming_privilege_with_ui(
        static_cast<gaming_privilege>(privilege),
        STRING_T_FROM_PLATFORM_STRING(friendlyMessage),
        user
        )
    .then([](xbox_live_result<bool> result)
    {
        THROW_IF_ERR(result);
        return result.payload();
    });

    return ASYNC_FROM_TASK(task);
}

#if defined(_APISET_TARGET_VERSION_WIN10_RS3)
Windows::Foundation::IAsyncAction^
TitleCallableUI::ShowFriendFinderForUser(
    _In_ Windows::System::User^ user
    )
{
    auto task = title_callable_ui::show_friend_finder_ui(
        user
        )
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncAction^
TitleCallableUI::ShowTitleHubForUser(
    _In_ Windows::System::User^ user
    )
{
    auto task = title_callable_ui::show_title_hub_ui(
        user
    )
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncAction^
TitleCallableUI::ShowUserSettingsForUser(
    _In_ Windows::System::User^ user
    )
{
    auto task = title_callable_ui::show_user_settings_ui(
        user
    )
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncAction^
TitleCallableUI::ShowCustomizeUserProfileForUser(
    _In_opt_ Windows::System::User^ user
    )
{
    auto task = title_callable_ui::show_customize_user_profile_ui(
        user
    )
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}
#endif
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
#endif