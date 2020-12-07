// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "notification_helper.h"

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#if XSAPI_I

xbox_live_result<xbox_live_notification> parse_notification(NSDictionary* notificationInfo)
{
    NSError *error;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:notificationInfo
        options:0
        error:&error];
    
    if (error != nil)
    {
        string_t errorDescription = string_t([[NSString stringWithFormat:@"Error parsing the notification payload! Error: %@", [error localizedDescription]] UTF8String]);
        return xbox_live_result<xbox_live_notification>(xbox_live_error_code::json_error, errorDescription);
    }
    else
    {
        xbox_live_notification result;
        string_t notificationStr([[[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding] UTF8String]);
        JsonDocument payload;
        payload.Parse(utils::internal_string_from_string_t(notificationStr).c_str());

        if(!payload.HasParseError())
        {
            HRESULT errc = S_OK;
            string_t title;
            string_t body;
            
            if(payload.IsObject() && payload.HasMember("aps") && payload.HasMember("data"))
            {
                const JsonValue& aps = payload["aps"]; //required
                const JsonValue& data = payload["data"]; //required
                
                string_t type;
                errc |= JsonUtils::ExtractJsonString(data, "type", type, true);
                
                if(aps.IsObject() && aps.HasMember("alert"))
                {
                    const JsonValue& alert = aps["alert"]; //required
                
                    if(utils::str_icmp(type, _T("xbox_live_game_invite")) == 0)
                    {
                        result.type = xbox_live_notification_type::game_invite;
                        
                        string_t locKey;
                        errc |= JsonUtils::ExtractJsonString(alert, "loc-key", locKey, true);
                        string_t titleLocKey;
                        errc |= JsonUtils::ExtractJsonString(alert, "title-loc-key", titleLocKey, true);
                        std::vector<string_t> bodyArgs;
                        
                        if(alert.IsObject() && alert.HasMember("loc-args"))
                        {
                            const JsonValue& locArgs= alert["loc-args"]; //requried
                            
                            for(const JsonValue& arg : locArgs.GetArray())
                            {
                                xsapi_internal_string argString;
                                errc |= JsonUtils::ExtractJsonAsString(arg, argString);
                                bodyArgs.push_back(utils::string_t_from_internal_string(argString));
                            }
                            
                            NSString* titleLocStr = NSLocalizedString([NSString stringWithUTF8String:titleLocKey.c_str()], nil);
                            NSString* bodyLocStr = NSLocalizedString([NSString stringWithUTF8String:locKey.c_str()], nil);

                            //The below snippet formats the localization string one parameter at a time.
                            stringstream_t ss;
                            NSArray *items = [bodyLocStr componentsSeparatedByString:@"%"];
                            uint32_t index = 0;
                            for (NSString* item in items)
                            {
                                if(item.length > 0)
                                {
                                    NSString *format = @"%";
                                    format = [format stringByAppendingString:item];
                                    ss << [[NSString stringWithFormat:format, [NSString stringWithUTF8String:bodyArgs[index].c_str()]] UTF8String];
                                    index = MIN((uint32_t)bodyArgs.size(), index + 1);
                                }
                            }
                            
                            title = [titleLocStr UTF8String];
                            body = ss.str();
                        }
                    }
                    else if(utils::str_icmp(type, _T("xbox_live_achievement_unlock")) == 0)
                    {
                        result.type = xbox_live_notification_type::achievement_unlocked;
                        errc |= JsonUtils::ExtractJsonString(alert, "title", title, true);
                        errc |= JsonUtils::ExtractJsonString(alert, "body", body, true);
                    }
                    else
                    {
                        result.type = xbox_live_notification_type::unknown;
                    }
                }
                
                if(data.IsObject() && data.HasMember("xbl"))
                {
                    const JsonValue& xblInfo = data["xbl"]; //required
                    
                    result.title = title;
                    result.body = body;
                    result.data = JsonUtils::SerializeJson(xblInfo);
                    
                    return xbox_live_result<xbox_live_notification>(result, ConvertHrToXblErrorCode(errc));
                }
            }
        }

        return xbox_live_result<xbox_live_notification>(result, xbox_live_error_code::json_error);
    }
}
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
