// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/stats_manager.h"
#include "stats_manager_internal.h"
#include "xsapi/services.h"
#include "xsapi/system.h"
#include "xbox_live_context_impl.h"

#if XSAPI_U
#include "ppltasks_extra_unix.h"
#else
#include "ppltasks_extra.h"
#endif

using namespace xbox::services;
using namespace xbox::services::system;
using namespace Concurrency::extras;

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_BEGIN

const std::chrono::seconds stats_manager_impl::TIME_PER_CALL_SEC =
#if UNIT_TEST_SERVICES
std::chrono::seconds::zero();
#else
std::chrono::seconds(30);
#endif

const std::chrono::milliseconds stats_manager_impl::STATS_POLL_TIME_MS = std::chrono::minutes(5);

stats_manager_impl::stats_manager_impl()
{
}

void
stats_manager_impl::initialize()
{
    std::weak_ptr<stats_manager_impl> thisWeakPtr = shared_from_this();
    m_statTimer = std::make_shared<call_buffer_timer>(
    [thisWeakPtr](std::vector<string_t> eventArgs, const call_buffer_timer_completion_context&)
    {
        std::shared_ptr<stats_manager_impl> pThis(thisWeakPtr.lock());
        if (pThis != nullptr && !eventArgs.empty())
        {
            pThis->request_flush_to_service_callback(eventArgs[0]);
        }
    },
    TIME_PER_CALL_SEC
    );

    m_statPriorityTimer = std::make_shared<call_buffer_timer>(
    [thisWeakPtr](std::vector<string_t> eventArgs, const call_buffer_timer_completion_context&)
    {
        std::shared_ptr<stats_manager_impl> pThis(thisWeakPtr.lock());
        if (pThis != nullptr && !eventArgs.empty())
        {
            pThis->request_flush_to_service_callback(eventArgs[0]);
        }
    },
    TIME_PER_CALL_SEC
    );

    run_flush_timer();
}

void
stats_manager_impl::run_flush_timer()
{
    std::weak_ptr<stats_manager_impl> thisWeakPtr = shared_from_this();
    create_delayed_task(
        STATS_POLL_TIME_MS,
        [thisWeakPtr]()
    {
        std::shared_ptr<stats_manager_impl> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            std::lock_guard<std::mutex> guard(pThis->m_statsServiceMutex);
            for (auto& user : pThis->m_users)
            {
                if (user.second.statValueDocument.is_dirty())
                {
                    pThis->flush_to_service(user.second);
                }
            }
            pThis->run_flush_timer();
        }
    });
}

xbox_live_result<void>
stats_manager_impl::add_local_user(
    _In_ const xbox_live_user_t& user
    )
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr = user_context::get_user_id(user);
    auto userIter = m_users.find(userStr);
    if (userIter != m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User already in local map");
    }

    auto xboxLiveContextImpl = std::make_shared<xbox_live_context_impl>(user);
    xboxLiveContextImpl->user_context()->set_caller_context_type(caller_context_type::stats_manager);
    xboxLiveContextImpl->init();
    auto simplifiedStatsService = simplified_stats_service(
        xboxLiveContextImpl->user_context(),
        xboxLiveContextImpl->settings(),
        xboxLiveContextImpl->application_config()
        );

    m_users[userStr] = stats_user_context(stats_value_document(), xboxLiveContextImpl, simplifiedStatsService, user);
    std::weak_ptr<stats_manager_impl> thisWeak = shared_from_this();
    simplifiedStatsService.get_stats_value_document()
    .then([thisWeak, user, xboxLiveContextImpl, simplifiedStatsService, userStr](xbox_live_result<stats_value_document> statsValueDocResult)
    {
        std::shared_ptr<stats_manager_impl> pThis(thisWeak.lock());
        if (pThis == nullptr)
        {
            return;
        }

        std::lock_guard<std::mutex> guard(pThis->m_statsServiceMutex);
        bool isSignedIn = false;
#if TV_API
        isSignedIn = user->IsSignedIn;
#else
        isSignedIn = user->is_signed_in();
#endif

        if (isSignedIn)
        {
            auto& svd = statsValueDocResult.payload();
            auto userStatContext = pThis->m_users.find(userStr);
            if (userStatContext != pThis->m_users.end())    // user could be removed by the time this completes
            {
                if (statsValueDocResult.err())  // if there was an error, but the user is signed in, we assume offline sign in
                {
                    userStatContext->second.statValueDocument.set_state(svd_state::offline_not_loaded);
                }
                else
                {
                    userStatContext->second.statValueDocument.merge_stat_value_documents(svd);
                }
            }

            userStatContext->second.statValueDocument.set_flush_function([thisWeak, userStr, user]()
            {
                std::shared_ptr<stats_manager_impl> pThis(thisWeak.lock());
                if (pThis == nullptr)
                {
                    return;
                }

                std::lock_guard<std::mutex> guard(pThis->m_statsServiceMutex);
                auto statContextIter = pThis->m_users.find(userStr);
                if (statContextIter == pThis->m_users.end())
                {
                    return;
                }

                pThis->flush_to_service(
                    statContextIter->second
                    );
            });
        }
        else    // not offline signed in
        {
            LOG_DEBUG("Could not successfully get SVD for user and user is offline");
        }

        pThis->m_statEventList.push_back(stat_event(stat_event_type::local_user_added, user, xbox_live_result<void>(statsValueDocResult.err(), statsValueDocResult.err_message())));
    });
    return xbox_live_result<void>();
}

xbox_live_result<void>
stats_manager_impl::remove_local_user(
    _In_ const xbox_live_user_t& user
)
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr = user_context::get_user_id(user);
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }

    auto statsUserContext = userIter->second;
    auto userSVD = statsUserContext.statValueDocument;
    userSVD.do_work();  // before removing the user apply all users
    if (userSVD.is_dirty())
    {
        std::weak_ptr<stats_manager_impl> thisWeak = shared_from_this();
        userIter->second.simplifiedStatsService.update_stats_value_document(userSVD)
        .then([thisWeak, userSVD, user, userStr](xbox_live_result<void> updateSVDResult)
        {
            std::shared_ptr<stats_manager_impl> pThis(thisWeak.lock());
            if (pThis == nullptr)
            {
                return;
            }

            std::lock_guard<std::mutex> guard(pThis->m_statsServiceMutex);
            auto statsUserContextIter = pThis->m_users.find(userStr);
            if (statsUserContextIter == pThis->m_users.end())
            {
                return;
            }

            if(should_write_offline(updateSVDResult))
            {
                pThis->write_offline(statsUserContextIter->second);
            }

            pThis->m_statEventList.push_back(stat_event(stat_event_type::local_user_removed, user, updateSVDResult));
            pThis->m_users.erase(userStr);
        });
    }
    else
    {
        m_statEventList.push_back(stat_event(stat_event_type::local_user_removed, user, xbox_live_result<void>()));
        m_users.erase(userIter);
    }

    return xbox_live_result<void>();
}

xbox_live_result<void>
stats_manager_impl::request_flush_to_service(
    _In_ const xbox_live_user_t& user,
    _In_ bool isHighPriority
    )
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr = user_context::get_user_id(user);
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }

    std::vector<string_t> userVec;
    userVec.push_back(userStr);

    if (isHighPriority)
    {
        m_statPriorityTimer->fire(userVec);
    }
    else
    {
        m_statTimer->fire(userVec);
    }

    return xbox_live_result<void>();
}

void
stats_manager_impl::flush_to_service(
    _In_ stats_user_context& statsUserContext
)
{
    std::weak_ptr<stats_manager_impl> thisWeak = shared_from_this();
    if (statsUserContext.xboxLiveUser == nullptr)
    {
        LOG_DEBUG("flush_to_service: user is null");
        return;
    }
    auto userStr = user_context::get_user_id(statsUserContext.xboxLiveUser);
    auto& svd = statsUserContext.statValueDocument;
    svd.clear_dirty_state();
    if (svd.state() != svd_state::loaded)   // if not loaded, try and get the SVD from the service
    {
        statsUserContext.simplifiedStatsService.get_stats_value_document().then([thisWeak, userStr](xbox_live_result<stats_value_document> svdResult)
        {
            std::shared_ptr<stats_manager_impl> pThis(thisWeak.lock());
            if (pThis == nullptr)
            {
                return;
            }

            std::lock_guard<std::mutex> guard(pThis->m_statsServiceMutex);

            auto userIter = pThis->m_users.find(userStr);
            if (userIter == pThis->m_users.end())
            {
                LOG_DEBUG("User not found in flush_to_service lambda");
                return;
            }

            if (!svdResult.err())
            {
                userIter->second.statValueDocument.merge_stat_value_documents(svdResult.payload());
                pThis->update_stats_value_document(userIter->second);
            }
        });
    }
    else
    {
        update_stats_value_document(statsUserContext);
    }

}
void
stats_manager_impl::update_stats_value_document(_In_ stats_user_context& statsUserContext)
{
    std::weak_ptr<stats_manager_impl> thisWeak = shared_from_this();
    xbox_live_user_t user = statsUserContext.xboxLiveUser;
    if (user == nullptr)
    {
        LOG_WARN("User disappeared");
        return;
    }

    auto userStr = user_context::get_user_id(user);

    statsUserContext.simplifiedStatsService.update_stats_value_document(statsUserContext.statValueDocument)
    .then([thisWeak, user, userStr](xbox_live_result<void> updateSVDResult)
    {
        std::shared_ptr<stats_manager_impl> pThis(thisWeak.lock());
        if (pThis == nullptr)
        {
            return;
        }

        std::lock_guard<std::mutex> guard(pThis->m_statsServiceMutex);
        auto statsUserContextIter = pThis->m_users.find(userStr);
        if (statsUserContextIter == pThis->m_users.end())
        {
            return;
        }

        auto& statsUserContext = statsUserContextIter->second;
        if (updateSVDResult.err())
        {
            if (should_write_offline(updateSVDResult))
            {
                if (statsUserContext.statValueDocument.state() == svd_state::loaded)
                {
                    statsUserContext.statValueDocument.set_state(svd_state::offline_loaded);
                }

                pThis->write_offline(statsUserContext);
            }
            else
            {
                LOG_ERROR("Stats manager could not write stats value document");
            }
        }

        pThis->m_statEventList.push_back(stat_event(stat_event_type::stat_update_complete, user, updateSVDResult));
    });
}

void
stats_manager_impl::request_flush_to_service_callback(
    _In_ const string_t& userXuid
    )
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    auto userIter = m_users.find(userXuid);
    if (userIter != m_users.end())
    {
        userIter->second.statValueDocument.do_work();
        flush_to_service(
            userIter->second
            );
    }
}

std::vector<stat_event>
stats_manager_impl::do_work()
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    auto copyList = m_statEventList;
    for (auto& statUserContext : m_users)
    {
        statUserContext.second.statValueDocument.do_work();
    }
    m_statEventList.clear();

    return copyList;
}

xbox_live_result<void>
stats_manager_impl::set_stat(
    _In_ const xbox_live_user_t& user,
    _In_ const string_t& name,
    _In_ double value
    )
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr = user_context::get_user_id(user);
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }
    auto result = userIter->second.statValueDocument.set_stat(name.c_str(), value);
    return result;
}

xbox_live_result<void>
stats_manager_impl::set_stat(
    _In_ const xbox_live_user_t& user,
    _In_ const string_t& name,
    _In_ const char_t* value
)
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr = user_context::get_user_id(user);
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }

    auto result = userIter->second.statValueDocument.set_stat(name.c_str(), value);
    return result;
}

xbox_live_result<stat_value>
stats_manager_impl::get_stat(
    _In_ const xbox_live_user_t& user,
    _In_ const string_t& name
    )
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr = user_context::get_user_id(user);
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<stat_value>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }

    return userIter->second.statValueDocument.get_stat(name.c_str());
}

xbox_live_result<void>
stats_manager_impl::get_stat_names(
    _In_ const xbox_live_user_t& user,
    _Inout_ std::vector<string_t>& statNameList
    )
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr = user_context::get_user_id(user);
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }

    userIter->second.statValueDocument.get_stat_names(statNameList);

    return xbox_live_result<void>();
}

xbox_live_result<void>
stats_manager_impl::delete_stat(
    _In_ const xbox_live_user_t& user,
    _In_ const string_t& name
    )
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr = user_context::get_user_id(user);
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }

    return userIter->second.statValueDocument.delete_stat(name.c_str());
}

#if TV_API
void
stats_manager_impl::write_offline(
    _In_ stats_user_context& userContext
    )
{
    UNREFERENCED_PARAMETER(userContext);
    // TODO: implement
}

#elif !UNIT_TEST_SERVICES
void
stats_manager_impl::write_offline(
    _In_ stats_user_context& userContext
    )
{
    web::json::value evtJson;
    evtJson[_T("svd")] = userContext.statValueDocument.serialize();
    auto result = userContext.xboxLiveContextImpl->events_service().write_in_game_event(_T("StatEvent"), evtJson, web::json::value());
    if (result.err())
    {
        LOG_ERROR("Offline write for stats failed");
    }
}
#else
void
stats_manager_impl::write_offline(
    _In_ stats_user_context& userContext
)
{
    UNREFERENCED_PARAMETER(userContext);
}
#endif

xbox_live_result<void> stats_manager_impl::get_leaderboard(const xbox_live_user_t& user, const string_t& statName, leaderboard::leaderboard_query query)
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr = user_context::get_user_id(user);
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }
    string_t xuid;
    if (query.skip_result_to_me())
    {
        xuid = user_context::get_user_id(user);
    }
    std::weak_ptr<stats_manager_impl> weakThisPtr = shared_from_this();
    auto context = userIter->second.xboxLiveContextImpl;
    context->leaderboard_service().get_leaderboard_internal(
        context->application_config()->scid(),
        statName,
        query.skip_result_to_rank(),
        xuid,
        user_context::get_user_id(user),
        _T(""),
        query.max_items(),
        query._Continuation_token(),
        std::vector<string_t>(),
        _T("2017"),
        query
        ).then([weakThisPtr, user](xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result> result)
    {
        auto pShared = weakThisPtr.lock();
        if (pShared.get() == nullptr)
        {
            LOG_DEBUG("Could not successfully get stats_manager while retrieving a leaderboard");
        }
        else
        {
            pShared->add_leaderboard_result(user, result);
        }
    });

    return xbox_live_result<void>();
}

xbox_live_result<void> stats_manager_impl::get_social_leaderboard(const xbox_live_user_t& user, const string_t& statName, const string_t& socialGroup, leaderboard::leaderboard_query query)
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr = user_context::get_user_id(user);
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }
    string_t xuid;
    if (query.skip_result_to_me())
    {
        xuid = user_context::get_user_id(user);
    }
    string_t order;
    if (query.order() == leaderboard::sort_order::ascending)
    {
        order = _T("ascending");
    }
    else
    {
        order = _T("descending");
    }

    std::weak_ptr<stats_manager_impl> weakThisPtr = shared_from_this();
    auto context = userIter->second.xboxLiveContextImpl;
    context->leaderboard_service().get_leaderboard_for_social_group_internal(
        user_context::get_user_id(user),
        context->application_config()->scid(),
        statName,
        socialGroup,
        query.skip_result_to_rank(),
        xuid,
        order,
        query.max_items(),
        query._Continuation_token(),
        _T("2017"),
        query
    ).then([weakThisPtr, user](xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result> result)
    {
        auto pShared = weakThisPtr.lock();
        if (pShared.get() == nullptr)
        {
            LOG_DEBUG("Could not successfully get stats_manager while retrieving a leaderboard");
        }
        else
        {
            pShared->add_leaderboard_result(user, result);
        }
    });

    return xbox_live_result<void>();

}

void stats_manager_impl::add_leaderboard_result(
    _In_ const xbox_live_user_t& user,
    _In_ const xbox_live_result<leaderboard::leaderboard_result>& result
)
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr = user_context::get_user_id(user);
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        LOG_DEBUG("stats_manager_impl User not found in local map");
        return;
    }
    stat_event statEvent(
        stat_event_type::get_leaderboard_complete,
        userIter->second.xboxLiveUser,
        xbox_live_result<void>(),
        std::make_shared<leaderboard_result_event_args>(result)
        );

    m_statEventList.push_back(statEvent);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_END
