//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#import "XLSCll.h"
#include "xbox_cll.h"
#include "ms.onlineid_reflection.h"
#include "xbox_system_factory.h"
#import <UIKit/UIKit.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#define VER_PARTA 1
#define VER_PARTB 1
#define VER_PAGEVIEW 1
#define VER_PAGEACTION 1
#define VER_CLIENTERROR 1
#define VER_SERVICEERROR 1


@interface XLSCll()
@property (nonatomic) std::shared_ptr<iOSCll> cll;
@property (nonatomic) com::microsoft::xbox::idp::telemetry::utc::CommonData* commonData;
@property (nonatomic, strong) NSMutableDictionary *additionalData;
@property (nonatomic, strong) NSString *curPage;
@property (nonatomic, strong) NSString *prevPage;
@property (nonatomic, strong) NSString *sessionID;


@end

@implementation XLSCll

#pragma mark - Constructors

+(id)sharedTelemetryManager
{
    static XLSCll *mySharedTelemetryManager = nil;
    @synchronized(self) {
        if (mySharedTelemetryManager == nil)
            mySharedTelemetryManager = [[self alloc] init];
    }
    return mySharedTelemetryManager;
}

+ (void)apiSignInEvent:(BOOL)silentAPI withState:(NSString *)state
{
    if(silentAPI)
    {
        [[self sharedTelemetryManager] pageActionEvent:[NSString stringWithFormat:@"API - signin_silently - %@", state]];
    }
    else
    {
        [[self sharedTelemetryManager] pageActionEvent:[NSString stringWithFormat:@"API - signin - %@", state]];
    }
}


-(id)init
{
    if (self = [super init])
    {
        // Setup Common Data
        _commonData = new com::microsoft::xbox::idp::telemetry::utc::CommonData;
     
        NSString *sandboxID = [NSString stringWithCString:xbox::services::xbox_live_app_config::get_app_config_singleton()->sandbox().c_str() encoding:[NSString defaultCStringEncoding]];
        self.curPage = @"NA";
        self.prevPage = @"";
        self.sessionID = [[NSUUID UUID] UUIDString];
        
        _commonData->eventVersion = [self NSStringToWString:[NSString stringWithFormat:@"%d.%d.", VER_PARTA, VER_PARTB]];
        _commonData->deviceModel = [self NSStringToWString:[self getCurrentDeviceModel]];
        _commonData->clientLanguage = [self NSStringToWString:[[NSLocale preferredLanguages] objectAtIndex:0]];
        _commonData->network = 0;
        _commonData->sandboxId = [self NSStringToWString:sandboxID];
        _commonData->appSessionId = [self NSStringToWString:self.sessionID];
        _commonData->additionalInfo = L"";
        _commonData->appName = [self NSStringToWString:[[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleIdentifier"]];
        
        _commonData->userId = L"0";
        _commonData->accessibilityInfo = L"";
        
        _commonData->titleDeviceId = [self NSStringToWString:[NSString stringWithUTF8String:xbox::services::xbox_live_app_config::get_app_config_singleton()->title_telemetry_device_id().c_str()]];
        _commonData->titleSessionId = L"";

        _cll = xbox::services::xbox_cll::get_xbox_cll_singleton()->raw_cll();
        
        // Setup additional data
        self.additionalData = [NSMutableDictionary new];
        [self buildAccessibilitySettings];

    }
    return self;
}

-(void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    delete _commonData;
}

#pragma mark - Event Senders

- (int)pageViewEvent:(NSString *)toPageName
{
    return [self pageViewEvent:toPageName withData:nil];
}

- (int)pageViewEvent:(NSString *)toPageName withData:(NSDictionary *)data
{
    if (![self.curPage isEqualToString:toPageName])
    {
        self.prevPage = self.curPage;
        self.curPage = toPageName;
    }
    return [self pageViewEvent:toPageName withData:data fromPage:self.prevPage];
}

- (int)pageViewEvent:(NSString *)toPageName withData:(NSDictionary *)data fromPage:(NSString *)fromPageName
{
    com::microsoft::xbox::idp::telemetry::utc::PageView pageView;
    
    pageView.fromPage = [self NSStringToWString:fromPageName];
    pageView.pageName = [self NSStringToWString:toPageName];
    pageView.baseData = [self buildCommonData:VER_PAGEVIEW additionalInfo:data];
    
    if (![self.curPage isEqualToString:toPageName])
    {
        self.prevPage = fromPageName;
        self.curPage = toPageName;
    }
    
    int ret = _cll->log(pageView);
    

#ifdef DEBUG
    NSLog(@"XSLCll:PageView|FromPage: %@|To Page: %@|Data: %@", fromPageName, toPageName, data);
#endif
    return ret;
}

- (int)pageActionEvent:(NSString *)actionName
{
    return [self pageActionEvent:actionName withData:nil];
}

- (int)pageActionEvent:(NSString *)actionName withData:(NSDictionary *)data
{
    return [self pageActionEvent:actionName withData:data onPage:self.curPage];
}

- (int)pageActionEvent:(NSString *)actionName withData:(NSDictionary *)data onPage:(NSString *)currentPageName
{
    com::microsoft::xbox::idp::telemetry::utc::PageAction pageAction;
    
    pageAction.actionName = [self NSStringToWString:actionName];
    pageAction.pageName = [self NSStringToWString:currentPageName];
    pageAction.baseData = [self buildCommonData:VER_PAGEACTION additionalInfo:data];
    
    int ret = _cll->log(pageAction);
    
#ifdef DEBUG
    NSLog(@"XLSCll:PageAction|onPage:%@|actionName:%@|data:%@", currentPageName, actionName, data);
#endif
    
    return ret;
}

- (int)clientErrorEvent:(NSString *)errorName errorText:(NSString *)errorText errorCode:(NSString *)errorCode callStack:(NSString *)callStack
{
    return [self clientErrorEvent:errorName errorText:errorText errorCode:errorCode callStack:callStack withData:nil];
}
- (int)clientErrorEvent:(NSString *)errorName errorText:(NSString *)errorText errorCode:(NSString *)errorCode callStack:(NSString *)callStack withData:(NSDictionary *)data
{
    return [self clientErrorEvent:errorName errorText:errorText errorCode:errorCode callStack:callStack withData:data onPage:self.curPage];
}

- (int)clientErrorEvent:(NSString *)errorName errorText:(NSString *)errorText errorCode:(NSString *)errorCode callStack:(NSString *)callStack withData:(NSDictionary *)data onPage:(NSString *)currentPageName
{
    com::microsoft::xbox::idp::telemetry::utc::ClientError clientError;
    
    clientError.errorName = [self NSStringToWString:errorName];
    clientError.errorText = [self NSStringToWString:errorText];
    clientError.errorCode = [self NSStringToWString:errorCode];
    clientError.callStack = [self NSStringToWString:callStack];
    clientError.pageName = [self NSStringToWString:currentPageName];
    clientError.baseData = [self buildCommonData:VER_CLIENTERROR additionalInfo:data];
    
    int ret = _cll->log(clientError);

#ifdef DEBUG
    NSLog(@"XLSCll:ClientError|errorName:%@|errorText:%@|errorCode:%@|callStack:%@|data:%@|onPage:%@", errorName, errorText, errorCode, callStack, data, currentPageName);
#endif

    
    return ret;
}

- (int)serviceErrorEvent:(NSString *)errorName errorText:(NSString *)errorText errorCode:(NSString *)errorCode
{
    return [self serviceErrorEvent:errorName errorText:errorText errorCode:errorCode withData:nil];
}
- (int)serviceErrorEvent:(NSString *)errorName errorText:(NSString *)errorText errorCode:(NSString *)errorCode withData:(NSDictionary *)data
{
    return [self serviceErrorEvent:errorName errorText:errorText errorCode:errorCode withData:data onPage:self.curPage];
}

- (int)serviceErrorEvent:(NSString *)errorName errorText:(NSString *)errorText errorCode:(NSString *)errorCode withData:(NSDictionary *)data onPage:(NSString *)currentPageName
{
    com::microsoft::xbox::idp::telemetry::utc::ServiceError serviceError;
    
    serviceError.errorName = [self NSStringToWString:errorName];
    serviceError.errorText = [self NSStringToWString:errorText];
    serviceError.errorCode = [self NSStringToWString:errorCode];
    serviceError.pageName = [self NSStringToWString:currentPageName];
    serviceError.baseData = [self buildCommonData:VER_SERVICEERROR additionalInfo:data];
    
    int ret = _cll->log(serviceError);

#ifdef DEBUG
    NSLog(@"XLSCll:ServiceError|errorName:%@|errorText:%@|errorCode:%@|data:%@|onPage:%@", errorName, errorText, errorCode, data, currentPageName);
#endif

    return ret;
}
#pragma mark - Parameter Updating

- (void)setCurrentUser:(NSString *)xuid
{
    
    if ([xuid isEqualToString:@""])
    {
        _commonData->userId = L"0";
    }
    else
    {
        _commonData->userId = [self NSStringToWString:[NSString stringWithFormat:@"x:%@",xuid]];
    }
}

- (void)setTitleSessionId:(NSString *)sessionId
{
    _commonData->titleSessionId = [self NSStringToWString:sessionId];
}

- (void)setCurrentPage:(NSString *)pageName
{
    self.curPage = pageName;
}

- (void)setPreviousPage:(NSString *)pageName
{
    self.prevPage = pageName;
}

#pragma mark - Parameter Getters
- (NSString *)getAppSessionId
{
    if (!self.sessionID)
    {
        self.sessionID = [[NSUUID UUID] UUIDString];
    }
    return self.sessionID;
}

- (NSString *)getAppBundleName
{
    return [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleIdentifier"];
}


#pragma mark - implements Private helpers

- (com::microsoft::xbox::idp::telemetry::utc::CommonData)buildCommonData:(uint)eventVersion additionalInfo:(NSDictionary*)additionalInfo
{
    com::microsoft::xbox::idp::telemetry::utc::CommonData eventCommonData;
    
    // Update fields
    eventCommonData.network = [self getNetworkConnection];
    eventCommonData.additionalInfo = [self buildAdditionalInfo:additionalInfo];
    
    // Copy struct over
    eventCommonData.eventVersion = std::wstring(_commonData->eventVersion) + std::to_wstring(eventVersion);
    eventCommonData.deviceModel = _commonData->deviceModel;
    eventCommonData.clientLanguage = _commonData->clientLanguage;
    eventCommonData.sandboxId = _commonData->sandboxId;
    eventCommonData.appSessionId = _commonData->appSessionId;
    eventCommonData.accessibilityInfo = _commonData->accessibilityInfo;
    eventCommonData.userId = _commonData->userId;
    eventCommonData.titleSessionId = _commonData->titleSessionId;
    eventCommonData.titleDeviceId = _commonData->titleDeviceId;

    
    return eventCommonData;
}

- (std::wstring)buildAdditionalInfo:(NSDictionary *)data
{
    // Combine dictionary items
    NSMutableDictionary *combinedDictionary = [[NSMutableDictionary alloc] initWithDictionary:self.additionalData copyItems:YES];
    NSString *jsonString = @"";
    
    if (data != nil)
    {
        [combinedDictionary addEntriesFromDictionary: data];
    }
    
    if (combinedDictionary.count > 0)
    {
        // Convert to JSON
        NSError *error;
        
        NSData *jsonData = [NSJSONSerialization dataWithJSONObject:combinedDictionary
                                                           options:NSJSONWritingPrettyPrinted
                                                             error:&error];
        if (!jsonData)
        {
#ifdef DEBUG
            NSLog(@"Error Generating JSON String: %@", error);
#endif
        }
        else
        {
            jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
        }
    }
    
    return [self NSStringToWString:jsonString];
}

- (void)buildAccessibilitySettings
{
    NSMutableDictionary *dict = [[NSMutableDictionary alloc] init];
    [dict setObject:[NSNumber numberWithBool:UIAccessibilityIsBoldTextEnabled()] forKey:@"BoldText"];
    [dict setObject:[NSNumber numberWithBool:UIAccessibilityIsClosedCaptioningEnabled()] forKey:@"ClosedCaption"];
    [dict setObject:[NSNumber numberWithBool:UIAccessibilityDarkerSystemColorsEnabled()] forKey:@"DarkSystemColors"];
    [dict setObject:[NSNumber numberWithBool:UIAccessibilityIsGrayscaleEnabled()] forKey:@"Grayscale"];
    [dict setObject:[NSNumber numberWithBool:UIAccessibilityIsGuidedAccessEnabled()] forKey:@"GuidedAccess"];
    [dict setObject:[NSNumber numberWithBool:UIAccessibilityIsInvertColorsEnabled()] forKey:@"InvertedColors"];
    [dict setObject:[NSNumber numberWithBool:UIAccessibilityIsMonoAudioEnabled()] forKey:@"MonoAudio"];
    [dict setObject:[NSNumber numberWithBool:UIAccessibilityIsReduceMotionEnabled()] forKey:@"ReduceMotion"];
    [dict setObject:[NSNumber numberWithBool:UIAccessibilityIsReduceTransparencyEnabled()] forKey:@"ReduceTransparency"];
    [dict setObject:[NSNumber numberWithBool:UIAccessibilityIsSpeakScreenEnabled()] forKey:@"SpeakScreen"];
    [dict setObject:[NSNumber numberWithBool:UIAccessibilityIsSpeakSelectionEnabled()] forKey:@"SpeakSelection"];
    [dict setObject:[NSNumber numberWithBool:UIAccessibilityIsSwitchControlRunning()] forKey:@"SwitchControl"];
    [dict setObject:[NSNumber numberWithBool:UIAccessibilityIsVoiceOverRunning()] forKey:@"VoiceOver"];
    
    NSError *error;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dict
                                                       options:NSJSONWritingPrettyPrinted
                                                         error:&error];
    if (!jsonData)
    {
        self.commonData->accessibilityInfo = L"";
#ifdef DEBUG
        NSLog(@"Error Generating JSON String: %@", error);
#endif
    }
    else
    {
        self.commonData->accessibilityInfo = [self NSStringToWString:[[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding]];
    }
}

- (std::wstring)NSStringToWString:(NSString *)string
{
    NSStringEncoding encoding = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE);
    NSData *data = [string dataUsingEncoding:encoding];
    
    std::wstring wstring = std::wstring((wchar_t*)[data bytes], [data length] / sizeof(wchar_t));
    
    return wstring;
}

- (uint)getNetworkConnection
{
    uint networkType = 0;
    NSString* networkName = [[XBLIDPScenario sharedIDPScenario].reachability nameForNetworkConnection];
    if ([networkName isEqualToString:@"Cellular"])
    {
        networkType = 2;
    }
    else if ([networkName isEqualToString:@"Wifi"])
    {
        networkType = 1;
    }
    
    return networkType;
}

- (NSString *)getCurrentDeviceModel
{
    NSString *currentDeviceModel = nil;

    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *machine = (char *)malloc(size);
    sysctlbyname("hw.machine", machine, &size, NULL, 0);
    NSString *platform = [NSString stringWithCString:machine encoding:NSUTF8StringEncoding];
    free(machine);
    machine = NULL;
        
    currentDeviceModel = [platform stringByReplacingOccurrencesOfString:@"," withString:@"-"];
    
    return currentDeviceModel;
}


@end

