// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils.h"
#include "user_impl.h"
//#include "auth_manager.h"

#if WINAPI_FAMILY==WINAPI_FAMILY_APP
#if !XSAPI_CPP
#include "User_WinRT.h"
#endif
#endif

using namespace std;
using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

std::unordered_map<function_context, std::function<void(const sign_out_completed_event_args&)>> user_impl::s_signOutCompletedHandlers;
std::unordered_map<function_context, std::function<void(const string_t&)>> user_impl::s_signInCompletedHandlers;
function_context user_impl::s_signOutCompletedHandlerIndexer = 0;
function_context user_impl::s_signInCompletedHandlerIndexer = 0;
XBOX_LIVE_NAMESPACE::system::xbox_live_mutex user_impl::s_trackingUsersLock;


std::shared_ptr<user_impl>
user_factory::create_user_impl(user_creation_context userCreationContext)
{
	return XBOX_LIVE_NAMESPACE::system::xbox_system_factory::get_factory()->create_user_impl(userCreationContext);
}

user_impl::user_impl(
    _In_ bool initConfig, 
    _In_opt_ user_creation_context creationContext
    ) :
    m_isSignedIn(false),
    m_creationContext(creationContext)
{
#if XSAPI_U
    if (initConfig)
    {
        m_authConfig = auth_manager::get_auth_manager_instance()->get_auth_config();
    }
#else
    UNREFERENCED_PARAMETER(initConfig);
    m_localConfig = xbox_system_factory::get_factory()->create_local_config();

    m_authConfig = std::make_shared<auth_config>(
        m_localConfig->sandbox(),
        m_localConfig->environment_prefix(),
        m_localConfig->environment(),
        m_localConfig->use_first_party_token(),
        m_localConfig->is_creators_title()
        );
#endif
}

pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_and_signature_result>>
user_impl::get_token_and_signature(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers
    )
{
    return internal_get_token_and_signature(
        httpMethod, 
        url, 
        url,
        headers,
        std::vector<unsigned char>(),
        false,
        false
        );
}

pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_and_signature_result> >
user_impl::get_token_and_signature(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers,
    _In_ const string_t& requestBodyString
    )
{ 
    std::string utf8Body(utility::conversions::to_utf8string(requestBodyString));
    std::vector<unsigned char> utf8Vec(utf8Body.begin(), utf8Body.end());
    return internal_get_token_and_signature(
        httpMethod,
        url,
        url,
        headers,
        utf8Vec,
        false,
        false
        );
}

pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_and_signature_result> >
user_impl::get_token_and_signature_array(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers,
    _In_ const std::vector<unsigned char>& requestBodyArray
    )
{
    return internal_get_token_and_signature(
        httpMethod,
        url,
        url,
        headers,
        requestBodyArray, 
        false,
        false
        );
}

void
user_impl::user_signed_in(
    _In_ string_t xboxUserId,
    _In_ string_t gamertag,
    _In_ string_t ageGroup,
    _In_ string_t privileges,
    _In_ string_t webAccountId
    )
{
    std::unordered_map<function_context, std::function<void(const string_t&)>> signInCompletedHandlersCopy;
    {
        std::lock_guard<std::mutex> lock(m_lock.get());

        m_xboxUserId = std::move(xboxUserId);
        m_gamertag = std::move(gamertag);
        m_ageGroup = std::move(ageGroup);
        m_privileges = std::move(privileges);
        m_webAccountId = std::move(webAccountId);

        m_isSignedIn = true;
        signInCompletedHandlersCopy = s_signInCompletedHandlers;
    }

    for (auto& handler : signInCompletedHandlersCopy)
    {
        if (handler.second != nullptr)
        {
            try
            {
                handler.second(m_xboxUserId);
            }
            catch (...)
            {
            }
        }
    }
}

function_context
user_impl::add_sign_in_completed_handler(
    _In_ std::function<void(const string_t&)> handler
    )
{
    std::lock_guard<std::mutex> lock(s_trackingUsersLock.get());

    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++s_signInCompletedHandlerIndexer;
        s_signInCompletedHandlers[s_signInCompletedHandlerIndexer] = std::move(handler);
    }

    return context;
}

void
user_impl::remove_sign_in_completed_handler(
    _In_ function_context context
    )
{
    std::lock_guard<std::mutex> lock(s_trackingUsersLock.get());

    s_signInCompletedHandlers.erase(context);
}

function_context
user_impl::add_sign_out_completed_handler(_In_ std::function<void(const sign_out_completed_event_args&)> handler)
{
    std::lock_guard<std::mutex> lock(s_trackingUsersLock.get());

    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++s_signOutCompletedHandlerIndexer;
        s_signOutCompletedHandlers[s_signOutCompletedHandlerIndexer] = std::move(handler);
    }

    return context;
}

void
user_impl::remove_sign_out_completed_handler(
    _In_ function_context context
    )
{
    std::lock_guard<std::mutex> lock(s_trackingUsersLock.get());

    s_signOutCompletedHandlers.erase(context);
}

void user_impl::user_signed_out()
{
    bool isSignedIn;
    std::unordered_map<function_context, std::function<void(const sign_out_completed_event_args&)>> signOutHandlers;
    {
        std::lock_guard<std::mutex> lock(m_lock.get());
        isSignedIn = m_isSignedIn;
        m_isSignedIn = false;
        signOutHandlers = s_signOutCompletedHandlers;
    }

    if (isSignedIn)
    {
        // trigger user sign out event
        for (auto& handler : signOutHandlers)
        {
            XSAPI_ASSERT(handler.second != nullptr);
            if (handler.second != nullptr)
            {
                try
                {
                    handler.second(sign_out_completed_event_args(m_weakUserPtr, shared_from_this()));
                }
                catch (...)
                {
                    LOG_ERROR("Calling signOutCompletedHandler failed.");
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(m_lock.get());
            // Check again on isSignedIn flag, in case users signed in again in signOutHandlers callback,
            // so we don't clean up the properties. 
            if (!isSignedIn)
            {
                m_xboxUserId.clear();
                m_gamertag.clear();
                m_ageGroup.clear();
                m_privileges.clear();
                m_webAccountId.clear();
            }
        }
    }
}

bool user_impl::is_signed_in()
{
    std::lock_guard<std::mutex> lock(m_lock.get());
    return m_isSignedIn;
}

void user_impl::set_user_pointer(_In_ std::shared_ptr<system::xbox_live_user> user)
{
    m_weakUserPtr = user;
}

void user_impl::set_title_telemetry_session_id(_In_ const string_t& sessionId)
{
    m_titleTelemetrySessionId = sessionId;
}

const string_t& user_impl::title_telemetry_session_id()
{
    return m_titleTelemetrySessionId;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
