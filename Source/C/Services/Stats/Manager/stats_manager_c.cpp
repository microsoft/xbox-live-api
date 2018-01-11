// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "leaderboard_helper.h"
#include "user_impl_c.h"
#include "stats_manager_helper.h"
#include "stats_manager_state.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::stats::manager;

std::shared_ptr<xsapi_singleton> get_singleton_for_stats() 
{
    auto singleton = get_xsapi_singleton();

    if (singleton->m_statsVars == nullptr) 
    {
        singleton->m_statsVars = std::make_shared<XSAPI_STATS_MANAGER_VARS>();
    }
    return singleton;
}

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerAddLocalUser(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _Out_ PCSTR* errMessage
) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_stats();

    singleton->m_statsVars->cppVoidResult = stats_manager::get_singleton_instance()->add_local_user(user->pImpl->cppUser());

    *errMessage = singleton->m_statsVars->cppVoidResult.err_message().c_str();
    auto cResult = utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_statsVars->cppVoidResult.err());

    if (cResult == XSAPI_RESULT::XSAPI_RESULT_OK)
    {
        singleton->m_statsVars->cUsersMapping[user->pImpl->cppUser()] = user;
    }

    return cResult;
}
CATCH_RETURN()

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerRemoveLocalUser(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _Out_ PCSTR* errMessage
) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_stats();

    singleton->m_statsVars->cppVoidResult = stats_manager::get_singleton_instance()->remove_local_user(user->pImpl->cppUser());

    *errMessage = singleton->m_statsVars->cppVoidResult.err_message().c_str();
    auto cResult = utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_statsVars->cppVoidResult.err());

    if (cResult == XSAPI_RESULT::XSAPI_RESULT_OK)
    {
        singleton->m_statsVars->cUsersMapping.erase(user->pImpl->cppUser());
    }

    return cResult;
}
CATCH_RETURN()

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerRequestFlushToService(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ bool isHighPriority,
    _Out_ PCSTR* errMessage
) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_stats();

    singleton->m_statsVars->cppVoidResult = stats_manager::get_singleton_instance()->request_flush_to_service(user->pImpl->cppUser(), isHighPriority);

    *errMessage = singleton->m_statsVars->cppVoidResult.err_message().c_str();
    return utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_statsVars->cppVoidResult.err());
}
CATCH_RETURN()

XBL_API XSAPI_STAT_EVENT** XBL_CALLING_CONV
StatsManagerDoWork(
    _Inout_ uint32_t* statEventsCount
) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_stats();

    auto cppEvents = stats_manager::get_singleton_instance()->do_work();

    singleton->m_statsVars->cEvents.clear();
    for (auto cppEvent : cppEvents) {
        singleton->m_statsVars->cEvents.push_back(CreateStatEventFromCpp(cppEvent));
    }
    *statEventsCount = (uint32_t)singleton->m_statsVars->cEvents.size();
    
    return singleton->m_statsVars->cEvents.data();
}
CATCH_RETURN_WITH(nullptr)

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerSetStatisticNumberData(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ PCSTR statName,
    _In_ double statValue,
    _Out_ PCSTR* errMessage
) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_stats();

    singleton->m_statsVars->cppVoidResult = stats_manager::get_singleton_instance()->set_stat_as_number(user->pImpl->cppUser(), utils_c::to_utf16string(statName), statValue);

    *errMessage = singleton->m_statsVars->cppVoidResult.err_message().c_str();
    return utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_statsVars->cppVoidResult.err());
}
CATCH_RETURN()

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerSetStatisticIntegerData(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ PCSTR statName,
    _In_ int64_t statValue,
    _Out_ PCSTR* errMessage
) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_stats();

    singleton->m_statsVars->cppVoidResult = stats_manager::get_singleton_instance()->set_stat_as_integer(user->pImpl->cppUser(), utils_c::to_utf16string(statName), statValue);

    *errMessage = singleton->m_statsVars->cppVoidResult.err_message().c_str();
    return utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_statsVars->cppVoidResult.err());
}
CATCH_RETURN()

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerSetStatisticStringData(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ PCSTR statName,
    _In_ PCSTR statValue,
    _Out_ PCSTR* errMessage
) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_stats();

    singleton->m_statsVars->cppVoidResult = stats_manager::get_singleton_instance()->set_stat_as_string(user->pImpl->cppUser(), utils_c::to_utf16string(statName), utils_c::to_utf16string(statValue));

    *errMessage = singleton->m_statsVars->cppVoidResult.err_message().c_str();
    return utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_statsVars->cppVoidResult.err());
}
CATCH_RETURN()

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerGetStatNames(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _Out_ PCSTR** statNameList,
    _Out_ uint32_t* statNameListCount,
    _Out_ PCSTR* errMessage
) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_stats();

    singleton->m_statsVars->cppStatNameList.clear();
    singleton->m_statsVars->cppVoidResult = stats_manager::get_singleton_instance()->get_stat_names(user->pImpl->cppUser(), singleton->m_statsVars->cppStatNameList);
    
    singleton->m_statsVars->cStatNameStringList.resize(singleton->m_statsVars->cppStatNameList.size());
    singleton->m_statsVars->cStatNameCharList.clear();
    for (size_t i = 0; i < singleton->m_statsVars->cppStatNameList.size(); i++)
    {
        auto name = utils_c::to_utf8string(singleton->m_statsVars->cppStatNameList.at(i));
        singleton->m_statsVars->cStatNameStringList[i] = name;
        singleton->m_statsVars->cStatNameCharList.push_back(singleton->m_statsVars->cStatNameStringList[i].c_str());
    }

    *statNameList = singleton->m_statsVars->cStatNameCharList.data();
    *statNameListCount = (uint32_t)singleton->m_statsVars->cStatNameCharList.size();

    *errMessage = singleton->m_statsVars->cppVoidResult.err_message().c_str();
    return utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_statsVars->cppVoidResult.err());
}
CATCH_RETURN()

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerGetStat(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ PCSTR statName,
    _Out_ XSAPI_STAT_VALUE** statValue,
    _Out_ PCSTR* errMessage
) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_stats();

    singleton->m_statsVars->cppStatValueResult = stats_manager::get_singleton_instance()->get_stat(user->pImpl->cppUser(), utils_c::to_utf16string(statName));
    *statValue = CreateStatValueFromCpp(singleton->m_statsVars->cppStatValueResult.payload());

    *errMessage = singleton->m_statsVars->cppStatValueResult.err_message().c_str();
    return utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_statsVars->cppStatValueResult.err());
}
CATCH_RETURN()

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerDeleteStat(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ PCSTR statName,
    _Out_ PCSTR* errMessage
) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_stats();

    singleton->m_statsVars->cppVoidResult = stats_manager::get_singleton_instance()->delete_stat(user->pImpl->cppUser(), utils_c::to_utf16string(statName));

    *errMessage = singleton->m_statsVars->cppVoidResult.err_message().c_str();
    return utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_statsVars->cppVoidResult.err());
}
CATCH_RETURN()

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerGetLeaderboard(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ PCSTR statName,
    _In_ XSAPI_LEADERBOARD_QUERY* query,
    _Out_ PCSTR* errMessage
) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_stats();

    singleton->m_statsVars->cppVoidResult = stats_manager::get_singleton_instance()->get_leaderboard(user->pImpl->cppUser(), utils_c::to_utf16string(statName), query->pImpl->cppQuery());

    *errMessage = singleton->m_statsVars->cppVoidResult.err_message().c_str();
    return utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_statsVars->cppVoidResult.err());
}
CATCH_RETURN()

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerGetSocialLeaderboard(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ PCSTR statName,
    _In_ PCSTR socialGroup,
    _In_ XSAPI_LEADERBOARD_QUERY* query,
    _Out_ PCSTR* errMessage
) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_stats();

    singleton->m_statsVars->cppVoidResult = stats_manager::get_singleton_instance()->get_social_leaderboard(user->pImpl->cppUser(), utils_c::to_utf16string(statName), utils_c::to_utf16string(socialGroup), query->pImpl->cppQuery());

    *errMessage = singleton->m_statsVars->cppVoidResult.err_message().c_str();
    return utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_statsVars->cppVoidResult.err());
}
CATCH_RETURN()