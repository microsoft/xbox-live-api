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
#define TEST_CLASS_OWNER L"jasonsa"
#define TEST_CLASS_AREA L"Profile"
#include "UnitTestIncludes.h"

#include "SocialGroupConstants_WinRT.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Social;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(ProfileTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(ProfileTests);

    struct XboxUserProfileTestValues
    {
        Platform::String^ appDisplayName;
        Platform::String^ appDisplayPictureResize;
        Platform::String^ gameDisplayName;
        Platform::String^ gameDisplayPictureResize;
        Platform::String^ gamerscore;
        Platform::String^ gamertag;
        Platform::String^ xboxUserId;
    };

    XboxUserProfileTestValues
    CreateXboxUserProfileTestValues(
        _In_ uint64 seed
        )
    {
        XboxUserProfileTestValues x;
        x.appDisplayName = PLATFORM_STRING_FROM_STRING_T(FormatString(L"appDisplayName_%s", seed.ToString()->Data()));
        x.appDisplayPictureResize = PLATFORM_STRING_FROM_STRING_T(FormatString(L"http://www.xbox.com/appDisplayPictureRaw?url=test_%s&format=png", seed.ToString()->Data()));
        x.gameDisplayName = PLATFORM_STRING_FROM_STRING_T(FormatString(L"gameDisplayName_%s", seed.ToString()->Data()));
        x.gameDisplayPictureResize = PLATFORM_STRING_FROM_STRING_T(FormatString(L"http://www.xbox.com/gameDisplayPictureRaw?url=test_%s&format=png", seed.ToString()->Data()));
        x.gamerscore = PLATFORM_STRING_FROM_STRING_T(FormatString(L"gamerscore_%s", seed.ToString()->Data()));
        x.gamertag = PLATFORM_STRING_FROM_STRING_T(FormatString(L"gamertag_%s", seed.ToString()->Data()));
        x.xboxUserId = PLATFORM_STRING_FROM_STRING_T(FormatString(L"xboxUserId_%s", seed.ToString()->Data()));
        return x;
    }

    web::json::value
    BuildXboxUserProfileJson(
        _In_ XboxUserProfileTestValues* val
        )
    {
        size_t arraySize = 0;
        auto jsonSettingsArray = web::json::value::array();

        {
            auto jsonSettingObject = web::json::value::object();
            jsonSettingObject[L"id"] = web::json::value::string(L"AppDisplayName");
            jsonSettingObject[L"value"] = web::json::value::string(val->appDisplayName->Data());
            jsonSettingsArray[arraySize++] = jsonSettingObject;
        }

        {
            auto jsonSettingObject = web::json::value::object();
            jsonSettingObject[L"id"] = web::json::value::string(L"AppDisplayPicRaw");
            jsonSettingObject[L"value"] = web::json::value::string(val->appDisplayPictureResize->Data());
            jsonSettingsArray[arraySize++] = jsonSettingObject;
        }

        {
            auto jsonSettingObject = web::json::value::object();
            jsonSettingObject[L"id"] = web::json::value::string(L"GameDisplayName");
            jsonSettingObject[L"value"] = web::json::value::string(val->gameDisplayName->Data());
            jsonSettingsArray[arraySize++] = jsonSettingObject;
        }

        {
            auto jsonSettingObject = web::json::value::object();
            jsonSettingObject[L"id"] = web::json::value::string(L"GameDisplayPicRaw");
            jsonSettingObject[L"value"] = web::json::value::string(val->gameDisplayPictureResize->Data());
            jsonSettingsArray[arraySize++] = jsonSettingObject;
        }

        {
            auto jsonSettingObject = web::json::value::object();
            jsonSettingObject[L"id"] = web::json::value::string(L"Gamerscore");
            jsonSettingObject[L"value"] = web::json::value::string(val->gamerscore->Data());
            jsonSettingsArray[arraySize++] = jsonSettingObject;
        }

        {
            auto jsonSettingObject = web::json::value::object();
            jsonSettingObject[L"id"] = web::json::value::string(L"Gamertag");
            jsonSettingObject[L"value"] = web::json::value::string(val->gamertag->Data());
            jsonSettingsArray[arraySize++] = jsonSettingObject;
        }

        // Prepare profileUser
        auto jsonObject = web::json::value::object();
        jsonObject[L"id"] = web::json::value::string(val->xboxUserId->Data());
        jsonObject[L"settings"] = jsonSettingsArray;

        return jsonObject;
    }

    web::json::value
    BuildXboxUserProfilesResultJsonResponse(
        _In_ std::vector<XboxUserProfileTestValues> profileList
        )
    {
        auto jsonArray = web::json::value::array();
        int i = 0;
        for (XboxUserProfileTestValues x : profileList)
        {
            auto jsonResponse = BuildXboxUserProfileJson(&x);
            jsonArray[i++] = jsonResponse;
        }

        auto jsonObject = web::json::value::object();
        jsonObject[L"profileUsers"] = jsonArray;

        return jsonObject;
    }


    void
    VerifyXboxUserProfileProperties(
        _In_ Microsoft::Xbox::Services::Social::XboxUserProfile^ profile,
        _In_ const XboxUserProfileTestValues* testValues
        )
    {
        VERIFY_IS_NOT_NULL(profile);

        VERIFY_ARE_EQUAL_STR(
            testValues->appDisplayName->Data(),
            profile->ApplicationDisplayName->Data()
            );

        VERIFY_ARE_EQUAL_STR(
            testValues->appDisplayPictureResize->Data(),
            profile->ApplicationDisplayPictureResizeUri->AbsoluteUri->Data()
            );

        VERIFY_ARE_EQUAL_STR(
            testValues->gameDisplayName->Data(),
            profile->GameDisplayName->Data()
            );

        VERIFY_ARE_EQUAL_STR(
            testValues->gameDisplayPictureResize->Data(),
            profile->GameDisplayPictureResizeUri->AbsoluteUri->Data()
            );

        VERIFY_ARE_EQUAL_STR(
            testValues->gamerscore->Data(),
            profile->Gamerscore->Data()
            );

        VERIFY_ARE_EQUAL_STR(
            testValues->gamertag->Data(),
            profile->Gamertag->Data()
            );

        VERIFY_ARE_EQUAL_STR(
            testValues->xboxUserId->Data(),
            profile->XboxUserId->Data()
            );

    }

    DEFINE_TEST_CASE(TestGetUserProfileAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetUserProfileAsync);
        std::vector<XboxUserProfileTestValues> profileList;
        XboxUserProfileTestValues x = CreateXboxUserProfileTestValues(0);
        profileList.push_back(x);
        web::json::value responseJson = BuildXboxUserProfilesResultJsonResponse(profileList);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto task = create_task(xboxLiveContext->ProfileService->GetUserProfileAsync(x.xboxUserId));
        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://profile.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/batch/profile/settings", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_STR(LR"({"settings":["AppDisplayName","AppDisplayPicRaw","GameDisplayName","GameDisplayPicRaw","Gamerscore","Gamertag"],"userIds":["xboxUserId_0"]})", httpCall->request_body().request_message_string());

        auto result = task.get();
        VerifyXboxUserProfileProperties(result, &x);
    }

    DEFINE_TEST_CASE(TestGetUserProfilesAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetUserProfilesAsync);
        uint64 seed;
        std::vector<XboxUserProfileTestValues> profileList;
        IVector<Platform::String^>^ xboxUserIds = ref new Vector <Platform::String^>();
        for (seed = 0; seed < 4; seed++)
        {
            XboxUserProfileTestValues x = CreateXboxUserProfileTestValues(seed);
            profileList.push_back(x);
            xboxUserIds->Append(x.xboxUserId);
        }

        web::json::value responseJson = BuildXboxUserProfilesResultJsonResponse(profileList);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto task = create_task(xboxLiveContext->ProfileService->GetUserProfilesAsync(xboxUserIds->GetView()));
        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://profile.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/batch/profile/settings", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_STR(LR"({"settings":["AppDisplayName","AppDisplayPicRaw","GameDisplayName","GameDisplayPicRaw","Gamerscore","Gamertag"],"userIds":["xboxUserId_0","xboxUserId_1","xboxUserId_2","xboxUserId_3"]})", httpCall->request_body().request_message_string());

        auto profiles = task.get();
        int index = 0;
        for (XboxUserProfile^ x : profiles)
        {
            VerifyXboxUserProfileProperties(x, &profileList[index++]);
        }
    }

    DEFINE_TEST_CASE(TestGetUserProfilesForSocialGroupAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetUserProfilesForSocialGroupAsync);
        uint64 seed;
        std::vector<XboxUserProfileTestValues> profileList;
        IVector<Platform::String^>^ xboxUserIds = ref new Vector <Platform::String^>();
        for (seed = 0; seed < 4; seed++)
        {
            XboxUserProfileTestValues x = CreateXboxUserProfileTestValues(seed);
            profileList.push_back(x);
            xboxUserIds->Append(x.xboxUserId);
        }

        web::json::value responseJson = BuildXboxUserProfilesResultJsonResponse(profileList);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto task = create_task(xboxLiveContext->ProfileService->GetUserProfilesForSocialGroupAsync(SocialGroupConstants::People));
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://profile.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/me/profile/settings/people/People?settings=AppDisplayName,AppDisplayPicRaw,GameDisplayName,GameDisplayPicRaw,Gamerscore,Gamertag", httpCall->PathQueryFragment.to_string());

        auto profiles = task.get();
        int index = 0;
        for (XboxUserProfile^ x : profiles)
        {
            VerifyXboxUserProfileProperties(x, &profileList[index++]);
        }
    }

    DEFINE_TEST_CASE(TestProfileServiceInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestProfileServiceInvalidArgs);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        TEST_LOG(L"TestGetUserProfileAsyncInvalidArgs: Null xboxUserId param.");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->ProfileService->GetUserProfileAsync(
            nullptr // Invalid
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetUserProfileAsyncInvalidArgs: Empty xboxUserId param.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->ProfileService->GetUserProfileAsync(
            L"" // Invalid
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetUserProfileAsyncInvalidArgs: Empty Platform::String^ xboxUserId param.");
        Platform::String^ emptyXboxUserId;
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->ProfileService->GetUserProfileAsync(
            emptyXboxUserId // Invalid
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetUserProfilesAsyncInvalidArgs: Null xboxUserIds param.");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->ProfileService->GetUserProfilesAsync(
            nullptr // Invalid
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetUserProfilesAsyncInvalidArgs: Empty xboxUserIds param.");
        IVector<Platform::String^>^ xboxUserIds = ref new Platform::Collections::Vector<Platform::String^>();
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->ProfileService->GetUserProfilesAsync(
            xboxUserIds->GetView() // Invalid
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetUserProfilesAsyncInvalidArgs: Empty Platform::String^ xboxUserId param in input vector.");
        xboxUserIds->Append("12345");
        xboxUserIds->Append(emptyXboxUserId);
        xboxUserIds->Append("56789");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->ProfileService->GetUserProfilesAsync(
            xboxUserIds->GetView() // Invalid
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetUserProfilesForSocialGroupAsyncInvalidArgs: Null socialGroup param.");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->ProfileService->GetUserProfilesForSocialGroupAsync(
            nullptr // Invalid
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetUserProfilesForSocialGroupAsyncInvalidArgs: Empty socialGroup param.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->ProfileService->GetUserProfilesForSocialGroupAsync(
            L"" // Invalid
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetUserProfilesForSocialGroupAsyncInvalidArgs: Empty Platform::String^ socialGroup param.");
        Platform::String^ emptySocialGroup;
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->ProfileService->GetUserProfilesForSocialGroupAsync(
            emptySocialGroup // Invalid
            )).get(),
            E_INVALIDARG
            );
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
