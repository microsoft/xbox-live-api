// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#import "XBLDictionaryToJSON.h"
#import "xsapi_utils.h"

using namespace web::http;
using namespace web::http::client;
using namespace xbox::services;
using namespace xbox::services::system;

@implementation XBLDictionaryToJSON

- (web::json::value)jsonForObject:(id)object {
    
    if ([object isKindOfClass:[NSString class]]) {
        return [self jsonForString:object];
    }
    else if ([object isKindOfClass:[NSNumber class]]) {
        return [self jsonForNumber:object];
    }
    else if ([object isKindOfClass:[NSArray class]]) {
        return [self jsonForArray:object];
    }
    else if ([object isKindOfClass:[NSDictionary class]]) {
        return [self jsonForDictionary:object];
    }
    else {
        return web::json::value::null();
    }
}

- (web::json::value)jsonForString:(NSString *)string {
    NSString *value = (NSString *)string;
    string_t value_t = string_t([value UTF8String]);
    return web::json::value(value_t);
}

- (web::json::value)jsonForNumber:(NSNumber *)number {
    if ([self numberIsBool:number]) {
        BOOL value = [number boolValue];
        if (value) {
            return web::json::value("true");
        } else {
            return web::json::value("false");
        }
    } else if ([self numberIsInteger:number]) {
        uint64_t value = (uint64_t)[number unsignedLongLongValue];
        return web::json::value(value);
    } else {
        float value = [number floatValue];
        return web::json::value(value);
    }
}

- (web::json::value)jsonForDictionary:(NSDictionary *)json {
    
    web::json::value node;
    
    for (NSString *key in json) {
        id value = json[key];
        
        string_t key_t = string_t([key UTF8String]);
        auto value_t = [self jsonForObject:value];
        
        node[key_t] = value_t;
    }
    
    return node;
}

- (web::json::value)jsonForArray:(NSArray *)json {
    std::vector<web::json::value> array;
    
    for (id value in json) {
        web::json::value jsonValue = [self jsonForObject:value];
        array.push_back(jsonValue);
    }
    
    return web::json::value::array(array);
}

- (BOOL)numberIsBool:(NSNumber *)number {
    
    NSNumber *trueNumber = [NSNumber numberWithBool:YES];
    NSString *trueString = [NSString stringWithUTF8String:[trueNumber objCType]];
    
    NSNumber *falseNumber = [NSNumber numberWithBool:NO];
    NSString *falseString = [NSString stringWithUTF8String:[falseNumber objCType]];
    
    NSString *objCType = [NSString stringWithUTF8String:[number objCType]];
    
    if (([trueNumber compare:number] == NSOrderedSame && [objCType isEqualToString:trueString])
        || ([falseNumber compare:number] == NSOrderedSame && [objCType isEqualToString:falseString])) {
        return YES;
    }
    
    return NO;
}

- (BOOL)numberIsInteger:(NSNumber *)number {
    if(CFNumberIsFloatType((CFNumberRef)number))
    {
        return NO;
    }
    else
    {
        return YES;
    }
}

@end
