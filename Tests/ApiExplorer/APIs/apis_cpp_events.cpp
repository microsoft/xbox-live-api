// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

int EventsServiceWriteInGameEvent_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED && HC_PLATFORM != HC_PLATFORM_XDK
    string_t eventName = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "PuzzleSolved").c_str());
    string_t dimensionsJson = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "{\"DifficultyLevelId\":100,\"EnemyRoleId\":3,\"GameplayModeId\":\"gameplay mode id\",\"KillTypeId\":4,\"MultiplayerCorrelationId\":\"multiplayer correlation id\",\"PlayerRoleId\":1,\"PlayerWeaponId\":2,\"RoundId\":1}").c_str());
    string_t measurementsJson = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 3, "{\"LocationX\":1,\"LocationY\":2.12121,\"LocationZ\":-90909093}").c_str());

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xbox::services::xbox_live_result<void> result = xblc->events_service().write_in_game_event(
        eventName,
        web::json::value::parse(dimensionsJson),
        web::json::value::parse(measurementsJson)
    );

    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
    LogToFile("EventsServiceWriteInGameEvent: hr=%s", ConvertHR(hr).c_str());
#else
    HRESULT hr = S_OK;
    LogToFile("EventsServiceWriteInGameEvent is disabled for this platform.");
#endif
    return LuaReturnHR(L, hr);
}

void SetupAPIs_CppEvents()
{
    lua_register(Data()->L, "EventsServiceWriteInGameEvent", EventsServiceWriteInGameEvent_Lua);
}