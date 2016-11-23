//
//  XBLiOSGlobalState.h
//  XboxLiveServices
//
//  Created by Sam Walker on 5/3/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface XBLiOSGlobalState : NSObject

/**
 Set the launch view controller to be used by XSAPI. This is held by a weak pointer and is not thread safe to set this.
 Typically you only need to set this once. If this is nil when XSAPI UI is launched, the application root view controller
 will be used
 
 @param viewController The view controller or nil to launch XSAPI from. Default is application root view controller
 */
+ (void)setLaunchViewController:(UIViewController * _Nullable)viewController;

/**
 Returns the launch view controller used by XSAPI to launch UI from. This is either the view controller set previously
 in the +setLaunchViewController method or the root view controller of the application
 
 @return The launch view controller used by XSAPI
 */
+ (UIViewController * _Nullable)launchViewController;

/**
 Set the notification registration token to be used by XSAPI.
 
 @param registrationToken Used to register for XBL Notifications
 */
+ (void)setNotificationRegistrationToken:(NSString * _Nullable)registrationToken;

/**
 Returns the notification registration token to be used by XSAPI
 
 @return registrationToken Used to register for XBL Notifications
 */
+ (string_t)notificationRegistrationToken;
@end
