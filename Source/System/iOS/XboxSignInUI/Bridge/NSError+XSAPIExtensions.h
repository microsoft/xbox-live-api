//
//  NSError+XSAPIExtensions.h
//  XboxLiveServices
//
//  Created by Sam Walker on 2/2/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSError (XSAPIExtensions)

+ (NSError *)errorWithSTDError:(const std::error_code)error message:(string_t)message;

@end
