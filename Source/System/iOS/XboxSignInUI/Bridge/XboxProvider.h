//
//  XboxProvider.h
//  XboxLiveServices
//
//  Created by Sam Walker on 2/1/16.
//  Copyright © 2016 Microsoft Corporation. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "user_auth_ios.h"
#import <XSAPITCUI/XSAPITCUI.h>

@class XBLMicrosoftProfile;
@class XBLXboxProfile;
// We get back all of the information we want for the microsoft profile in our call to get user profile
// so we have no need for a separate microsoft provider class
@interface XboxProvider : NSObject <XBLIDPServiceProvider, XBLMicrosoftServiceProvider>

@property (nonatomic, strong, readonly) XBLMicrosoftProfile *microsoftProfile;
@property (nonatomic, strong, readonly) XBLXboxProfile *xboxProfile;

- (void)reset;
- (void)setUser:(std::shared_ptr<xbox::services::system::user_auth_ios>)user;
@end
