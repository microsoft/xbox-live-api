// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#if TV_API || UNIT_TEST_SERVICES
#include "xsapi/entertainment_profile.h"
#include "MediaItemType_WinRT.h"
#include "EntertainmentProfileListContainsItemResult_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_BEGIN

public ref class EntertainmentProfileListXboxOnePins sealed
{
public:
    /// <summary>
    /// Adds an item to the end of the video queue.
    /// </summary>
    /// <param name="itemType">The type of item being added to the video queue (Movie, TelevisionShow, TelevisionSeries, or TelevisionSeason).</param>
    /// <param name="providerId">The Id that a provider (3rd party app) uses to identify a piece of content.</param>
    /// <param name="provider">The string used to identify the provider of the content.</param>
    /// <param name="imageUrl">A Url pointing to a box shot, cover art, etc that is representative of the content. (Optional)</param>
    /// <param name="providerLogoImageUrl">A Url for the provider's logo image. (Optional)</param>
    /// <param name="title">The title of the item. (Optional)</param>
    /// <param name="subTitle">The subtitle of the item. (Optional)</param>
    /// <param name="locale">The locale of the item.</param>
    /// <remarks>Calls V2 POST /users/xuid({xuid})/lists/PINS/XBLPins</remarks>
    Windows::Foundation::IAsyncAction^ AddItemAsync(
        _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType itemType,
        _In_ Platform::String^ providerId,
        _In_ Platform::String^ provider,
        _In_opt_ Windows::Foundation::Uri^ imageUrl,
        _In_opt_ Windows::Foundation::Uri^ providerLogoImageUrl,
        _In_opt_ Platform::String^ title,
        _In_opt_ Platform::String^ subTitle,
        _In_ Platform::String^ locale
        );

    /// <summary>
    /// Removes an item from the video queue.
    /// </summary>
    /// <param name="providerId">The Id that a provider (3rd party app) uses to identify a piece of content.</param>
    /// <param name="provider">The string used to identify the provider of the content.</param>
    /// <remarks>Calls V2 POST /users/xuid({xuid})/lists/PINS/XBLPins/RemoveItems</remarks>
    Windows::Foundation::IAsyncAction^ RemoveItemAsync(
        _In_ Platform::String^ providerId,
        _In_ Platform::String^ provider
        );

    /// <summary>
    /// Determines if an item exists in the video queue.
    /// </summary>
    /// <param name="providerId">The Id that a provider (3rd party app) uses to identify a piece of content.</param>
    /// <param name="provider">The string used to identify the provider of the content.</param>
    /// <remarks>Calls V2 POST /users/xuid({xuid})/lists/PINS/XBLPins/ContainsItems</remarks>
    Windows::Foundation::IAsyncOperation<EntertainmentProfileListContainsItemResult^>^ ContainsItemAsync(
        _In_ Platform::String^ providerId,
        _In_ Platform::String^ provider
        );

internal:
    EntertainmentProfileListXboxOnePins( 
        _In_ xbox::services::entertainment_profile::entertainment_profile_list_xbox_one_pins cppObj
        );

private:
    xbox::services::entertainment_profile::entertainment_profile_list_xbox_one_pins m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_END
#endif
