// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "local_storage.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

String LocalStorage::GetDefaultStoragePath()
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString* applicationSupportDirectory = [paths firstObject];
    if (applicationSupportDirectory.length == 0)
    {
        LOGS_DEBUG << "Failed to find application support directory.";
        assert(false);
    }

    NSString* storagePath = [NSString stringWithFormat:@"%@/", applicationSupportDirectory];

    NSError* createDirectoryError = nil;
    [[NSFileManager defaultManager] createDirectoryAtPath:storagePath
                              withIntermediateDirectories:YES
                                               attributes:nil
                                                    error:&createDirectoryError];
    if (createDirectoryError != nil)
    {
        LOGS_DEBUG << "Failed to create application support directory: " << createDirectoryError.localizedDescription.UTF8String;
        
        assert(false);
    }

    return storagePath.UTF8String;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
