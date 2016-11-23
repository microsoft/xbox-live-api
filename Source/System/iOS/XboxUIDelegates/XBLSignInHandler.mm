//
//  XBLSignInHandler.m
//  XboxLiveServices
//
//  Created by Samuel Walker on 3/1/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#import "XBLSignInHandler.h"
#import "XboxProvider.h"
#import "sign_in_delegate_ios.h"
#import <XSAPITCUI/XSAPITCUI.h>
#import "XBLiOSGlobalState.h"
#import "XLSCll.h"
#import "UTCIDPNames.h"
#import "user_auth_ios.h"

@interface XBLSignInHandler() <XBLIDPScenarioDelegate>

@property (nonatomic, weak, readwrite) UIViewController *topViewController;
@property (nonatomic, assign) BOOL hasLaunched;
@property (nonatomic, assign) XBLIDPState launchedState;
@property (nonatomic, assign) XBLIDPError launchedError;
@property (nonatomic, strong) XboxProvider *provider;
/** 
 Strong pointer so as to retain the view controller until we are done with it
 */
@property (nonatomic, strong) UIViewController *launchViewController;

@end

@implementation XBLSignInHandler

- (instancetype)init
{
    self = [super init];
    if (self) {
        self.provider = [[XboxProvider alloc] init];
        self.launchViewController = [XBLiOSGlobalState launchViewController];
        [XBLIDPScenario setUpWithInitialViewController:self.launchViewController
                              microsoftServiceProvider:self.provider
                                   xboxServiceProvider:self.provider
                                              delegate:self];
    }
    
    return self;
}

- (void)dealloc {
    [XBLIDPScenario tearDown];
}

- (void)setUser:(std::shared_ptr<xbox::services::system::user_auth_ios>)user {
    [self.provider setUser:user];
}

- (void)signedInWithNewAccount:(BOOL)isNewAccount error:(XBLSignInError)error {
    if (error != XBLSignInErrorNone) {
        [self launchXboxError:error];
        return;
    }
    
    if (isNewAccount) {
        // User has a new account. Show GamertagCreate
        self.launchedState = XBLIDPStateSignUp;
        [XBLIDPScenario launchState:XBLIDPStateSignUp];
    } else {
        self.launchedState = XBLIDPStateWelcome;
        [XBLIDPScenario launchState:XBLIDPStateWelcome];
    }
}

- (void)launchLoadingScreen {
    [XBLIDPScenario launchState:XBLIDPStateLoading];
}

- (BOOL)isConnectedToInternet {
    return [[XBLIDPScenario sharedIDPScenario].reachability isNetworkConnected];
}

- (void)dismissUIWithCompletion:(void (^)())completion {
    if ([NSThread isMainThread]) {
        [self.launchViewController dismissViewControllerAnimated:YES completion:completion];
    } else {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.launchViewController dismissViewControllerAnimated:YES completion:completion];
        });
    }
}

#pragma mark - XBLIDPStateManagerDelegate
- (void)userCompletedSignedIn {
    [self setTCEWithResult:XBLUISignInResult::Success];
}

- (void)userCancelledSignIn {
    [self setTCEWithResult:XBLUISignInResult::Cancel];
}

- (void)switchAccounts {
    [self.provider reset];
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^() {
        [XBLIDPScenario launchState:XBLIDPStateLoading];
        [self tce].set(XBLUISignInResult::SwitchAccounts);
    });
}

- (void)userChangedGamertag:(NSString *)gamertag {
    NSAssert(self.gamertagChangeBlock, @"gamertagChangeBlock is required");
    self.gamertagChangeBlock(string_t([gamertag UTF8String]));
}

#pragma mark - Helpers
- (UIViewController *)topViewController {
    return [[XBLIDPScenario sharedIDPScenario] baseViewController] ?: [self launchViewController];
}

- (void)launchXboxError:(XBLSignInError)error {
    // Direct mapping
    XBLIDPError launchError = (XBLIDPError)error;
    self.launchedError = launchError;
    
    [XBLIDPScenario launchError:launchError];
}

- (void)attemptErrorSolution {
    [XBLIDPScenario launchState:XBLIDPStateLoading];

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^() {
        
        switch (self.launchedError) {
            case XBLIDPErrorUnknown:
            case XBLIDPErrorOffline:
                [[XLSCll sharedTelemetryManager] pageActionEvent:IDP_PageAction_Errors_RetrySignIn];
                [self tce].set(XBLUISignInResult::RetrySignIn);
                break;
            case XBLIDPErrorCreation:
                [[XLSCll sharedTelemetryManager] pageActionEvent:IDP_PageAction_Errors_RetryAccountCreation];
                [self tce].set(XBLUISignInResult::RetryAccountCreation);
                break;
            default:
                NSAssert(false, @"We only handle these three errors");
                [[XLSCll sharedTelemetryManager] pageActionEvent:IDP_PageAction_Errors_SignOut];
                [self tce].set(XBLUISignInResult::Cancel);
                break;
        }
    });
}

- (void)setTCEWithResult:(XBLUISignInResult)result {
    [self tce].set(xbox::services::xbox_live_result<XBLUISignInResult>(result));
}

@end
