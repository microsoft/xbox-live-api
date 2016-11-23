//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#if TV_API || UNIT_TEST_SERVICES
#include "EntertainmentProfileListVideoQueue_WinRT.h"

using namespace xbox::services;
using namespace xbox::services::entertainment_profile;

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_BEGIN

EntertainmentProfileListXboxOnePins::EntertainmentProfileListXboxOnePins( 
    _In_ xbox::services::entertainment_profile::entertainment_profile_list_xbox_one_pins cppObj
    ) :
    m_cppObj(cppObj)
{
}

Windows::Foundation::IAsyncAction^
EntertainmentProfileListXboxOnePins::AddItemAsync(
    _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType itemType,
    _In_ Platform::String^ providerId,
    _In_ Platform::String^ provider,
    _In_opt_ Windows::Foundation::Uri^ imageUrl,
    _In_opt_ Windows::Foundation::Uri^ providerLogoImageUrl,
    _In_opt_ Platform::String^ title,
    _In_opt_ Platform::String^ subTitle,
    _In_ Platform::String^ locale
    )
{
    auto task = m_cppObj.add_item(
        static_cast<xbox::services::marketplace::media_item_type>(itemType),
        STRING_T_FROM_PLATFORM_STRING(providerId),
        STRING_T_FROM_PLATFORM_STRING(provider),
        web::uri(STRING_T_FROM_PLATFORM_STRING(imageUrl->AbsoluteUri)),
        web::uri(STRING_T_FROM_PLATFORM_STRING(providerLogoImageUrl->AbsoluteUri)),
        STRING_T_FROM_PLATFORM_STRING(title),
        STRING_T_FROM_PLATFORM_STRING(subTitle),
        STRING_T_FROM_PLATFORM_STRING(locale))
    .then([](xbox_live_result<void> cppResult)
    {
        THROW_IF_ERR(cppResult);
    });

    return ASYNC_FROM_TASK(task);
}


Windows::Foundation::IAsyncAction^
EntertainmentProfileListXboxOnePins::RemoveItemAsync(
    _In_ Platform::String^ providerId,
    _In_ Platform::String^ provider
    )
{
    auto task = m_cppObj.remove_item(
        STRING_T_FROM_PLATFORM_STRING(providerId),
        STRING_T_FROM_PLATFORM_STRING(provider))
    .then([](xbox_live_result<void> cppResult)
    {
        THROW_IF_ERR(cppResult);
    });

    return ASYNC_FROM_TASK(task);
}


Windows::Foundation::IAsyncOperation<EntertainmentProfileListContainsItemResult^>^
EntertainmentProfileListXboxOnePins::ContainsItemAsync(
    _In_ Platform::String^ providerId,
    _In_ Platform::String^ provider
    )
{
    auto task = m_cppObj.contains_item(
        STRING_T_FROM_PLATFORM_STRING(providerId),
        STRING_T_FROM_PLATFORM_STRING(provider))
    .then([](xbox_live_result<entertainment_profile_list_contains_item_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new EntertainmentProfileListContainsItemResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_END
#endif
