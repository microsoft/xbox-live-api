//
//  XBLSignInHandler.h
//  XboxLiveServices
//
//  Created by Samuel Walker on 3/1/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "xsapi/services.h"

namespace xbox { namespace services { namespace system {
    class user_auth_ios;
}}}

typedef xbox::services::xbox_live_result<xbox::services::system::sign_in_result> XBLSignInResult;
typedef pplx::task<XBLSignInResult> (^XBLUserSignInBlock)(void);
typedef void (^XBLGamertagChangeBlock)(string_t);

// Note: These are the only errors that signing in can result in
typedef NS_ENUM(NSInteger, XBLSignInError) {
    XBLSignInErrorNone,
    XBLSignInErrorUnknown,
    XBLSignInErrorCreation,
    XBLSignInErrorBan,
    XBLSignInErrorOffline
};

enum class XBLUISignInResult
{
    Cancel,
    SwitchAccounts,
    Success,
    RetrySignIn,
    RetryAccountCreation
};

@interface XBLSignInHandler : NSObject

@property pplx::task_completion_event<xbox::services::xbox_live_result<XBLUISignInResult>> tce;
@property (nonatomic, weak, readonly) UIViewController *topViewController;
@property (nonatomic, copy) XBLGamertagChangeBlock gamertagChangeBlock;

- (void)setUser:(std::shared_ptr<xbox::services::system::user_auth_ios>)user;
- (void)launchLoadingScreen;
- (void)signedInWithNewAccount:(BOOL)isNewAccount
                     error:(XBLSignInError)error;

- (BOOL)isConnectedToInternet;
- (void)dismissUIWithCompletion:(void (^)(void))completion;

@end
