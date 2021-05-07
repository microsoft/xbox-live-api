// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "leaderboard_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

LeaderboardRow::LeaderboardRow()
{

}

LeaderboardRow::LeaderboardRow(
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
) :
    m_gamertag{ std::move(gamertag) },
    m_modernGamertag{ std::move(modernGamertag) },
    m_modernGamertagSuffix{ std::move(modernGamertagSuffix) },
    m_uniqueModernGamertag{ std::move(uniqueModernGamertag) },
    m_xuid{ xboxUserId },
    m_percentile{ percentile },
    m_rank{ rank },
    m_globalRank{ globalRank },
    m_columnValues(std::move(columnValues))
{
    if(!metadata.empty())
    {
        m_metadata.Parse(metadata.c_str());
    }
}

LeaderboardRow::LeaderboardRow(const LeaderboardRow& other)
{
    m_gamertag = other.m_gamertag;
    m_modernGamertag = other.m_modernGamertag;
    m_modernGamertagSuffix = other.m_modernGamertagSuffix;
    m_uniqueModernGamertag = other.m_uniqueModernGamertag;
    m_xuid = other.m_xuid;
    m_percentile = other.m_percentile;
    m_rank = other.m_rank;
    m_globalRank = other.m_globalRank;
    m_columnValues = other.m_columnValues;
    m_columnValuesC = other.m_columnValuesC;
    JsonUtils::CopyFrom(m_metadata, other.m_metadata);
}

LeaderboardRow& LeaderboardRow::operator =(const LeaderboardRow& other)
{
    m_gamertag = other.m_gamertag;
    m_modernGamertag = other.m_modernGamertag;
    m_modernGamertagSuffix = other.m_modernGamertagSuffix;
    m_uniqueModernGamertag = other.m_uniqueModernGamertag;
    m_xuid = other.m_xuid;
    m_percentile = other.m_percentile;
    m_rank = other.m_rank;
    m_globalRank = other.m_globalRank;
    m_columnValues = other.m_columnValues;
    m_columnValuesC = other.m_columnValuesC;
    JsonUtils::CopyFrom(m_metadata, other.m_metadata);

    return *this;
}

const xsapi_internal_string& LeaderboardRow::Gamertag() const
{
    return m_gamertag;
}

const xsapi_internal_string& LeaderboardRow::ModernGamertag() const
{
    return m_modernGamertag;
}

const xsapi_internal_string& LeaderboardRow::ModernGamertagSuffix() const
{
    return m_modernGamertagSuffix;
}

const xsapi_internal_string& LeaderboardRow::UniqueGamertagSuffix() const
{
    return m_uniqueModernGamertag;
}

uint64_t LeaderboardRow::XboxUserId() const
{
    return m_xuid;
}

double LeaderboardRow::Percentile() const
{
    return m_percentile;
}

uint32_t LeaderboardRow::Rank() const
{
    return m_rank;
}

uint32_t LeaderboardRow::GlobalRank() const 
{
    return m_globalRank;
}

const xsapi_internal_vector<xsapi_internal_string>& LeaderboardRow::ColumnValues() const
{
    return m_columnValues;
}

/* static */ Result<LeaderboardRow> LeaderboardRow::Deserialize(
    _In_ const JsonValue& json
)
{
    xsapi_internal_string gamertag;
    xsapi_internal_string modernGamertag;
    xsapi_internal_string modernGamertagSuffix;
    xsapi_internal_string uniqueModernGamertag;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "gamertag", gamertag, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "moderngamertag", modernGamertag, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "moderngamertagsuffix", modernGamertagSuffix, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "uniquemoderngamertag", uniqueModernGamertag, true));
    uint64_t xuid = 0;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonXuid(json, "xuid", xuid, true));
    double percentile = 0.0;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonDouble(json, "percentile", percentile, true));
    int rank = 0;
    int globalRank = 0;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(json, "rank", rank, true));
    if (json.IsObject() && json.HasMember("globalrank") && !json["globalrank"].IsNull())
    {
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(json, "globalrank", globalRank, false));
    }
    xsapi_internal_vector<xsapi_internal_string> values;
    if (json.IsObject() && json.HasMember("value") && !json["value"].IsNull())
    {
        xsapi_internal_string value;
        JsonUtils::ExtractJsonString(json, "value", value, true);
        values.push_back(value);
    }
    else
    {
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<xsapi_internal_string>(JsonUtils::JsonStringExtractor, json, "values", values, true));
    }
    xsapi_internal_string metadata;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "valuemetadata", metadata, false));

    return LeaderboardRow(
        gamertag,
        modernGamertag,
        modernGamertagSuffix,
        uniqueModernGamertag,
        xuid,
        percentile,
        rank,
        globalRank,
        values,
        metadata
    );
}

size_t LeaderboardRow::SizeOf()
{
    // size must be rounded up to support word aligned data because of the arbitrary length string packing
    // we won't actually use the extra space, but this will report that it is how much is needed
    size_t size = sizeof(XblLeaderboardRow);
    size += sizeof(char*) * m_columnValues.size();
    for (auto columnVal : m_columnValues)
    {
        size += columnVal.size() + 1;
    }
    size = static_cast<size_t>((size + XBL_ALIGN_SIZE - 1) / XBL_ALIGN_SIZE) * XBL_ALIGN_SIZE;
    return size;
}

char* LeaderboardRow::Serialize(XblLeaderboardRow* row, char* buffer)
{
    // the function needs to return the final position of the buffer;  however
    // we have to ensure that the buffer is left word-aligned so that the next
    // data can be read on proper boundaries.
    row->percentile = m_percentile;

    row->rank = m_rank;
    row->globalRank = m_globalRank;
    row->xboxUserId = m_xuid;

    utils::strcpy(row->gamertag, sizeof(XblLeaderboardRow::gamertag), m_gamertag.c_str());
    utils::strcpy(row->modernGamertag, sizeof(XblLeaderboardRow::modernGamertag), m_modernGamertag.data());
    utils::strcpy(row->modernGamertagSuffix, sizeof(XblLeaderboardRow::modernGamertagSuffix), m_modernGamertagSuffix.data());
    utils::strcpy(row->uniqueModernGamertag, sizeof(XblLeaderboardRow::uniqueModernGamertag), m_uniqueModernGamertag.data());

    m_columnValuesC.resize(m_columnValues.size());
    row->columnValuesCount = m_columnValues.size();
    row->columnValues = reinterpret_cast<const char**>(buffer);
    size_t s1 = sizeof(char*) * m_columnValues.size();
    buffer += s1;
    size_t s2 = 0;
    for (size_t i = 0; i < m_columnValues.size(); i++)
    {
        m_columnValuesC[i] = m_columnValues[i].c_str();

        utils::strcpy(buffer, m_columnValues[i].size() + 1, m_columnValuesC[i]);
        row->columnValues[i] = static_cast<char*>(buffer);
        s2 += m_columnValues[i].size() + 1;
        buffer += m_columnValues[i].size() + 1;
    }
    size_t s = s1 + s2;
    if ((s % XBL_ALIGN_SIZE) != 0) {
        // calculate how much padding is needed
        s = (static_cast<size_t>((s + XBL_ALIGN_SIZE - 1) / XBL_ALIGN_SIZE) * XBL_ALIGN_SIZE) - s;
        buffer += s;
    }

    return buffer;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END