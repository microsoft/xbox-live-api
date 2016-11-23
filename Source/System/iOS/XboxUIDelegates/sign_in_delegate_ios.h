//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#import "user_impl.h"

// A ticket if success, nil otherwise
typedef void (^RPSTicketRequestBlock)(NSString *ticket, NSError *error);

@class UIViewController;
@class MSASecurityScope;

/**
 Handles all of authentication and sign in with MSA including displaying UI when needed.
 */
@interface XBLMSASignInDelagate : NSObject

/**
 Provide a view controller for launching if UI is enabled
 */
@property (nonatomic, strong) UIViewController *launchViewController;
/**
 State of MSA auth
 */
@property (nonatomic, assign, readonly) BOOL signingIn;
@property (nonatomic, assign, readonly) BOOL signedIn;

/**
 Sign in the user to MSA. The error code will most likely be an MSAAuthenticationCode object, 
 but very rarely may be a plain NSError if it is related to not being able to get an RPS ticket
 */
- (void)signInWithUIAllowed:(BOOL)uiAllowed silentAPI:(BOOL)silentAPI completion:(void (^)(BOOL success, NSError *error))completion;
/**
 Sign out of MSA.
 
 @param completion A completion block that will be called upon completion. Completion is guarenteed to happen.
 */
- (void)signOutWithCompletion:(void (^)(void))completion;
/**
 Get the RPS ticket associated with the scope
 
 @param scope The scope associated with the desired RPS ticket.
 @param completion A completion block that is called when the RPS ticket retrieval succeeds or fails.
 */
- (void)getRPSTicketWithScope:(MSASecurityScope *)scope withCompletion:(RPSTicketRequestBlock)completion;

@end
