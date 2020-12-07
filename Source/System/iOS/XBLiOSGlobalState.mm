// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#import "XBLiOSGlobalState.h"

__weak static UIViewController *gLaunchViewController;

@implementation XBLiOSGlobalState

+ (void)setLaunchViewController:(UIViewController *)viewController {
    gLaunchViewController = viewController;
}

+ (UIViewController *)launchViewController {
    return nullptr;
    // TODO 1808 return gLaunchViewController ?: [XBLIDPScenario getTopViewController];
}

@end
