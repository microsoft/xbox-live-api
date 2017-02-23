// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "xsapi\social.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN

/// <summary>
/// Contains constant data about social groups.
/// </summary>

public ref class SocialGroupConstants sealed
{
public:

    /// <summary>
    /// A string describing the social group's favorites.
    /// </summary>
    static property Platform::String^ Favorite 
    {
        Platform::String^ get()
        {
            return ref new Platform::String(xbox::services::social::social_group_constants::favorite().c_str());
        }
    }
    
    /// <summary>
    /// A string describing the people in the social group.
    /// </summary>

    static property Platform::String^ People 
    {
        Platform::String^ get() 
        {
            return ref new Platform::String(xbox::services::social::social_group_constants::people().c_str());
        }
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END