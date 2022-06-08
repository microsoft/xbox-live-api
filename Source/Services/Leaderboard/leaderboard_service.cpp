// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbox_live_context_internal.h"
#include "leaderboard_internal.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::legacy;

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

// We need to use a different XBL contract versions depending on 
// 1) whether the leaderboard is backed event based stats vs. title managed stats
// 2) whether it is a social leaderboard or a global leaderboard
// Using set header manually instead of SetContractVersion since leaderboard service is using contract versions that are
// not integer values.

constexpr const char* g_titleMangedStatsGlobalLeaderboardVersion{ "4.1" };
constexpr const char* g_titleMangedStatsSocialLeaderboardVersion{ "2.1" };
constexpr const char* g_eventBasedStatsGlobalLeaderboardVersion{ "3.1" };
constexpr const char* g_eventBasedStatsSocialLeaderboardVersion{ "1.1" };

LeaderboardService::LeaderboardService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::AppConfig> appConfig
) :
    m_user{ std::move(user) },
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

xbl_result<xsapi_internal_string> 
CreateLeaderboardUrl(
    _In_ const xsapi_internal_string& scid,
    _In_ const xsapi_internal_string& name,
    _In_ uint32_t skipToRank,
    _In_ const xsapi_internal_string& skipToXuid,
    _In_ uint32_t maxItems,
    _In_ const xsapi_internal_string& continuationToken,
    _In_ bool metadata,
    _In_ bool isTitleManaged,
    _In_ const xsapi_internal_string& xuid = xsapi_internal_string(),
    _In_ const xsapi_internal_string& socialGroup = xsapi_internal_string()
    )
{
    if (scid.empty())
        return xbl_result<xsapi_internal_string>(xbl_error_code::invalid_argument, "scid is required for getting leaderboards");
    if (name.empty())
        return xbl_result<xsapi_internal_string>(xbl_error_code::invalid_argument, "name is required for getting leaderboards");

    xbox::services::uri_builder builder;

    xsapi_internal_stringstream path;
    path << "/scids/";
    path << xbox::services::uri::encode_uri(scid, xbox::services::uri::components::path);
    if (isTitleManaged)
    {
        path << "/leaderboards/stat(";
        path << xbox::services::uri::encode_uri(name, xbox::services::uri::components::path);
        path << ")";
    }
    else
    {
        path << "/leaderboards/";
        path << xbox::services::uri::encode_uri(name, xbox::services::uri::components::path);
    }
    builder.set_path(path.str());

    if (metadata)
    {
        builder.append_query("include", "valuemetadata");
    }

    if (!xuid.empty())
    {
        builder.append_query("xuid", xuid);
    }

    if (maxItems > 0)
    {
        builder.append_query("maxItems", maxItems);
    }

    if (!skipToXuid.empty())
    {
        if (skipToRank > 0)
        {
            return xbl_result<xsapi_internal_string>(xbl_error_code::invalid_argument, "Cannot skip to XUID and rank");
        }

        builder.append_query("skipToUser", skipToXuid);
    }
    else
    {
        if (!continuationToken.empty())
        {
            builder.append_query("continuationToken", continuationToken);
        }
        else if (skipToRank > 0)
        {
            builder.append_query("skipToRank", skipToRank);
        }
    }

    if (!socialGroup.empty())
    {
        builder.append_query("view", "People");
        builder.append_query("viewTarget", socialGroup);
    }

    return xbl_result<xsapi_internal_string>(builder.to_string());
}

HRESULT
LeaderboardService::GetLeaderboard(
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
    )
{
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(scid);
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(name);

    xbl_result<xsapi_internal_string> url = CreateLeaderboardUrl(
        scid,
        name,
        skipToRank,
        skipToXuid,
        maxItems,
        continuationToken,
        additionalColumnNames.size() != 0,
        isTitleManaged,
        xuid,
        socialGroup
        );

    if (url.err()) return utils::convert_xbox_live_error_code_to_hresult(url.err());

    std::shared_ptr<LeaderboardGlobalQuery> query = MakeShared<LeaderboardGlobalQuery>();
    query->scid = scid;
    query->name = name;
    query->xuid = xuid;
    query->socialGroup = socialGroup;
    query->columns = additionalColumnNames;
    query->isTitleManaged = isTitleManaged;

    return RunAsync(async, __FUNCTION__,
        [
            result = LeaderboardResult{},
            sharedThis{ shared_from_this() }, 
            url,
            query,
            additionalColumnNames
        ]
    (_In_ XAsyncOp op, _In_ const XAsyncProviderData* data) mutable
    {
        if (op == XAsyncOp::DoWork)
        {

            Result<User> userResult = sharedThis->m_user.Copy();
            RETURN_HR_IF_FAILED(userResult.Hresult());

            auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
            HRESULT hr = httpCall->Init(
                sharedThis->m_xboxLiveContextSettings,
                "GET",
                XblHttpCall::BuildUrl("leaderboards", url.payload()),
                xbox_live_api::get_leaderboard_internal
            );

            RETURN_HR_IF_FAILED(hr);

            // Using set header directly instead of SetContractVersion since leaderboard service is using contract versions that are
            // not integer values.
            if (query->isTitleManaged)
            {
                RETURN_HR_IF_FAILED(httpCall->SetHeader(CONTRACT_VERSION_HEADER, g_titleMangedStatsGlobalLeaderboardVersion));
            }
            else
            {
                RETURN_HR_IF_FAILED(httpCall->SetHeader(CONTRACT_VERSION_HEADER, g_eventBasedStatsGlobalLeaderboardVersion));
            }

            hr = httpCall->Perform(AsyncContext<HttpResult>{
                TaskQueue::DeriveWorkerQueue(data->async->queue),
                    [
                        &result,
                        data,
                        additionalColumnNames,
                        query
                    ]
                (HttpResult httpResult)
                {
                    HRESULT hr = httpResult.Hresult();
                    if (SUCCEEDED(hr))
                    {
                        hr = httpResult.Payload()->Result();
                        if (SUCCEEDED(hr))
                        {
                            auto xblResult = LeaderboardResult::Deserialize(
                                httpResult.Payload()->GetResponseBodyJson()
                            );
                            result = xblResult.Payload();
                            result.SetNextQuery(query);
                            if (additionalColumnNames.size() > 0)
                            {
                                result.ParseAdditionalColumns(additionalColumnNames);
                            }

                            hr = xblResult.Hresult();
                        }
                    }
                    XAsyncComplete(data->async, hr, result.SizeOf());
                }});
            
            return SUCCEEDED(hr) ? E_PENDING : hr;
        }
        else if (op == XAsyncOp::GetResult)
        {
            char* buffer = static_cast<char*>(data->buffer);
            ZeroMemory(buffer, data->bufferSize);
            buffer = result.Serialize(buffer);
            XSAPI_ASSERT(static_cast<void*>(buffer) == static_cast<void*>(static_cast<char*>(data->buffer) + result.SizeOf()));
        }

        return S_OK;
    });
}

xbl_result<xsapi_internal_string> 
CreateLeaderboardForSocialGroupUrl(
    _In_ const xsapi_internal_string& xuid,
    _In_ const xsapi_internal_string& scid,
    _In_ const xsapi_internal_string& statName,
    _In_ const xsapi_internal_string& socialGroup,
    _In_ uint32_t skipToRank,
    _In_ const xsapi_internal_string& skipToXuid,
    _In_ const xsapi_internal_string& sortOrder,
    _In_ uint32_t maxItems,
    _In_ const xsapi_internal_string& continuationToken
    )
{

    if (xuid.empty()) return xbl_result<xsapi_internal_string>(xbl_error_code::invalid_argument, "xuid is required for getting leaderboard for social group");
    if (scid.empty()) return xbl_result<xsapi_internal_string>(xbl_error_code::invalid_argument, "scid is required for getting leaderboard for social group");
    if (statName.empty()) return xbl_result<xsapi_internal_string>(xbl_error_code::invalid_argument, "statName is required for getting leaderboard for social group");
    if (socialGroup.empty()) return xbl_result<xsapi_internal_string>(xbl_error_code::invalid_argument, "socialGroup is required for getting leaderboard for social group");

    // "/users/xuid({xuid})/scids/{scid}/stats/{statname}/people/{group}"

    xbox::services::uri_builder builder;
    
    xsapi_internal_stringstream path;
    path << "/users/xuid(";
    path << xbox::services::uri::encode_uri(xuid, xbox::services::uri::components::path);
    path << ")/scids/";
    path << xbox::services::uri::encode_uri(scid, xbox::services::uri::components::path);
    path << "/stats/";
    path << xbox::services::uri::encode_uri(statName, xbox::services::uri::components::path);
    path << "/people/";
    path << xbox::services::uri::encode_uri(socialGroup, xbox::services::uri::components::path);
    builder.set_path(path.str());

    if (!sortOrder.empty())
    {
        builder.append_query("sort", sortOrder);
    }

    if (maxItems > 0)
    {
        builder.append_query("maxItems", maxItems);
    }

    if (!skipToXuid.empty())
    {
        builder.append_query("skipToUser", skipToXuid);
    }
    else
    {
        if (!continuationToken.empty())
        {
            builder.append_query("continuationToken", continuationToken);
        }
        else if (skipToRank > 0)
        {
            builder.append_query("skipToRank", skipToRank);
        }
    }

    return xbl_result<xsapi_internal_string>(builder.to_string());
}

HRESULT 
LeaderboardService::GetLeaderboardForSocialGroup(
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
    )
{
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(xuid);
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(scid);
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(statName);

    // To align with People moniker support, we are mapping "People" to "all" until the
    // leaderboard service can align with naming conventions.
    xsapi_internal_string group = socialGroup;
    if (utils::str_icmp_internal(socialGroup, "People") == 0)
    {
        group = "all";
    }

    xbl_result<xsapi_internal_string> url = CreateLeaderboardForSocialGroupUrl(
        xuid,
        scid,
        statName,
        group,
        skipToRank,
        skipToXuid,
        sortOrder,
        maxItems,
        continuationToken);

    if (url.err()) return utils::convert_xbox_live_error_code_to_hresult(url.err());

    auto query = MakeShared<LeaderboardSocialQuery>();
    query->xuid = xuid;
    query->scid = scid;
    query->statName = statName;
    query->socialGroup = group;
    query->sortOrder = sortOrder;
    query->isTitleManaged = isTitleManaged;

    return RunAsync(async, __FUNCTION__,
        [
            result = LeaderboardResult{},
            sharedThis{ shared_from_this() }, 
            url, 
            query
        ]
    (_In_ XAsyncOp op, _In_ const XAsyncProviderData* data) mutable
    {
        if (op == XAsyncOp::DoWork)
        {
            Result<User> userResult = sharedThis->m_user.Copy();
            RETURN_HR_IF_FAILED(userResult.Hresult());

            auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
            HRESULT hr = httpCall->Init(
                sharedThis->m_xboxLiveContextSettings,
                "GET",
                XblHttpCall::BuildUrl("leaderboards", url.payload()),
                xbox_live_api::get_leaderboard_for_social_group_internal
            );

            RETURN_HR_IF_FAILED(hr);
            if (query->isTitleManaged)
            {
                RETURN_HR_IF_FAILED(httpCall->SetHeader(CONTRACT_VERSION_HEADER, g_titleMangedStatsSocialLeaderboardVersion));
            }
            else
            {
                RETURN_HR_IF_FAILED(httpCall->SetHeader(CONTRACT_VERSION_HEADER, g_eventBasedStatsSocialLeaderboardVersion));
            }

            hr = httpCall->Perform(AsyncContext<HttpResult>{
                TaskQueue::DeriveWorkerQueue(data->async->queue),
                    [
                        &result,
                        data,
                        query
                    ]
                (HttpResult httpResult)
                {
                    HRESULT hr = httpResult.Hresult();
                    if (SUCCEEDED(hr))
                    {
                        hr = httpResult.Payload()->Result();
                        if (SUCCEEDED(hr))
                        {
                            auto xblResult = LeaderboardResult::Deserialize(
                                httpResult.Payload()->GetResponseBodyJson()
                            );
                            result = xblResult.Payload();
                            result.SetNextQuery(query);

                            hr = xblResult.Hresult();
                        }
                    }
                    XAsyncComplete(data->async, hr, result.SizeOf());
                }});

            return SUCCEEDED(hr) ? E_PENDING : hr;
        }
        else if (op == XAsyncOp::GetResult)
        {
            char* buffer = static_cast<char*>(data->buffer);
            ZeroMemory(buffer, data->bufferSize);
            buffer = result.Serialize(buffer);
            XSAPI_ASSERT(static_cast<void*>(buffer) == static_cast<void*>(static_cast<char*>(data->buffer) + data->bufferSize));
        }

        return S_OK;
    });
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END

STDAPI XblLeaderboardGetLeaderboardAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblLeaderboardQuery leaderboardQuery,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    VERIFY_XBL_INITIALIZED();
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContext);

    if (leaderboardQuery.queryType != XblLeaderboardQueryType::UserStatBacked)
    {
        // For title managed stat backed leaderboards, the statistic name must be specified
        RETURN_HR_INVALIDARGUMENT_IF_NULL(leaderboardQuery.statName);
    }
    else
    {
        // For event stat backed leaderboards, the query must specify a leaderboard name OR a stat name
        RETURN_HR_INVALIDARGUMENT_IF(leaderboardQuery.leaderboardName == nullptr && leaderboardQuery.statName == nullptr);
    }

    xsapi_internal_string xuid = utils::uint64_to_internal_string(leaderboardQuery.xboxUserId);
    if (leaderboardQuery.xboxUserId == 0) xuid = "";

    xsapi_internal_string skipToXuid = utils::uint64_to_internal_string(leaderboardQuery.skipToXboxUserId);
    if (leaderboardQuery.skipToXboxUserId == 0) skipToXuid = "";

    xsapi_internal_string continuationToken("");
    if (leaderboardQuery.continuationToken) continuationToken = leaderboardQuery.continuationToken;

    xsapi_internal_string socialGroup("");
    if (leaderboardQuery.socialGroup == XblSocialGroupType::People)
    {
        socialGroup = xbox::services::social::legacy::social_group_constants::people();
    }
    else if (leaderboardQuery.socialGroup == XblSocialGroupType::Favorites)
    {
        socialGroup = xbox::services::social::legacy::social_group_constants::favorite();
    }

    if ((leaderboardQuery.queryType == XblLeaderboardQueryType::UserStatBacked && leaderboardQuery.leaderboardName) || 
         leaderboardQuery.queryType == XblLeaderboardQueryType::TitleManagedStatBackedGlobal)
    {
        // Depending on the type of query, we either use the leaderboard name or the stat name
        String name{};
        switch (leaderboardQuery.queryType)
        {
        case XblLeaderboardQueryType::UserStatBacked:
        {
            name = leaderboardQuery.leaderboardName;
            break;
        }
        case XblLeaderboardQueryType::TitleManagedStatBackedGlobal:
        {
            name = leaderboardQuery.statName;
            break;
        }
        default: break;
        }

        return xboxLiveContext->LeaderboardService()->GetLeaderboard(
            leaderboardQuery.scid,
            name,
            leaderboardQuery.skipResultToRank,
            skipToXuid,
            xuid,
            socialGroup,
            leaderboardQuery.maxItems,
            continuationToken,
            utils::string_array_to_internal_string_vector(leaderboardQuery.additionalColumnleaderboardNames, leaderboardQuery.additionalColumnleaderboardNamesCount),
            leaderboardQuery.queryType != XblLeaderboardQueryType::UserStatBacked,
            async
        );
    }
    else
    {
        if (socialGroup.empty())
        {
            socialGroup = xbox::services::social::legacy::social_group_constants::people();
        }

        xsapi_internal_string statName("");
        if (leaderboardQuery.statName) statName = leaderboardQuery.statName;

        xsapi_internal_string sortOrder("");
        if (leaderboardQuery.order == XblLeaderboardSortOrder::Ascending)
        {
            sortOrder = "ascending";
        }
        if (leaderboardQuery.order == XblLeaderboardSortOrder::Descending)
        {
            sortOrder = "descending";
        }

        return xboxLiveContext->LeaderboardService()->GetLeaderboardForSocialGroup(
            xuid,
            leaderboardQuery.scid,
            statName,
            socialGroup,
            leaderboardQuery.skipResultToRank,
            skipToXuid,
            sortOrder,
            leaderboardQuery.maxItems,
            continuationToken,
            leaderboardQuery.queryType != XblLeaderboardQueryType::UserStatBacked,
            async
        );
    }
}
CATCH_RETURN()

STDAPI XblLeaderboardGetLeaderboardResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT
try
{
    return XAsyncGetResultSize(async, resultSizeInBytes);
}
CATCH_RETURN()

STDAPI XblLeaderboardGetLeaderboardResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblLeaderboardResult** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(ptrToBuffer);
    auto hr = XAsyncGetResult(async, nullptr, bufferSize, buffer, bufferUsed);
    if (SUCCEEDED(hr))
    {
        *ptrToBuffer = static_cast<XblLeaderboardResult*>(buffer);
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblLeaderboardResultGetNextAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblLeaderboardResult* leaderboardResult,
    _In_ uint32_t maxItems,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContext);
    RETURN_HR_IF(leaderboardResult->nextQuery.continuationToken == nullptr, E_BOUNDS);

    leaderboardResult->nextQuery.maxItems = maxItems;
    return XblLeaderboardGetLeaderboardAsync(
        xboxLiveContext,
        leaderboardResult->nextQuery,
        async
    );
}
CATCH_RETURN()

STDAPI XblLeaderboardResultGetNextResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT
try
{
    return XAsyncGetResultSize(async, resultSizeInBytes);
}
CATCH_RETURN()

STDAPI XblLeaderboardResultGetNextResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblLeaderboardResult** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(ptrToBuffer);
    VERIFY_XBL_INITIALIZED();

    auto hr = XAsyncGetResult(async, nullptr, bufferSize, buffer, bufferUsed);
    if (SUCCEEDED(hr))
    {
        *ptrToBuffer = static_cast<XblLeaderboardResult*>(buffer);
    }
    return hr;
}
CATCH_RETURN()
