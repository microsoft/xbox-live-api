// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"jasonsa"
#define TEST_CLASS_AREA L"PeopleHub"
#include "UnitTestIncludes.h"
#include "social_manager_internal.h"
#include "xsapi/services.h"
#include "xbox_live_context_impl.h"
#include "SocialManagerHelper.h"
#include "Event_WinRT.h"

using namespace xbox::services;
using namespace xbox::services::social::manager;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(PeoplehubTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(PeoplehubTests)

    void VerifyPresenceRecord(social_manager_presence_record presenceRecord, web::json::value response)
    {
        VERIFY_IS_TRUE(presenceRecord.presence_title_records().size() == response.size());
        for (uint32_t i = 0; i < response.size(); ++i)
        {
            auto jsonEntry = response[i];
            auto titleEntry = presenceRecord.presence_title_records()[i];

            VERIFY_ARE_EQUAL(jsonEntry[L"IsBroadcasting"].as_bool(), titleEntry.is_broadcasting());
            VERIFY_IS_TRUE(xbox::services::presence::presence_device_record_internal::convert_string_to_presence_device_type(utils::internal_string_from_string_t(jsonEntry[L"Device"].as_string())) == titleEntry.device_type());
            auto state = jsonEntry[L"State"].as_string();
            auto presenceState = (!state.empty() && utils::str_icmp(state, _T("active")) == 0);
            VERIFY_ARE_EQUAL(presenceState, titleEntry.is_title_active());
            VERIFY_IS_TRUE(utils::string_t_to_uint32(jsonEntry[L"TitleId"].as_string()) == titleEntry.title_id());
            VERIFY_ARE_EQUAL_STR(jsonEntry[L"PresenceText"].as_string(), titleEntry.presence_text());
        }
    }
    void VerifyTitleHistory(title_history titleHistory, web::json::value response)
    {
        if (response.is_null())
        {
            VERIFY_IS_FALSE(titleHistory.has_user_played());
        }
        else
        {
            VERIFY_IS_TRUE(titleHistory.has_user_played());
            DateTime dt;
            dt.UniversalTime = utils::extract_json_time(response, L"LastTimePlayed").to_interval();
            VERIFY_ARE_EQUAL_UINT(titleHistory.last_time_user_played().to_interval(), dt.UniversalTime);
        }
    }

    void VerifyPreferredColor(const preferred_color& preferredColor, web::json::value response)
    {
        if (response.is_null())
        {
            VERIFY_ARE_EQUAL(preferredColor.primary_color(), _T(""));
            VERIFY_ARE_EQUAL(preferredColor.secondary_color(), _T(""));
            VERIFY_ARE_EQUAL(preferredColor.tertiary_color(), _T(""));
        }
        else
        {
            VERIFY_ARE_EQUAL(preferredColor.primary_color(), response[L"primaryColor"].as_string());
            VERIFY_ARE_EQUAL(preferredColor.secondary_color(), response[L"secondaryColor"].as_string());
            VERIFY_ARE_EQUAL(preferredColor.tertiary_color(), response[L"tertiaryColor"].as_string());
        }
    }

    void VerifyXboxSocialUser(const xbox_social_user& xboxSocialUser, web::json::value& people)
    {
        VERIFY_ARE_EQUAL(xboxSocialUser.is_favorite(), people[L"isFavorite"].as_bool());
        VERIFY_ARE_EQUAL(xboxSocialUser.is_following_user(), people[L"isFollowingCaller"].as_bool());
        VERIFY_ARE_EQUAL(xboxSocialUser.is_followed_by_caller(), people[L"isFollowedByCaller"].as_bool());
        VERIFY_ARE_EQUAL(xboxSocialUser.display_name(), people[L"displayName"].as_string());
        VERIFY_ARE_EQUAL(xboxSocialUser.real_name(), people[L"realName"].as_string());
        VERIFY_ARE_EQUAL(xboxSocialUser.display_pic_url_raw(), people[L"displayPicRaw"].as_string());
        VERIFY_ARE_EQUAL(xboxSocialUser.use_avatar(), people[L"useAvatar"].as_bool());
        VERIFY_ARE_EQUAL(xboxSocialUser.gamertag(), people[L"gamertag"].as_string());
        VERIFY_ARE_EQUAL(xboxSocialUser.gamerscore(), people[L"gamerScore"].as_string());
        VerifyTitleHistory(xboxSocialUser.title_history(), people[L"titleHistory"]);
        VerifyPreferredColor(xboxSocialUser.preferred_color(), people[L"preferredColor"]);
        VerifyPresenceRecord(xboxSocialUser.presence_record(), people[L"presenceDetails"]);
    }

    DEFINE_TEST_CASE(PeopleHubTestGetSocialGraph)
    {
        DEFINE_TEST_CASE_PROPERTIES(PeopleHubTestGetSocialGraph);
        xsapi_internal_vector<xsapi_internal_string> xuids;
        xuids.push_back("1");

        auto peoplehubService = SocialManagerHelper::GetPeoplehubService();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValueInternal = StockMocks::CreateMockHttpCallResponseInternal(web::json::value::parse(peoplehubResponse));

        Event^ callComplete = ref new Event();

        peoplehubService.get_social_graph("TestXboxUserId", social_manager_extra_detail_level::preferred_color_level, xuids, 0,
            [&callComplete, this](xbox_live_result<xsapi_internal_vector<xbox_social_user>> userGroup)
        {
            VERIFY_IS_TRUE(!userGroup.err());

            web::json::array userGroupArr = web::json::value::parse(peoplehubResponse)[_T("people")].as_array();

            uint32_t counter = 0;
            for (auto& socialUsers : userGroup.payload())
            {
                web::json::value socialUserJson = userGroupArr[counter];
                VerifyXboxSocialUser(socialUsers, socialUserJson);
                ++counter;
            }
            callComplete->Set();
        });

        callComplete->Wait();

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://peoplehub.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/users/xuid(TestXboxUserId)/people/batch/decoration/preferredcolor,presenceDetail",
            httpCall->PathQueryFragment.to_string()
            );
    }

    DEFINE_TEST_CASE(PeopleHubTestGetSocialGraphWithDecorations)
    {
        DEFINE_TEST_CASE_PROPERTIES(GetSocialGraphWithDecorations);
        auto peoplehubService = SocialManagerHelper::GetPeoplehubService();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(peoplehubResponse));
    }

    DEFINE_TEST_CASE(PeopleHubTestOverloadStrings)
    {
        DEFINE_TEST_CASE_PROPERTIES(PeopleHubTestOverloadStrings);
        auto peoplehubService = SocialManagerHelper::GetPeoplehubService();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValueInternal = StockMocks::CreateMockHttpCallResponseInternal(web::json::value::parse(peoplehubOversizedResponse));
        xsapi_internal_vector<xsapi_internal_string> xuids;
        xuids.push_back("1");

        Event^ callComplete = ref new Event();
        peoplehubService.get_social_graph("TestXboxUserId", social_manager_extra_detail_level::preferred_color_level, xuids, 0,
            [&callComplete](xbox_live_result<xsapi_internal_vector<xbox_social_user>> userGroupResult)
        {
            VERIFY_IS_TRUE(!userGroupResult.err());
            auto xboxUserId = web::json::value::parse(peoplehubOversizedResponse)[_T("people")][0][_T("xuid")];
            auto user = userGroupResult.payload()[0];
            auto compareUser = xboxUserId.serialize().substr(0, 21);
            wchar_t* compareUserChar = &compareUser[1];
            VERIFY_IS_TRUE(utils::str_icmp(user.xbox_user_id(), compareUserChar) == 0);
            callComplete->Set();
        });
        callComplete->Wait();
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END