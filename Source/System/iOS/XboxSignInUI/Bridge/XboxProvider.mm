//
//  XboxProvider.m
//  XboxLiveServices
//
//  Created by Sam Walker on 2/1/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#import "XboxProvider.h"
#import "user_impl.h"
#import "NSError+XSAPIExtensions.h"
#import "XboxProviderBridge.h"

using namespace xbox::services;
using namespace xbox::services::system;

@interface XboxProvider()
{
    std::shared_ptr<xbox::services::social::XboxProviderBridge> _bridge;
}

+ (NSURLSession *)sharedXboxProviderURLSession;
+ (XBLMicrosoftProfile *)microsoftProfileWithData:(web::json::value)data;

@property (nonatomic, strong, readwrite) XBLMicrosoftProfile *microsoftProfile;
@property (nonatomic, strong, readwrite) XBLXboxProfile *xboxProfile;

@end
@implementation XboxProvider

+ (NSURLSession *)sharedXboxProviderURLSession {
    static NSURLSession *urlSession;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        NSURLSessionConfiguration *config = [NSURLSessionConfiguration defaultSessionConfiguration];
        urlSession = [NSURLSession sessionWithConfiguration:config];
    });
    
    return urlSession;
}

+ (XBLMicrosoftProfile *)microsoftProfileWithData:(web::json::value)data {
    std::error_code errc = xbox_live_error_code::no_error;
    auto firstName = utils::extract_json_string(data, "firstName", errc, true);
    auto lastName = utils::extract_json_string(data, "lastName", errc, true);
    auto email = utils::extract_json_string(data, "email", errc, true);
    auto imageUrl = utils::extract_json_string(data, "imageUrl", errc, true);
    
    // Sanity check to make sure we received proper fields back
    if (!(firstName.length() > 0 || lastName.length() > 0 || email.length() > 0 || imageUrl.length() > 0)) {
        return nil;
    }
    
    XBLMicrosoftProfile *profile = [[XBLMicrosoftProfile alloc] init];
    if (firstName.length() > 0) {
        profile.firstName = [NSString stringWithUTF8String:firstName.c_str()];
    }
    
    if (lastName.length() > 0) {
        profile.lastName = [NSString stringWithUTF8String:lastName.c_str()];
    }

    if (email.length() > 0) {
        profile.email = [NSString stringWithUTF8String:email.c_str()];
    }
    
    if (imageUrl.length() > 0) {
        profile.imageUrl = [NSString stringWithUTF8String:imageUrl.c_str()];
    }
    
    return profile;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        self->_bridge = std::make_shared<xbox::services::social::XboxProviderBridge>();
        self.xboxProfile = [[XBLXboxProfile alloc] init];
    }
    return self;
}

- (void)reset {
    self.xboxProfile = [[XBLXboxProfile alloc] init];
    self.microsoftProfile = nil;
}

- (void)setUser:(std::shared_ptr<user_auth_ios>)user {
    self->_bridge->m_xuid = user->xbox_user_id();
    
    string_t gamertag = user->gamertag();
    if (gamertag.length() > 0) {
        self.xboxProfile.gamertag = [NSString stringWithUTF8String:gamertag.c_str()];
    }
    
    NSString *ageGroup = [NSString stringWithUTF8String:user->age_group().c_str()];
    if ([ageGroup caseInsensitiveCompare:@"adult"] == NSOrderedSame) {
        self.xboxProfile.ageGroup = XBLAgeGroupAdult;
    } else if ([ageGroup caseInsensitiveCompare:@"teen"] == NSOrderedSame) {
        self.xboxProfile.ageGroup = XBLAgeGroupTeen;
    } else {
        self.xboxProfile.ageGroup = XBLAgeGroupChild;
    }
}

- (string_t)currentLocale {
    NSLocale *currentLocale = [NSLocale currentLocale];  // get the current locale.
    NSString *countryCode = [currentLocale objectForKey:NSLocaleCountryCode];
    return string_t([countryCode UTF8String]);
}

- (void)loadUserXboxProfileWithCompletionBlock:(void (^)(XBLXboxProfile *, NSError *))completionBlock {
    // Call completion to provide partial information
    completionBlock(self.xboxProfile, nil);
    
    auto result = self->_bridge->get_xbox_profile();
    if(result.err())
    {
        NSError *error = [NSError errorWithSTDError:result.err() message:result.err_message()];
        completionBlock(nil, error);
        return;
    }
    
    social::xbox_user_profile payload = result.payload();
    self.xboxProfile.gamerscore = atoi(payload.gamerscore().c_str());
    
    completionBlock(self.xboxProfile, nil);
    
    string_t displayPicRaw = payload.game_display_picture_resize_uri().to_string();
    [self loadGamerpicWithDisplayPicRaw:displayPicRaw forXboxProfile:self.xboxProfile completionBlock:completionBlock];
}

- (void)checkGamertagValidity:(NSString *)gamertag withCompletionBlock:(void (^)(BOOL, NSError *))completionBlock {
    string_t gamertag_t = string_t([gamertag UTF8String]);
    
    auto result = self->_bridge->check_gamertag(gamertag_t);
    
    if (result.err()) {
        NSError *error = [NSError errorWithSTDError:result.err() message:result.err_message()];
        completionBlock(NO, error);
        return;
    }
    
    BOOL isValid = result.payload();
    completionBlock(isValid, nil);
}

- (void)claimGamertag:(NSString *)gamertag withCompletionBlock:(void (^)(BOOL, NSError *))completionBlock {
    string_t gamertag_t = string_t([gamertag UTF8String]);
    
    auto result = self->_bridge->claim_gamertag(gamertag_t);
    if (result.err()) {
        NSError *error = [NSError errorWithSTDError:result.err() message:result.err_message()];
        completionBlock(NO, error);
        return;
    }
    
    BOOL isValid = result.payload();
    if (isValid) {
        self.xboxProfile.gamertag = gamertag;
    }
    completionBlock(isValid, nil);
}

- (void)loadNextAvailableGamertagsForSeed:(NSString *)baseGamertag withCompletionBlock:(void (^)(NSArray *, NSError *))completionBlock {
    string_t seed = string_t([baseGamertag UTF8String]);
    string_t locale = [self currentLocale];
    
    
    auto result = self->_bridge->get_gamertag_suggestions(seed, locale);
    if (result.err()) {
        NSError *error = [NSError errorWithSTDError:result.err() message:result.err_message()];
        completionBlock(nil, error);
        return;
    }
    
    NSMutableArray *gamertags = [NSMutableArray array];
    for (string_t gamertag : result.payload()) {
        [gamertags addObject:[NSString stringWithUTF8String:gamertag.c_str()]];
    }
    
    completionBlock(gamertags, nil);
}

- (void)provisionUserXuidWithCompletionBlock:(void (^)(BOOL, NSError *))completionBlock {
    string_t locale = [self currentLocale];

    auto result = self->_bridge->set_up_microsoft_profile(locale);
    if (result.err()) {
        NSError *error = [NSError errorWithSTDError:result.err() message:result.err_message()];
        completionBlock(NO, error);
        return;
    }
    
    self.microsoftProfile = [XboxProvider microsoftProfileWithData:result.payload()];
    completionBlock(YES, nil);
}

- (void)configureRealNameSharingSettings {
    if (self.xboxProfile.ageGroup == XBLAgeGroupChild) {
        return;
    }
    
    self->_bridge->set_up_real_name_settings();
}

- (void)loadUserMicrosoftProfileWithCompletionBlock:(void (^)(XBLMicrosoftProfile *, NSError *))completionBlock {
    if (self.microsoftProfile) {
        completionBlock(self.microsoftProfile, nil);
        if (!self.microsoftProfile.profileImage) {
            [self loadMicrosoftImageForProfile:self.microsoftProfile completionBlock:completionBlock];
        }
        return;
    }
    
    auto result = self->_bridge->get_microsoft_profile(false);
    if (result.err()) {
        NSError *error = [NSError errorWithSTDError:result.err() message:result.err_message()];
        completionBlock(nil, error);
    }
    
    XBLMicrosoftProfile *profile = [XboxProvider microsoftProfileWithData:result.payload()];
    self.microsoftProfile = profile;
    completionBlock(profile, nil);
    [self loadMicrosoftImageForProfile:profile completionBlock:completionBlock];
}

#pragma mark - Private
- (void)loadGamerpicWithDisplayPicRaw:(string_t)displayPicRaw forXboxProfile:(XBLXboxProfile *)profile completionBlock:(void (^)(XBLXboxProfile *, NSError *))completionBlock {
    NSString *rawURL = [[NSString stringWithUTF8String:displayPicRaw.c_str()] xblRawURLForMediumGamerpic];
    
    if (rawURL) {
        NSURL *url = [NSURL URLWithString:rawURL];
        [[[XboxProvider sharedXboxProviderURLSession] dataTaskWithURL:url completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
            if (!error && data) {
                profile.gamerpic = [UIImage imageWithData:data];
            }
            completionBlock(profile, error);
        }] resume];
    }
}

- (void)loadMicrosoftImageForProfile:(XBLMicrosoftProfile *)profile completionBlock:(void (^)(XBLMicrosoftProfile *, NSError *))completionBlock {
    if (profile.imageUrl) {
        NSURL *url = [NSURL URLWithString:profile.imageUrl];
        [[[XboxProvider sharedXboxProviderURLSession] dataTaskWithURL:url completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
            if (!error && data) {
                profile.profileImage = [UIImage imageWithData:data];
            }
            completionBlock(profile, error);
        }] resume];
    }
}

- (void)setMicrosoftProfile:(XBLMicrosoftProfile *)microsoftProfile {
    if (_microsoftProfile && microsoftProfile) {
        [_microsoftProfile marshalDataFromProfile:microsoftProfile];
    } else {
        _microsoftProfile = microsoftProfile;
    }
}

@end
