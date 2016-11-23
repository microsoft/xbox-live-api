//
//  XBLDictionaryToJSON.h
//  XboxLiveServices
//
//  Created by Sam Walker on 4/7/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "xsapi/services.h"

@interface XBLDictionaryToJSON : NSObject 

- (web::json::value)jsonForObject:(id)object;

@end
