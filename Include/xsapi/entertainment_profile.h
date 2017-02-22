// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if TV_API || UNIT_TEST_SERVICES
#pragma once
#include "xbox_live_app_config.h"
#include "xsapi/marketplace.h"

namespace xbox { namespace services { 
    /// <summary>
    /// Classes for managing the entertainment profile
    /// </summary>
    namespace entertainment_profile {

class entertainment_profile_list_contains_item_result
{
public:
    _XSAPIIMP bool is_contained() const;

    _XSAPIIMP const string_t& provider_id() const;

    _XSAPIIMP const string_t& provider() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<entertainment_profile_list_contains_item_result> _Deserialize(_In_ const web::json::value& json);

private:
    bool m_isContained;
    string_t m_providerId;
    string_t m_provider;
};

/// <summary>
/// Represents an endpoint that you can use to access the Entertainment Profile.
/// </summary>
class entertainment_profile_list_xbox_one_pins
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
    pplx::task<xbox::services::xbox_live_result<void>> add_item(
        _In_ xbox::services::marketplace::media_item_type itemType,
        _In_ const string_t& providerId,
        _In_ const string_t& provider,
        _In_ const web::uri& imageUrl,
        _In_ const web::uri& providerLogoImageUrl,
        _In_ const string_t& title,
        _In_ const string_t& subTitle,
        _In_ const string_t& locale
        );

    /// <summary>
    /// Removes an item from the video queue.
    /// </summary>
    /// <param name="providerId">The Id that a provider (3rd party app) uses to identify a piece of content.</param>
    /// <param name="provider">The string used to identify the provider of the content.</param>
    /// <remarks>Calls V2 POST /users/xuid({xuid})/lists/PINS/XBLPins/RemoveItems</remarks>
    pplx::task<xbox::services::xbox_live_result<void>> remove_item(
        _In_ const string_t& providerId,
        _In_ const string_t& provider
        );

    /// <summary>
    /// Determines if an item exists in the video queue.
    /// </summary>
    /// <param name="providerId">The Id that a provider (3rd party app) uses to identify a piece of content.</param>
    /// <param name="provider">The string used to identify the provider of the content.</param>
    /// <remarks>Calls V2 POST /users/xuid({xuid})/lists/PINS/XBLPins/ContainsItems</remarks>
    pplx::task<xbox::services::xbox_live_result<entertainment_profile_list_contains_item_result>> contains_item(
        _In_ const string_t& providerId,
        _In_ const string_t& provider
        );
private:
    entertainment_profile_list_xbox_one_pins() {};

    entertainment_profile_list_xbox_one_pins(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    // Entertainment Profile endpoints
    static string_t entertainment_profile_list_xbox_one_pins_add_item_sub_path(
        _In_ const string_t& xboxUserId
        );

    static string_t entertainment_profile_list_xbox_one_pins_remove_item_sub_path(
        _In_ const string_t& xboxUserId
        );

    static string_t entertainment_profile_list_xbox_one_pins_contains_item_sub_path(
        _In_ const string_t& xboxUserId
        );

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;

    friend class entertainment_profile_list_service;
};

/// <summary>
/// Represents an endpoint that you can use to access the Entertainment Profile.
/// </summary>
class entertainment_profile_list_service
{
public:
    entertainment_profile_list_xbox_one_pins& xbox_one_pins();

private:
    entertainment_profile_list_service() {};

    entertainment_profile_list_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;
    entertainment_profile_list_xbox_one_pins m_xboxOnePins;

    friend class xbox_live_context_impl;
};

}}}
#endif
