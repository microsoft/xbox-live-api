//
//  XBLiOSGlobalState.m
//  XboxLiveServices
//
//  Created by Sam Walker on 5/3/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#import "XBLiOSGlobalState.h"

__weak static UIViewController *gLaunchViewController;
string_t gNotificationRegistrationToken;

@implementation XBLiOSGlobalState

+ (void)setLaunchViewController:(UIViewController *)viewController {
    gLaunchViewController = viewController;
}

+ (UIViewController *)launchViewController {
    return gLaunchViewController ?: [UIApplication sharedApplication].keyWindow.rootViewController;
}

+ (void)setNotificationRegistrationToken:(NSString * _Nullable)registrationToken{
    gNotificationRegistrationToken = [registrationToken UTF8String];
}

+ (string_t)notificationRegistrationToken{
    return gNotificationRegistrationToken;
}

@end
