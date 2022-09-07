// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "public_utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_BEGIN

permission_deny_reason::permission_deny_reason(
    const XblPermissionDenyReasonDetails& reasonDetails
)
    : m_reasonDetails(reasonDetails)
{
}

string_t permission_deny_reason::reason() const
{
    static const std::unordered_map<uint32_t, string_t> reasonsMap =
    {
        { static_cast<uint32_t>(XblPermissionDenyReason::Unknown), _T("Unknown") },
        { static_cast<uint32_t>(XblPermissionDenyReason::NotAllowed), _T("NotAllowed") },
        { static_cast<uint32_t>(XblPermissionDenyReason::MissingPrivilege), _T("MissingPrivilege") },
        { static_cast<uint32_t>(XblPermissionDenyReason::PrivilegeRestrictsTarget), _T("PrivilegeRestrictsTarget") },
        { static_cast<uint32_t>(XblPermissionDenyReason::BlockListRestrictsTarget), _T("BlockListRestrictsTarget") },
        { static_cast<uint32_t>(XblPermissionDenyReason::MuteListRestrictsTarget), _T("MuteListRestrictsTarget") },
        { static_cast<uint32_t>(XblPermissionDenyReason::PrivacySettingRestrictsTarget), _T("PrivacySettingRestrictsTarget") }
    };
    return reasonsMap.at(static_cast<uint32_t>(m_reasonDetails.reason));
}

string_t permission_deny_reason::restricted_setting() const
{
    static const std::unordered_map<uint32_t, string_t> settingsMap =
    {
        // Privacy settings
        { static_cast<uint32_t>(XblPrivacySetting::ShareFriendList), _T("ShareFriendList") },
        { static_cast<uint32_t>(XblPrivacySetting::ShareGameHistory), _T("ShareGameHistory") },
        { static_cast<uint32_t>(XblPrivacySetting::CommunicateUsingTextAndVoice), _T("CommunicateUsingTextAndVoice") },
        { static_cast<uint32_t>(XblPrivacySetting::SharePresence), _T("SharePresence") },
        { static_cast<uint32_t>(XblPrivacySetting::ShareProfile), _T("ShareProfile") },
        { static_cast<uint32_t>(XblPrivacySetting::ShareVideoAndMusicStatus), _T("ShareVideoAndMusicStatus") },
        { static_cast<uint32_t>(XblPrivacySetting::CommunicateUsingVideo), _T("CommunicateUsingVideo") },
        { static_cast<uint32_t>(XblPrivacySetting::CollectVoiceData), _T("CollectVoiceData") },
        { static_cast<uint32_t>(XblPrivacySetting::ShareXboxMusicActivity), _T("ShareXboxMusicActivity") },
        { static_cast<uint32_t>(XblPrivacySetting::ShareExerciseInfo), _T("ShareExerciseInfo") },
        { static_cast<uint32_t>(XblPrivacySetting::ShareIdentity), _T("ShareIdentity") },
        { static_cast<uint32_t>(XblPrivacySetting::ShareIdentityInGame), _T("ShareIdentityInGame") },
        { static_cast<uint32_t>(XblPrivacySetting::ShareRecordedGameSessions), _T("ShareRecordedGameSessions") },
        { static_cast<uint32_t>(XblPrivacySetting::CollectLiveTvData), _T("CollectLiveTvData") },
        { static_cast<uint32_t>(XblPrivacySetting::CollectXboxVideoData), _T("CollectXboxVideoData") },
        { static_cast<uint32_t>(XblPrivacySetting::ShareIdentityTransitively), _T("ShareIdentityTransitively") },
        { static_cast<uint32_t>(XblPrivacySetting::ShareVideoHistory), _T("ShareVideoHistory") },
        { static_cast<uint32_t>(XblPrivacySetting::ShareMusicHistory), _T("ShareMusicHistory") },
        { static_cast<uint32_t>(XblPrivacySetting::AllowUserCreatedContentViewing), _T("AllowUserCreatedContentViewing") },
        { static_cast<uint32_t>(XblPrivacySetting::AllowProfileViewing), _T("AllowProfileViewing") },
        { static_cast<uint32_t>(XblPrivacySetting::ShowRealTimeActivity), _T("ShowRealTimeActivity") },
        { static_cast<uint32_t>(XblPrivacySetting::CollectVoiceDataXboxOneFull), _T("CollectVoiceDataXboxOneFull") },
        { static_cast<uint32_t>(XblPrivacySetting::CanShareIdentity), _T("CanShareIdentity") },
        { static_cast<uint32_t>(XblPrivacySetting::ShareContentToExternalNetworks), _T("ShareContentToExternalNetworks") },
        { static_cast<uint32_t>(XblPrivacySetting::CollectVoiceSearchData), _T("CollectVoiceSearchData") },
        { static_cast<uint32_t>(XblPrivacySetting::ShareClubMembership), _T("ShareClubMembership") },
        { static_cast<uint32_t>(XblPrivacySetting::CollectVoiceGameChatData), _T("CollectVoiceGameChatData") },
        { static_cast<uint32_t>(XblPrivacySetting::ShareActivityFeed), _T("ShareActivityFeed") },
        { static_cast<uint32_t>(XblPrivacySetting::CommunicateDuringCrossNetworkPlay), _T("CommunicateDuringCrossNetworkPlay") },
    };

    auto iter = settingsMap.find(static_cast<uint32_t>(m_reasonDetails.restrictedPrivacySetting));
    if (iter != settingsMap.end())
    {
        return iter->second;
    }

    static const std::unordered_map<uint32_t, string_t> privMap =
    {
        // Permissions
        { static_cast<uint32_t>(XblPrivilege::AllowIngameVoiceCommunications), _T("AllowIngameVoiceCommunications") },
        { static_cast<uint32_t>(XblPrivilege::AllowVideoCommunications), _T("PrivilegeVideoCommunications") },
        { static_cast<uint32_t>(XblPrivilege::AllowProfileViewing), _T("AllowProfileViewing") },
        { static_cast<uint32_t>(XblPrivilege::AllowCommunications), _T("AllowCommunications") },
        { static_cast<uint32_t>(XblPrivilege::AllowMultiplayer), _T("AllowMultiplayer") },
        { static_cast<uint32_t>(XblPrivilege::AllowAddFriend), _T("AllowAddFriend") }
    };

    auto iter2 = privMap.find(static_cast<uint32_t>(m_reasonDetails.restrictedPrivacySetting));
    if (iter2 != privMap.end())
    {
        return iter2->second;
    }

    return string_t();
}

permission_check_result::permission_check_result(
    const XblPermissionCheckResult* result
)
    : m_result(*result)
{
    for (auto i = 0u; i < m_result.reasonsCount; ++i)
    {
        m_reasons.push_back(permission_deny_reason(m_result.reasons[i]));
    }
}

bool permission_check_result::is_allowed() const
{
    return m_result.isAllowed;
}

string_t permission_check_result::permission_requested() const
{
    static const std::unordered_map<uint32_t, string_t> permissionsMap =
    {
        { static_cast<uint32_t>(XblPermission::CommunicateUsingText), permission_id_constants::communicate_using_text() },
        { static_cast<uint32_t>(XblPermission::CommunicateUsingVideo), permission_id_constants::communicate_using_video() },
        { static_cast<uint32_t>(XblPermission::CommunicateUsingVoice), permission_id_constants::communicate_using_voice() },
        { static_cast<uint32_t>(XblPermission::ViewTargetProfile), permission_id_constants::view_target_profile() },
        { static_cast<uint32_t>(XblPermission::ViewTargetGameHistory), permission_id_constants::view_target_game_history() },
        { static_cast<uint32_t>(XblPermission::ViewTargetVideoHistory), permission_id_constants::view_target_video_history() },
        { static_cast<uint32_t>(XblPermission::ViewTargetMusicHistory), permission_id_constants::view_target_music_history() },
        { static_cast<uint32_t>(XblPermission::ViewTargetExerciseInfo), permission_id_constants::view_target_exercise_info() },
        { static_cast<uint32_t>(XblPermission::ViewTargetPresence), permission_id_constants::view_target_presence() },
        { static_cast<uint32_t>(XblPermission::ViewTargetVideoStatus), permission_id_constants::view_target_video_status() },
        { static_cast<uint32_t>(XblPermission::ViewTargetMusicStatus), permission_id_constants::view_target_music_status() },
        { static_cast<uint32_t>(XblPermission::PlayMultiplayer), permission_id_constants::play_multiplayer() },
        { static_cast<uint32_t>(XblPermission::ViewTargetUserCreatedContent), permission_id_constants::view_target_user_created_content() },
        { static_cast<uint32_t>(XblPermission::BroadcastWithTwitch), permission_id_constants::broadcast_with_twitch() }
    };
    return permissionsMap.at(static_cast<uint32_t>(m_result.permissionRequested));
}

const std::vector<permission_deny_reason>& permission_check_result::deny_reasons() const
{
    return m_reasons;
}

inline XblAnonymousUserType AnonymousUserTypeFromString(
    const string_t& anonymousUserTypeString
)
{
    static const std::unordered_map<string_t, XblAnonymousUserType> anonymousUserTypes
    {
        { anonymous_user_type_constants::cross_network_user(), XblAnonymousUserType::CrossNetworkUser },
        { anonymous_user_type_constants::crost_network_friend(), XblAnonymousUserType::CrossNetworkFriend }
    };

    auto iter = anonymousUserTypes.find(anonymousUserTypeString);
    if (iter != anonymousUserTypes.end())
    {
        return iter->second;
    }
    return XblAnonymousUserType::Unknown;
}

multiple_permissions_check_result::multiple_permissions_check_result(
    const XblPermissionCheckResult* results,
    size_t resultCount,
    string_t target
) :
    m_target{ std::move(target) }
{
    for (auto i = 0u; i < resultCount; ++i)
    {
        XblAnonymousUserType anonymousUserType{ AnonymousUserTypeFromString(m_target) };

        if ((anonymousUserType != XblAnonymousUserType::Unknown && results[i].targetUserType == anonymousUserType) ||
            results[i].targetXuid == Utils::Uint64FromStringT(m_target))
        {
            m_items.push_back(permission_check_result(results + i));
        }
    }
}

const string_t& multiple_permissions_check_result::xbox_user_id() const
{
    return m_target;
}

const std::vector<permission_check_result>& multiple_permissions_check_result::items() const
{
    return m_items;
}

privacy_service::privacy_service(_In_ XblContextHandle contextHandle)
{
    XblContextDuplicateHandle(contextHandle, &m_xblContext);
}

privacy_service::privacy_service(const privacy_service& other)
{
    XblContextDuplicateHandle(other.m_xblContext, &m_xblContext);
}

privacy_service& privacy_service::operator=(privacy_service other)
{
    std::swap(m_xblContext, other.m_xblContext);
    return *this;
}

privacy_service::~privacy_service()
{
    XblContextCloseHandle(m_xblContext);
}

pplx::task<xbox_live_result<std::vector<string_t>>> privacy_service::get_avoid_list()
{
    auto asyncWrapper = new AsyncWrapper<std::vector<string_t>>(
        [](XAsyncBlock* async, std::vector<string_t>& result)
    {
        size_t xuidCount;
        auto hr = XblPrivacyGetAvoidListResultCount(async, &xuidCount);
        if (SUCCEEDED(hr))
        {
            std::vector<uint64_t> xuids(xuidCount);
            hr = XblPrivacyGetAvoidListResult(async, xuidCount, xuids.data());
            for (auto& xuid : xuids)
            {
                result.push_back(Utils::StringTFromUint64(xuid));
            }
        }
        return hr;
    });

    auto hr = XblPrivacyGetAvoidListAsync(m_xblContext, &asyncWrapper->async);
    return asyncWrapper->Task(hr);
}

inline XblPermission XblPermissionFromString(const string_t& permission)
{
    static const std::unordered_map<string_t, uint32_t> map =
    {
        { permission_id_constants::communicate_using_text(), static_cast<uint32_t>(XblPermission::CommunicateUsingText) },
        { permission_id_constants::communicate_using_video(), static_cast<uint32_t>(XblPermission::CommunicateUsingVideo) },
        { permission_id_constants::communicate_using_voice(), static_cast<uint32_t>(XblPermission::CommunicateUsingVoice) },
        { permission_id_constants::view_target_profile(), static_cast<uint32_t>(XblPermission::ViewTargetProfile) },
        { permission_id_constants::view_target_game_history(), static_cast<uint32_t>(XblPermission::ViewTargetGameHistory) },
        { permission_id_constants::view_target_video_history(), static_cast<uint32_t>(XblPermission::ViewTargetVideoHistory) },
        { permission_id_constants::view_target_music_history(), static_cast<uint32_t>(XblPermission::ViewTargetMusicHistory) },
        { permission_id_constants::view_target_exercise_info(), static_cast<uint32_t>(XblPermission::ViewTargetExerciseInfo) },
        { permission_id_constants::view_target_presence(), static_cast<uint32_t>(XblPermission::ViewTargetPresence) },
        { permission_id_constants::view_target_video_status(), static_cast<uint32_t>(XblPermission::ViewTargetVideoStatus) },
        { permission_id_constants::view_target_music_status(), static_cast<uint32_t>(XblPermission::ViewTargetMusicStatus) },
        { permission_id_constants::play_multiplayer(), static_cast<uint32_t>(XblPermission::PlayMultiplayer) },
        { permission_id_constants::view_target_user_created_content(), static_cast<uint32_t>(XblPermission::ViewTargetUserCreatedContent) },
        { permission_id_constants::broadcast_with_twitch(), static_cast<uint32_t>(XblPermission::BroadcastWithTwitch) }
    };

    auto iter = map.find(permission);
    if (iter == map.end())
    {
        return XblPermission::Unknown;
    }
    return static_cast<XblPermission>(iter->second);
}

pplx::task<xbox_live_result<permission_check_result>> privacy_service::check_permission_with_target_user(
    _In_ const string_t& permissionId,
    _In_ const string_t& target
)
{
    auto permission = XblPermissionFromString(permissionId);
    if (permission == XblPermission::Unknown || target.empty())
    {
        return pplx::task_from_result(xbox_live_result<permission_check_result>(std::make_error_code(xbox_live_error_code::invalid_argument)));
    }

    auto asyncWrapper = new AsyncWrapper<permission_check_result>(
        [](XAsyncBlock* async, permission_check_result& result)
    {
        size_t bufferSize;
        auto hr = XblPrivacyCheckPermissionResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            auto buffer = new char[bufferSize];
            XblPermissionCheckResult* resultPtr;
            hr = XblPrivacyCheckPermissionResult(async, bufferSize, buffer, &resultPtr, nullptr);

            result = permission_check_result(resultPtr);
            delete[] buffer;
        }
        return hr;
    });

    HRESULT hr{ S_OK };

    XblAnonymousUserType anonymousUserType{ AnonymousUserTypeFromString(target) };
    if (anonymousUserType != XblAnonymousUserType::Unknown)
    {
        hr = XblPrivacyCheckPermissionForAnonymousUserAsync(
            m_xblContext,
            permission,
            anonymousUserType,
            &asyncWrapper->async
        );
    }
    else
    {
        hr = XblPrivacyCheckPermissionAsync(
            m_xblContext,
            permission,
            Utils::Uint64FromStringT(target),
            &asyncWrapper->async
        );
    }
    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::vector<multiple_permissions_check_result>>> privacy_service::check_multiple_permissions_with_multiple_target_users(
    _In_ const std::vector<string_t>& permissionIds,
    _In_ const std::vector<string_t>& targets
)
{
    std::vector<uint64_t> xuids{};
    std::vector<XblAnonymousUserType> userTypes{};

    for (auto& target : targets)
    {
        XblAnonymousUserType anonymousUserType{ AnonymousUserTypeFromString(target) };
        if (anonymousUserType == XblAnonymousUserType::Unknown)
        {
            xuids.push_back(Utils::Uint64FromStringT(target));
        }
        else
        {
            userTypes.push_back(anonymousUserType);
        }
    }

    auto asyncWrapper = new AsyncWrapper<std::vector<multiple_permissions_check_result>>(
        [targets](XAsyncBlock* async, std::vector<multiple_permissions_check_result>& results)
    {
        size_t bufferSize;
        auto hr = XblPrivacyBatchCheckPermissionResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            auto buffer = new char[bufferSize];
            XblPermissionCheckResult* resultPtr;
            size_t resultCount;
            hr = XblPrivacyBatchCheckPermissionResult(async, bufferSize, buffer, &resultPtr, &resultCount, nullptr);

            if (SUCCEEDED(hr))
            {
                for (auto& target : targets)
                {
                    results.push_back(multiple_permissions_check_result(resultPtr, resultCount, target));
                }
            }
            delete[] buffer;
        }
        return hr;
    });

    auto hr = XblPrivacyBatchCheckPermissionAsync(
        m_xblContext,
        Utils::Transform<XblPermission>(permissionIds, XblPermissionFromString).data(),
        permissionIds.size(),
        xuids.data(),
        xuids.size(),
        userTypes.data(),
        userTypes.size(),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::vector<string_t>>> privacy_service::get_mute_list()
{
    auto asyncWrapper = new AsyncWrapper<std::vector<string_t>>(
        [](XAsyncBlock* async, std::vector<string_t>& result)
    {
        size_t xuidCount;
        auto hr = XblPrivacyGetMuteListResultCount(async, &xuidCount);
        if (SUCCEEDED(hr))
        {
            std::vector<uint64_t> xuids(xuidCount);
            hr = XblPrivacyGetMuteListResult(async, xuidCount, xuids.data());
            for (auto& xuid : xuids)
            {
                result.push_back(Utils::StringTFromUint64(xuid));
            }
        }
        return hr;
    });

    auto hr = XblPrivacyGetMuteListAsync(m_xblContext, &asyncWrapper->async);
    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::vector<string_t>>> privacy_service::get_avoid_or_mute_list(
    _In_ const string_t& subPathName
)
{
    if (Utils::Stricmp(subPathName, _T("mute")) == 0)
    {
        return get_mute_list();
    }
    else if (Utils::Stricmp(subPathName, _T("avoid")) == 0)
    {
        return get_avoid_list();
    }
    return pplx::task_from_result(xbox_live_result<std::vector<string_t>>(std::make_error_code(xbox_live_error_code::invalid_argument)));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_END
