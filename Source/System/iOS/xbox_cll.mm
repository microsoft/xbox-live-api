#include "pch.h"
#include "xsapi/system.h"
#include "iOSCll.h"
#include "TicketObject.h"
#include "user_auth_ios.h"
#include "xbox_system_factory.h"
#include "xbox_cll.h"

using namespace xbox::services::system;

string_t msaDeviceTicketCallback(bool forceRefresh)
{
    pplx::task_completion_event<string_t> vortexTicketEvent = pplx::task_completion_event<string_t>();
    auto task = create_task(vortexTicketEvent);
    auto user = user_auth_ios::get_instance();
    if(user == nullptr)
    {
        return string_t();
    }
    XBLMSASignInDelagate *signInDelegate = user->get_sign_in_delegate();
    
    if (!signInDelegate)
    {
        return string_t();
    }
    MSASecurityScope* scope = [[MSASecurityScope alloc] initWithTarget:@"vortex.data.microsoft.com"
                                                                policy:@"mbi_ssl"];
    [signInDelegate getRPSTicketWithScope:scope withCompletion:^(NSString *ticket, NSError *error)
    {
         if (ticket)
         {
             vortexTicketEvent.set([ticket UTF8String]);
         }
         else
         {
             vortexTicketEvent.set(string_t());
         }
    }];
    try
    {
        return task.get();
    }
    catch (const pplx::task_canceled&)
    {
        return string_t();
    };
}

string_t authXTokenCallback(bool forceRefresh)
{
    string_t xToken;
    try
    {
        if(xbox_live_user::get_last_signed_in_user() != nullptr && !xbox_live_user::get_last_signed_in_user()->is_signed_in())
        {
            return string_t();
        }
        if(forceRefresh)
        {
            auth_manager::get_auth_manager_instance()->initialize_default_nsal().get();
        }
        xToken = auth_manager::get_auth_manager_instance()->internal_get_token_and_signature(
            _T("GET"),
            _T("https://test.vortex.data.microsoft.com"),
            _T("https://test.vortex.data.microsoft.com"),
            string_t(),
            std::vector<uint8_t>(),
            false,
            forceRefresh).then([](xbox::services::xbox_live_result<token_and_signature_result> result)
            {
                if (!result.err())
                {
                    token_and_signature_result res = result.payload();
                    return res.token();
                }
                return string_t();
            }).get();
    }
    catch (const pplx::task_canceled&)
    {
    }
    
    return xToken;
}

TicketObject xTicketForIdCallback(std::string id)
{
    string_t xToken = xbox::services::system::xbox_system_factory::get_factory()->create_local_config()->get_value_from_local_storage(id);
    TicketObject ticketObject;
    ticketObject.ticket = "x:" + xToken;
    ticketObject.hasDeviceClaims = false;
    return ticketObject;
}

using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

std::shared_ptr<xbox_cll> xbox_cll::s_xboxCll;

std::shared_ptr<xbox_cll> xbox_cll::get_xbox_cll_singleton()
{
    if (s_xboxCll != nullptr)
    {
        return s_xboxCll;
    }
    s_xboxCll = std::make_shared<xbox_cll>();
    return s_xboxCll;
}

xbox_cll::xbox_cll()
{
    string_t iKey = system::xbox_system_factory::get_factory()->create_local_config()->get_value_from_config("iKey", false, string_t());
    if (iKey.empty())
    {
        stringstream_t defaultIKey;
        defaultIKey << _T("P-XBL-T") << xbox_live_app_config::get_app_config_singleton()->title_id();
        iKey = defaultIKey.str();
    }
    m_cll = std::shared_ptr<iOSCll>(new iOSCll(iKey));
    TicketCallbacks callback(&msaDeviceTicketCallback, &authXTokenCallback, &xTicketForIdCallback);
    m_cll->setTicketCallback(callback);
    m_cll->setDebugVerbosity(cll::ILogger::LevelNone);
    m_cll->start();
}

xbox_cll::~xbox_cll()
{
}

std::shared_ptr<iOSCll>
xbox_cll::raw_cll()
{
    return m_cll;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
