//
//  XBLServiceManager.m
//  XboxLiveServices
//
//  Created by Sam Walker on 4/7/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#import "pch.h"
#import "XBLServiceManager.h"
#import "utils.h"
#import "xsapi/services.h"
#import "xsapi/profile.h"
#import "token_result.h"
#import "user_auth_ios.h"
#import "XBLDictionaryToJSON.h"
#import "NSError+XSAPIExtensions.h"

using namespace web::http;
using namespace web::http::client;
using namespace xbox::services;
using namespace xbox::services::system;

@interface XBLServiceManager() {
    std::shared_ptr<xbox::services::xbox_live_context_settings> _xboxLiveContextSettings;
    std::shared_ptr<xbox_live_app_config> _appConfig;
}

@end

@implementation XBLServiceManager

- (instancetype)init
{
    self = [super init];
    if (self) {
        _appConfig = xbox_live_app_config::get_app_config_singleton();
        _xboxLiveContextSettings = std::make_shared<xbox::services::xbox_live_context_settings>();
    }
    return self;
}

- (NSString *)userXuid {
    auto user = [self currentUser];
    return user == nullptr ? nil : [NSString stringWithUTF8String:user->xbox_user_id().c_str()];
}

- (NSString *)userGamertag
{
    auto user = [self currentUser];
    return user == nullptr ? nil : [NSString stringWithUTF8String:user->gamertag().c_str()];
}

- (std::shared_ptr<xbox::services::system::user_auth_ios>)currentUser {
    return xbox::services::system::user_auth_ios::get_instance();
}

- (BOOL)privilegeForAddFriend
{
    auto user = [self currentUser];
    NSString *priviledges = [NSString stringWithUTF8String:user->privileges().c_str()];
    NSArray *priviledgeArray = [priviledges componentsSeparatedByCharactersInSet:
                        [NSCharacterSet characterSetWithCharactersInString:@" "]
                        ];
    //Defined in title_callable_ui.h, 255 is for
    //"The user can follow other Xbox LIVE users and add Xbox LIVE friends when this privilege is present"
    if (priviledgeArray != nil && [priviledgeArray containsObject:@"255"])
    {
        return YES;
    }

    return NO;
}

- (void)invokeRequest:(NSURLRequest *)request completion:(void(^)(NSURLRequest *, NSHTTPURLResponse *, NSData *, NSError *))completion {
    auto httpCall = [self httpCallFromRequest:request];
    
    httpCall->get_response_with_auth(http_call_response_body_type::vector_body).then([completion, request] (std::shared_ptr<xbox::services::http_call_response> response) {
        [XBLServiceManager handleResponse:response originalRequest:request completion:completion];
    });
}

- (std::shared_ptr<http_call>)httpCallFromRequest:(NSURLRequest *)request {
    // Base Request
    auto httpCall = [self basehttpCallFromRequest:request];
    
    // Headers
    auto headers = [XBLServiceManager headersForRequest:request];
    for(auto iter = headers.begin(); iter != headers.end(); ++iter)
    {
        httpCall->set_custom_header(iter->first, iter->second);
    }
    
    // Body
    if (request.HTTPBody) {
        auto body = [XBLServiceManager bodyForRequest:request];
        httpCall->set_request_body(body);
    }
    
    return httpCall;
}

- (std::shared_ptr<http_call>)basehttpCallFromRequest:(NSURLRequest *)request {
    NSURLComponents *components = [NSURLComponents componentsWithURL:request.URL resolvingAgainstBaseURL:NO];
    
    NSString *endpoint = [NSString stringWithFormat:@"%@://%@", [components scheme], [components percentEncodedHost]];
    NSString *pathQueryFragment;
    
    NSString *path = [components percentEncodedPath];
    NSString *query = [components percentEncodedQuery];
    NSString *fragment = [components percentEncodedFragment];
    
    // Per http://www.ietf.org/rfc/rfc1738.txt
    // All of path, query, and fragment is optional.
    // url.path returns the path + initial /
    // If we have a path, we can assume it be the initial value for pathQueryFragment
    // Otherwise, query and fragment would need to add the / first
    
    if (path) {
        pathQueryFragment = path;
    }
    
    if (query) {
        if (!pathQueryFragment) {
            pathQueryFragment = @"/";
        }
        
        pathQueryFragment = [pathQueryFragment stringByAppendingString:[NSString stringWithFormat:@"?%@", query]];
    }
    
    if (fragment) {
        if (!pathQueryFragment) {
            pathQueryFragment = @"/";
        }
        
        pathQueryFragment = [pathQueryFragment stringByAppendingString:[NSString stringWithFormat:@"#%@", fragment]];
    }
    
    string_t method_t = string_t([[request HTTPMethod] UTF8String]);
    string_t endpoint_t = string_t([endpoint UTF8String]);
    string_t path_t = string_t([pathQueryFragment UTF8String]);
    
    return xbox_system_factory::get_factory()->create_http_call(
                                                                _xboxLiveContextSettings,
                                                                method_t,
                                                                endpoint_t,
                                                                path_t,
                                                                xbox_live_api::unspecified);
}

+ (void)handleResponse:(std::shared_ptr<xbox::services::http_call_response>)response originalRequest:(NSURLRequest *)request
            completion:(void(^)(NSURLRequest *, NSHTTPURLResponse *, NSData *, NSError *))completion {
    auto vector = response->response_body_vector();
    auto bytes = &vector.front();
    NSData *data = [NSData dataWithBytes:bytes length:vector.size()];
    uint32_t statusCode = response->http_status();
    NSError *error = nil;
    
    if (response->err_code() != xbox_live_error_code::no_error) {
        error = [NSError errorWithSTDError:response->err_code() message:response->err_message()];
    }
    
    NSMutableDictionary<NSString *, NSString *> *allHeaderFields = [[NSMutableDictionary alloc] init];
    auto headers = response->response_headers();
    
    for(auto iter = headers.begin(); iter != headers.end(); ++iter) {
        NSString *key = [NSString stringWithUTF8String:iter->first.c_str()];
        NSString *value = [NSString stringWithUTF8String:iter->second.c_str()];
        allHeaderFields[key] = value;
    }
    
    NSHTTPURLResponse *urlResponse = [[NSHTTPURLResponse alloc] initWithURL:request.URL
                                                              statusCode:statusCode
                                                             HTTPVersion:@"HTTP/1.1"
                                                            headerFields:allHeaderFields];
    
    completion(request, urlResponse, data, error);
}

#pragma mark - Get Token and Sign Request
- (void)getTokenAndSignRequest:(NSMutableURLRequest *)request completion:(void (^)(NSMutableURLRequest *, NSError *))completion {
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
        [self getTokenAndSignRequestSynchronously:request completion:completion];
    });
}

- (void)getTokenAndSignRequestSynchronously:(NSMutableURLRequest *)request completion:(void (^)(NSMutableURLRequest *, NSError *))completion {
    auto user = [self currentUser];
    
    string_t httpMethod = string_t([request.HTTPMethod UTF8String]);
    string_t url = string_t([[[request URL] absoluteString] UTF8String]);
    string_t headers = [XBLServiceManager stringHeadersForRequest:request];
    
    xbox_live_result<token_and_signature_result> result;
    
    if (request.HTTPBody) {
        string_t requestBodyString = [XBLServiceManager bodyForRequest:request].serialize();
        result = user->internal_get_token_and_signature(httpMethod, url, headers, requestBodyString, std::vector<uint8_t>(), false, false).get();
    } else {
        result = user->internal_get_token_and_signature(httpMethod, url, headers, string_t(), std::vector<uint8_t>(), false, false).get();
    }
    
    if (result.err()) {
        NSError *error = [NSError errorWithSTDError:result.err() message:result.err_message()];
        completion(nil, error);
    } else {
        auto tokenSignature = result.payload();
        NSString *token = [NSString stringWithUTF8String:tokenSignature.token().c_str()];
        NSString *signature = [NSString stringWithUTF8String:tokenSignature.signature().c_str()];
        
        if (token && token.length) {
            
            [request setValue:token forHTTPHeaderField:@"Authorization"];
        }
        
        if (signature && signature.length) {
            [request setValue:signature forHTTPHeaderField:@"Signature"];
        }
        
        completion(request, nil);
    }
}

+ (string_t)stringHeadersForRequest:(NSURLRequest *)request {
    web::http::http_headers headerMap = [self headersForRequest:request];
    return utils::headers_to_string(headerMap);
}

+ (web::http::http_headers)headersForRequest:(NSURLRequest *)request {
    web::http::http_headers headerMap;
    
    NSDictionary<NSString *, NSString *> *allHeaders = request.allHTTPHeaderFields;
    
    for (NSString *key in allHeaders) {
        NSString *value = allHeaders[key];
        string_t key_t = string_t([key UTF8String]);
        string_t value_t = string_t([value UTF8String]);
        
        headerMap[key_t] = value_t;
    }
    
    return headerMap;
}

+ (web::json::value)bodyForRequest:(NSURLRequest *)request {
    NSDictionary *json = [NSJSONSerialization JSONObjectWithData:request.HTTPBody options:0 error:nil];
    
    XBLDictionaryToJSON *jsonParser = [[XBLDictionaryToJSON alloc] init];
    
    return [jsonParser jsonForObject:json];
}

#pragma mark - GetRPSTicket
- (void)getRPSTicketWithTarget:(NSString *)target policy:(NSString *)policy completion:(void (^)(NSString *, NSError *))completion {
    XBLMSASignInDelagate *signInDelegate = [self msaSignInDelegate];
    
    if (!signInDelegate) {
        NSAssert(NO, @"Retrieval of an RPS ticket should only happen during the sign in process or once signed in");
        completion(nil, nil);
        return;
    }

    MSASecurityScope *scope = [[MSASecurityScope alloc] initWithTarget:target policy:policy];
    [signInDelegate getRPSTicketWithScope:scope withCompletion:completion];
}

#pragma mark - Current Authentication Item

- (NSString *)currentXToken {
    auto manager = auth_manager::get_auth_manager_instance();
    auto tokenManager = manager->auth_token_manager();
    auto config = auth_manager::get_auth_manager_instance()->get_auth_config();
    auto relyingParty = config->xbox_live_relying_party();
    
    auto token = tokenManager->get_xtoken(relyingParty, relyingParty, "JWT", false, false).get().payload().token();
    
    if (token.length() > 0) {
        return [NSString stringWithUTF8String:token.c_str()];
    }
    
    return nil;
}

- (NSString *)currentPrimaryRPSTicket {
    auto ticket = auth_manager::get_auth_manager_instance()->auth_token_manager()->get_rps_ticket();
    return [NSString stringWithUTF8String:ticket.c_str()];
}

- (XBLMSASignInDelagate *)msaSignInDelegate {
    auto user = [self currentUser];
    if (user == nullptr) {
        return nil;
    }
    return user->get_sign_in_delegate();
}

@end
