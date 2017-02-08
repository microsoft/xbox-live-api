//
//  NSErrors+XSAPIExtensions.mm
//  XboxLiveServices
//
//  Created by Sam Walker on 2/2/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#import "NSError+XSAPIExtensions.h"
#import "errors.h"
#import <XSAPITCUI/XSAPITCUI.h>

@implementation NSError (XSAPIExtensions)

+ (NSError *)errorWithSTDError:(const std::error_code)error message:(string_t)message {
    
    NSString *description = [NSString stringWithFormat:@"%s - XBL Message: %s", error.message().c_str(), message.c_str()];
    NSDictionary *userInfo = @{
                               NSLocalizedDescriptionKey: description,
                               NSLocalizedFailureReasonErrorKey: [NSString stringWithUTF8String:error.message().c_str()]
                               };
    
    NSString *domain = [NSString stringWithUTF8String:error.category().name()];
    if (!domain || domain.length == 0) {
        domain = @"XboxAuth";
    }
    
    int code = error.value();
    
    // Transform to CF Errors when known
    if (error == xbox::services::xbox_live_error_code::AM_E_NO_NETWORK || ![XBLServiceProvider sharedProvider].reachability.isNetworkConnected) {
        code = NSURLErrorNotConnectedToInternet;
    }
    
    return [NSError errorWithDomain:domain code:code userInfo:userInfo];
}

@end
