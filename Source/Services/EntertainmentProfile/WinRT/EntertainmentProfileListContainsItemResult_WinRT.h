//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
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
