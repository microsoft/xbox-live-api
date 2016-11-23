//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#import "pch.h"
#import <XSAPITCUI/XSAPITCUI.h>
#import "sign_in_delegate_ios.h"
#import "MSAAuthentication/MSAAccountManager.h"
#import "MSAAuthentication/MSAOnlineIDConfiguration+MSInternal.h"
#import "XLSCll.h"
#import "UTCIDPNames.h"
#import "XBLiOSGlobalState.h"

#pragma mark - XBLMSAErrorCreation
@interface NSError (XBLMSAErrorCreation)

/**
 *  Class method to easily create an MSAAuthenticationError
 *
 *  @param code One of 8 possible MSAAuthenticationErrorCode values
 *
 *  @return MSAAuthenticationError with MSAAuthenticationErrorDomain domain, MSAAuthenticationErrorCode code, and nil userInfo
 */
+ (MSAAuthenticationError *)xblMSAErrorWithCode:(MSAAuthenticationErrorCode)code;

@end

@implementation NSError (XBLMSAErrorCreation)

/**
 Create a `MSAAuthenticationError` error with the relevant code
 
 @param code The error code for an MSA auth issue
 
 @return An `MSAAuthenticationError` for the given code
 */
+ (MSAAuthenticationError *)xblMSAErrorWithCode:(MSAAuthenticationErrorCode)code {
    return [[MSAAuthenticationError alloc] initWithDomain:MSAAuthenticationErrorDomain code:code userInfo:nil];
}

@end

#pragma mark - XBLRPSServiceDelegate
@class XBLRPSService;
/**
 *  Callback protocol for when retrieving an RPS ticket has finished
 */
@protocol XBLRPSServiceDelegate<NSObject>

@required
/**
 *  Notifies the delegate the RPS ticket request finished
 */
- (void)rpsTicketRequestDidFinish:(XBLRPSService *)service;

@end

#pragma mark - XBLRPSService
/**
 *  `XBLRPSService` retrieves an RPS ticket and handles success and failure callbacks from the `MSAUserAccount`
 */
@interface XBLRPSService: NSObject<MSAFailureDelegate, MSAViewControllerTelemetryDelegate>

/**
 *  Delegate for callback upon a finished request
 */
@property (nonatomic, weak) id<XBLRPSServiceDelegate> delegate;
/**
 *  The account to request the ticket through
 */
@property (nonatomic, strong) MSAUserAccount *account;

/**
 *  Do not use. Please use -initWithAccount:
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 *  Required initializer
 *
 *  @param account The MSAUserAccount to query for RPS tickets from
 *  @param viewController A view controller to present for additional information if allowed. Nil if no ui is allowed.
 *
 *  @return An instance of XBLRPSService
 */
- (instancetype)initWithAccount:(MSAUserAccount *)account launchViewController:(UIViewController *)viewController NS_DESIGNATED_INITIALIZER;

/**
 *  Fetch an RPS Ticket with the desired scope
 *
 *  @param scope      The scope associated with the desired RPS ticket.
 *  @param delegate   The delegate to notify upon completion of an RPSTicket request
 *  @param completion A completion block that is called when the RPS ticket retrieval succeeds or fails.
 */
- (void)getRPSTicketWithScope:(MSASecurityScope *)scope delegate:(id<XBLRPSServiceDelegate>)delegate completion:(RPSTicketRequestBlock)completion;
@end

/**
 *  Private interface for XBLRPSService
 */
@interface XBLRPSService()

/**
 *  Upon completion of the RPS retrieval, this completion block will be called
 */
@property (nonatomic, copy) RPSTicketRequestBlock completion;

@end

#pragma mark - XBLMSAUserAccountService

@interface XBLMSAUserAccountService: NSObject

@property (nonatomic, strong) MSAUserAccount *userAccount;

- (instancetype)initWithManager:(MSAAccountManager *)manager;
/**
 *  Gets the user account while also handling all of the errors
 *
 *  @param viewController A UIViewController if showing ui is allowed, nil otherwise
 *  @param completion     Called when retrieval of an account succeeds or fails
 */
- (void)getUserAccountWithLaunchViewController:(UIViewController *)viewController completion:(void (^)(MSAUserAccount *account, NSError *error))completion;
- (void)discardUserAccountWithCompletion:(void (^)(void))completion;
@end

#pragma mark - XBLMSASignInDelagate

using namespace xbox::services;
using namespace xbox::services::system;

typedef void (^XBLMSASignInCompletionBlock)(BOOL, NSError *);

/**
 *  Private interface of XBLMSASignInDelagate
 */
@interface XBLMSASignInDelagate() <XBLRPSServiceDelegate> {
    std::shared_ptr<auth_config> _authConfig;
    std::shared_ptr<auth_manager> _authManager;
}

@property (nonatomic, assign, readwrite) BOOL signingIn;
@property (nonatomic, assign, readwrite) BOOL signedIn;

@property (nonatomic, strong, readonly) MSAUserAccount *userAccount;
@property (nonatomic, strong) MSAAccountManager *manager;
@property (nonatomic, strong) MSAOnlineIDConfiguration *onlineIDConfiguration;

@property (nonatomic, strong) XBLMSAUserAccountService *accountService;
@property (nonatomic, strong) NSMutableSet *rpsTicketRequestSet;

@property (nonatomic, assign) BOOL uiAllowed;
@property (nonatomic, assign) BOOL silentAPI;
@property (nonatomic, copy) XBLMSASignInCompletionBlock completion;

@end

@implementation XBLMSASignInDelagate

#pragma mark Initialization
- (instancetype)init {
    
    self = [super init];
    if (self) {
        _authManager = auth_manager::get_auth_manager_instance();
        _authConfig = _authManager->get_auth_config();
        
        self.onlineIDConfiguration = [[MSAOnlineIDConfiguration alloc] init];
        self.onlineIDConfiguration.cobrandingID = @"90011";
        self.onlineIDConfiguration.preferredSignUpMembername = MSAPreferredSignUpMembernameTypeEmail;
        self.manager = [[MSAAccountManager alloc] initWithConfiguration:self.onlineIDConfiguration];
        self.rpsTicketRequestSet = [NSMutableSet set];
        self.accountService = [[XBLMSAUserAccountService alloc] initWithManager:self.manager];
        
        [self configureLoadingScreenColor];
    }
    
    return self;
}

#pragma mark Sign In
- (void)signInWithUIAllowed:(BOOL)uiAllowed silentAPI:(BOOL)silentAPI completion:(void (^)(BOOL, NSError *))completion {
    NSAssert(!self.signedIn && !self.signingIn, @"Can't be signed in already or in the process of signing in");

    if (self.signedIn || self.signingIn) {
        completion(NO, [NSError xblMSAErrorWithCode:MSAAuthenticationErrorCodeInternal]);
        return;
    }
    
    self.signingIn = YES;
    self.uiAllowed = uiAllowed;
    self.silentAPI = silentAPI;
    self.completion = completion;
    
    __weak typeof(self) weakSelf = self;
    [XLSCll apiSignInEvent:silentAPI withState:@"MSA Get Account Invoking"];
    [self getUserAccountIfNeededWithCompletion:^(MSAUserAccount *account, NSError *error) {
        [weakSelf handleUserAccountRequest:account error:error silentAPI:silentAPI];
    }];
}

- (void)signOutWithCompletion:(void (^)())completion {
    if (self.userAccount) {
        __weak typeof(self) weakSelf = self;
        [self.accountService discardUserAccountWithCompletion:^{
            self.signedIn = NO;
            [weakSelf configureLoadingScreenColor];
            completion();
        }];
    } else {
        completion();
    }
}

- (void)getRPSTicketWithScope:(MSASecurityScope *)scope withCompletion:(RPSTicketRequestBlock)completion {
    if (!self.userAccount) {
        // Used by MSA to indicate the user failed sign in
        MSAAuthenticationError *error = [NSError xblMSAErrorWithCode:MSAAuthenticationErrorCodeAccountNotFound];
        completion(nil, error);
        return;
    }
    
    // By default, we always need to be able to authenticate. This is a terrible but necessary experience.
    UIViewController *launchViewController = [XBLiOSGlobalState launchViewController];
    
    XBLRPSService *service = [[XBLRPSService alloc] initWithAccount:self.userAccount launchViewController:launchViewController];
    
    [self.rpsTicketRequestSet addObject:service];
    [service getRPSTicketWithScope:scope delegate:self completion:completion];
}

- (void)getRPSTicketWithScope:(MSASecurityScope *)scope launchViewController:(UIViewController *)viewController withCompletion:(RPSTicketRequestBlock)completion {
    if (!self.userAccount) {
        // Used by MSA to indicate the user failed sign in
        MSAAuthenticationError *error = [NSError xblMSAErrorWithCode:MSAAuthenticationErrorCodeAccountNotFound];
        completion(nil, error);
        return;
    }
    
    XBLRPSService *service = [[XBLRPSService alloc] initWithAccount:self.userAccount launchViewController:viewController];
    
    [self.rpsTicketRequestSet addObject:service];
    [service getRPSTicketWithScope:scope delegate:self completion:completion];
}

#pragma mark XBLRPSServiceDelegate

- (void)rpsTicketRequestDidFinish:(XBLRPSService *)service {
    [self.rpsTicketRequestSet removeObject:service];
}

#pragma mark Private Sign In Helpers
/**
 *  Wrapper around account retrieval so that if an account exists already, to not attempt to get a new one
 *
 *  @param completion Block to be called upon completion
 */
- (void)getUserAccountIfNeededWithCompletion:(void (^)(MSAUserAccount *, NSError *))completion {
    if (self.userAccount) {
        completion(self.userAccount, nil);
    } else {
        UIViewController *launchViewController = self.uiAllowed ? self.launchViewController : nil;
        __weak typeof(self) weakSelf = self;
        [self.accountService getUserAccountWithLaunchViewController:launchViewController completion:^(MSAUserAccount *account, NSError *error) {
            completion(account, error);
            [weakSelf notifyTelemetryUserAccountRequestSucceeded:(account != nil) withError:error];
        }];
    }
}

/**
 *  Handles the user account request upon completion
 *
 *  @param account An account if retrieved
 *  @param error   An error if errored during account retrieval
 */
- (void)handleUserAccountRequest:(MSAUserAccount *)account error:(NSError *)error silentAPI:(BOOL)silentAPI {
    [self configureLoadingScreenColor];
    
    if (account) {
        [XLSCll apiSignInEvent:silentAPI withState:@"MSA Get Account returned with Account"];
        __weak typeof(self) weakSelf = self;
        UIViewController *launchViewController = self.uiAllowed ? self.launchViewController : nil;
        
        [XLSCll apiSignInEvent:silentAPI withState:@"MSA Get Ticket Invoking"];
        [self getRPSTicketWithScope:[weakSelf primarySecurityScope]
               launchViewController:launchViewController
                     withCompletion:^(NSString *ticket, NSError *error) {
                         if([ticket length] == 0)
                         {
                             if(error)
                             {
                                 [XLSCll apiSignInEvent:silentAPI withState:[NSString stringWithFormat:@"MSA Get Ticket Returned No Ticket - %@", [error localizedDescription]]];
                             }
                             else
                             {
                                 [XLSCll apiSignInEvent:silentAPI withState:@"MSA Get Ticket Returned No Ticket"];
                             }
                         }
                         else
                         {
                             [XLSCll apiSignInEvent:silentAPI withState:@"MSA Get Ticket Returned With Ticket"];
                         }
                         [weakSelf handlePrimaryRPSTicketRequest:ticket error:error];
                     }];
    } else {
        if(error)
        {
            [XLSCll apiSignInEvent:silentAPI withState:[NSString stringWithFormat:@"MSA Get Account returned No Account - %@", [error localizedDescription]]];
        }
        else
        {
            [XLSCll apiSignInEvent:silentAPI withState:@"MSA Get Account returned No Account"];
        }
        [self completeSignInWithStatus:NO error:error];
    }
}

/**
 *  Handle the completion of retrieving an RPS ticket
 *
 *  @param ticket A ticket if retrieved
 *  @param error  An error if errored while retrieving the RPS ticket
 */
- (void)handlePrimaryRPSTicketRequest:(NSString *)ticket error:(NSError *)error {
    if (ticket) {
        _authManager->set_rps_ticket([ticket UTF8String]);
        [self completeSignInWithStatus:YES error:nil];
    } else {
        [self completeSignInWithStatus:NO error:error];
    }
    
    [self notifyTelemetryRPSTicketRequestSucceeded:(ticket != nil) withError:error];
}

/**
 *  Configures the loading screen background color to not visually freak people out.
 *  Because this the loading screen is so heavily depenent on whether the user account exists
 *  it is called here
 */
- (void)configureLoadingScreenColor {
    BOOL viewShouldBeGreen = self.userAccount != nil;
    dispatch_async(dispatch_get_main_queue(), ^{
        // Do this because once we get the user account, the MSA VC will
        // dismiss itself and we want to return to a green loading screen
        if (viewShouldBeGreen) {
            [XBLIDPScenario enableGreenLoadingScreen];
        } else {
            [XBLIDPScenario enableWhiteLoadingScreen];
        }
    });
}

/**
 *  Primary rps ticket for the user account
 *
 *  @return RPS Ticket to be used for XToken auth
 */
- (MSASecurityScope *)primarySecurityScope {
    return [[MSASecurityScope alloc] initWithTarget:[[NSString alloc] initWithUTF8String:_authConfig->rps_ticket_service().c_str()]
                                             policy:[[NSString alloc] initWithUTF8String:_authConfig->rps_ticket_policy().c_str()]];
}

/**
 *  Helper method to handle sign in completion with an arbitrary result
 *
 *  @param status Sign In status
 *  @param error  Error if errored while retrieving account or rps ticket
 */
- (void)completeSignInWithStatus:(BOOL)status error:(NSError *)error {
    self.signedIn = status;
    
    // We must guard against this because MSAAuth can call fail more than once if the user cancels after already
    // having been signed in and then signing out
    if (self.completion) {
        self.completion(status, error);
        self.completion = nil;
    }
    
    [self notifyTelemetrySignInSucceeded:status withUIShown:self.uiAllowed withError:error];
    
    self.signingIn = NO;
    self.launchViewController = nil;
    self.uiAllowed = NO;
}

/**
 *  User account for the current user
 *
 *  @return an MSAUserAccount if it exists, nil otherwise
 */
- (MSAUserAccount *)userAccount {
    return self.accountService.userAccount;
}

#pragma mark - Telemetry
- (void)notifyTelemetrySignInSucceeded:(BOOL)succeeded withUIShown:(BOOL)uiShown withError:(NSError *)error{
    if (succeeded)
    {
        [[XLSCll sharedTelemetryManager] pageActionEvent:IDP_PageAction_Signin_Signin withData:@{@"cid" : [[self userAccount] cid],
                                                                                                 @"isSilent" : [NSNumber numberWithBool:uiShown]}];
        if (!uiShown)
        {
            [[XLSCll sharedTelemetryManager] pageViewEvent:@"XsapiSilentSignIn" withData:@{@"cid" : [[self userAccount] cid],
                                                                                           @"isSilent" : [NSNumber numberWithBool:uiShown]}];
            
        }
    }
    else
    {
        [[XLSCll sharedTelemetryManager] clientErrorEvent:IDP_Client_Errors_Failure errorText:[error localizedDescription] errorCode:[@(error.code) stringValue] callStack:@"MSA Account Signin"];
    }
}

- (void)notifyTelemetryUserAccountRequestSucceeded:(BOOL)succeeded withError:(NSError *)error {
    if (succeeded)
    {
        [[XLSCll sharedTelemetryManager] pageActionEvent:IDP_PageAction_Signin_AccountSuccess withData:@{@"cid" : [[self userAccount] cid],
                                                                                                @"source" : @"Account acquisition"}];
    }
    else
    {
        [[XLSCll sharedTelemetryManager] clientErrorEvent:IDP_Client_Errors_Failure errorText:[error localizedDescription] errorCode:[@(error.code) stringValue] callStack:@"Account acquisition"];
    }
}

- (void)notifyTelemetryRPSTicketRequestSucceeded:(BOOL)succeeded withError:(NSError *)error {
    if (succeeded)
    {
        [[XLSCll sharedTelemetryManager] pageActionEvent:IDP_PageAction_Signin_TicketSuccess withData:@{@"cid" : [[self userAccount] cid],
                                                                                                @"source" : @"ticket acquisition"}];
    }
    else
    {
        [[XLSCll sharedTelemetryManager] clientErrorEvent:IDP_Client_Errors_Failure errorText:[error localizedDescription] errorCode:[@(error.code) stringValue] callStack:@"ticket acquisition"];
    }
}

@end
         
#pragma mark - XBLMSAUserAccountService

typedef void (^XBLUserAccountCompletionBlock)(MSAUserAccount *, NSError *);

@interface XBLMSAUserAccountService ()<MSAFailureDelegate, MSAViewControllerTelemetryDelegate>

@property (nonatomic, strong) MSAAccountManager *manager;
@property (nonatomic, strong) UIViewController *launchViewController;

@property (nonatomic, assign) BOOL isGettingAccount;
@property (nonatomic, assign) BOOL isDiscardingAccount;

@property (nonatomic, copy) void (^getAccountCompletion)(MSAUserAccount *, NSError *);
@property (nonatomic, copy) void (^discardAccountCompletion)(void);
@end

@implementation XBLMSAUserAccountService

- (instancetype)initWithManager:(MSAAccountManager *)manager {
    
    self = [super init];
    if (self) {
        self.manager = manager;
    }
    
    return self;
}

- (void)dealloc {
    self.getAccountCompletion = nil;
    self.discardAccountCompletion = nil;
}

- (void)getUserAccountWithLaunchViewController:(UIViewController *)viewController completion:(void (^)(MSAUserAccount *, NSError *))completion {
    NSAssert(!self.userAccount, @"User Account already aquired");
    self.launchViewController = viewController;
    self.getAccountCompletion = completion;
    self.isGettingAccount = YES;
    
    __weak typeof(self) weakSelf = self;
    void (^getAccount)(void) = ^() {
        [weakSelf.manager getAccountWithFailureDelegate:weakSelf successBlock:^(MSAUserAccount *account) {
            [weakSelf handleCompletionWithAccount:account error:nil];
        }];
    };
    
    if ([NSThread isMainThread]) {
        getAccount();
    } else {
        dispatch_async(dispatch_get_main_queue(), getAccount);
    }
}

- (void)discardUserAccountWithCompletion:(void (^)(void))completion {
    NSAssert(self.userAccount, @"User Account required for discarding");
    self.isDiscardingAccount = YES;
    self.discardAccountCompletion = completion;
    
    __weak typeof(self) weakSelf = self;
    [self.manager getSignOutViewControllerForAccount:self.userAccount failureDelegate:self successBlock:^{
        [weakSelf handleCompletionWithAccount:nil error:nil];
    }];
}


- (void)handleCompletionWithAccount:(MSAUserAccount *)account error:(NSError *)error {
    self.userAccount = account;
    self.launchViewController = nil;
    
    if (self.isGettingAccount) {
        if (self.getAccountCompletion) {
            self.getAccountCompletion(account, error);
            self.getAccountCompletion = nil;
        }
        self.isGettingAccount = NO;
    } else if (self.isDiscardingAccount) {
        if (self.discardAccountCompletion) {
            self.discardAccountCompletion();
            self.discardAccountCompletion = nil;
        }
        self.isDiscardingAccount = NO;
    }
}
#pragma mark - MSAViewControllerTelemetryDelegate Methods

- (void) viewController:(MSAViewController *)viewController receivedEvent:(NSString *)event
{
    [[XLSCll sharedTelemetryManager] pageActionEvent:@"MSA SDK Event"  withData:@{@"MSAData": event}];
}

#pragma mark MSAFailureDelegate
- (void)didFailWithNeededViewController:(MSAViewController *)uiController {
    if(self.launchViewController) {
        
        // Show the MSAViewController and cross-dissolve the presentation so that when it dismisses, it is not
        // doing the traditional modal dismissal animation
        uiController.modalTransitionStyle = UIModalTransitionStyleCrossDissolve;
        uiController.telemetryDelegate = self;
        
        dispatch_async(dispatch_get_main_queue(), ^() {
            [self.launchViewController presentViewController:uiController
                                                    animated:NO
                                                  completion:nil];
        });
    } else {
        [self handleCompletionWithAccount:nil error:[NSError xblMSAErrorWithCode:MSAAuthenticationErrorCodeUIRequired]];
    }
}

- (void)didFailBecauseUserCancelled {
    [self handleCompletionWithAccount:nil error:[NSError xblMSAErrorWithCode:MSAAuthenticationErrorCodeUserCancelled]];
}

- (void)didFailWithError:(MSAAuthenticationError *)error {
    [self handleCompletionWithAccount:nil error:error];
}

@end


#pragma mark - XBLRPSService
@interface XBLRPSService()

@property (nonatomic, strong) UIViewController *launchViewController;

@end

@implementation XBLRPSService

- (instancetype)initWithAccount:(MSAUserAccount *)account launchViewController:(UIViewController *)viewController {
    self = [super init];
    
    if (self) {
        self.account = account;
        self.launchViewController = viewController;
    }
    
    return self;
}

- (void)getRPSTicketWithScope:(MSASecurityScope *)scope delegate:(id<XBLRPSServiceDelegate>)delegate completion:(RPSTicketRequestBlock)completion {
    NSAssert(self.account, @"Account required");
    NSAssert(delegate, @"Delegate required");
    NSAssert(self.completion == nil, @"Don't queue up more than one request at once");
    
    self.delegate = delegate;
    self.completion = completion;
    
    [self.account getTicketWithScope:scope failureDelegate:self successBlock:^(MSATicket *ticket) {
        completion([ticket value], nil);
        self.completion = nil;
    }];
}

- (void)didFailWithError:(MSAAuthenticationError *)error {
    [self handleCompletion:nil error:error];
}

- (void)didFailBecauseUserCancelled {
    [self handleCompletion:nil error:nil];
}

- (void)handleCompletion:(MSATicket *)ticket error:(NSError *)error {
    if (self.completion) {
        self.completion([ticket value], error);
        self.completion = nil;
    }
    
    [self.delegate rpsTicketRequestDidFinish:self];
}

- (void)didFailWithNeededViewController:(MSAViewController *)uiController {
    if(self.launchViewController) {
        
        // Show the MSAViewController and cross-dissolve the presentation so that when it dismisses, it is not
        // doing the traditional modal dismissal animation
        uiController.modalTransitionStyle = UIModalTransitionStyleCrossDissolve;
        uiController.telemetryDelegate = self;
        
        dispatch_async(dispatch_get_main_queue(), ^() {
            [self.launchViewController presentViewController:uiController
                                                    animated:NO
                                                  completion:nil];
        });
    } else {
        [self handleCompletion:nil error:[NSError xblMSAErrorWithCode:MSAAuthenticationErrorCodeUIRequired]];
    }
}
#pragma mark - MSAViewControllerTelemetryDelegate Methods

- (void) viewController:(MSAViewController *)viewController receivedEvent:(NSString *)event
{
    [[XLSCll sharedTelemetryManager] pageActionEvent:@"MSA SDK Event"  withData:@{@"MSAData": event}];
}

@end