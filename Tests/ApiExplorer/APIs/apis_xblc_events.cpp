// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#if HC_PLATFORM == HC_PLATFORM_WIN32
#include <Shlwapi.h>
#endif

int XblEventsWriteInGameEvent_Lua(lua_State *L)
{
#if HC_PLATFORM != HC_PLATFORM_XDK
    auto eventName = GetStringFromLua(L, 1, "PuzzleSolved");
    auto dimensionsJson = GetStringFromLua(L, 2, "{\"DifficultyLevelId\":100,\"EnemyRoleId\":3,\"GameplayModeId\":\"gameplay mode id\",\"KillTypeId\":4,\"MultiplayerCorrelationId\":\"multiplayer correlation id\",\"PlayerRoleId\":1,\"PlayerWeaponId\":2,\"RoundId\":1}");
    auto measurementsJson = GetStringFromLua(L, 3, "{\"LocationX\":1,\"LocationY\":2.12121,\"LocationZ\":-90909093}");

    HRESULT hr = XblEventsWriteInGameEvent(
        Data()->xboxLiveContext,
        eventName.c_str(),
        dimensionsJson.c_str(),
        measurementsJson.c_str()
    );

    LogToFile("XblEventsWriteInGameEvent: hr=%s", ConvertHR(hr).c_str());
#else
    HRESULT hr = S_OK;
#endif

    return LuaReturnHR(L, hr);
}

int ValidateOfflineEventsDirectoryFileExistsAndDelete_Lua(lua_State *L)
{
    HRESULT hr = S_OK;

    // Location of the offline events file is not exposed from XSAPI. We could hard code
    // it here but it may not always be the same, depending on platform
#if HC_PLATFORM == HC_PLATFORM_WIN32
    char pathArray[MAX_PATH + 1];
    GetCurrentDirectoryA(MAX_PATH + 1, pathArray);
    std::string path{ pathArray };
    std::string searchPath{ path + "\\XblEvents*" };

    WIN32_FIND_DATAA fd{};
    HANDLE hFind = FindFirstFileA(searchPath.data(), &fd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            DeleteFileA(std::string{ path + "\\" + fd.cFileName }.data());
        } while (FindNextFileA(hFind, &fd));
    }
    else
    {
        hr = E_FAIL;
    }
#endif
    LogToFile("Validating that XblEvents.dir exists: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

void SetupAPIs_XblEvents()
{
    lua_register(Data()->L, "XblEventsWriteInGameEvent", XblEventsWriteInGameEvent_Lua);
    lua_register(Data()->L, "ValidateOfflineEventsDirectoryFileExistsAndDelete", ValidateOfflineEventsDirectoryFileExistsAndDelete_Lua);
}
