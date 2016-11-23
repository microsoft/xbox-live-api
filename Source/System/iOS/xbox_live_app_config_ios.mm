//
//  xbox_live_app_config.m
//  XboxLiveServices
//
//  Created by Sam Walker on 5/3/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#include "pch.h"
#include "xsapi/system.h"
#import "XBLiOSGlobalState.h"


using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#if defined(XSAPI_I) && defined(__OBJC__)

void xbox_live_app_config::set_launch_view_controller(UIViewController *viewController)
{
    [XBLiOSGlobalState setLaunchViewController:viewController];
}

void xbox_live_app_config::set_notification_registration_token(_In_ NSString *registrationToken)
{
    [XBLiOSGlobalState setNotificationRegistrationToken:registrationToken];
}

#endif

const string_t& xbox_live_app_config::_Apns_Environment() const
{
    return m_apnsEnvironment;
}

string_t get_proxy_name()
{
    CFDictionaryRef dict = CFNetworkCopySystemProxySettings();
    const CFStringRef proxy = (const CFStringRef)CFDictionaryGetValue(dict, kCFNetworkProxiesHTTPProxy);

    CFRelease(dict);
    
    if (proxy != NULL)
    {
        return CFStringGetCStringPtr(proxy, kCFStringEncodingUTF8);
    }
    
    return string_t();
}

string_t get_proxy_port()
{
    CFDictionaryRef dict = CFNetworkCopySystemProxySettings();
    const CFNumberRef portNumber = (const CFNumberRef)CFDictionaryGetValue(dict, kCFNetworkProxiesHTTPPort);
    
    CFRelease(dict);
    
    if (portNumber != NULL)
    {
        int port = [(__bridge NSNumber *)portNumber intValue];
        return std::to_string(port);
    }
    
    return string_t();
}

string_t xbox_live_app_config::get_proxy_string()
{
    string_t proxyName = get_proxy_name();
    string_t proxy;
    if(!proxyName.empty())
    {
        string_t proxyPort = get_proxy_port();
        if(!proxyPort.empty())
        {
            proxy = "http://" + proxyName + ":" + proxyPort;
        }
    }
    return proxy;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END