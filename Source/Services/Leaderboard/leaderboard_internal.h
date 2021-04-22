// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi-c/leaderboard_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

class LeaderboardResult;

namespace legacy
{
    /// <summary>Enumerates the data type of a leaderboard statistic.</summary>
    enum class leaderboard_stat_type
    {
        /// <summary>Unsigned 64 bit integer.</summary>
        stat_uint64,

        /// <summary>Boolean.</summary>
        stat_boolean,

        /// <summary>Double.</summary>
        stat_double,

        /// <summary>String.</summary>
        stat_string,

        /// <summary>Unknown.</summary>
        stat_other
    };
}

struct LeaderboardGlobalQuery
{
    xsapi_internal_string scid;
    xsapi_internal_string name;
    xsapi_internal_string xuid;
    xsapi_internal_string socialGroup;
    xsapi_internal_vector<xsapi_internal_string> columns;
    bool isTitleManaged{ false };
};

struct LeaderboardSocialQuery
{
    xsapi_internal_string xuid;
    xsapi_internal_string scid;
    xsapi_internal_string statName;
    xsapi_internal_string socialGroup;
    xsapi_internal_string sortOrder;
    bool isTitleManaged{ false };
};

class LeaderboardColumn
{
public:
    LeaderboardColumn();
    LeaderboardColumn(
        _In_ xsapi_internal_string statName,
        _In_ legacy::leaderboard_stat_type stat_type
    );

    static Result<LeaderboardColumn> Deserialize(
        _In_ const JsonValue& json
    );
    size_t SizeOf();
    char* Serialize(XblLeaderboardColumn* column, char* buffer);

    const xsapi_internal_string& StatName() const;
    legacy::leaderboard_stat_type StatType() const;

private:
    xsapi_internal_string m_statName;
    legacy::leaderboard_stat_type m_statType;
};

class LeaderboardRow
{
public:
    LeaderboardRow();

    LeaderboardRow(
        _In_ xsapi_internal_string gamertag,
        _In_ xsapi_internal_string modernGamertag,
        _In_ xsapi_internal_string modernGamertagSuffix,
        _In_ xsapi_internal_string uniqueModernGamertag,
        _In_ uint64_t xboxUserId,
        _In_ double percentile,
        _In_ uint32_t rank,
        _In_ uint32_t globalRank,
        _In_ xsapi_internal_vector<xsapi_internal_string> columnValues,
        _In_ xsapi_internal_string metadata
    );

    LeaderboardRow(const LeaderboardRow& other);
    
    LeaderboardRow& operator =(const LeaderboardRow& other);

    static Result<LeaderboardRow> Deserialize(
        _In_ const JsonValue& json
    );
    size_t SizeOf();
    char* Serialize(XblLeaderboardRow* row, char* buffer);

    const xsapi_internal_string& Gamertag() const;
    const xsapi_internal_string& ModernGamertag() const;
    const xsapi_internal_string& ModernGamertagSuffix() const;
    const xsapi_internal_string& UniqueGamertagSuffix() const;
    uint64_t XboxUserId() const;
    double Percentile() const;
    uint32_t Rank() const;
    uint32_t GlobalRank() const;
    const xsapi_internal_vector<xsapi_internal_string>& ColumnValues() const;

private:
    xsapi_internal_string m_gamertag;
    xsapi_internal_string m_modernGamertag;
    xsapi_internal_string m_modernGamertagSuffix;
    xsapi_internal_string m_uniqueModernGamertag;
    uint64_t m_xuid{ 0 };
    double m_percentile{ 0.0 };
    uint32_t m_rank{ 0 };
    uint32_t m_globalRank{ 0 };
    xsapi_internal_vector<xsapi_internal_string> m_columnValues;
    xsapi_internal_vector<const char*> m_columnValuesC;
    JsonDocument m_metadata;

    friend LeaderboardResult;
};

class LeaderboardResult
{
public:
    LeaderboardResult() = default;
    static Result<LeaderboardResult> Deserialize(_In_ const JsonValue& json);

    size_t SizeOfQuery();
    char* SerializeQuery(XblLeaderboardQuery* query, char* buffer);
    size_t SizeOf();
    char* Serialize(char* buffer);
    XblSocialGroupType ParseSocialGroup(xsapi_internal_string socialGroupStr);

    uint32_t TotalRowCount() const;
    const xsapi_internal_vector<LeaderboardColumn>& Columns() const;
    const xsapi_internal_vector<LeaderboardRow>& Rows() const;
    bool HasNext() const;
    
    void SetNextQuery(std::shared_ptr<LeaderboardGlobalQuery> query);
    void SetNextQuery(std::shared_ptr<LeaderboardSocialQuery> query);
    void ParseAdditionalColumns(const xsapi_internal_vector<xsapi_internal_string>& additionalColumnNames);

private:
    LeaderboardResult(
        _In_ uint32_t totalRowCount,
        _In_ String continuationToken,
        _In_ Vector<LeaderboardColumn> columns,
        _In_ Vector<LeaderboardRow> rows
    );

    uint32_t m_totalRowCount{};
    String m_continuationToken;
    Vector<LeaderboardColumn> m_columns;
    Vector<LeaderboardRow> m_rows;

    std::shared_ptr<LeaderboardGlobalQuery> m_globalQuery;
    xsapi_internal_vector<const char*> m_additionalColumnleaderboardNamesC;
    std::shared_ptr<LeaderboardSocialQuery> m_socialQuery;
};

class LeaderboardService : public std::enable_shared_from_this<LeaderboardService>
{
public:
    LeaderboardService(
        _In_ User&& user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::AppConfig> appConfig
    );

    HRESULT GetLeaderboardForSocialGroup(
        _In_ const xsapi_internal_string& xuid,
        _In_ const xsapi_internal_string& scid,
        _In_ const xsapi_internal_string& statName,
        _In_ const xsapi_internal_string& socialGroup,
        _In_ uint32_t skipToRank,
        _In_ const xsapi_internal_string& skipToXuid,
        _In_ const xsapi_internal_string& sortOrder,
        _In_ uint32_t maxItems,
        _In_ const xsapi_internal_string& continuationToken,
        _In_ bool isTitleManaged,
        _In_ XAsyncBlock* async
    );

    HRESULT GetLeaderboard(
        _In_ const xsapi_internal_string& scid,
        _In_ const xsapi_internal_string& name,
        _In_ uint32_t skipToRank,
        _In_ const xsapi_internal_string& skipToXuid,
        _In_ const xsapi_internal_string& xuid,
        _In_ const xsapi_internal_string& socialGroup,
        _In_ uint32_t maxItems,
        _In_ const xsapi_internal_string& continuationToken,
        _In_ const xsapi_internal_vector<xsapi_internal_string>& additionalColumnNames,
        _In_ bool isTitleManaged,
        _In_ XAsyncBlock* async
    );

private:
    User m_user;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::AppConfig> m_appConfig;

    friend LeaderboardResult;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END