// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#import <Foundation/Foundation.h>
#import "xsapi-cpp/services.h"

@interface XBLDictionaryToJSON : NSObject 

- (web::json::value)jsonForObject:(id)object;

@end
