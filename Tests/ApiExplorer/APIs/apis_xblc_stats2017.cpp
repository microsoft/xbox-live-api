// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( push )
#pragma warning( disable : 4365 )
#pragma warning( disable : 4061 )
#pragma warning( disable : 4996 )
#endif
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( pop )
#endif

using namespace utility;

struct StatsValueDocument
{
public:
    StatsValueDocument(rapidjson::Document&& svd) noexcept
        : m_svd{ std::move(svd) }
    {
        assert(m_svd.HasMember("stats"));
        assert(m_svd["stats"].HasMember("title"));
    }

    StatsValueDocument(const StatsValueDocument& other) noexcept
    {
        m_svd.CopyFrom(other.m_svd, m_svd.GetAllocator());
    }

    StatsValueDocument(StatsValueDocument&& other) noexcept
        : m_svd{ std::move(other.m_svd) }
    {
    }

    StatsValueDocument& operator=(const StatsValueDocument& other) noexcept
    {
        m_svd.CopyFrom(other.m_svd, m_svd.GetAllocator());
        return *this;
    }

    StatsValueDocument() noexcept = default;
    ~StatsValueDocument() noexcept = default;

    bool operator==(const StatsValueDocument& other) const noexcept
    {
        return m_svd["stats"]["title"] == other.m_svd["stats"]["title"];
    }

    std::vector<XblTitleManagedStatistic> Stats() const noexcept
    {
        std::vector<XblTitleManagedStatistic> stats;

        auto& titleJson{ m_svd["stats"]["title"] };
        for (auto it = titleJson.MemberBegin(); it != titleJson.MemberEnd(); ++it)
        {
            auto& value{ it->value["value"] };
            stats.push_back(XblTitleManagedStatistic
                {
                    it->name.GetString(),
                    value.IsString() ? XblTitleManagedStatType::String : XblTitleManagedStatType::Number,
                    value.IsNumber() ? value.GetDouble() : 0,
                    value.IsString() ? value.GetString() : nullptr
                });
        }
        return stats;
    }

    std::string Serialize() const noexcept
    {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        m_svd["stats"]["title"].Accept(writer);
        return buffer.GetString();
    }

    void SetStat(const std::string& name, const std::string& value) noexcept
    {
        SetStat(name, rapidjson::Value{ value.data(), m_svd.GetAllocator() }.Move());
    }

    void SetStat(const std::string& name, double value) noexcept
    {
        SetStat(name, rapidjson::Value{ value }.Move());
    }

    void DeleteStat(const std::string& name) noexcept
    {
        m_svd["stats"]["title"].EraseMember(name.data());
    }

    void ClearStats() noexcept
    {
        m_svd["stats"]["title"] = rapidjson::Value{ rapidjson::kObjectType };
    }

private:
    void SetStat(const std::string& name, rapidjson::Value& value) noexcept
    {
        auto& titleJson{ m_svd["stats"]["title"] };
        if (titleJson.HasMember(name.data()))
        {
            titleJson[name.data()]["value"] = value.Move();
        }
        else
        {
            auto& a{ m_svd.GetAllocator() };
            rapidjson::Value statJson{ rapidjson::kObjectType };
            statJson.AddMember("value", value.Move(), a);
            titleJson.AddMember(rapidjson::Value{ name.data(), a }, statJson.Move(), a);
        }
    }

    rapidjson::Document m_svd{};
};

static std::unique_ptr<StatsValueDocument> s_svd{ nullptr };

StatsValueDocument GetStatsValueDocument(uint64_t xuid, HRESULT* resultHr)
{
    XblHttpCallHandle call{};

    std::stringstream uri;
    uri << "https://statsread.xboxlive.com/stats/users/" << xuid << "/scids/" << Data()->scid;

    HRESULT hr = XblHttpCallCreate(Data()->xboxLiveContext, "GET", uri.str().data(), &call);
    assert(SUCCEEDED(hr));

    XAsyncBlock async{};
    hr = XblHttpCallPerformAsync(call, XblHttpCallResponseBodyType::String, &async);
    assert(SUCCEEDED(hr));

    hr = XAsyncGetStatus(&async, true);
    if (SUCCEEDED(hr))
    {
        assert(SUCCEEDED(hr));

        const char* responseString{ nullptr };
        hr = XblHttpCallGetResponseString(call, &responseString);
        assert(SUCCEEDED(hr));

        rapidjson::Document svd{};
        svd.Parse(responseString);
        assert(!svd.HasParseError());

        *resultHr = S_OK;
        return StatsValueDocument{ std::move(svd) };
    }
    else
    {
        *resultHr = E_FAIL;
        return StatsValueDocument();
    }
}

int XblTitleManagedStatsWriteAsyncWithSVD_Lua(lua_State *L)
{
    if (!s_svd)
    {
        HRESULT hr = S_OK;
        s_svd = std::make_unique<StatsValueDocument>(GetStatsValueDocument(Data()->xboxUserId, &hr));
        if (FAILED(hr))
        {
            LogToFile("XblTitleManagedStatsWriteAsyncWithSVD: hr = %s", ConvertHR(hr).data());
            s_svd = nullptr;
            CallLuaFunction("OnXblTitleManagedStatsUnableToGetTokenAndSignature");
            return LuaReturnHR(L, S_OK);
        }
    }

    auto now = datetime::utc_now();
    auto timeString = conversions::to_utf8string(now.to_string(datetime::date_format::ISO_8601));

    s_svd->SetStat("TimeString", timeString.data());
    s_svd->SetStat("NumericStat", 100.0);

    std::vector<XblTitleManagedStatistic> statList{ s_svd->Stats() };

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);

        if (hr == E_FAIL)
        {
            CallLuaFunction("OnXblTitleManagedStatsUnableToGetTokenAndSignature");
            return;
        }

        CallLuaFunctionWithHr(hr, "OnXblTitleManagedStatsWriteAsyncWithSVD"); // CODE SNIP SKIP
    };

    HRESULT hr = XblTitleManagedStatsWriteAsync(
        Data()->xboxLiveContext,
        Data()->xboxUserId,
        statList.data(), statList.size(),
        asyncBlock.get()
    );

    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    LogToFile("XblTitleManagedStatsWriteAsyncWithSVD: hr = %s", ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

int XblTitleManagedStatsWriteAsync_Lua(lua_State *L)
{   
    static double s_stat1Value = 0.0;
    s_stat1Value++;

    // CODE SNIPPET START: XblTitleManagedStatsWriteAsync
    XblTitleManagedStatistic stat1{};
    std::string stat1Name = "TestStat1";
    stat1.statisticName = stat1Name.c_str();
    stat1.statisticType = XblTitleManagedStatType::Number;
    stat1.numberValue = s_stat1Value;

    XblTitleManagedStatistic stat2{};
    std::string stat2Name = "TestStat2";
    std::string stat2Value = "TestValue1234";
    stat2.statisticName = stat2Name.c_str();
    stat2.statisticType = XblTitleManagedStatType::String;
    stat2.stringValue = stat2Value.c_str();
    std::vector<XblTitleManagedStatistic> statList{ stat1, stat2 };

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);
        if (hr != HTTP_E_STATUS_429_TOO_MANY_REQUESTS)
        {
            LogToScreen("XblTitleManagedStatsWriteAsync: 0x%0.8x", hr);
        }

        if (hr == E_FAIL)
        {
            CallLuaFunction("OnXblTitleManagedStatsUnableToGetTokenAndSignature");
            return;
        }

        CallLuaFunctionWithHr(hr, "OnXblTitleManagedStatsWriteAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblTitleManagedStatsWriteAsync(
        Data()->xboxLiveContext,
        Data()->xboxUserId,
        statList.data(), statList.size(),
        asyncBlock.get()
    );

    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("XblTitleManagedStatsWriteAsync: hr = %s", ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

int XblTitleManagedStatsUpdateStatsAsync_Lua(lua_State* L)
{
    if (!s_svd)
    {
        HRESULT hr = S_OK;
        s_svd = std::make_unique<StatsValueDocument>(GetStatsValueDocument(Data()->xboxUserId, &hr));
        if (FAILED(hr))
        {
            LogToFile("XblTitleManagedStatsWriteAsyncWithSVD: hr = %s", ConvertHR(hr).data());
            s_svd = nullptr;
            CallLuaFunction("OnXblTitleManagedStatsUnableToGetTokenAndSignature");
            return LuaReturnHR(L, S_OK);
        }
    }

    auto timeString = conversions::to_utf8string(datetime::utc_now().to_string(datetime::date_format::ISO_8601));
    s_svd->SetStat("AddedStat",timeString.data());
    s_svd->SetStat("NumericStat", 85.0);

    // CODE SNIPPET START: XblTitleManagedStatsUpdateStatsAsync
    std::vector<XblTitleManagedStatistic> stats{ s_svd->Stats() };
    std::vector<XblTitleManagedStatistic> updatedStats;
    for (const XblTitleManagedStatistic& stat : stats)
    {
        if (std::string{ "AddedStat" } == stat.statisticName ||
            std::string{ "NumericStat" } == stat.statisticName)
        {
            updatedStats.push_back(stat);
        }
    }
    auto stat1 = updatedStats.begin();

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);

        if (hr == E_FAIL)
        {
            CallLuaFunction("OnXblTitleManagedStatsUnableToGetTokenAndSignature");
            return;
        }

        CallLuaFunctionWithHr(hr, "OnXblTitleManagedStatsUpdateStatsAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblTitleManagedStatsUpdateStatsAsync(
        Data()->xboxLiveContext,
        &(*stat1),
        updatedStats.size(),
        asyncBlock.get()
    );

    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("XblTitleManagedStatsUpdateStatsAsync: hr = %s", ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

int XblTitleManagedStatsDeleteStatsAsync_Lua(lua_State* L)
{
    if (!s_svd)
    {
        HRESULT hr = S_OK;
        s_svd = std::make_unique<StatsValueDocument>(GetStatsValueDocument(Data()->xboxUserId, &hr));
        if (FAILED(hr))
        {
            LogToFile("XblTitleManagedStatsWriteAsyncWithSVD: hr = %s", ConvertHR(hr).data());
            s_svd = nullptr;
            CallLuaFunction("OnXblTitleManagedStatsUnableToGetTokenAndSignature");
            return LuaReturnHR(L, S_OK);
        }
    }

    // CODE SNIPPET START: XblTitleManagedStatsDeleteStats
    const char* statName{ "TimeString" };
    s_svd->DeleteStat(statName); // CODE SNIP SKIP

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);

        if (hr == E_FAIL)
        {
            CallLuaFunction("OnXblTitleManagedStatsUnableToGetTokenAndSignature");
            return;
        }

        CallLuaFunctionWithHr(hr, "OnXblTitleManagedStatsDeleteStatsAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblTitleManagedStatsDeleteStatsAsync(
        Data()->xboxLiveContext,
        &statName,
        1,
        asyncBlock.get()
    );

    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("XblTitleManagedStatsDeleteStatsAsync: hr = %s", ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

int ValidateSVD_Lua(lua_State* L)
{
    UNREFERENCED_PARAMETER(L);

    // statsread enpoint has very aggressive throttling, so we shouldn't read the SVD after each
    // update. Just make sure we are in sync once at the end of the test.
    if (s_svd)
    {

        HRESULT hr = S_OK;
        auto updatedSVD{ GetStatsValueDocument(Data()->xboxUserId, &hr) };
        if (FAILED(hr))
        {
            return 0;
        }

        LogToFile("Service SVD: %s", updatedSVD.Serialize().data());
        LogToFile("Local SVD: %s", s_svd->Serialize().data());
        assert(*s_svd == updatedSVD);
    }

    return 0;
}

int ClearSVD_Lua(lua_State* L)
{
    UNREFERENCED_PARAMETER(L);

    if (!s_svd)
    {
        HRESULT hr = S_OK;
        s_svd = std::make_unique<StatsValueDocument>(GetStatsValueDocument(Data()->xboxUserId, &hr));
        if (FAILED(hr))
        {
            LogToFile("XblTitleManagedStatsWriteAsyncWithSVD: hr = %s", ConvertHR(hr).data());
            s_svd = nullptr;
            lua_pushnumber(L, 1);
            return 1;
        }
    }
    s_svd->ClearStats();
    return 0;
}

void SetupAPIs_XblTitleManagedStats()
{
    lua_register(Data()->L, "XblTitleManagedStatsWriteAsyncWithSVD", XblTitleManagedStatsWriteAsyncWithSVD_Lua);
    lua_register(Data()->L, "XblTitleManagedStatsWriteAsync", XblTitleManagedStatsWriteAsync_Lua);
    lua_register(Data()->L, "XblTitleManagedStatsUpdateStatsAsync", XblTitleManagedStatsUpdateStatsAsync_Lua);
    lua_register(Data()->L, "XblTitleManagedStatsDeleteStatsAsync", XblTitleManagedStatsDeleteStatsAsync_Lua);

    // Test helpers
    lua_register(Data()->L, "ValidateSVD", ValidateSVD_Lua);
    lua_register(Data()->L, "ClearSVD", ClearSVD_Lua);
}
