// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
@end
