// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class xbox_live_context_impl;
/// <summary>
/// Contains classes and enumerations that let you retrieve
/// information about a player's Xbox Live profile.
/// </summary>
namespace social {

    /// <summary> 
    /// Represents a user's Xbox Live profile.
    /// </summary>
    class xbox_user_profile
    {
    public:
        /// <summary>Constructor for a user profile object.</summary>
        /// <param name="appDisplayName">The user's display name to be used in application UI.</param>
        /// <param name="appDisplayPictureResizeUri">Uri for the user's display picture to be used in application UI.</param>
        /// <param name="gameDisplayName">The user's display name to be used in game UI.</param>
        /// <param name="gameDisplayPictureResizeUri">Uri for the user's display picture to be used in games.</param>
        /// <param name="gamerscore">The user's Gamerscore.</param>
        /// <param name="gamertag">The user's Gamertag.</param>
        /// <param name="xboxUserId">The user's Xbox user ID.</param>
        _XSAPIIMP xbox_user_profile(
        _In_ string_t appDisplayName,
        _In_ web::uri appDisplayPictureResizeUri,
        _In_ string_t gameDisplayName,
        _In_ web::uri gameDisplayPictureResizeUri,
        _In_ string_t gamerscore,
        _In_ string_t gamertag,
        _In_ string_t xboxUserId
        );

        /// <summary>
        /// Internal function
        /// </summary>
        _XSAPIIMP xbox_user_profile();

        /// <summary>
        /// The user's display name to be used in application UI.  This value is privacy gated and could
        /// be a user's real name or their Gamertag.
        /// </summary>
        _XSAPIIMP const string_t& app_display_name() const;

        /// <summary>
        /// Uri for the user's display picture to be used in application UI.
        /// The Uri is a resizable Uri. It can be used to specify one of the following sizes and formats by appending &apos;&amp;format={format}&amp;w={width}&amp;h={height}:
        /// Format: png
        /// Width   Height
        /// 64      64
        /// 208     208
        /// 424     424
        /// </summary>
        _XSAPIIMP const web::uri& app_display_picture_resize_uri() const;

        /// <summary>
        /// The user's display name to be used in game UI.  This value is privacy gated and could
        /// be a user's real name or their Gamertag.
        /// </summary>
        _XSAPIIMP const string_t& game_display_name() const;

        /// <summary>
        /// Uri for the user's display picture to be used in games.
        /// The Uri is a resizable Uri. It can be used to specify one of the following sizes and formats by appending &apos;&amp;format={format}&amp;w={width}&amp;h={height}:
        /// Format: png
        /// Width   Height
        /// 64      64
        /// 208     208
        /// 424     424
        /// </summary>
        _XSAPIIMP const web::uri& game_display_picture_resize_uri() const;

        /// <summary>
        /// The user's Gamerscore.
        /// </summary>
        _XSAPIIMP const string_t& gamerscore() const;

        /// <summary>
        /// The user's Gamertag.
        /// </summary>
        _XSAPIIMP const string_t& gamertag() const;

        /// <summary>
        /// The user's Xbox user ID.
        /// </summary>
        _XSAPIIMP const string_t& xbox_user_id() const;

        /// <summary>
        /// Internal function
        /// </summary>
        static xbox_live_result<xbox_user_profile> _Deserialize(_In_ const web::json::value& json);

    private:
        string_t m_appDisplayName;
        web::uri m_appDisplayPictureResizeUri;
        string_t m_gameDisplayName;
        web::uri m_gameDisplayPictureResizeUri;
        string_t m_gamerscore;
        string_t m_gamertag;
        string_t m_xboxUserId;
    };

    /// <summary>
    /// Services that manage user profile.
    /// </summary>
    class profile_service
    {
    public:
        /// <summary>
        /// Gets a user profile for a specific Xbox user.
        /// </summary>
        /// <param name="xboxUserId">The Xbox User ID of the user to get the profile for.</param>
        /// <returns>
        /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
        /// The result of the asynchronous operation is an xbox_user_profile object.
        /// </returns>
        /// <remarks>Calls V2 GET /users/batch/profile/settings</remarks>
        _XSAPIIMP pplx::task<xbox::services::xbox_live_result<xbox_user_profile>> get_user_profile(
        _In_ string_t xboxUserId
        );

        /// <summary>
        /// Gets one or more user profiles for a collection of specified Xbox users.
        /// </summary>
        /// <param name="xboxUserIds">The collection of Xbox User IDs of the users to get profiles for</param>
        /// <returns>
        /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
        /// The result of the asynchronous operation is a collection of xbox_user_profile objects.
        /// </returns>
        /// <remarks>Calls V2 GET /users/batch/profile/settings</remarks>
        _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<xbox_user_profile>>> get_user_profiles(
        _In_ const std::vector<string_t>& xboxUserIds
        );

        /// <summary>
        /// Gets user profiles for users in a specified social group.
        /// </summary>
        /// <param name="socialGroup">The name of the social group of users to search.
        /// See xbox::services::social::social_group_constants for the latest options.</param>
        /// <returns>
        /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
        /// The result of the asynchronous operation is a collection of xbox_user_profile objects.
        /// </returns>
        /// <remarks>Calls V2 GET /users/{userId}/profile/settings/people/{socialGroup}</remarks>
        _XSAPIIMP pplx::task<xbox::services::xbox_live_result< std::vector< xbox_user_profile>>> get_user_profiles_for_social_group(
        _In_ const string_t& socialGroup
        );

    private:
        profile_service() {};

        profile_service(
            _In_ std::shared_ptr<xbox::services::user_context> userContext,
            _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
            _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
            );

        static const string_t settings_query();

        static const string_t pathandquery_user_profiles_for_social_group(
        _In_ const string_t& socialGroup
        );

        static web::json::value serialize_settings_json();

        static const string_t SETTINGS_ARRAY[];

        static const web::json::value SETTINGS_SERIALIZED;

        static const string_t SETTINGS_QUERY;

        std::shared_ptr<xbox::services::user_context> m_userContext;
        std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
        std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;

        friend xbox_live_context_impl;
    };
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
