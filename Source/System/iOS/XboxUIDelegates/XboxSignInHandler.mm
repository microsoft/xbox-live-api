//
//  XboxSignInHandler.m
//  XboxLiveServices
//
//  Created by Samuel Walker on 3/1/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#import "XboxSignInHandler.h"
#import "XboxProvider.h"

using namespace pplx;
using namespace xbox::services;
using namespace xbox::services::system;

@interface XboxSignInHandler() <XboxXSAPIManagerDelegate> {
    pplx::task_completion_event<xbox::services::xbox_live_result<sign_in_result>> _tce;
}

@property (nonatomic, assign) BOOL hasLaunched;
@property (nonatomic, assign) XboxState launchedState;
@property (nonatomic, strong) XboxXSAPIManager *manager;
@property (nonatomic, weak) id<XboxServiceProvider> provider;
@end

@implementation XboxSignInHandler

- (instancetype)initWithXboxProvider:(id<XboxServiceProvider>)xboxProvider withMicrosoftProvider:(id<MicrosoftServiceProvider>)microsoftProvider
{
    self = [super init];
    if (self) {
        _tce = pplx::task_completion_event<xbox_live_result<sign_in_result>>();
        self.manager = [[XboxXSAPIManager alloc] initWithMicrosoftServiceProvider:microsoftProvider xboxServiceProvider:xboxProvider];
    }
    return self;
}

- (pplx::task_completion_event<XBLUISignInResult>)tce {
    return _tce;
}

- (UIViewController *)topViewController {
    if (self.manager.loadingViewController) {
        return self.manager.loadingViewController;
    } else {
        return self.launchViewController;
    }
}

- (void)launchLoadingScreen {
    if (self.hasLaunched) {
        [self.manager dismissToLoadingScreen];
    } else {
        [self.manager launchState:XboxStateLoading fromViewController:self.launchViewController withDelegate:self];
    }
}

- (void)signedInWithResult:(xbox::services::xbox_live_result<xbox::services::system::sign_in_result>)result isNewAccount:(BOOL)isNewAccount error:(XboxError)error {
    auto status = result.payload().status();
    NSAssert(status == user_cancel || status == success, @"Invalid result passed to %s", __PRETTY_FUNCTION__);
    
    if (!result.err() && status == sign_in_status::user_cancel) {
        [self signOutWithStatus:sign_in_status::user_cancel];
    } else {
        BOOL launched = [self launchSignInForNewAccount:isNewAccount error:error];
        NSAssert(launched, @"There is reason for this to have failed. Something is drastically wrong.");
    }
}

- (BOOL)launchSignInForNewAccount:(BOOL)isNewAccount error:(XboxError)error {
    if (error != XboxErrorNone) {
        return [self launchXboxError:error];
    }
    
    if (isNewAccount) {
        // User has a new account. Show GamertagCreate
        self.launchedState = XboxStateSignUp;
        return [self.manager launchState:XboxStateSignUp fromViewController:self.launchViewController withDelegate:self];
    } else {
        self.launchedState = XboxStateWelcome;
        return [self.manager launchState:XboxStateWelcome fromViewController:self.launchViewController withDelegate:self];
    }
}

#pragma mark - XboxXSAPIManagerDelegate
- (void)userSignedIn {
    [self dismissUIWithSignInStatus:sign_in_status::success];
}

- (void)userCancelledSignIn {
    [self signOutWithStatus:sign_in_status::user_cancel];
}

- (void)switchAccounts {
    [self signOutWithStatus:sign_in_status::user_interaction_required];
}

- (void)userChangedGamertag:(NSString *)gamertag {
    NSAssert(self.gamertagChangeBlock, @"gamertagChangeBlock is required");
    self.gamertagChangeBlock(string_t([gamertag UTF8String]));
}

#pragma mark - Helpers
- (BOOL)launchXboxError:(XboxError)error {
    __weak typeof(self) weakSelf = self;
    return [self.manager launchState:XboxStateError fromViewController:self.launchViewController withDelegate:self forErrorState:error errorActionCompletionBlock:^{
        [weakSelf handleXboxError:error];
    }];
}

- (void)handleXboxError:(XboxError)error {
    NSAssert(self.errorHandlingBlock, @"errorHandlingBlock is required");
    [self.manager dismissToLoadingScreen];
    self.errorHandlingBlock(error);
}

- (void)signOutWithStatus:(sign_in_status)status {
    NSAssert(status != sign_in_status::success, @"Why are we signing out with a success?");
    
    __weak typeof(self) weakSelf = self;
    self.signOutBlock(^() {
        [weakSelf dismissUIWithSignInStatus:status];
    });
}

- (void)dismissUIWithSignInStatus:(sign_in_status)status {
    auto tce = [self tce];
    void (^completion)(void) = ^() {
        tce.set(xbox_live_result<sign_in_result>(sign_in_result(status)));
    };
    
    if ([NSThread isMainThread]) {
        [self.launchViewController dismissViewControllerAnimated:YES completion:completion];
    } else {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.launchViewController dismissViewControllerAnimated:YES completion:completion];
        });
    }
}

@end
