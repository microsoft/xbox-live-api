// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if TV_API|| UNIT_TEST_SERVICES
#pragma once
#include "xsapi/entertainment_profile.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_BEGIN

public ref class EntertainmentProfileListContainsItemResult sealed
{
public:
    /// <summary>
    /// Specifies if the item is contained in the queue
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsContained, is_contained, bool);

    /// <summary>
    /// The Id that a provider (3rd party app) uses to identify a piece of content
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ProviderId, provider_id);

    /// <summary>
    /// The string used to identify the provider of the content
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Provider, provider);

internal:
    EntertainmentProfileListContainsItemResult(xbox::services::entertainment_profile::entertainment_profile_list_contains_item_result cppObj);

private:
    xbox::services::entertainment_profile::entertainment_profile_list_contains_item_result m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_END
#endif
