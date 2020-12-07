// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

std::string ConvertXblAchievementProgressStateToString(XblAchievementProgressState state)
{
    std::string stateStr;
    switch (state)
    {
    case XblAchievementProgressState::Unknown:
        stateStr = "XblAchievementProgressState::Unknown";
    case XblAchievementProgressState::Achieved:
        stateStr = "XblAchievementProgressState::Achieved";
    case XblAchievementProgressState::NotStarted:
        stateStr = "XblAchievementProgressState::NotStarted";
    case XblAchievementProgressState::InProgress:
        stateStr = "XblAchievementProgressState::InProgress";
    }
    return stateStr;
}

int XblAchievementsAddAchievementProgressChangeHandler_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    void* context = nullptr;
    auto xblContext = Data()->xboxLiveContext;
    XblFunctionContext achievementProgressContext = XblAchievementsAddAchievementProgressChangeHandler(
        xblContext,
        [](_In_ const XblAchievementProgressChangeEventArgs* args, _In_opt_ void*)
        {
            LogToScreen("XblAchievementsAddAchievementProgressChangeHandler");
            for (uint32_t progressIndex = 0; progressIndex < args->entryCount; ++progressIndex)
            {
                
                LogToScreen("Achievement ID:");
                LogToScreen(args->updatedAchievementEntries[progressIndex].achievementId);
                LogToScreen("Achievement State:");
                LogToScreen(ConvertXblAchievementProgressStateToString(args->updatedAchievementEntries[progressIndex].progressState).c_str());
                for (uint32_t requirementIndex = 0; requirementIndex < args->updatedAchievementEntries[progressIndex].progression.requirementsCount; ++requirementIndex)
                {
                    char response[256];
                    SPRINTF(response, 256, "Achievement Requirement %u", requirementIndex);
                    LogToScreen(response);
                    LogToScreen("Requirement Id:");
                    LogToScreen(args->updatedAchievementEntries[progressIndex].progression.requirements[requirementIndex].id);
                    LogToScreen("Requirement Progress:");
                    LogToScreen(args->updatedAchievementEntries[progressIndex].progression.requirements[requirementIndex].currentProgressValue);
                }
            }
            CallLuaFunctionWithHr(S_OK, "OnXblAchievementsAddAchievementProgressHandler");
        },
        context
    );

    Data()->m_achievementProgressNotificationFunctionContext = achievementProgressContext;
    LogToScreen("XblAchievementsAddAchievementProgressChangeHandler: done");
    return LuaReturnHR(L, S_OK);
}

int XblAchievementsRemoveAchievementProgressChangeHandler_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    
    XblAchievementsRemoveAchievementProgressChangeHandler(Data()->xboxLiveContext, Data()->m_achievementProgressNotificationFunctionContext);
    Data()->m_achievementProgressNotificationFunctionContext = 0;

    LogToScreen("XblAchievementsRemoveAchievementProgressChangeHandler: done");

    return LuaReturnHR(L, S_OK);
}


void SetupAPIs_XblAchievementsProgressNotifications()
{
    lua_register(Data()->L, "XblAchievementsAddAchievementProgressChangeHandler", XblAchievementsAddAchievementProgressChangeHandler_Lua);
    lua_register(Data()->L, "XblAchievementsRemoveAchievementProgressChangeHandler", XblAchievementsRemoveAchievementProgressChangeHandler_Lua);
}