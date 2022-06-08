// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

// Request and response format from http://xboxwiki/wiki/Profile

DEFINE_TEST_CLASS(ProfileTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(ProfileTests);

    struct UserProfile
    {
        UserProfile(uint64_t xuid_) noexcept
            : xuid{ xuid_ }
        {
            auto xuidStr = Utils::StringFromUint64(xuid);

            appDisplayName = "appDisplayName_" + xuidStr;
            gameDisplayName = "gameDisplayName_" + xuidStr;

            std::stringstream ss;
            ss << "http://www.xbox.com/appDisplayPictureRaw?url=test_" << xuid << "&format=png";
            appDisplayPictureResizeUri = ss.str();

            ss.clear();
            ss << "http://www.xbox.com/gameDisplayPictureRaw?url=test_" << xuid << "&format=png";
            gameDisplayPictureResizeUri = ss.str();

            gamerscore = "gamerscore_" + xuidStr;
            gamertag = "gamertag" + xuidStr;
            modernGamertag = "mgt" + xuidStr;
            modernGamertagSuffix = Utils::StringFromUint64(nextSuffix++);
            uniqueModernGamertag = modernGamertag + modernGamertagSuffix;

            assert(gamertag.length() < XBL_GAMERTAG_CHAR_SIZE);
            assert(modernGamertag.length() < XBL_MODERN_GAMERTAG_CHAR_SIZE);
        }

        static JsonDocument Serialize(const std::vector<UserProfile>& profiles) noexcept
        {
            JsonDocument d{ rapidjson::kObjectType };
            auto& a{ d.GetAllocator() };

            JsonValue profileUsersArray{ rapidjson::kArrayType };
            for (auto& profile : profiles)
            {
                profileUsersArray.PushBack(profile.Serialize(a).Move(), a);
            }
            d.AddMember("profileUsers", profileUsersArray.Move(), a);

            return d;
        }

        uint64_t xuid;
        std::string appDisplayName;
        std::string appDisplayPictureResizeUri;
        std::string gameDisplayName;
        std::string gameDisplayPictureResizeUri;
        std::string gamerscore;
        std::string gamertag;
        std::string modernGamertag;
        std::string modernGamertagSuffix;
        std::string uniqueModernGamertag;

    private:
        JsonValue Serialize(_In_ JsonDocument::AllocatorType& a) const noexcept
        {
            JsonValue settingsArray{ rapidjson::kArrayType };

            {
                JsonValue setting{ rapidjson::kObjectType };
                setting.AddMember("id", "AppDisplayName", a);
                setting.AddMember("value", JsonValue{ appDisplayName.data(), a }.Move(), a);
                settingsArray.PushBack(setting.Move(), a);
            }

            {
                JsonValue setting{ rapidjson::kObjectType };
                setting.AddMember("id", "AppDisplayPicRaw", a);
                setting.AddMember("value", JsonValue{ appDisplayPictureResizeUri.data(), a }.Move(), a);
                settingsArray.PushBack(setting.Move(), a);
            }

            {
                JsonValue setting{ rapidjson::kObjectType };
                setting.AddMember("id", "GameDisplayName", a);
                setting.AddMember("value", JsonValue{ gameDisplayName.data(), a }.Move(), a);
                settingsArray.PushBack(setting.Move(), a);
            }

            {
                JsonValue setting{ rapidjson::kObjectType };
                setting.AddMember("id", "GameDisplayPicRaw", a);
                setting.AddMember("value", JsonValue{ gameDisplayPictureResizeUri.data(), a }.Move(), a);
                settingsArray.PushBack(setting.Move(), a);
            }

            {
                JsonValue setting{ rapidjson::kObjectType };
                setting.AddMember("id", "Gamerscore", a);
                setting.AddMember("value", JsonValue{ gamerscore.data(), a }.Move(), a);
                settingsArray.PushBack(setting.Move(), a);
            }

            {
                JsonValue setting{ rapidjson::kObjectType };
                setting.AddMember("id", "Gamertag", a);
                setting.AddMember("value", JsonValue{ gamertag.data(), a }.Move(), a);
                settingsArray.PushBack(setting.Move(), a);
            }

            {
                JsonValue setting{ rapidjson::kObjectType };
                setting.AddMember("id", "ModernGamertag", a);
                setting.AddMember("value", JsonValue{ modernGamertag.data(), a }.Move(), a);
                settingsArray.PushBack(setting.Move(), a);
            }

            {
                JsonValue setting{ rapidjson::kObjectType };
                setting.AddMember("id", "ModernGamertagSuffix", a);
                setting.AddMember("value", JsonValue{ modernGamertagSuffix.data(), a }.Move(), a);
                settingsArray.PushBack(setting.Move(), a);
            }

            {
                JsonValue setting{ rapidjson::kObjectType };
                setting.AddMember("id", "UniqueModernGamertag", a);
                setting.AddMember("value", JsonValue{ uniqueModernGamertag.data(), a }.Move(), a);
                settingsArray.PushBack(setting.Move(), a);
            }

            JsonValue profile{ rapidjson::kObjectType };
            profile.AddMember("id", JsonValue{ Utils::StringFromUint64(xuid).data(), a }.Move(), a);
            profile.AddMember("settings", settingsArray, a);

            return profile;
        }

        static uint8_t nextSuffix;
    };

    void VerifyUserProfile(
        _In_ const XblUserProfile& actual,
        _In_ const UserProfile& expected
    )
    {
        VERIFY_ARE_EQUAL_INT(expected.xuid, actual.xboxUserId);
        VERIFY_ARE_EQUAL_STR(expected.appDisplayName, actual.appDisplayName);
        VERIFY_ARE_EQUAL_STR(expected.appDisplayPictureResizeUri, actual.appDisplayPictureResizeUri);
        VERIFY_ARE_EQUAL_STR(expected.gameDisplayName, actual.gameDisplayName);
        VERIFY_ARE_EQUAL_STR(expected.gameDisplayPictureResizeUri, actual.gameDisplayPictureResizeUri);
        VERIFY_ARE_EQUAL_STR(expected.gamerscore, actual.gamerscore);
        VERIFY_ARE_EQUAL_STR(expected.gamertag, actual.gamertag);
        VERIFY_ARE_EQUAL_STR(expected.modernGamertag, actual.modernGamertag);
        VERIFY_ARE_EQUAL_STR(expected.modernGamertagSuffix, actual.modernGamertagSuffix);
        VERIFY_ARE_EQUAL_STR(expected.uniqueModernGamertag, actual.uniqueModernGamertag);
    }

    // Validate that a batch request is well formed
    bool VerifyBatchRequest(
        _In_ const xsapi_internal_string& requestUri,
        _In_ const xsapi_internal_string& requestBody,
        _In_ const std::vector<uint64_t>& requestedXuids
    )
    {
        bool requestWellFormed{ true };

        requestWellFormed &= ("https://profile.xboxlive.com/users/batch/profile/settings" == requestUri);

        std::stringstream expectedBody;
        expectedBody << R"({"settings":["AppDisplayName","AppDisplayPicRaw","GameDisplayName","GameDisplayPicRaw","Gamerscore","Gamertag","ModernGamertag","ModernGamertagSuffix","UniqueModernGamertag"],"userIds":[)";
        for (size_t i = 0; i < requestedXuids.size(); ++i)
        {
            if (i > 0)
            {
                expectedBody << ",";
            }
            expectedBody << "\"" << requestedXuids[i] << "\"";
        }
        expectedBody << "]}";
        requestWellFormed &= VerifyJson(expectedBody.str().data(), requestBody.data());

        return requestWellFormed;
    }

    DEFINE_TEST_CASE(TestGetUserProfileAsync)
    {
        TEST_LOG(L"Test starting: TestGetUserProfileAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        UserProfile expectedProfile{ 1 };

        auto mock = std::make_shared<HttpMock>( "", "https://profile.xboxlive.com" );
        mock->SetResponseBody(UserProfile::Serialize({ expectedProfile }));

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock*, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                // XblProfileGetUserProfileResult just makes a batch request with 1 xuid
                requestWellFormed = VerifyBatchRequest(requestUrl, requestBody, std::vector<uint64_t>{ expectedProfile.xuid });
            }
        );

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblProfileGetUserProfileAsync(xboxLiveContext.get(), expectedProfile.xuid, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);

        XblUserProfile actualProfile{};
        VERIFY_SUCCEEDED(XblProfileGetUserProfileResult(&async, &actualProfile));
        VerifyUserProfile(actualProfile, expectedProfile);
    }

    DEFINE_TEST_CASE(TestGetUserProfilesAsync)
    {
        TEST_LOG(L"Test starting: TestGetUserProfilesAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        std::vector<UserProfile> expectedProfiles;
        std::vector<uint64_t> xuids{ 1, 2, 3, 4, 5 };
        for (auto& xuid : xuids)
        {
            expectedProfiles.push_back(UserProfile{ xuid });
        }

        auto mock = std::make_shared<HttpMock>( "", "https://profile.xboxlive.com" );
        mock->SetResponseBody(UserProfile::Serialize(expectedProfiles));

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock*, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                requestWellFormed = VerifyBatchRequest(requestUrl, requestBody, xuids);
            }
        );
        
        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblProfileGetUserProfilesAsync(xboxLiveContext.get(), xuids.data(), xuids.size(), &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);

        size_t count{ 0 };
        VERIFY_SUCCEEDED(XblProfileGetUserProfilesResultCount(&async, &count));
        VERIFY_ARE_EQUAL_UINT(xuids.size(), count);

        auto profiles{ new XblUserProfile[count] };
        VERIFY_SUCCEEDED(XblProfileGetUserProfilesResult(&async, count, profiles));

        for (size_t i = 0; i < count; ++i)
        {
            VerifyUserProfile(profiles[i], expectedProfiles[i]);
        }
    }

    DEFINE_TEST_CASE(TestGetUserProfilesForSocialGroupAsync)
    {
        TEST_LOG(L"Test starting: TestGetUserProfilesForSocialGroupAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        std::vector<UserProfile> expectedProfiles;
        std::vector<uint64_t> xuids{ 1, 2 };
        const char* socialGroup{ "People" };
        for (auto& xuid : xuids)
        {
            expectedProfiles.push_back(UserProfile{ xuid });
        }

        auto mock = std::make_shared<HttpMock>( "", "https://profile.xboxlive.com" );
        mock->SetResponseBody(UserProfile::Serialize(expectedProfiles));

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock*, xsapi_internal_string requestUri, xsapi_internal_string requestBody)
            {
                xsapi_internal_stringstream expectedUri;
                expectedUri << "https://profile.xboxlive.com/users/me/profile/settings/people/";
                expectedUri << socialGroup;
                expectedUri << "?settings=AppDisplayName,AppDisplayPicRaw,GameDisplayName,GameDisplayPicRaw,Gamerscore,Gamertag,ModernGamertag,ModernGamertagSuffix,UniqueModernGamertag";

                requestWellFormed &= (expectedUri.str() == requestUri);
                requestWellFormed &= requestBody.empty();
            }
        );

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblProfileGetUserProfilesForSocialGroupAsync(xboxLiveContext.get(), socialGroup, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);

        size_t count{ 0 };
        VERIFY_SUCCEEDED(XblProfileGetUserProfilesForSocialGroupResultCount(&async, &count));
        VERIFY_ARE_EQUAL_UINT(xuids.size(), count);

        auto profiles{ new XblUserProfile[count] };
        VERIFY_SUCCEEDED(XblProfileGetUserProfilesForSocialGroupResult(&async, count, profiles));

        for (size_t i = 0; i < count; ++i)
        {
            VerifyUserProfile(profiles[i], expectedProfiles[i]);
        }
    }

    DEFINE_TEST_CASE(TestProfileServiceInvalidArgs)
    {
        TEST_LOG(L"Test starting: TestProfileServiceInvalidArgs");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        XAsyncBlock async{};

        TEST_LOG(L"TestGetUserProfilesAsyncInvalidArgs: Null xboxUserIds param.");
        HRESULT hr = XblProfileGetUserProfilesAsync(
            xboxLiveContext.get(),
            nullptr, // invalid
            1,
            &async
        );
        VERIFY_ARE_EQUAL(E_INVALIDARG, hr);

        TEST_LOG(L"TestGetUserProfilesAsyncInvalidArgs: xboxUserIdsCount = 0.");
        uint64_t xuid{ 1 };
        hr = XblProfileGetUserProfilesAsync(
            xboxLiveContext.get(),
            &xuid,
            0, // invalid
            &async
        );
        VERIFY_ARE_EQUAL(E_INVALIDARG, hr);

        TEST_LOG(L"TestGetUserProfilesAsyncInvalidArgs: null socialGroup");
        hr = XblProfileGetUserProfilesForSocialGroupAsync(
            xboxLiveContext.get(),
            nullptr,
            &async
        );
        VERIFY_ARE_EQUAL(E_INVALIDARG, hr);
    }
};

uint8_t ProfileTests::UserProfile::nextSuffix{ 1 };

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
