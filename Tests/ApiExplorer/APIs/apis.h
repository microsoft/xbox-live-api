// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once

void SetupAPIs_Xal();
void SetupAPIs_Xbl();
void SetupAPIs_Async();
void SetupAPIs_XblAchievements();
void SetupAPIs_XblAchievementsManager();
void SetupAPIs_XblAchievementsProgressNotifications();
void SetupAPIs_XblSocial();
void SetupAPIs_XblSocialManager();
void SetupAPIs_XblProfile();
void SetupAPIS_Platform();
void SetupAPIs_LibHttp();
void SetupAPIs_XblMultiplayer();
void SetupAPIs_XblMultiplayerManager();
void SetupAPIs_XblPrivacy();
void SetupAPIs_XblEvents();
void SetupAPIs_XblStatistics();
void SetupAPIs_XblLeaderboard();
void SetupAPIs_XblRta();
void SetupAPIs_GrtsGameInvite();
void SetupAPIs_XblPresence();
void SetupAPIs_XblHttp();
void SetupAPIs_XblTitleManagedStats();
void SetupAPIs_XblTitleStorage();
void SetupAPIs_XblStringVerify();
void SetupAPIs_GRTS();
void SetupAPIs_XblMultiplayerActivity();

void SetupAPIs_CppAchievements();
void SetupAPIs_CppLeaderboard();
void SetupAPIs_CppProfile();
void SetupAPIs_CppPrivacy();
void SetupAPIs_CppStringVerify();
void SetupAPIs_CppTitleStorage();
void SetupAPIs_CppEvents();
void SetupAPIs_CppSocial();
void SetupAPIs_CppSocialManager();
void SetupAPIs_CppPresence();
void SetupAPIs_CppStatistics();
void SetupAPIs_CppRta();
void SetupAPIs_CppMultiplayer();

#if HC_PLATFORM == HC_PLATFORM_WIN32
void SetupupAPIs_XblGameInviteNotifications();
void SetupAPIs_XblAchievementUnlockNotification();
#endif

int LuaReturnHR(lua_State *L, HRESULT hr, int extraParams = 0);
void LuaStopTestIfFailed(HRESULT hr);
HRESULT CallLuaString(std::string str);
HRESULT CallLuaStringWithDefault(std::string customFn, std::string defaultFn);
HRESULT CallLuaFunction(std::string fnName);
HRESULT CallLuaFunctionWithHr(HRESULT hr, std::string fnName);
int StopTestFile_Lua(lua_State *L);

uint64_t GetUint64FromLua(lua_State *L, int paramNum, uint64_t defaultArg);
uint32_t GetUint32FromLua(lua_State *L, int paramNum, uint32_t defaultArg);
bool GetBoolFromLua(lua_State *L, int paramNum, bool defaultArg);
std::string GetStringFromLua(lua_State *L, int paramNum, std::string defaultArg);

int XalPlatformWebSetEventHandler_Lua(lua_State *L);
int XalPlatformStorageSetEventHandlers_Lua(lua_State *L);
