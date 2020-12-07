// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "xsapi-cpp/system.h"
#include "xsapi-cpp/title_callable_ui.h"
#import "XBLServiceManager.h"
#import <XSAPITCUI/XSAPITCUI.h>
#import <XSAPITCUI/XBLService.h>

static NSString *const XboxAppStoreLink = @"itms-apps://itunes.apple.com/app/xbox-one-smartglass/id736179781?mt=8";
using namespace pplx;
using namespace xbox::services;
using namespace xbox::services::system;

pplx::task<xbox_live_result<void>>
title_callable_ui::show_profile_card_ui(
                     _In_ const string_t& targetXboxUserId,
                     _In_ xbox_live_user_t user
                     )
{
    auto completionEvent = pplx::task_completion_event<xbox_live_result<void>>();
    
    XBLServiceManager *manager = [[XBLServiceManager alloc] init];
    [manager setUser:user];
    [XBLService setUpWithServiceManager:manager];
    [TCUIManager displayProfileCard:manager.userXuid youXuid:[NSString stringWithUTF8String:targetXboxUserId.c_str()] canAddFriend: manager.privilegeForAddFriend completionBlock:^{
        completionEvent.set(xbox_live_result<void>());
    }];
    
    return create_task(completionEvent);
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_user_profile_ui(_In_ const string_t& targetXboxUserId)
{
    auto completionEvent = pplx::task_completion_event<xbox_live_result<void>>();
    auto task = create_task(completionEvent);
    /* TODO 1808
    XLSCll *telemetry = [XLSCll sharedTelemetryManager];
    
    NSString *deeplink = [NSString stringWithFormat:@"smartglass://profile?xuid=%@&deepLinkId=%@&deepLinkCaller=%@", [NSString stringWithUTF8String:targetXboxUserId.c_str()], [telemetry getAppSessionId], [telemetry getAppBundleName]];

    if ([[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:deeplink]])
    {
        [telemetry pageActionEvent:IDP_PageAction_DeepLink_UserProfile withData:@{@"deepLinkId" : [telemetry getAppSessionId],
                                                                                  @"deepLinkCaller" : [telemetry getAppBundleName],
                                                                                  @"targetXUID" : [NSString stringWithFormat:@"x:%@",[NSString stringWithUTF8String:targetXboxUserId.c_str()]]}];
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:deeplink]];
    }
    else
    {
        [telemetry pageActionEvent:IDP_PageAction_DeepLink_SendToStore withData:@{@"deepLinkId" : [telemetry getAppSessionId],
                                                                                  @"deepLinkCaller" : [telemetry getAppBundleName],
                                                                                  @"intendedAction" : IDP_PageAction_DeepLink_UserProfile}];
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:XboxAppStoreLink]];
    }
     */
    completionEvent.set(xbox_live_result<void>());
    return task;
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_title_hub_ui()
{
    auto completionEvent = pplx::task_completion_event<xbox_live_result<void>>();
    auto task = create_task(completionEvent);
    /* TODO 1808
    auto titleId = xbox_live_app_config::get_app_config_singleton()->title_id();
    XLSCll *telemetry = [XLSCll sharedTelemetryManager];
    
    NSString *deeplink = [NSString stringWithFormat:@"smartglass://game?titleid=%u&deepLinkId=%@&deepLinkCaller=%@", titleId, [telemetry getAppSessionId], [telemetry getAppBundleName]];
    
    if ([[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:deeplink]])
    {
        [telemetry pageActionEvent:IDP_PageAction_DeepLink_TitleHub withData:@{@"deepLinkId" : [telemetry getAppSessionId],
                                                                               @"deepLinkCaller" : [telemetry getAppBundleName],
                                                                               @"targetTitleId" : [NSNumber numberWithInt:titleId]}];
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:deeplink]];

    }
    else
    {
        [telemetry pageActionEvent:IDP_PageAction_DeepLink_SendToStore withData:@{@"deepLinkId" : [telemetry getAppSessionId],
                                                                                  @"deepLinkCaller" : [telemetry getAppBundleName],
                                                                                  @"intendedAction" : IDP_PageAction_DeepLink_TitleHub}];
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:XboxAppStoreLink]];
    }
    
     */
    completionEvent.set(xbox_live_result<void>());
    return task;
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_title_achievements_ui(_In_ uint32_t titleId)
{
    auto completionEvent = pplx::task_completion_event<xbox_live_result<void>>();
    auto task = create_task(completionEvent);
    /* TODO 1808
    XLSCll *telemetry = [XLSCll sharedTelemetryManager];

    NSString *deeplink = [NSString stringWithFormat:@"smartglass://achievement?titleid=%u&deepLinkId=%@&deepLinkCaller=%@", titleId, [telemetry getAppSessionId], [telemetry getAppBundleName]];
    
    if ([[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:deeplink]])
    {
        [telemetry pageActionEvent:IDP_PageAction_DeepLink_TitleAchievements withData:@{@"deepLinkId" : [telemetry getAppSessionId],
                                                                               @"deepLinkCaller" : [telemetry getAppBundleName],
                                                                               @"targetTitleId" : [NSNumber numberWithInt:titleId]}];
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:deeplink]];
    }
    else
    {
        [telemetry pageActionEvent:IDP_PageAction_DeepLink_SendToStore withData:@{@"deepLinkId" : [telemetry getAppSessionId],
                                                                                  @"deepLinkCaller" : [telemetry getAppBundleName],
                                                                                  @"intendedAction" : IDP_PageAction_DeepLink_TitleAchievements}];
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:XboxAppStoreLink]];
    }
    
     */
    completionEvent.set(xbox_live_result<void>());
    return task;
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_user_settings_ui()
{
    auto completionEvent = pplx::task_completion_event<xbox_live_result<void>>();
    auto task = create_task(completionEvent);
     /* TODO 1808
    XLSCll *telemetry = [XLSCll sharedTelemetryManager];

    NSString *deeplink = [NSString stringWithFormat:@"smartglass://settings?deepLinkId=%@&deepLinkCaller=%@", [telemetry getAppSessionId], [telemetry getAppBundleName]];
     
    if ([[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:deeplink]])
    {
        [telemetry pageActionEvent:IDP_PageAction_DeepLink_UserSettings withData:@{@"deepLinkId" : [telemetry getAppSessionId],
                                                                                    @"deepLinkCaller" : [telemetry getAppBundleName]}];
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:deeplink]];
    }
    else
    {
        [telemetry pageActionEvent:IDP_PageAction_DeepLink_SendToStore withData:@{@"deepLinkId" : [telemetry getAppSessionId],
                                                                                  @"deepLinkCaller" : [telemetry getAppBundleName],
                                                                                  @"intendedAction" : IDP_PageAction_DeepLink_UserSettings}];
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:XboxAppStoreLink]];
    }
    
      */
    completionEvent.set(xbox_live_result<void>());
    return task;
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_add_friends_ui()
{
     return pplx::task_from_result<xbox::services::xbox_live_result<void>>(xbox::services::xbox_live_result<void>(xbox_live_error_code::unsupported));
}

pplx::task<xbox::services::xbox_live_result<std::vector<string_t>>>
title_callable_ui::show_player_picker_ui(
    _In_ const string_t& promptDisplayText,
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const std::vector<string_t>& preselectedXboxUserIds,
    _In_ uint32_t minSelectionCount,
    _In_ uint32_t maxSelectionCount
    )
{
    return pplx::task_from_result<xbox::services::xbox_live_result<std::vector<string_t>>>(xbox::services::xbox_live_result<std::vector<string_t>>(xbox_live_error_code::unsupported));
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_game_invite_ui(
    _In_ const xbox::services::multiplayer::multiplayer_session_reference& sessionReference,
    _In_ const string_t& invitationDisplayText,
    _In_ const string_t& contextStringId
    )
{
    return pplx::task_from_result<xbox::services::xbox_live_result<void>>(xbox::services::xbox_live_result<void>(xbox_live_error_code::unsupported));
}

pplx::task<xbox::services::xbox_live_result<void>>
title_callable_ui::show_change_friend_relationship_ui(
    _In_ const string_t& targetXboxUserId
    )
{
    return pplx::task_from_result<xbox::services::xbox_live_result<void>>(xbox::services::xbox_live_result<void>(xbox_live_error_code::unsupported));
}
