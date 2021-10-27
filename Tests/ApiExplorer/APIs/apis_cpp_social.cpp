// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

XBL_WARNING_DISABLE_DEPRECATED

#if CPP_TESTS_ENABLED

xbox::services::social::xbox_social_relationship_filter ConvertStringToCppSocialRelationshipFilter(const char* str)
{
    xbox::services::social::xbox_social_relationship_filter filter = xbox::services::social::xbox_social_relationship_filter::all;

    if (pal::stricmp(str, "xbox_social_relationship_filter::all") == 0) filter = xbox::services::social::xbox_social_relationship_filter::all;
    else if (pal::stricmp(str, "xbox_social_relationship_filter::Favorite") == 0) filter = xbox::services::social::xbox_social_relationship_filter::favorite;
    else if (pal::stricmp(str, "xbox_social_relationship_filter::LegacyXboxLiveFriends") == 0) filter = xbox::services::social::xbox_social_relationship_filter::legacy_xbox_live_friends;

    return filter;
}

#endif

int SocialServiceGetSocialRelationships_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    xbox::services::social::xbox_social_relationship_filter socialRelationshipFilter = ConvertStringToCppSocialRelationshipFilter(GetStringFromLua(L, 1, "xbox_social_relationship_filter::all").c_str());
    uint32_t startIndex = 0;
    uint32_t maxItems = 0;

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    auto task = xblc->social_service().get_social_relationships(
        socialRelationshipFilter,
        startIndex,
        maxItems
    ).then([xblc](xbox::services::xbox_live_result<xbox::services::social::xbox_social_relationship_result> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("SocialServiceGetSocialRelationships: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                xbox::services::social::xbox_social_relationship_result socialRelationshipResult = result.payload();
                Data()->socialRelationshipResult = socialRelationshipResult;

                auto relationships = socialRelationshipResult.items();
                size_t relationshipsCount = relationships.size();

                LogToFile("Got %u SocialRelationships:", relationshipsCount);;
                for (size_t i = 0; i < relationshipsCount; i++)
                {
                    LogToFile("Xuid = %s, isFollowingCaller = %u", xbox::services::Utils::StringFromStringT(relationships[i].xbox_user_id()).c_str(), relationships[i].is_following_caller());
                }
            }

            CallLuaFunctionWithHr(hr, "OnSocialServiceGetSocialRelationships");

        });
#else
    CallLuaFunctionWithHr(S_OK, "OnSocialServiceGetSocialRelationships");
    LogToFile("SocialServiceGetSocialRelationships is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int SocialRelationshipResultHasNextCpp_Lua(lua_State *L)
{
    bool hasNext{ false };
#if CPP_TESTS_ENABLED
    hasNext = Data()->socialRelationshipResult.has_next();
    LogToFile("SocialRelationshipResultHasNextCpp: hasNext=%s", hasNext ? "true" : "false");
#else
    LogToFile("SocialRelationshipResultHasNextCpp is disabled for this platform.");
#endif
    lua_pushnumber(L, (int)hasNext);
    return LuaReturnHR(L, S_OK, 1);
}

int SocialRelationshipResultGetNextCpp_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    uint32_t maxItems = 100;
    LogToFile("SocialRelationshipResultGetNextCpp MaxItems: %d", maxItems);
    Data()->socialRelationshipResult.get_next(maxItems).then(
        [](xbox::services::xbox_live_result< xbox::services::social::xbox_social_relationship_result > result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            if (SUCCEEDED(hr))
            {
                xbox::services::social::xbox_social_relationship_result socialRelationshipResult = result.payload();
                Data()->socialRelationshipResult = socialRelationshipResult;

                auto relationships = socialRelationshipResult.items();
                size_t relationshipsCount = relationships.size();

                LogToFile("Got %u SocialRelationships:", relationshipsCount);;
                for (size_t i = 0; i < relationshipsCount; i++)
                {
                    LogToFile("Xuid = %s, isFollowingCaller = %u", xbox::services::Utils::StringFromStringT(relationships[i].xbox_user_id()).c_str(), relationships[i].is_following_caller());
                }
            }
            CallLuaFunctionWithHr(hr, "OnSocialRelationshipsResultGetNextCpp");
        });
#else
    LogToFile("SocialRelationshipResultGetNextCpp is disabled for this platform.");
    CallLuaFunctionWithHr(S_OK, "OnSocialRelationshipsResultGetNextCpp");
#endif
    return LuaReturnHR(L, S_OK);
}

int SocialRelationshipResultCloseHandleCpp_Lua(lua_State* L)
{
    //The relationship result needs to be cleaned up before the lua script ends and global state is cleaned up, 
    //otherwise it will be the last reference to the xblContext, and cause the xblContext and xalUser to cleanup
    //without global state.
#if CPP_TESTS_ENABLED
    //Force the relationship result to release it's handles early. Realistically this should never be called in a real use case scenario.
    Data()->socialRelationshipResult.~xbox_social_relationship_result();
#else
#endif
    return LuaReturnHR(L, S_OK);
}

int SocialServiceSubscribeToSocialRelationshipChange_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    string_t xboxUserID = xbox::services::Utils::StringTFromUint64(Data()->xboxUserId);

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    auto result = xblc->social_service().subscribe_to_social_relationship_change(xboxUserID);

    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
    if (SUCCEEDED(hr))
    {
        Data()->socialRelationshipChangeSubscription = result.payload();
    }
    LogToFile("SocialServiceSubscribeToSocialRelationshipChange: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
#else
    LogToFile("SocialServiceSubscribeToSocialRelationshipChange is disabled on this platform.");
    return LuaReturnHR(L, S_OK);
#endif

}

int SocialServiceUnsubscribeFromSocialRelationshipChange_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    auto result = xblc->social_service().unsubscribe_from_social_relationship_change(Data()->socialRelationshipChangeSubscription);

    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
    if (SUCCEEDED(hr))
    {
        Data()->socialRelationshipChangeSubscription = nullptr;
    }
    LogToFile("SocialServiceUnsubscribeFromSocialRelationshipChange: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
#else
    LogToFile("SocialServiceUnsubscribeFromSocialRelationshipChange is disabled on this platform.");
    return LuaReturnHR(L, S_OK);
#endif
}

int SocialServiceAddSocialRelationshipChangedHandler_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    Data()->socialRelationshipChangedHandlerContext = xblc->social_service().add_social_relationship_changed_handler(
        [](xbox::services::social::social_relationship_change_event_args args)
        {
            LogToFile("Social relationship changed:");
            std::stringstream ss;
            for (size_t i = 0; i < args.xbox_user_ids().size(); ++i)
            {
                if (i > 0)
                {
                    ss << ", ";
                }
                ss << xbox::services::Utils::StringFromStringT(args.xbox_user_ids()[i]);
            }
            LogToFile("socialNotification = %u, affectedXuids = %s", args.social_notification(), ss.str().data());
        });
    LogToFile("SocialServiceAddSocialRelationshipChangedHandler");
#else
    LogToFile("SocialServiceAddSocialRelationshipChangedHandler is disabled on this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int SocialServiceRemoveSocialRelationshipChangedHandler_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->social_service().remove_social_relationship_changed_handler(Data()->socialRelationshipChangedHandlerContext);
    Data()->socialRelationshipChangedHandlerContext = nullptr;
    LogToFile("SocialServiceRemoveSocialRelationshipChangedHandler");
#else
    LogToFile("SocialServiceRemoveSocialRelationshipChangedHandler is disabled on this platform");
#endif
    return LuaReturnHR(L, S_OK);
}

int ReputationServiceSubmitReputationFeedback_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    xbox::services::social::reputation_feedback_type reputationFeedbackType = xbox::services::social::reputation_feedback_type::positive_helpful_player;

    string_t xuid = _T("2814639011617876");
    string_t sessionName = _T("");
    string_t reasonMessage = _T("Helpful player");
    string_t evidenceResourceId = _T("");

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->reputation_service().submit_reputation_feedback(
        xuid,
        reputationFeedbackType,
        sessionName,
        reasonMessage,
        evidenceResourceId
    ).then([](xbox::services::xbox_live_result<void> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("ReputationServiceSubmitReputationFeedback: hr=%s", ConvertHR(hr).c_str());
            CallLuaFunctionWithHr(hr, "OnReputationServiceSubmitReputationFeedback");

        });
#else
    LogToFile("ReputationServiceSubmitReputationFeedback is disabled for this platform.");
    CallLuaFunctionWithHr(S_OK, "OnReputationServiceSubmitReputationFeedback");
#endif
    return LuaReturnHR(L, S_OK);
}

int ReputationServiceSubmitBatchReputationFeedback_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::vector<xbox::services::social::reputation_feedback_item> feedbackItems;

    feedbackItems.push_back(xbox::services::social::reputation_feedback_item
        {
            _T("2814639011617876"),
            xbox::services::social::reputation_feedback_type::positive_helpful_player,
            xbox::services::multiplayer::multiplayer_session_reference(),
            _T("Helpful player"),
            _T("")
        });

    // Add any additional feedback items here
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->reputation_service().submit_batch_reputation_feedback(feedbackItems).then(
        [](xbox::services::xbox_live_result<void> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("ReputationServiceSubmitBatchReputationFeedback: hr=%s", ConvertHR(hr).c_str());
            CallLuaFunctionWithHr(hr, "OnReputationServiceSubmitBatchReputationFeedback");
        });
#else
    LogToFile("ReputationServiceSubmitBatchReputationFeedback is disabled for this platform.");
    CallLuaFunctionWithHr(S_OK, "OnReputationServiceSubmitBatchReputationFeedback");
#endif
    return LuaReturnHR(L, S_OK);
}

void SetupAPIs_CppSocial()
{
    lua_register(Data()->L, "SocialServiceGetSocialRelationships", SocialServiceGetSocialRelationships_Lua);
    lua_register(Data()->L, "SocialRelationshipResultHasNextCpp", SocialRelationshipResultHasNextCpp_Lua);
    lua_register(Data()->L, "SocialRelationshipResultGetNextCpp", SocialRelationshipResultGetNextCpp_Lua);
    lua_register(Data()->L, "SocialRelationshipResultCloseHandleCpp", SocialRelationshipResultCloseHandleCpp_Lua);

    lua_register(Data()->L, "SocialServiceSubscribeToSocialRelationshipChange", SocialServiceSubscribeToSocialRelationshipChange_Lua);
    lua_register(Data()->L, "SocialServiceUnsubscribeFromSocialRelationshipChange", SocialServiceUnsubscribeFromSocialRelationshipChange_Lua);
    lua_register(Data()->L, "SocialServiceAddSocialRelationshipChangedHandler", SocialServiceAddSocialRelationshipChangedHandler_Lua);
    lua_register(Data()->L, "SocialServiceRemoveSocialRelationshipChangedHandler", SocialServiceRemoveSocialRelationshipChangedHandler_Lua);

    lua_register(Data()->L, "ReputationServiceSubmitReputationFeedback", ReputationServiceSubmitReputationFeedback_Lua);
    lua_register(Data()->L, "ReputationServiceSubmitBatchReputationFeedback", ReputationServiceSubmitBatchReputationFeedback_Lua);
}

