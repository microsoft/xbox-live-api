// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

    class xbox_live_context_impl;
    /// <summary>
    /// Contains classes and enumerations that perform contextual search on Xbox Live.
    /// </summary>
    namespace contextual_search {

/// <summary>Enumerates the visibility of a contextual search statistic.</summary>
enum class contextual_search_stat_visibility
{
    /// <summary>Unknown visibility.</summary>
    unknown,

    /// <summary>This is important, as third party applications like Twitch will only be able to view OPEN stats.</summary>
    public_visibility,

    /// <summary>If a stat is private, only the title itself, or a companion application that has special privileges, can access this information.</summary>
    private_visibility
};

/// <summary>Enumerates the display type of a contextual search statistic.</summary>
enum class contextual_search_stat_display_type
{
    /// <summary>Unknown type.</summary>
    unknown,

    /// <summary>This stat should be displayed as a undefined range.</summary>
    undefined_range,

    /// <summary>This stat should be displayed as a defined range.</summary>
    defined_range,

    /// <summary>This stat should be displayed using the value_to_display_name_map.</summary>
    set
};

/// <summary>Enumerates the operators of a contextual search filter.</summary>
enum class contextual_search_filter_operator
{
    /// <summary>Matches if filter name is equals to filter value.</summary>
    equal,

    /// <summary>Matches if filter name is not equal to filter value.</summary>
    not_equal,

    /// <summary>Matches if filter name is greater than filter value.</summary>
    greater_than,

    /// <summary>Matches if filter name is greater than or equal to filter value.</summary>
    greater_than_or_equal,

    /// <summary>Matches if filter name is less than filter value.</summary>
    less_than,

    /// <summary>Matches if filter name is less than or equal to filter value.</summary>
    less_than_or_equal,
};

/// <summary>Contains information about a contextual search stat.</summary>
class contextual_search_configured_stat
{
public:
    /// <summary>
    /// The actual name of the configured stat.  This is what you will use as a parameter in the search API.  
    /// </summary>
    _XSAPIIMP const string_t& name() const;

    /// <summary>
    /// The data type of the stat.  It should align with the values you are passing for the stat into the Search API.
    /// </summary>
    _XSAPIIMP const string_t& data_type() const;

    /// <summary>
    /// The visibility of the stat
    /// </summary>
    _XSAPIIMP contextual_search_stat_visibility visibility() const;

    /// <summary>
    /// This is the localizable string exposed to the end user. Depending on the language / culture you 
    /// pass up in the header, you will get a localized version of this string.
    /// </summary>
    _XSAPIIMP const string_t& display_name() const;

    /// <summary>
    /// Boolean on whether you can use this stat in a filter
    /// </summary>
    _XSAPIIMP bool can_be_filtered() const;

    /// <summary>
    /// Boolean on whether you can make sort queries
    /// </summary>
    _XSAPIIMP bool can_be_sorted() const;

    /// <summary>
    /// How this Stat should be represented, as either a Set, Defined Range, or Undefined Range
    /// </summary>
    _XSAPIIMP contextual_search_stat_display_type display_type() const;

    /// <summary>
    /// If the representation type is set, this contains a map to convert values to display names
    /// </summary>
    _XSAPIIMP const std::unordered_map<string_t, string_t>& value_to_display_name_map() const;

    /// <summary>
    /// If the representation type is defined range, this is the min range
    /// </summary>
    _XSAPIIMP uint64_t range_min() const;

    /// <summary>
    /// If the representation type is defined range, this is the max range
    /// </summary>
    _XSAPIIMP uint64_t range_max() const;

    contextual_search_configured_stat();

    /// <summary>
    /// Internal function
    /// </summary>
    contextual_search_configured_stat(
        _In_ string_t name,
        _In_ string_t dataType,
        _In_ contextual_search_stat_visibility visibility,
        _In_ string_t displayName,
        _In_ bool canBeFiltered,
        _In_ bool canBeSorted,
        _In_ contextual_search_stat_display_type displayType,
        _In_ std::unordered_map<string_t, string_t> valueToDisplayNameMap,
        _In_ uint64_t rangeMin,
        _In_ uint64_t rangeMax
        );

private:
    string_t m_name;
    string_t m_dataType;
    contextual_search_stat_visibility m_visibility;
    string_t m_displayName;
    bool m_canBeFiltered;
    bool m_canBeSorted;
    contextual_search_stat_display_type m_displayType;
    std::unordered_map<string_t, string_t> m_valueToDisplayNameMap;
    uint64_t m_rangeMin;
    uint64_t m_rangeMax;
};

/// <summary>Contains information about a contextual search broadcast.</summary>
class contextual_search_broadcast
{
public:
    /// <summary>
    /// The Xbox user ID of the player.
    /// </summary>
    _XSAPIIMP const string_t& xbox_user_id() const;

    /// <summary>
    /// Returns the broadcast provider, for example "Twitch". 
    /// This is leveraged to enable differing logic to handle content from different providers
    /// </summary>
    _XSAPIIMP const string_t& provider() const;

    /// <summary>
    /// The identifier of the broadcaster on the provider. This is leveraged to created deeplinks to the broadcast.  
    /// In the example of Twitch, this is the Twitch user id.
    /// </summary>
    _XSAPIIMP const string_t& broadcaster_id_from_provider() const;

    /// <summary>
    /// The current number of viewers in the broadcast, per the last update from Presence.
    /// </summary>
    _XSAPIIMP uint64_t viewers() const;

    /// <summary>
    /// The date value of when the broadcast started
    /// </summary>
    _XSAPIIMP const utility::datetime& started_date() const;

    /// <summary>
    /// The current stats the title has configured for Contextual Search and what their current values are.  
    /// Additional information on how to surface the information is contained in the configuration handler.  
    /// </summary>
    _XSAPIIMP const std::unordered_map<string_t, string_t>& current_stats() const;

    contextual_search_broadcast();

    /// <summary>
    /// Internal function
    /// </summary>
    contextual_search_broadcast(
        _In_ string_t xboxUserId,
        _In_ string_t provider,
        _In_ string_t broadcasterIdFromProvider,
        _In_ uint64_t viewers,
        _In_ utility::datetime startedDate,
        _In_ std::unordered_map<string_t, string_t> currentStats
        );

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<contextual_search_broadcast> _Deserialize(_In_ const web::json::value& inputJson);

private:
    string_t m_xboxUserId;
    string_t m_provider;
    string_t m_broadcasterIdFromProvider;
    uint64_t m_viewers;
    utility::datetime m_startedDate;
    std::unordered_map<string_t, string_t> m_currentStats;
};

/// <summary>Describes the type of game clip</summary>
enum class contextual_search_game_clip_type
{
    /// <summary>Game clip type is unknown or not set. (not valid for upload or queries)</summary>
    none,

    /// <summary>Game clip is initiated by a developer / title</summary>
    developer_initiated,

    /// <summary>Game clip is an achievement type</summary>
    achievement,

    /// <summary>Game clip is a user-generated type</summary>
    user_generated
};

/// <summary>Describes the type of thumbnail</summary>
enum class contextual_search_game_clip_thumbnail_type
{
    /// <summary>Thumbnail type is not specified. This is an invalid value in most cases.</summary>
    none,

    /// <summary>Specifies a small thumbnail. Small is 370 pixels in height by 208 pixels in width.</summary>
    small_thumbnail,

    /// <summary>Specifies a large thumbnail. Large is 754 pixels in height by 424 pixels in width.</summary>
    large_thumbnail
};

/// <summary>Describes the type of game clip URI.</summary>
enum class contextual_search_game_clip_uri_type
{
    /// <summary>Game clip URI type is not known or not set.</summary>
    none,

    /// <summary>Game clip URI is the location of the original game clip in unprocessed form.</summary>
    original,

    /// <summary>Game clip URI is the location of the clip available for download.</summary>
    download,

    /// <summary>Game clip URI is the IIS Smooth Streaming URL(manifest pointer).</summary>
    smooth_streaming,

    /// <summary>Game clip URI is the Apple HTTP Live Streaming URL.</summary>
    http_live_streaming
};

/// <summary>
/// The information related to an instance of a game clip video asset. 
/// Since there can be multiple representations for a clip, it is up to the client to select the most appropriate one for playback.
/// </summary>
class contextual_search_game_clip_uri_info
{
public:
    /// <summary>
    /// The URI to the location of the video asset.
    /// </summary>
    _XSAPIIMP const web::uri& url() const;

    /// <summary>
    /// The file size at the location of the URL for formats that are downloaded
    /// This is 0 for streaming formats
    /// </summary>
    _XSAPIIMP uint64_t file_size() const;

    /// <summary>
    /// Specifies the type of the URI.
    /// </summary>
    _XSAPIIMP contextual_search_game_clip_uri_type uri_type() const;

    /// <summary>
    /// The expiration time of the URI that is included in this response.
    /// </summary>
    _XSAPIIMP const utility::datetime& expiration() const;

    /// <summary>
    /// Internal function
    /// </summary>
    contextual_search_game_clip_uri_info();

    /// <summary>
    /// Internal function
    /// </summary>
    contextual_search_game_clip_uri_info(
        _In_ web::uri url,
        _In_ uint64_t fileSize,
        _In_ contextual_search_game_clip_uri_type uriType,
        _In_ utility::datetime expiration
        );

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<contextual_search_game_clip_uri_info> _Deserialize(_In_ const web::json::value& inputJson);

private:
    static contextual_search_game_clip_uri_type convert_string_to_clip_uri_type(_In_ const string_t& value);

    web::uri m_url;
    uint64_t m_fileSize;
    contextual_search_game_clip_uri_type m_uriType;
    utility::datetime m_expiration;
};

/// <summary>
/// Contains the information related to an individual thumbnail. 
/// There can be multiple sizes per clip and it is up to the client to select the proper one for display.
/// </summary>
class contextual_search_game_clip_thumbnail
{
public:
    /// <summary>
    /// The URI for the thumbnail image.
    /// </summary>
    _XSAPIIMP const web::uri& url() const;

    /// <summary>
    /// The total file size of the thumbnail image.
    /// </summary>
    _XSAPIIMP uint64_t file_size() const;

    /// <summary>
    /// The type of thumbnail image.
    /// </summary>
    _XSAPIIMP contextual_search_game_clip_thumbnail_type thumbnail_type() const;

    /// <summary>
    /// Internal function
    /// </summary>
    contextual_search_game_clip_thumbnail();

    /// <summary>
    /// Internal function
    /// </summary>
    contextual_search_game_clip_thumbnail(
        _In_ web::uri url,
        _In_ uint64_t fileSize,
        _In_ contextual_search_game_clip_thumbnail_type thumbnailType
        );

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<contextual_search_game_clip_thumbnail> _Deserialize(_In_ const web::json::value& inputJson);

private:
    static contextual_search_game_clip_thumbnail_type convert_string_to_thumbnail_type(_In_ const string_t& value);

    web::uri m_url;
    uint64_t m_fileSize;
    contextual_search_game_clip_thumbnail_type m_thumbnailType;
};

class contextual_search_game_clip_stat
{
public:
    /// <summary>
    /// Name of the stat
    /// </summary>
    _XSAPIIMP const string_t& stat_name() const;

    /// <summary>
    /// Value of the stat for string stats and integer stats representing a set value.
    /// If the stat changed during the clip, the field will have multiple values separated by a comma.
    /// If the "value" field is present, "min", "max" and "delta" fields will not be present.
    /// </summary>
    _XSAPIIMP const string_t& value() const;

    /// <summary>
    /// The minimum value the stat had during the recording of the game clip.
    /// If the "min" field is present, the "value" field will not be present.
    /// </summary>
    _XSAPIIMP const string_t& min_value() const;

    /// <summary>
    /// The maximum value the stat had during the recording of the game clip.
    /// If the "max" field is present, the "value" field will not be present.
    /// </summary>
    _XSAPIIMP const string_t& max_value() const;

    /// <summary>
    /// "max"–"min". If the delta field is present, the "value" field will not be present.
    /// </summary>
    _XSAPIIMP const string_t& delta_value() const;

    /// <summary>
    /// Internal function
    /// </summary>
    contextual_search_game_clip_stat();

    /// <summary>
    /// Internal function
    /// </summary>
    contextual_search_game_clip_stat(
        _In_ string_t statName,
        _In_ string_t value,
        _In_ string_t minValue,
        _In_ string_t maxValue,
        _In_ string_t deltaValue
        );

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<contextual_search_game_clip_stat> _Deserialize(_In_ const web::json::value& inputJson);

private:
    string_t m_statName;
    string_t m_value;
    string_t m_minValue;
    string_t m_maxValue;
    string_t m_deltaValue;
};


/// <summary>
/// Contains all the relevant metadata for a video.
/// </summary>
class contextual_search_game_clip
{
public:
    /// <summary>
    /// The localized version of the clip's name based on the input locale of the request as looked up from the title management system.
    /// </summary>
    _XSAPIIMP const string_t& clip_name() const;

    /// <summary>
    /// Specifies the duration of the game clip in seconds.
    /// </summary>
    _XSAPIIMP uint64_t duration_in_seconds() const;

    /// <summary>
    /// The ID assigned to the game clip.
    /// </summary>
    _XSAPIIMP const string_t& game_clip_id() const;

    /// <summary>
    /// The locale the game clip was created under.
    /// </summary>
    _XSAPIIMP const string_t& game_clip_locale() const;

    /// <summary>
    /// Array of playback options for this clip.
    /// </summary>
    _XSAPIIMP const std::vector<contextual_search_game_clip_uri_info>& game_clip_uris() const;

    /// <summary>
    /// Array of thumbnails for this clip.
    /// </summary>
    _XSAPIIMP const std::vector<contextual_search_game_clip_thumbnail>& thumbnails() const;

    /// <summary>
    /// The type of clip. Can be multiple values and if so will be comma delimited.
    /// </summary>
    _XSAPIIMP contextual_search_game_clip_type game_clip_type() const;

    /// <summary>
    /// The number of views associated with a game clip.
    /// </summary>
    _XSAPIIMP uint64_t views() const;

    /// <summary>
    /// The XUID of the owner of the game clip
    /// </summary>
    _XSAPIIMP const string_t& xbox_user_id() const;

    /// <summary>
    /// Stats associated with the game clip. 
    /// This contains the initial values of stats at the start of the game clip and any new value a stat had during the recording of the game clip. 
    /// Note that these are only the stats configured for intermedia in the title configuration.
    /// </summary>
    _XSAPIIMP const std::vector<contextual_search_game_clip_stat>& stats() const;

    /// <summary>
    /// Internal function
    /// </summary>
    contextual_search_game_clip();

    /// <summary>
    /// Internal function
    /// </summary>
    contextual_search_game_clip(
        _In_ string_t clipName,
        _In_ uint64_t durationInSeconds,
        _In_ string_t gameClipId,
        _In_ string_t gameClipLocale,
        _In_ std::vector<contextual_search_game_clip_uri_info> gameClipUris,
        _In_ std::vector<contextual_search_game_clip_thumbnail> thumbnails,
        _In_ contextual_search_game_clip_type gameClipType,
        _In_ uint64_t views,
        _In_ string_t xboxUserId,
        _In_ std::vector<contextual_search_game_clip_stat> stats
        );

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<contextual_search_game_clip> _Deserialize(_In_ const web::json::value& inputJson);

private:
    static string_t convert_xuid_to_string(_In_ uint64_t xuid, _In_ string_t xuidName);
    static contextual_search_game_clip_type convert_string_to_clip_type(_In_ const string_t& value);

    string_t m_clipName;
    uint64_t m_durationInSeconds;
    string_t m_gameClipId;
    string_t m_gameClipLocale;
    std::vector<contextual_search_game_clip_uri_info> m_gameClipUris;
    std::vector<contextual_search_game_clip_thumbnail> m_thumbnails;
    contextual_search_game_clip_type m_gameClipType;
    uint64_t m_views; 
    string_t m_xboxUserId;
    std::vector<contextual_search_game_clip_stat> m_stats;
};

// Returns the list of returned game clips along with the corresponding paging information
class contextual_search_game_clips_result
{
public:
    /// <summary>
    /// Returns the next page of game clips.
    /// </summary>
    /// <param name="maxItems">The maximum number of game clips the result can contain.  Pass 0 to attempt to retrieve the top 100 items.</param>
    _XSAPIIMP pplx::task<xbox_live_result<contextual_search_game_clips_result>> get_next(
        _In_ uint32_t maxItems
        );

    /// <summary>
    /// Array of game clips returned.
    /// </summary>
    _XSAPIIMP const std::vector<contextual_search_game_clip>& items() const;

    /// <summary>
    /// Internal function
    /// </summary>
    contextual_search_game_clips_result();

    /// <summary>
    /// Internal function
    /// </summary>
    contextual_search_game_clips_result(
        _In_ std::vector<contextual_search_game_clip> clips
        );

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<contextual_search_game_clips_result> _Deserialize(_In_ const web::json::value& json);

    /// <summary>
    /// Internal function
    /// </summary>
    void _Initialize(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
        _In_ uint32_t titleId,
        _In_ uint32_t skipItems
        );

private:
    std::vector<contextual_search_game_clip> m_clips;
    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;
    uint32_t m_titleId;
    uint32_t m_skipItems;
};

/// <summary>
/// Provides methods to perform Xbox Live contextual search.
/// </summary>
class contextual_search_service
{
public:
    /// <summary>
    /// Get information to hydrate a UI for the end user to choose what data from a game they want to use 
    /// to filter or sort the broadcast
    /// </summary>
    /// <param name="titleId">The title ID.</param>
    /// <returns>
    /// The result contains information to hydrate a UI.
    /// </returns>
    /// <remarks>Calls V1 GET /titles/{titleid}/configuration</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<std::vector<contextual_search_configured_stat>>> get_configuration(
        _In_ uint32_t titleId
        );

    /// <summary>
    /// Get information to hydrate a UI for the end user to choose what data from a game they want to use 
    /// to filter or sort the broadcast.  Defaults to the top 100 broadcasts ordered by viewers descending with no filter
    /// </summary>
    /// <param name="titleId">The title ID.</param>
    /// <returns>
    /// The result is a contextual_search_broadcasts_result object which contains information about broadcasts.
    /// </returns>
    /// <remarks>Calls V1 GET /titles/{titleId}/broadcasts</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<std::vector<contextual_search_broadcast>>> get_broadcasts(
        _In_ uint32_t titleId
        );

    /// <summary>
    /// Get information to hydrate a UI for the end user to choose what data from a game they want to use 
    /// to filter or sort the broadcast
    /// </summary>
    /// <param name="titleId">The title ID.</param>
    /// <param name="skipItems">The number of broadcasts to skip.</param>
    /// <param name="maxItems">The maximum number of broadcasts the result can contain.  Pass 0 to attempt to retrieve the top 100 items.</param>
    /// <param name="orderByStatName">This specifies a stat to sort the broadcasts by.  Pass empty string to default to ordering by viewers descending</param>
    /// <param name="orderAscending">Pass true to order ascending, false to order descending</param>
    /// <param name="searchQuery">This specifies a OData Filter Expressions string that specifies the stats to filter on.
    /// For example "stats:maptype eq 1" means filtering by the stat "maptype" to the value 1.
    /// Be sure to wrap any user data values in single quotes and escape single quotes with 2 single quotes.
    /// For example, "player eq 'Killer''s Hand'"</param>
    /// <returns>
    /// The result is a contextual_search_broadcasts_result object which contains information about broadcasts.
    /// </returns>
    /// <remarks>Calls V1 GET /titles/{titleId}/broadcasts</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<std::vector<contextual_search_broadcast>>> get_broadcasts(
        _In_ uint32_t titleId,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ const string_t& orderByStatName,
        _In_ bool orderAscending,
        _In_ const string_t& searchQuery
        );

    /// <summary>
    // Get information to hydrate a UI that allows end users to filter or sort broadcasts
    /// </summary>
    /// <param name="titleId">The title ID.</param>
    /// <param name="skipItems">The number of broadcasts to skip.</param>
    /// <param name="maxItems">The maximum number of broadcasts the result can contain.  Pass 0 to attempt to retrieve the top 100 items.</param>
    /// <param name="orderByStatName">This specifies a stat to sort the broadcasts by.  Pass empty string to default to ordering by viewers descending</param>
    /// <param name="orderAscending">Pass true to order ascending, false to order descending</param>
    /// <param name="filterStatName">This specifies the stat to filter on. Pass empty string for no filter</param>
    /// <param name="filterOperator">This specifies operator to apply for the filter.</param>
    /// <param name="filterStatValue">This specifies the value to filter on. Pass empty string for no filter</param>
    /// <returns>
    /// The result is a contextual_search_broadcasts_result object which contains information about broadcasts.
    /// </returns>
    /// <remarks>Calls V1 GET /titles/{titleId}/broadcasts</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<std::vector<contextual_search_broadcast>>> get_broadcasts(
        _In_ uint32_t titleId,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ const string_t& orderByStatName,
        _In_ bool orderAscending,
        _In_ const string_t& filterStatName,
        _In_ contextual_search_filter_operator filterOperator,
        _In_ const string_t& filterStatValue
        );

    /// <summary>
    /// Get information to hydrate a UI that allows end users to filter or sort game clips
    /// </summary>
    /// <param name="titleId">The title ID.</param>
    /// <param name="skipItems">The number of game clips to skip.</param>
    /// <param name="maxItems">The maximum number of game clips the result can contain.  Pass 0 to attempt to retrieve the top 10 items.  The max is 20</param>
    /// <param name="orderByStatName">This specifies a stat to sort the game clips by.  Pass empty string to default to ordering by viewers descending</param>
    /// <param name="orderAscending">Pass true to order ascending, false to order descending</param>
    /// <param name="filterStatName">This specifies the stat to filter on. Pass empty string for no filter. For example, "gametype.value".
    /// The "value" property is only available for string stats and integer stats. 
    /// The "min", "max" and "delta" properties are only available for double stats and integer stats.
    /// </param>
    /// <param name="filterOperator">This specifies operator to apply for the filter.</param>
    /// <param name="filterStatValue">This specifies the value to filter on. The stat properties that can be used to search with are 'value', 'min', 'max', and 'delta'. Pass empty string for no filter</param>
    /// <returns>
    /// The result is an object which contains information about game clips.
    /// </returns>
    /// <remarks>Calls V1 GET /titles/{titleId}/gameclips</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<contextual_search_game_clips_result>> get_game_clips(
        _In_ uint32_t titleId,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ const string_t& orderByStatName,
        _In_ bool orderAscending,
        _In_ const string_t& filterStatName,
        _In_ contextual_search_filter_operator filterOperator,
        _In_ const string_t& filterStatValue
        );

    /// <summary>
    /// Get information to hydrate a UI that allows end users to filter or sort game clips
    /// </summary>
    /// <param name="titleId">The title ID.</param>
    /// <param name="skipItems">The number of game clips to skip.</param>
    /// <param name="maxItems">The maximum number of game clips the result can contain.  Pass 0 to attempt to retrieve the top 10 items.  The max is 20</param>
    /// <param name="orderByStatName">This specifies a stat to sort the game clips by.  Pass empty string to default to ordering by viewers descending</param>
    /// <param name="orderAscending">Pass true to order ascending, false to order descending</param>
    /// <param name="searchQuery">
    /// The search query.
    /// The query syntax is an OData like syntax with only the following operators supported EQ, NE, GE, GT, LE and LT along with the logical operators of AND and OR. You can also use grouping operator '()'. The stat properties that can be used to search with are "value", "min", "max", and "delta".
    ///
    /// Example 1:
    /// To search for game clips during which the "gametype" stat had a value of 1 use
    ///     "stats:gametype.value eq 1"
    ///
    /// Example 2:
    /// To search for game clips during which the "rank" stat jumped more than 2.
    ///     "stats:rank.value gt 2"
    ///
    /// Example 3:
    /// To search for game clips during which the "rank" stat jumped more than 2 and the "gametype" stat had a value of 1 use
    ///     "stats:rank.value gt 2 and stat:gametype.value eq 1"
    ///
    /// Note:
    /// - The "value" property is only available for string stats and integer stats that are used with a Set.
    /// - The "min", "max" and "delta" properties are only available for double stats and integer stats that are not used with a Set.
    /// </param>
    /// <returns>
    /// The result is an object which contains information about game clips.
    /// </returns>
    /// <remarks>Calls V1 GET /titles/{titleId}/gameclips</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<contextual_search_game_clips_result>> get_game_clips(
        _In_ uint32_t titleId,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ const string_t& orderByStatName,
        _In_ bool orderAscending,
        _In_ const string_t& searchQuery
        );

private:
    contextual_search_service() {}

    pplx::task<xbox_live_result<contextual_search_game_clips_result>> get_game_clips_internal(
        _In_ uint32_t titleId,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ const string_t& orderByStatName,
        _In_ bool orderAscending,
        _In_ const string_t& searchQuery
        );

    xbox_live_result<string_t> create_broadcasts_url(
        _In_ uint32_t titleId,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ const string_t& orderByStatName,
        _In_ bool orderAscending,
        _In_ const string_t& filter
        );

    static xbox_live_result<string_t> create_game_clips_url(
        _In_ uint32_t titleId,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ const string_t& orderByStatName,
        _In_ bool orderAscending,
        _In_ const string_t& filter
        );

    static xbox_live_result<std::vector<contextual_search_broadcast>>
    deserialize_broadcasts_result(_In_ const web::json::value& inputJson);

    static xbox_live_result<std::vector<contextual_search_configured_stat>>
    deserialize_configuration_result(_In_ const web::json::value& inputJson);

    static string_t convert_operator_to_string(
        _In_ contextual_search_filter_operator filterOperator
        );

    contextual_search_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> settings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_settings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;

    friend xbox_live_context_impl;
    friend class contextual_search_game_clips_result;
};


}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END