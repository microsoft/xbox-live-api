//
//  XboxProviderBridge.h
//  XboxLiveServices
//
//  Created by Samuel Walker on 3/1/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#pragma once

#include "xsapi/profile.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

enum class XboxProviderRequestType
{
    XboxProfile,
    MicrosoftProfile,
    AccountTroubleShooter,
    CheckGamertag,
    ClaimGamertag,
    GamertagSuggestions,
    RealNameSettings,
};

/**
 This class is used to bridge between the XSAPI http stack and iOS. All calls are not pplx::tasks because
 they are meant to be called from background threads launched by the caller of the XboxProvider methods.
 */
class XboxProviderBridge
{
public:
    XboxProviderBridge();
    
    string_t m_xuid;
    
    xbox::services::xbox_live_result<xbox_user_profile> get_xbox_profile();
    
    xbox_live_result<web::json::value> get_microsoft_profile(
                                                             _In_ bool isNewAccount
                                                             );
    xbox_live_result<web::json::value> post_microsoft_profile(
                                                              bool isNewAccount,
                                                              _In_ web::json::value body
                                                              );
    xbox_live_result<web::json::value> set_up_microsoft_profile(
                                                                _In_ string_t locale
                                                                );
    xbox::services::xbox_live_result<bool> check_gamertag(
                                                          _In_ string_t gamertag
                                                          );
    xbox::services::xbox_live_result<bool> claim_gamertag(
                                                          _In_ string_t gamertag
                                                          );
    xbox::services::xbox_live_result<std::vector<string_t>> get_gamertag_suggestions(
                                                                                     _In_ string_t seed,
                                                                                     _In_ string_t locale
                                                                                     );
    
    xbox::services::xbox_live_result<bool> set_up_real_name_settings();
    
private:
    string_t createSettingsQuery();
    web::json::value serializeSettingsToJSON();
    web::json::value serializedSettings;
    string_t settingsQuery;
    
    static std::mutex m_settingsLock;
    
    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox_live_app_config> m_appConfig;
    
    std::shared_ptr<http_call> xbox_provider_http_call(
                                                       _In_ XboxProviderRequestType type,
                                                       _In_ string_t httpMethod,
                                                       _In_ string_t query
                                                       );
    
    pplx::task<std::shared_ptr<http_call_response>> get_response(
                                                                 _In_ std::shared_ptr<http_call> httpCall,
                                                                 _In_ http_call_response_body_type bodyType
                                                                 );
    
    // Real name settings
    xbox::services::xbox_live_result<web::json::value> get_real_name_settings();
    xbox::services::xbox_live_result<bool> post_real_name_settings(
                                                                   _In_ web::json::value body
                                                                   );
    
    // Configure json helpers
    web::json::value configure_microsoft_profile_body(
                                                      _In_ web::json::value body,
                                                      _In_ string_t locale
                                                      );
    web::json::value configure_real_name_settings(
                                                  _In_ web::json::value body
                                                  );
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END

