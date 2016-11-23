#include "pch.h"
#include "notification_helper.h"
#include "utils.h"


using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#if XSAPI_I

xbox_live_result<xbox_live_notification> parse_notification(NSDictionary* notificationInfo)
{
    NSError *error;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:notificationInfo
        options:0
        error:&error];
    
    if (error == nil)
    {
        return xbox_live_result<xbox_live_notification>(xbox_live_error_code::json_error, _T("Error parsing the notification payload!"));
    }
    else
    {
        xbox_live_notification result;
        string_t notificationStr([[[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding] UTF8String]);
        std::error_code errc;
        web::json::value payload = web::json::value::parse(notificationStr, errc);

        if(!errc)
        {
            string_t title;
            string_t body;
            
            web::json::value aps = utils::extract_json_field(payload, "aps", errc, true);
            web::json::value data = utils::extract_json_field(payload, "data", errc, true);
            
            string_t type = utils::extract_json_string(data, "type", errc, true);
            web::json::value xblInfo = utils::extract_json_field(data, "xbl", errc, true);
            
            web::json::value alert = utils::extract_json_field(aps, "alert", errc, true);
            
            if(utils::str_icmp(type, _T("xbox_live_game_invite")) == 0)
            {
                result.type = xbox_live_notification_type::game_invite;
                
                web::json::value locArgs= utils::extract_json_field(alert, "loc-args", errc, true);
                string_t locKey = utils::extract_json_string(alert, "loc-key", errc, true);
                string_t titleLocKey = utils::extract_json_string(alert, "title-loc-key", errc, true);
                
                web::json::array args = utils::extract_json_as_array(locArgs, errc);
                std::vector<string_t> bodyArgs;
                
                for(auto iter = args.cbegin(); iter != args.cend(); ++iter)
                {
                    bodyArgs.push_back(utils::extract_json_as_string(*iter, errc));
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
                        index = MIN(bodyArgs.size(), index + 1);
                    }
                }
                
                title = [titleLocStr UTF8String];
                body = ss.str();
            }
            else if(utils::str_icmp(type, _T("xbox_live_achievement_unlock")) == 0)
            {
                result.type = xbox_live_notification_type::achievement_unlocked;
                
                title = utils::extract_json_string(alert, "title", errc, true);
                body = utils::extract_json_string(alert, "body", errc, true);
            }
            else
            {
                result.type = xbox_live_notification_type::unknown;
            }
            
            result.title = title;
            result.body = body;
            result.data = xblInfo.serialize();
            
            return xbox_live_result<xbox_live_notification>(result, errc);
        }
        
        
        return xbox_live_result<xbox_live_notification>(result, errc);
    }
}
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
