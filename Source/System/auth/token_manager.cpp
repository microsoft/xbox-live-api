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
#include "shared_macros.h"
#include "token_manager.h"
#include "xbox_system_factory.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

token_manager::token_info::token_info() : 
    IsRefreshInProgress(false)
{
}

token_manager::token_info::token_info(
    _In_ string_t relyingParty,
    _In_ string_t subRelyingParty,
    _In_ string_t tokenType) :
    RelyingParty(std::move(relyingParty)),
    SubRelyingParty(std::move(subRelyingParty)),
    TokenType(std::move(tokenType)),
    IsRefreshInProgress(false)
{
}

token_manager::token_state::token_state(
    _In_ std::shared_ptr<ecdsa> proofKey,
    _In_ std::shared_ptr<auth_config> authConfig,
    _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings) :
    ProofKey(proofKey),
    AuthConfig(authConfig),
    XboxLiveContextSettings(xboxLiveContextSettings)
{
}

token_manager::token_manager(
    _In_ std::shared_ptr<ecdsa> proofKey,
    _In_ std::shared_ptr<auth_config> authConfig,
    _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings
    ) :
    m_state(std::make_shared<token_state>(proofKey, authConfig, xboxLiveContextSettings))
{
}

pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>> token_manager::get_xtoken(
    _In_ const string_t& relyingParty,
    _In_ const string_t& subRelyingParty,
    _In_ const string_t tokenType,
    _In_ bool promptForCreds,
    _In_ bool forceRefresh)
{
    return get_token_from_cache_or_service(
        m_state, 
        relyingParty, 
        subRelyingParty,
        tokenType, 
        promptForCreds, 
        token_identity_type::x_token,
        forceRefresh
        );
}

pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>> token_manager::get_token_from_cache_or_service(
    _In_ std::shared_ptr<token_state> state,
    _In_ const string_t& relyingParty,
    _In_ const string_t& subRelyingParty,
    _In_ const string_t tokenType,
    _In_ bool promptForCreds,
    _In_ token_identity_type tokenIdentityType,
    _In_ bool forceRefresh
    )
{
    std::shared_ptr<token_info> tokenInfo = get_token_from_cache(state, relyingParty, subRelyingParty, tokenType, tokenIdentityType, forceRefresh);
    token_result tokenResult;
    uint64_t expiration = 0;
    {
        if (tokenInfo != nullptr)
        {
            std::lock_guard<std::mutex> tokenLock(tokenInfo->Lock);
            expiration = tokenInfo->Token.expiration();
            tokenResult = tokenInfo->Token;
        }
    }

    if (utility::datetime::utc_now().to_interval() + utility::datetime::from_minutes(10) > expiration)
    {
        bool isRefreshInProgress;
        {
            std::lock_guard<std::mutex> tokenLock(tokenInfo->Lock);
            isRefreshInProgress = tokenInfo->IsRefreshInProgress;
            if (!isRefreshInProgress)
            {
                // Only kick off to get a new token if not doing so already
                tokenInfo->IsRefreshInProgress = true;
                tokenInfo->RefreshEvent = pplx::task_completion_event<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>>();
            }
        }
        if (!isRefreshInProgress)
        {
            try
            {
                switch (tokenIdentityType)
                {
                    case token_identity_type::x_token: return refresh_x_token(state, tokenInfo, promptForCreds);
                    case token_identity_type::t_token: return refresh_t_token(state, tokenInfo);
                    case token_identity_type::d_token: return refresh_d_token(state, tokenInfo);
                    case token_identity_type::u_token: return refresh_u_token(state, tokenInfo);
#if XSAPI_SERVER
                    case token_identity_type::s_token: return refresh_s_token(state, tokenInfo);
#endif
                    default: throw std::runtime_error("");
                }
            }
            catch (const std::exception&)
            {
                std::exception_ptr exPtr = std::current_exception();
                std::lock_guard<std::mutex> xtokenLock(tokenInfo->Lock);
                tokenInfo->RefreshEvent.set_exception(exPtr);
                tokenInfo->IsRefreshInProgress = false;
                return pplx::create_task(tokenInfo->RefreshEvent);
            }
        }
        else
        {
            return pplx::create_task(tokenInfo->RefreshEvent);
        }
    }
    else
    {
        XBOX_LIVE_NAMESPACE::xbox_live_result<token_result> xblResult(tokenResult);
        return pplx::task_from_result(xblResult);
    }
}

std::shared_ptr<token_manager::token_info> token_manager::get_token_from_cache(
    _In_ std::shared_ptr<token_state> state,
    _In_ const string_t& relyingParty,
    _In_ const string_t& subRelyingParty,
    _In_ const string_t& tokenType,
    _In_ token_identity_type tokenIdentityType,
    _In_ bool forceRefresh)
{
    string_t key(create_cache_key(relyingParty, subRelyingParty, tokenType, tokenIdentityType));

    std::lock_guard<std::mutex> lock(state->DataLock);
    
    auto it(state->Cache.find(key));
    if (!forceRefresh && it != state->Cache.end())
    {
        return it->second;
    }
    
    std::shared_ptr<token_info> result = std::make_shared<token_info>(relyingParty, subRelyingParty, tokenType);
    state->Cache[key] = result;
    return result;
}

string_t token_manager::create_cache_key(
    _In_ const string_t& relyingParty,
    _In_ const string_t& subRelyingParty,
    _In_ const string_t& tokenType,
    _In_ token_identity_type tokenIdentityType)
{
    // Key looks like: 1_JWT_http://xboxlive.com

    stringstream_t ss;
    ss << static_cast<int>(tokenIdentityType);
    ss << _T("_");
    ss << tokenType;
    if (tokenIdentityType == token_identity_type::x_token || tokenIdentityType == token_identity_type::s_token)
    {
        ss << _T("_");
        ss << relyingParty;
        ss << _T("_");
        ss << subRelyingParty;
    }
    return ss.str();
}

pplx::task<xbox_live_result<token_result>> 
token_manager::refresh_x_token(
    _In_ std::shared_ptr<token_state> state,
    _In_ std::shared_ptr<token_info> xTokenInfo,
    _In_ bool promptForCreds
    )
{
    
    return pplx::create_task([state, xTokenInfo]()
    {
        uint32_t prereqTokenCount = 0;
        // before we start any tasks, correctly initialize the pre-req token count
        // these are all of the tokens we need before we can fetch the title token
        for( auto& tokenType : state->AuthConfig->xtoken_composition())
        {
            if (tokenType == token_identity_type::d_token)
            {
                ++prereqTokenCount;
            }
            if (tokenType == token_identity_type::s_token)
            {
                ++prereqTokenCount;
            }
        }
        
        string_t utoken, stoken, ttoken, dtoken;
        
        std::vector<pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>>> tokenTasks;
        std::condition_variable preTokenCondition;
        std::mutex tokenMutex;
        uint32_t prereqTokensFetched = 0;
        // if the token comp has any of D, U, or S, fetch them in parallel
        // if the token comp has T, wait for the D or S request(s) to finish before fetching the T
        // once D,U,T,S are fetched, fetch X
        auto tokenComp = state->AuthConfig->xtoken_composition();
        
        for (auto& tokenType : tokenComp)
        {
            switch(tokenType)
            {
                case token_identity_type::t_token:
                {
                    pplx::task<xbox_live_result<token_result>> tTokenTask = pplx::create_task(
                        [state, xTokenInfo, &tokenMutex, &preTokenCondition, &prereqTokenCount, &prereqTokensFetched, &ttoken]()
                        {
                            {
                                // Take the mutex that protects the preTokenFetched variable
                                std::unique_lock<std::mutex> localLock(tokenMutex);
                                // Wait on the condition to be true
                                preTokenCondition.wait(localLock, [&prereqTokensFetched, &prereqTokenCount](){return prereqTokenCount == prereqTokensFetched;});
                            }
                
                            // now that we are unblocked, we've got a D token, so we can fetch the T token
                            return get_token_from_cache_or_service(
                                        state,
                                        xTokenInfo->RelyingParty,
                                        xTokenInfo->SubRelyingParty,
                                        _T("JWT"),
                                        false,
                                        token_identity_type::t_token)
                            
                                .then([&ttoken](xbox_live_result<token_result> result)
                                      {
                                          // if the request was successful, grab the token
                                          if (!result.err())
                                          {
                                              ttoken = result.payload().token();
                                          }
                          
                                          return result;
                                      });
                        });
                    
                    tokenTasks.emplace_back(tTokenTask);
                }
                    break;
                case token_identity_type::d_token:
                {
                    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>> dTokenTask =
                        get_token_from_cache_or_service(
                            state,
                            xTokenInfo->RelyingParty,
                            xTokenInfo->SubRelyingParty,
                            _T("JWT"),
                            false,
                            token_identity_type::d_token)
                    
                    .then([&preTokenCondition, &prereqTokensFetched, &tokenMutex, &dtoken](xbox_live_result<token_result> result){
                        
                        // if the result was successful, grab the token
                        if (!result.err())
                        {
                            dtoken = result.payload().token();
                        }
                        
                        // acquire the lock that protects preTokenFetched, but only *after* setting the dtoken variable
                        std::lock_guard<std::mutex> guard(tokenMutex);
                        ++prereqTokensFetched;
                        
                        preTokenCondition.notify_all();
                        return result;
                    });
                    
                    tokenTasks.emplace_back(dTokenTask);
                }
                    break;
                case token_identity_type::u_token:
                {
                    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>> uTokenTask =
                        get_token_from_cache_or_service(
                            state,
                            xTokenInfo->RelyingParty,
                            xTokenInfo->SubRelyingParty,
                            _T("JWT"),
                            false,
                            token_identity_type::u_token)
                    
                    .then([&utoken](xbox_live_result<token_result> result)
                          {
                      
                              // if the result was successful, grab the token
                              if (!result.err())
                              {
                                  utoken = result.payload().token();
                              }
                      
                              return result;
                          });
            
                    tokenTasks.emplace_back(uTokenTask);
                }
                    break;
                case token_identity_type::s_token:
                {
                    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>> sTokenTask =
                        get_token_from_cache_or_service(
                            state,
                            xTokenInfo->RelyingParty,
                            xTokenInfo->SubRelyingParty,
                            _T("JWT"),
                            false,
                            token_identity_type::s_token)
                    
                    .then([&preTokenCondition, &prereqTokensFetched, &tokenMutex, &stoken](xbox_live_result<token_result> result){
                        
                        // if the result was successful, grab the token
                        if (!result.err())
                        {
                            stoken = result.payload().token();
                        }
                        
                        // acquire the lock that protects preTokenFetched, but only *after* setting the stoken variable
                        std::lock_guard<std::mutex> guard(tokenMutex);
                        ++prereqTokensFetched;
                        
                        preTokenCondition.notify_all();
                        return result;
                    });
            
                    tokenTasks.emplace_back(sTokenTask);
                }
                    break;
                default:
                    break;
            }
        }
        
        // this call blocks until all of the outstanding token requests are complete
        // this blocking call is only safe because we're already in a task
        std::vector<xbox_live_result<token_result>> results = pplx::when_all(tokenTasks.begin(), tokenTasks.end()).get();
        
        // success cases are handled in the continuations for the individual tasks.  We need to check for any failures here
        for (auto& result : results)
        {
            // just return the first error
            if (result.err())
            {
                xTokenInfo->RefreshEvent.set(result);
                xTokenInfo->IsRefreshInProgress = false;
                return result;
            }
        }

        // now that we have all of the D,U,T,S tokens that were requested, get an X token that encapsulates them all
        auto xtokenResult = xbox_system_factory::get_factory()->create_xsts_token()->get_x_token_from_service(
            state->ProofKey,
            dtoken,
            ttoken,
            utoken,
            stoken,
            xTokenInfo->RelyingParty,
            xTokenInfo->TokenType,
            state->AuthConfig,
            state->XboxLiveContextSettings,
            string_t()
            ).get();
        xTokenInfo->set_token_result(xtokenResult.payload());
        return xtokenResult;
    });
}

pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>> 
token_manager::refresh_t_token(
    _In_ std::shared_ptr<token_state> state,
    _In_ std::shared_ptr<token_info> tokenInfo
    )
{
    // Pull the D token from the cache so we can use it to fetch the T token
    auto dTokenInfo = get_token_from_cache(
        state, 
        tokenInfo->RelyingParty,
        tokenInfo->SubRelyingParty,
        _T("JWT"),
        token_identity_type::d_token);

    string_t rpsTicket;
    {
        std::lock_guard<std::mutex> lock(state->DataLock);
        rpsTicket = state->RpsTicket;
    }

    return xbox_system_factory::get_factory()->create_title_token()->get_t_token_from_service(
        rpsTicket,
        state->ProofKey,
        state->AuthConfig,
        state->XboxLiveContextSettings,
        dTokenInfo->Token.token()
        )
    .then([tokenInfo](XBOX_LIVE_NAMESPACE::xbox_live_result<token_result> xblResult)
    {
        tokenInfo->set_token_result(xblResult.payload());
        return xblResult;
    });
}

pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>>
token_manager::refresh_d_token(
    _In_ std::shared_ptr<token_state> state,
    _In_ std::shared_ptr<token_info> tokenInfo
    )
{
    string_t rpsTicket;
    {
        std::lock_guard<std::mutex> lock(state->DataLock);
        rpsTicket = state->RpsTicket;
    }

    return xbox_system_factory::get_factory()->create_device_token()->get_d_token_from_service(
        rpsTicket,
        state->ProofKey,
        state->AuthConfig,
        state->XboxLiveContextSettings
        )
        .then([tokenInfo](XBOX_LIVE_NAMESPACE::xbox_live_result<token_result> xblResult)
    {
        tokenInfo->set_token_result(xblResult.payload());
        return xblResult;
    });
}

pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>> 
token_manager::refresh_u_token(
    _In_ std::shared_ptr<token_state> state,
    _In_ std::shared_ptr<token_info> tokenInfo
    )
{
    string_t rpsTicket;
    {
        std::lock_guard<std::mutex> lock(state->DataLock);
        rpsTicket = state->RpsTicket;
    }

    return xbox_system_factory::get_factory()->create_user_token()->get_u_token_from_service(
        rpsTicket,
        state->ProofKey,
        state->AuthConfig,
        state->XboxLiveContextSettings)
        .then([tokenInfo](XBOX_LIVE_NAMESPACE::xbox_live_result<token_result> xblResult)
    {
        tokenInfo->set_token_result(xblResult.payload());
        return xblResult;
    });
}

#if XSAPI_SERVER
pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>>
token_manager::refresh_s_token(
    _In_ std::shared_ptr<token_state> state,
    _In_ std::shared_ptr<token_info> tokenInfo
    )
{
    return xbox_system_factory::get_factory()->create_service_token()->get_s_token_from_service(
        state->ProofKey,
        state->AuthConfig,
        state->XboxLiveContextSettings)
        .then([tokenInfo](XBOX_LIVE_NAMESPACE::xbox_live_result<token_result> xblResult)
    {
        tokenInfo->set_token_result(xblResult.payload());
        return xblResult;
    });
}
#endif

void
token_manager::set_rps_ticket(const string_t& rpsTicket)
{
    std::lock_guard<std::mutex> lock(m_state->DataLock);
    m_state->RpsTicket = rpsTicket;
    m_ticket = rpsTicket;
}

void
token_manager::clear_token_cache()
{
    std::lock_guard<std::mutex> lock(m_state->DataLock);
    m_state->Cache.clear();
}

const string_t&
token_manager::get_rps_ticket()
{
    return m_ticket;
}

string_t
token_manager::get_event_token_from_xuid(const string_t& xuid)
{
    const auto& iter = m_xuidTokenMap.find(xuid);
    if (iter != m_xuidTokenMap.end())
    {
        return iter->second;
    }
    else
    {
        return string_t();
    }
}

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif
