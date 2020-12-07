// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "profile_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

using namespace xbox::services;

static const char* s_settings[] = {
    "AppDisplayName",
    "AppDisplayPicRaw",
    "GameDisplayName",
    "GameDisplayPicRaw",
    "Gamerscore",
    "Gamertag",
    "ModernGamertag",
    "ModernGamertagSuffix",
    "UniqueModernGamertag"
};

ProfileService::ProfileService(
    _In_ User&& user,
    _In_ std::shared_ptr<XboxLiveContextSettings> xboxLiveContextSettings,
    _In_ std::shared_ptr<AppConfig> appConfig
) noexcept :
    m_user{ std::move(user) },
    m_xboxLiveContextSettings{ std::move(xboxLiveContextSettings) },
    m_appConfig{ std::move(appConfig) }
{
}

HRESULT ProfileService::GetUserProfiles(
    _In_ xsapi_internal_vector<uint64_t>&& xuids,
    _In_ AsyncContext<Result<xsapi_internal_vector<XblUserProfile>>> async
) const noexcept
{
    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "POST",
        XblHttpCall::BuildUrl("profile", "/users/batch/profile/settings"),
        xbox_live_api::get_user_profiles
    ));

    httpCall->SetHeader(CONTRACT_VERSION_HEADER, "2");

    JsonDocument request(rapidjson::kObjectType);
    JsonValue userIdsJson(rapidjson::kArrayType);
    JsonUtils::SerializeVector<uint64_t>(JsonUtils::JsonXuidSerializer, xuids, userIdsJson, request.GetAllocator());
    request.AddMember("userIds", userIdsJson, request.GetAllocator());

    JsonValue settingsArray(rapidjson::kArrayType);
    for (size_t i = 0; i < ARRAYSIZE(s_settings); ++i)
    {
        settingsArray.PushBack(JsonValue(s_settings[i], request.GetAllocator()).Move(), request.GetAllocator());
    }
    request.AddMember("settings", settingsArray, request.GetAllocator());

    httpCall->SetRequestBody(request);

    return httpCall->Perform({
        async.Queue(),
        [
            async
        ]
    (HttpResult result)
    {
        HRESULT hr{ Failed(result) ? result.Hresult() : result.Payload()->Result() };
        if (FAILED(hr))
        {
            return async.Complete(hr);
        }
        return async.Complete(DeserializeUserProfiles(result.Payload()->GetResponseBodyJson()));
    } });
}

HRESULT ProfileService::GetUserProfilesForSocialGroup(
    _In_ xsapi_internal_string&& socialGroup,
    _In_ AsyncContext<Result<xsapi_internal_vector<XblUserProfile>>> async
) const noexcept
{
    xsapi_internal_stringstream pathAndQuery;
    pathAndQuery << "/users/me/profile/settings/people/";
    pathAndQuery << socialGroup;
    pathAndQuery << "?settings=";

    for (uint32_t i = 0; i < ARRAYSIZE(s_settings); ++i)
    {
        if (i > 0)
        {
            pathAndQuery << ",";
        }
        pathAndQuery << s_settings[i];
    }

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    RETURN_HR_IF_FAILED(httpCall->Init(
        m_xboxLiveContextSettings,
        "GET",
        XblHttpCall::BuildUrl("profile", pathAndQuery.str()),
        xbox_live_api::get_user_profiles_for_social_group
    ));

    httpCall->SetHeader(CONTRACT_VERSION_HEADER, "2");

    return httpCall->Perform({
        async.Queue(),
        [
            async
        ]
    (HttpResult result)
    {
        HRESULT hr{ Failed(result) ? result.Hresult() : result.Payload()->Result() };
        if (FAILED(hr))
        {
            return async.Complete(hr);
        }
        return async.Complete(DeserializeUserProfiles(result.Payload()->GetResponseBodyJson()));
    } });
}

Result<xsapi_internal_vector<XblUserProfile>> ProfileService::DeserializeUserProfiles(
    _In_ const JsonValue& json
) noexcept
{
    if (json.IsNull())
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    HRESULT errc = S_OK;
    xsapi_internal_vector<XblUserProfile> profilesVector;
    RETURN_HR_IF_FAILED( JsonUtils::ExtractJsonVector<XblUserProfile>(
        DeserializeUserProfile,
        json,
        "profileUsers",
        profilesVector,
        true
        ));

    return Result<xsapi_internal_vector<XblUserProfile>>{ profilesVector, errc };
}

Result<XblUserProfile> ProfileService::DeserializeUserProfile(
    _In_ const JsonValue& json
) noexcept
{
    if (json.IsNull())
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    XblUserProfile profile{};

    HRESULT errc = S_OK;

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonXuid(json, "id", profile.xboxUserId, true));

    if (json.IsObject() && json.HasMember("settings"))
    {
        const JsonValue& settingsJson = json["settings"];
        if(settingsJson.IsArray())
        for (const auto& setting : settingsJson.GetArray())
        {
            xsapi_internal_string id;
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(setting, "id", id, true));
            xsapi_internal_string value;
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(setting, "value", value, true));

            if (id == "AppDisplayName")
            {
                utils::strcpy(profile.appDisplayName, sizeof(profile.appDisplayName), value.data());
            }
            else if (id == "AppDisplayPicRaw")
            {
                utils::strcpy(profile.appDisplayPictureResizeUri, sizeof(profile.appDisplayPictureResizeUri), value.data());
            }
            else if (id == "GameDisplayName")
            {
                utils::strcpy(profile.gameDisplayName, sizeof(profile.gameDisplayName), value.data());
            }
            else if (id == "GameDisplayPicRaw")
            {
                utils::strcpy(profile.gameDisplayPictureResizeUri, sizeof(profile.gameDisplayPictureResizeUri), value.data());
            }
            else if (id == "Gamerscore")
            {
                utils::strcpy(profile.gamerscore, sizeof(profile.gamerscore), value.data());
            }
            else if (id == "Gamertag")
            {
                utils::strcpy(profile.gamertag, sizeof(profile.gamertag), value.data());
            }
            else if (id == "ModernGamertag")
            {
                utils::strcpy(profile.modernGamertag, sizeof(profile.modernGamertag), value.data());
            }
            else if (id == "ModernGamertagSuffix")
            {
                utils::strcpy(profile.modernGamertagSuffix, sizeof(profile.modernGamertagSuffix), value.data());
            }
            else if (id == "UniqueModernGamertag")
            {
                utils::strcpy(profile.uniqueModernGamertag, sizeof(profile.uniqueModernGamertag), value.data());
            }
        }
    }
    else
    {
        //required
        return WEB_E_INVALID_JSON_STRING;
    }

    return Result<XblUserProfile>{ std::move(profile), errc };
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END