//
//  XboxSignInHandler.h
//  XboxLiveServices
//
//  Created by Samuel Walker on 3/1/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "XboxXSAPIManager.h"
#import "xsapi/services.h"
#import "user_impl.h"
#import "XboxServices.h"

typedef void (^XBLWorkBlock)(void);
typedef void (^XBLDependentBlock)(XBLWorkBlock);
typedef void (^XBLErrorHandleBlock)(XboxError);
typedef void (^XBLGamertagChangeBlock)(string_t);
typedef xbox::services::xbox_live_result<xbox::services::system::sign_in_result> XBLUISignInResult;

@interface XboxSignInHandler : NSObject

@property (nonatomic, weak) UIViewController *launchViewController;
@property (nonatomic, weak) UIViewController *topViewController;
@property (nonatomic, copy) XBLDependentBlock signOutBlock;
@property (nonatomic, copy) XBLErrorHandleBlock errorHandlingBlock;
@property (nonatomic, copy) XBLGamertagChangeBlock gamertagChangeBlock;

- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithXboxProvider:(id<XboxServiceProvider>)xboxProvider withMicrosoftProvider:(id<MicrosoftServiceProvider>)microsoftProvider NS_DESIGNATED_INITIALIZER;

- (pplx::task_completion_event<XBLUISignInResult>)tce;
- (void)launchLoadingScreen;
- (void)signedInWithResult:(XBLUISignInResult)result
              isNewAccount:(BOOL)isNewAccount
                     error:(XboxError)error;
@end
