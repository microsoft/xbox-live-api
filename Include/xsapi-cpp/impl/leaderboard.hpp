// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-cpp/social.h"
#include "public_utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

leaderboard_column::leaderboard_column(
    std::shared_ptr<char> buffer, 
    const XblLeaderboardColumn& leaderboardColumn
) : 
    m_buffer(buffer),
    m_leaderboardColumn(leaderboardColumn)
{ }

string_t leaderboard_column::stat_name() const
{
    return Utils::StringTFromUtf8(m_leaderboardColumn.statName);
}

leaderboard_stat_type leaderboard_column::stat_type() const
{
    return static_cast<leaderboard_stat_type>(m_leaderboardColumn.statType);
}

leaderboard_row::leaderboard_row(
    std::shared_ptr<char> buffer,
    const XblLeaderboardRow& leaderboardRow
) :
    m_buffer(buffer),
    m_leaderboardRow(leaderboardRow)
{ }

string_t leaderboard_row::gamertag() const
{
    return Utils::StringTFromUtf8(m_leaderboardRow.gamertag);
}

string_t leaderboard_row::xbox_user_id() const
{
    return Utils::StringTFromUint64(m_leaderboardRow.xboxUserId);
}

double leaderboard_row::percentile() const
{
    return m_leaderboardRow.percentile;
}

uint32_t leaderboard_row::rank() const
{
    return m_leaderboardRow.rank;
}

std::vector<string_t> leaderboard_row::column_values() const
{
    return Utils::StringTVectorFromCStringArray(m_leaderboardRow.columnValues, m_leaderboardRow.columnValuesCount);
}

leaderboard_result::leaderboard_result(
    std::shared_ptr<char> buffer,
    const XblContextHandle& xblContext
    ) : 
    m_buffer(buffer),
    m_leaderboardResult(*reinterpret_cast<XblLeaderboardResult*>(m_buffer.get())),
    m_xblContext(xblContext)
{
    for (uint32_t i = 0; i < m_leaderboardResult.columnsCount; i++)
    {
        m_columns.push_back(leaderboard_column(m_buffer, m_leaderboardResult.columns[i]));
    }

    for (uint32_t i = 0; i < m_leaderboardResult.rowsCount; i++)
    {
        m_rows.push_back(leaderboard_row(m_buffer, m_leaderboardResult.rows[i]));
    }
}

uint32_t leaderboard_result::total_row_count() const
{
    return m_leaderboardResult.totalRowCount;
}

const std::vector<leaderboard_column>& leaderboard_result::columns() const
{
    return m_columns;
}

const std::vector<leaderboard_row>& leaderboard_result::rows() const
{
    return m_rows;
}

bool leaderboard_result::has_next() const
{
    return m_leaderboardResult.hasNext;
}

#if !defined(XBOX_LIVE_CREATORS_SDK)
pplx::task<xbox_live_result<leaderboard_result>> leaderboard_result::get_next(_In_ uint32_t maxItems)
{
    auto xblContext = m_xblContext;

    auto asyncWrapper = new AsyncWrapper<leaderboard_result>(
        [xblContext](XAsyncBlock* async, leaderboard_result& result)
    {
        size_t bufferSize;
        auto hr = XblLeaderboardResultGetNextResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
            XblLeaderboardResult* resultPtr;
            hr = XblLeaderboardResultGetNextResult(async, bufferSize, buffer.get(), &resultPtr, nullptr);

            if (SUCCEEDED(hr))
            {
                result = leaderboard_result(buffer, xblContext);
            }
        }
        return hr;
    });
    
    auto hr = XblLeaderboardResultGetNextAsync(
        m_xblContext,
        &m_leaderboardResult,
        maxItems,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

leaderboard_service::leaderboard_service(_In_ XblContextHandle contextHandle)
{
    XblContextDuplicateHandle(contextHandle, &m_xblContext);
}

leaderboard_service::leaderboard_service(const leaderboard_service& other)
{
    XblContextDuplicateHandle(other.m_xblContext, &m_xblContext);
}

leaderboard_service& leaderboard_service::operator=(leaderboard_service other)
{
    std::swap(m_xblContext, other.m_xblContext);
    return *this;
}

leaderboard_service::~leaderboard_service()
{
    XblContextCloseHandle(m_xblContext);
}

XblSocialGroupType 
leaderboard_service::XblSocialGroupTypeFromString(string_t socialGroup)
{
    XblSocialGroupType group = XblSocialGroupType::None;
    if (Utils::Stricmp(socialGroup, xbox::services::social::social_group_constants::people()) == 0 || 
        Utils::Stricmp(socialGroup, _T("all")) == 0)
    {
        group = XblSocialGroupType::People;
    }
    else if (Utils::Stricmp(socialGroup, xbox::services::social::social_group_constants::favorite()) == 0)
    {
        group = XblSocialGroupType::Favorites;
    }
    return group;
}

XblLeaderboardSortOrder 
leaderboard_service::XblLeaderboardSortOrderFromString(string_t sortOrder)
{
    XblLeaderboardSortOrder order = XblLeaderboardSortOrder::Descending;
    if (Utils::Stricmp(sortOrder, _T("ascending")) == 0)
    {
        order = XblLeaderboardSortOrder::Ascending;
    }
    return order;
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ const std::vector<string_t>& additionalColumnNames
)
{
    auto leaderboardName = Utils::StringFromStringT(name);
    UTF8StringArrayRef additionalColumnleaderboardNames{ additionalColumnNames };

    XblLeaderboardQuery query = {};
    Utils::Utf8FromCharT(scid.c_str(), query.scid, XBL_SCID_LENGTH);
    query.leaderboardName = leaderboardName.c_str();
    query.additionalColumnleaderboardNames = additionalColumnleaderboardNames.Data();
    query.additionalColumnleaderboardNamesCount = additionalColumnleaderboardNames.Size();

    auto xblContext = m_xblContext;

    auto asyncWrapper = new AsyncWrapper<leaderboard_result>(
        [xblContext](XAsyncBlock* async, leaderboard_result& result)
    {
        size_t bufferSize;
        auto hr = XblLeaderboardGetLeaderboardResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
            XblLeaderboardResult* resultPtr;
            hr = XblLeaderboardGetLeaderboardResult(async, bufferSize, buffer.get(), &resultPtr, nullptr);

            if (SUCCEEDED(hr))
            {
                result = leaderboard_result(buffer, xblContext);
            }
        }
        return hr;
    });

    auto hr = XblLeaderboardGetLeaderboardAsync(
        m_xblContext,
        query,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ const string_t& xuid,
    _In_ const string_t& socialGroup,
    _In_ uint32_t maxItems,
    _In_ const std::vector<string_t>& additionalColumnNames
)
{
    auto leaderboardName = Utils::StringFromStringT(name);
    UTF8StringArrayRef additionalColumnleaderboardNames{ additionalColumnNames };

    XblLeaderboardQuery query = {};
    Utils::Utf8FromCharT(scid.c_str(), query.scid, XBL_SCID_LENGTH);
    query.leaderboardName = leaderboardName.c_str();
    query.xboxUserId = Utils::Uint64FromStringT(xuid);
    query.socialGroup = XblSocialGroupTypeFromString(socialGroup);
    query.maxItems = maxItems;
    query.additionalColumnleaderboardNames = additionalColumnleaderboardNames.Data();
    query.additionalColumnleaderboardNamesCount = (uint32_t)additionalColumnleaderboardNames.Size();

    auto xblContext = m_xblContext;

    auto asyncWrapper = new AsyncWrapper<leaderboard_result>(
        [xblContext](XAsyncBlock* async, leaderboard_result& result)
    {
        size_t bufferSize;
        auto hr = XblLeaderboardGetLeaderboardResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
            XblLeaderboardResult* resultPtr;
            hr = XblLeaderboardGetLeaderboardResult(async, bufferSize, buffer.get(), &resultPtr, nullptr);

            if (SUCCEEDED(hr))
            {
                result = leaderboard_result(buffer, xblContext);
            }
        }
        return hr;
    });

    auto hr = XblLeaderboardGetLeaderboardAsync(
        m_xblContext,
        query,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ uint32_t skipToRank,
    _In_ uint32_t maxItems,
    _In_ const std::vector<string_t>& additionalColumnNames
)
{
    auto leaderboardName = Utils::StringFromStringT(name);
    UTF8StringArrayRef additionalColumnleaderboardNames{ additionalColumnNames };

    XblLeaderboardQuery query = {};
    Utils::Utf8FromCharT(scid.c_str(), query.scid, XBL_SCID_LENGTH);
    query.leaderboardName = leaderboardName.c_str();
    query.skipResultToRank = skipToRank;
    query.maxItems = maxItems;
    query.additionalColumnleaderboardNames = additionalColumnleaderboardNames.Data();
    query.additionalColumnleaderboardNamesCount = additionalColumnleaderboardNames.Size();

    auto xblContext = m_xblContext;

    auto asyncWrapper = new AsyncWrapper<leaderboard_result>(
        [xblContext](XAsyncBlock* async, leaderboard_result& result)
    {
        size_t bufferSize;
        auto hr = XblLeaderboardGetLeaderboardResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
            XblLeaderboardResult* resultPtr;
            hr = XblLeaderboardGetLeaderboardResult(async, bufferSize, buffer.get(), &resultPtr, nullptr);

            if (SUCCEEDED(hr))
            {
                result = leaderboard_result(buffer, xblContext);
            }
        }
        return hr;
    });

    auto hr = XblLeaderboardGetLeaderboardAsync(
        m_xblContext,
        query,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}


pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ uint32_t skipToRank,
    _In_ const string_t& xuid,
    _In_ const string_t& socialGroup,
    _In_ uint32_t maxItems,
    _In_ const std::vector<string_t>& additionalColumnNames
)
{
    auto leaderboardName = Utils::StringFromStringT(name);
    UTF8StringArrayRef additionalColumnleaderboardNames{ additionalColumnNames };

    XblLeaderboardQuery query = {};
    Utils::Utf8FromCharT(scid.c_str(), query.scid, XBL_SCID_LENGTH);
    query.leaderboardName = leaderboardName.c_str();
    query.skipResultToRank = skipToRank;
    query.xboxUserId = Utils::Uint64FromStringT(xuid);
    query.socialGroup = XblSocialGroupTypeFromString(socialGroup);
    query.maxItems = maxItems;
    query.additionalColumnleaderboardNames = additionalColumnleaderboardNames.Data();
    query.additionalColumnleaderboardNamesCount = (uint32_t)additionalColumnleaderboardNames.Size();

    auto xblContext = m_xblContext;

    auto asyncWrapper = new AsyncWrapper<leaderboard_result>(
        [xblContext](XAsyncBlock* async, leaderboard_result& result)
    {
        size_t bufferSize;
        auto hr = XblLeaderboardGetLeaderboardResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
            XblLeaderboardResult* resultPtr;
            hr = XblLeaderboardGetLeaderboardResult(async, bufferSize, buffer.get(), &resultPtr, nullptr);

            if (SUCCEEDED(hr))
            {
                result = leaderboard_result(buffer, xblContext);
            }
        }
        return hr;
    });

    auto hr = XblLeaderboardGetLeaderboardAsync(
        m_xblContext,
        query,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_skip_to_xuid(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ const string_t& skipToXuid,
    _In_ uint32_t maxItems,
    _In_ const std::vector<string_t>& additionalColumnNames
)
{
    auto leaderboardName = Utils::StringFromStringT(name);
    UTF8StringArrayRef additionalColumnleaderboardNames{ additionalColumnNames };

    XblLeaderboardQuery query = {};
    Utils::Utf8FromCharT(scid.c_str(), query.scid, XBL_SCID_LENGTH);
    query.leaderboardName = leaderboardName.c_str();
    query.skipToXboxUserId = Utils::Uint64FromStringT(skipToXuid);
    query.maxItems = maxItems;
    query.additionalColumnleaderboardNames = additionalColumnleaderboardNames.Data();
    query.additionalColumnleaderboardNamesCount = additionalColumnleaderboardNames.Size();

    auto xblContext = m_xblContext;

    auto asyncWrapper = new AsyncWrapper<leaderboard_result>(
        [xblContext](XAsyncBlock* async, leaderboard_result& result)
    {
        size_t bufferSize;
        auto hr = XblLeaderboardGetLeaderboardResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
            XblLeaderboardResult* resultPtr;
            hr = XblLeaderboardGetLeaderboardResult(async, bufferSize, buffer.get(), &resultPtr, nullptr);

            if (SUCCEEDED(hr))
            {
                result = leaderboard_result(buffer, xblContext);
            }
        }
        return hr;
    });

    auto hr = XblLeaderboardGetLeaderboardAsync(
        m_xblContext,
        query,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_skip_to_xuid(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ const string_t& xuid,
    _In_ const string_t& socialGroup,
    _In_ const string_t& skipToXuid,
    _In_ uint32_t maxItems,
    _In_ const std::vector<string_t>& additionalColumnNames
)
{
    auto leaderboardName = Utils::StringFromStringT(name);
    UTF8StringArrayRef additionalColumnleaderboardNames{ additionalColumnNames };

    XblLeaderboardQuery query = {};
    Utils::Utf8FromCharT(scid.c_str(), query.scid, XBL_SCID_LENGTH);
    query.leaderboardName = leaderboardName.c_str();
    query.xboxUserId = Utils::Uint64FromStringT(xuid);
    query.socialGroup = XblSocialGroupTypeFromString(socialGroup);
    query.skipToXboxUserId = Utils::Uint64FromStringT(skipToXuid);
    query.maxItems = maxItems;
    query.additionalColumnleaderboardNames = additionalColumnleaderboardNames.Data();
    query.additionalColumnleaderboardNamesCount = (uint32_t)additionalColumnleaderboardNames.Size();

    auto xblContext = m_xblContext;

    auto asyncWrapper = new AsyncWrapper<leaderboard_result>(
        [xblContext](XAsyncBlock* async, leaderboard_result& result)
    {
        size_t bufferSize;
        auto hr = XblLeaderboardGetLeaderboardResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
            XblLeaderboardResult* resultPtr;
            hr = XblLeaderboardGetLeaderboardResult(async, bufferSize, buffer.get(), &resultPtr, nullptr);

            if (SUCCEEDED(hr))
            {
                result = leaderboard_result(buffer, xblContext);
            }
        }
        return hr;
    });

    auto hr = XblLeaderboardGetLeaderboardAsync(
        m_xblContext,
        query,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_for_social_group(
    _In_ const string_t& xuid,
    _In_ const string_t& scid,
    _In_ const string_t& statName,
    _In_ const string_t& socialGroup,
    _In_ uint32_t maxItems
)
{
    auto statNameCpp = Utils::StringFromStringT(statName);

    XblLeaderboardQuery query = {};
    query.xboxUserId = Utils::Uint64FromStringT(xuid);
    Utils::Utf8FromCharT(scid.c_str(), query.scid, XBL_SCID_LENGTH);
    query.statName = statNameCpp.c_str();
    query.socialGroup = XblSocialGroupTypeFromString(socialGroup);
    query.maxItems = maxItems;

    auto xblContext = m_xblContext;

    auto asyncWrapper = new AsyncWrapper<leaderboard_result>(
        [xblContext](XAsyncBlock* async, leaderboard_result& result)
    {
        size_t bufferSize;
        auto hr = XblLeaderboardGetLeaderboardResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
            XblLeaderboardResult* resultPtr;
            hr = XblLeaderboardGetLeaderboardResult(async, bufferSize, buffer.get(), &resultPtr, nullptr);

            if (SUCCEEDED(hr))
            {
                result = leaderboard_result(buffer, xblContext);
            }
        }
        return hr;
    });

    auto hr = XblLeaderboardGetLeaderboardAsync(
        m_xblContext,
        query,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_for_social_group(
    _In_ const string_t& xuid,
    _In_ const string_t& scid,
    _In_ const string_t& statName,
    _In_ const string_t& socialGroup,
    _In_ const string_t& sortOrder,
    _In_ uint32_t maxItems
)
{
    auto statNameCpp = Utils::StringFromStringT(statName);

    XblLeaderboardQuery query = {};
    query.xboxUserId = Utils::Uint64FromStringT(xuid);
    Utils::Utf8FromCharT(scid.c_str(), query.scid, XBL_SCID_LENGTH);
    query.statName = statNameCpp.c_str();
    query.socialGroup = XblSocialGroupTypeFromString(socialGroup);
    query.order = XblLeaderboardSortOrderFromString(sortOrder);
    query.maxItems = maxItems;

    auto xblContext = m_xblContext;

    auto asyncWrapper = new AsyncWrapper<leaderboard_result>(
        [xblContext](XAsyncBlock* async, leaderboard_result& result)
    {
        size_t bufferSize;
        auto hr = XblLeaderboardGetLeaderboardResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
            XblLeaderboardResult* resultPtr;
            hr = XblLeaderboardGetLeaderboardResult(async, bufferSize, buffer.get(), &resultPtr, nullptr);

            if (SUCCEEDED(hr))
            {
                result = leaderboard_result(buffer, xblContext);
            }
        }
        return hr;
    });

    auto hr = XblLeaderboardGetLeaderboardAsync(
        m_xblContext,
        query,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_for_social_group_skip_to_rank(
    _In_ const string_t& xuid,
    _In_ const string_t& scid,
    _In_ const string_t& statName,
    _In_ const string_t& socialGroup,
    _In_ uint32_t skipToRank,
    _In_ const string_t& sortOrder,
    _In_ uint32_t maxItems
)
{
    auto statNameCpp = Utils::StringFromStringT(statName);

    XblLeaderboardQuery query = {};
    query.xboxUserId = Utils::Uint64FromStringT(xuid);
    Utils::Utf8FromCharT(scid.c_str(), query.scid, XBL_SCID_LENGTH);
    query.statName = statNameCpp.c_str();
    query.socialGroup = XblSocialGroupTypeFromString(socialGroup);
    query.skipResultToRank = skipToRank;
    query.order = XblLeaderboardSortOrderFromString(sortOrder);
    query.maxItems = maxItems;

    auto xblContext = m_xblContext;

    auto asyncWrapper = new AsyncWrapper<leaderboard_result>(
        [xblContext](XAsyncBlock* async, leaderboard_result& result)
    {
        size_t bufferSize;
        auto hr = XblLeaderboardGetLeaderboardResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
            XblLeaderboardResult* resultPtr;
            hr = XblLeaderboardGetLeaderboardResult(async, bufferSize, buffer.get(), &resultPtr, nullptr);

            if (SUCCEEDED(hr))
            {
                result = leaderboard_result(buffer, xblContext);
            }
        }
        return hr;
    });

    auto hr = XblLeaderboardGetLeaderboardAsync(
        m_xblContext,
        query,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_for_social_group_skip_to_xuid(
    _In_ const string_t& xuid,
    _In_ const string_t& scid,
    _In_ const string_t& statName,
    _In_ const string_t& socialGroup,
    _In_ const string_t& skipToXuid,
    _In_ const string_t& sortOrder,
    _In_ uint32_t maxItems
)
{
    auto statNameCpp = Utils::StringFromStringT(statName);

    XblLeaderboardQuery query = {};
    query.xboxUserId = Utils::Uint64FromStringT(xuid);
    Utils::Utf8FromCharT(scid.c_str(), query.scid, XBL_SCID_LENGTH);
    query.statName = statNameCpp.c_str();
    query.socialGroup = XblSocialGroupTypeFromString(socialGroup);
    query.skipToXboxUserId = Utils::Uint64FromStringT(skipToXuid);
    query.order = XblLeaderboardSortOrderFromString(sortOrder);
    query.maxItems = maxItems;

    auto xblContext = m_xblContext;

    auto asyncWrapper = new AsyncWrapper<leaderboard_result>(
        [xblContext](XAsyncBlock* async, leaderboard_result& result)
    {
        size_t bufferSize;
        auto hr = XblLeaderboardGetLeaderboardResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<char> buffer(new char[bufferSize], std::default_delete<char[]>());
            XblLeaderboardResult* resultPtr;
            hr = XblLeaderboardGetLeaderboardResult(async, bufferSize, buffer.get(), &resultPtr, nullptr);

            if (SUCCEEDED(hr))
            {
                result = leaderboard_result(buffer, xblContext);
            }
        }
        return hr;
    });

    auto hr = XblLeaderboardGetLeaderboardAsync(
        m_xblContext,
        query,
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}
#endif // !defined(XBOX_LIVE_CREATORS_SDK)

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END