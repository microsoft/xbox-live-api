// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbl_guid.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

xsapi_internal_string generate_guid()
{
    
    NSUUID* guid = [NSUUID UUID];
    NSString* guidNSString = [guid UUIDString];
    return guidNSString.UTF8String;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
