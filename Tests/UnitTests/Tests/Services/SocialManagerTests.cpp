// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"
#include "xsapi-c/social_manager_c.h"
#include "xsapi-cpp/social_manager.h"
#include "social_manager_internal.h"
#include "httpClient/httpClient.h"
#include "httpClient/httpProvider.h"

using namespace xbox::services::presence;
using namespace xbox::services::real_time_activity;
using namespace xbox::services::social::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const char defaultPeoplehubTemplate[] = R"(
{
    "xuid": "1",
    "isFriend": true,
    "isFavorite": false,
    "isFollowingCaller": true,
    "isFollowedByCaller": true,
    "isIdentityShared": false,
    "displayName": "TestGamerTag",
    "realName": "",
    "displayPicRaw": "http://images-eds.xboxlive.com/image?url=mHGRD8KXEf2sp2LC58XhBQKNl2IWRp.J.q8mSURKUUeiPPf0Y7Kl7zLN7rafayiPptVaX_XIUmNOPotNmNubbx4bHmf6It7Oj1ChU5UAo9k-&background=0xababab&mode=Padding&format=png",
    "useAvatar": false,
    "gamertag": "TestGamerTag",
    "modernGamertag": "TestGamerTag",
    "modernGamertagSuffix": "",
    "uniqueModernGamertag": "TestGamerTag",
    "gamerScore": "9001",
    "presenceState": "Online",
    "presenceDevices": null,
    "isBroadcasting": false,
    "titleHistory": 
    {
        "titleName": "Forza Horizon 2",
        "titleId": "1234",
        "lastTimePlayed": "2015-01-26T22:54:54.6630000Z",
        "lastTimePlayedText": "8 months ago"
    },
    "suggestion": null,
    "multiplayerSummary": {
        "InMultiplayerSession": 0,
        "InParty": 0
    },
    "recentPlayer": null,
    "follower": null,
    "preferredColor": {
        "primaryColor": "193e91",
        "secondaryColor": "2458cf",
        "tertiaryColor": "122e6b"
    },
    "titlePresence": null,
    "titleSummaries": null,
    "presenceDetails": [{
        "IsBroadcasting": false,
        "Device": "PC",
        "State": "Active",
        "TitleId": "1234",
        "PresenceText": "Home"
    }]
    }
)";

const char onlinePresenceResponseTemplate[] = R"(
{
    "xuid": "1",
    "state": "Online",
    "devices": [
        {
            "type": "PC",
            "titles": [
                {
                    "id": "1234",
                    "name": "Home",
                    "lastModified": "2013-02-01T00:00:00Z",
                    "state": "active",
                    "placement": "Full",
                    "activity": {
                        "richPresence": "Home"
                    }
                }
            ]
        }
    ]
})";

const char offlinePresenceResponseTemplate[] = R"(
{
    "xuid": "1",
    "state": "Offline",
    "devices": []
}
)";

const char* presenceOnlineRtaMessageSubscribeComplete = R"({"xuid":"2814613569642996","state":"Online","devices":[{"type":"MCapensis","titles":[{"id":"1234","name":"Default Title","placement":"Full","state":"Active", "activity": {"richPresence":"Home"}, "lastModified":"2016-09-30T00:15:35.5994615Z"}]}]})";
const char* presenceOfflineRtaMessageSubscribeComplete = R"({"xuid":"2814613569642996","state":"Offline"})";

// ---------------------------------------------------------------------------------------------------
// Fault-injection infrastructure for the initial People Hub GetFollowedUsers stall (AB#63099583).
//
// SocialGraph only schedules a People Hub retry from the request's completion callback (the Failed
// branch) and only sets m_initialized on success. A People Hub GetFollowedUsers request that NEVER
// completes must therefore be recovered by the completion-independent watchdog; otherwise
// m_initialized stays false and LocalUserAdded is never raised - a permanent empty-friends stall
// (only a reboot or the 20-min refresh would recover). The tests below reproduce the never-completing
// case deterministically by hanging the initial People Hub request at the libHttpClient provider
// layer.
//
// Mechanism: libHttpClient resolves HCMocks BEFORE the provider (httpcall.cpp), so to reach a custom
// perform function the target call must be UNMOCKED; all other traffic stays mocked and never reaches
// the hook. The external perform function lives in a process-wide singleton that CANNOT be unset
// (ExternalHttpProvider::SetCallback rejects null) and persists across HCCleanup, so state is kept in
// a file-static guarded by 'active': while active it hangs People Hub calls; once the test ends it
// fast-completes anything that reaches it, which is safe because the unit-test suite mocks all HTTP.
// It must be installed BEFORE XblInitialize (HCSetHttpCallPerformFunction requires HC uninitialized).
struct HangingPeoplehubState
{
    std::mutex mutex;
    bool active{ false };
    bool releasing{ false };
    bool hangOnlyFirstCall{ false }; // If set, only the first People Hub call hangs; the rest succeed.
    int peoplehubCallCount{ 0 };     // Total People Hub calls that reached the provider (issued + retried).
    std::vector<std::pair<HCCallHandle, XAsyncBlock*>> hungCalls;
};

static HangingPeoplehubState s_hangingPeoplehubState;

inline void CompletePeoplehubEmptyOk(HCCallHandle call, XAsyncBlock* asyncBlock) noexcept
{
    static const char emptyPeople[] = R"({"people":[]})";
    HCHttpCallResponseSetStatusCode(call, 200);
    HCHttpCallResponseSetResponseBodyBytes(call, reinterpret_cast<const uint8_t*>(emptyPeople), sizeof(emptyPeople) - 1);
    XAsyncComplete(asyncBlock, S_OK, 0);
}

static void CALLBACK HangingPeoplehubPerform(HCCallHandle call, XAsyncBlock* asyncBlock, void* /*context*/, HCPerformEnv /*env*/)
{
    const char* method{ nullptr };
    const char* url{ nullptr };
    HCHttpCallRequestGetUrl(call, &method, &url);
    const bool isPeoplehub{ url != nullptr && strstr(url, "peoplehub.xboxlive.com") != nullptr };

    bool hang{ false };
    {
        std::lock_guard<std::mutex> lock{ s_hangingPeoplehubState.mutex };
        if (s_hangingPeoplehubState.active && !s_hangingPeoplehubState.releasing && isPeoplehub)
        {
            const int callIndex{ ++s_hangingPeoplehubState.peoplehubCallCount };
            const bool shouldHang{ !s_hangingPeoplehubState.hangOnlyFirstCall || callIndex == 1 };
            if (shouldHang)
            {
                // Never complete: capture and drop. The request hangs for its entire lifetime.
                s_hangingPeoplehubState.hungCalls.emplace_back(call, asyncBlock);
                hang = true;
            }
        }
    }

    if (hang)
    {
        return;
    }

    // Non-hung People Hub calls (and any released / post-test / unexpected call) complete immediately
    // so nothing hangs at teardown and no later test dereferences stale state.
    CompletePeoplehubEmptyOk(call, asyncBlock);
}

// RAII installer for the hanging People Hub provider. MUST be declared BEFORE the SMTestEnvironment
// (i.e. before XblInitialize) in a test.
class ScopedHangingPeoplehubProvider
{
public:
    // hangOnlyFirstCall: when true, only the first People Hub request hangs and any subsequent (e.g.
    // watchdog-retried) request completes successfully - used to validate watchdog-driven recovery.
    explicit ScopedHangingPeoplehubProvider(bool hangOnlyFirstCall = false) noexcept
    {
        {
            std::lock_guard<std::mutex> lock{ s_hangingPeoplehubState.mutex };
            s_hangingPeoplehubState.active = true;
            s_hangingPeoplehubState.releasing = false;
            s_hangingPeoplehubState.hangOnlyFirstCall = hangOnlyFirstCall;
            s_hangingPeoplehubState.peoplehubCallCount = 0;
            s_hangingPeoplehubState.hungCalls.clear();
        }
        // Requires HC to be uninitialized (before the environment's XblInitialize / HCInit).
        VERIFY_SUCCEEDED(HCSetHttpCallPerformFunction(HangingPeoplehubPerform, &s_hangingPeoplehubState));
    }

    ~ScopedHangingPeoplehubProvider() noexcept
    {
        Release();
        std::lock_guard<std::mutex> lock{ s_hangingPeoplehubState.mutex };
        s_hangingPeoplehubState.active = false;
        // The external provider persists process-wide and cannot be unset (SetCallback rejects null).
        // Leaving active=false turns the hook into a safe fast-completer for any later call; the unit
        // test suite mocks all HTTP, so it is effectively never invoked again.
    }

    size_t HungCallCount() noexcept
    {
        std::lock_guard<std::mutex> lock{ s_hangingPeoplehubState.mutex };
        return s_hangingPeoplehubState.hungCalls.size();
    }

    int PeoplehubCallCount() noexcept
    {
        std::lock_guard<std::mutex> lock{ s_hangingPeoplehubState.mutex };
        return s_hangingPeoplehubState.peoplehubCallCount;
    }

    // Complete any hung People Hub calls with an empty successful response so outstanding async work
    // can drain and test teardown (XblCleanup) does not block.
    void Release() noexcept
    {
        std::vector<std::pair<HCCallHandle, XAsyncBlock*>> calls;
        {
            std::lock_guard<std::mutex> lock{ s_hangingPeoplehubState.mutex };
            s_hangingPeoplehubState.releasing = true;
            calls.swap(s_hangingPeoplehubState.hungCalls);
        }

        for (auto& c : calls)
        {
            CompletePeoplehubEmptyOk(c.first, c.second);
        }
    }
};

// Releases any hung People Hub calls when it leaves scope. Declared immediately AFTER the
// SMTestEnvironment so it is destroyed BEFORE that environment: on a thrown VERIFY_* the hung calls are
// completed before env's dtor runs XblCleanup, which would otherwise block forever on the still-hung
// call (its releaser, the provider dtor, cannot run until env is gone) and mask the assertion failure.
// Release() is idempotent, so this composes safely with the explicit Release() on the success path.
struct ScopedHungCallReleaser
{
    ScopedHangingPeoplehubProvider& provider;
    ~ScopedHungCallReleaser() noexcept { provider.Release(); }
};

#define NUM_USERS 100

// Peak-allocation tracking for TestLargeGraphPeakMemory (AB#63480128). These have to live at file
// scope because XblMemSetFunctions takes plain function pointers and a local class cannot hold the
// static state they need.
namespace PeakMemory
{
    // XSAPI allocates from its task queue threads as well as the caller's, so this state must be
    // synchronized. std::map/std::mutex use the default allocator, not the hook, so there is no
    // reentrancy risk here.
    std::mutex g_mutex;
    std::map<void*, size_t> g_allocations;
    size_t g_liveBytes{ 0 };
    size_t g_peakBytes{ 0 };

    // These hooks must be interchangeable with XSAPI's DefaultAlloc/DefaultFree (malloc/free),
    // because memory crosses the hook window in both directions: process-lifetime mocks such as
    // MockRealTimeActivityService lazily free strings earlier tests allocated through the default
    // hook, and strings allocated here outlive the tracker and are later freed by DefaultFree.
    _Ret_maybenull_ _Post_writable_byte_size_(size) void* STDAPIVCALLTYPE AllocHook(
        _In_ size_t size,
        _In_ HCMemoryType memType
    )
    {
        UNREFERENCED_PARAMETER(memType);
        if (size == 0)
        {
            return nullptr;
        }

        void* ptr{ malloc(size) };
        if (ptr)
        {
            std::lock_guard<std::mutex> lock{ g_mutex };
            g_allocations[ptr] = size;
            g_liveBytes += size;
            g_peakBytes = std::max(g_peakBytes, g_liveBytes);
        }
        return ptr;
    }

    void STDAPIVCALLTYPE FreeHook(
        _In_ void* address,
        _In_ HCMemoryType memType
    )
    {
        UNREFERENCED_PARAMETER(memType);
        if (!address)
        {
            return;
        }

        {
            // Pointers allocated before the tracker was installed are expected here (see above);
            // they are freed normally but don't affect the live-byte count.
            std::lock_guard<std::mutex> lock{ g_mutex };
            auto iter{ g_allocations.find(address) };
            if (iter != g_allocations.end())
            {
                g_liveBytes -= iter->second;
                g_allocations.erase(iter);
            }
        }
        free(address);
    }

    size_t PeakBytes() noexcept
    {
        std::lock_guard<std::mutex> lock{ g_mutex };
        return g_peakBytes;
    }

    // RAII installer. Hooks must be set before XblInitialize, so declare this before the
    // SMTestEnvironment it is measuring.
    struct ScopedTracker
    {
        ScopedTracker() noexcept
        {
            {
                std::lock_guard<std::mutex> lock{ g_mutex };
                g_allocations.clear();
                g_liveBytes = 0;
                g_peakBytes = 0;
            }
            VERIFY_SUCCEEDED(XblMemSetFunctions(AllocHook, FreeHook));
        }

        ~ScopedTracker() noexcept
        {
            VERIFY_SUCCEEDED(XblMemSetFunctions(nullptr, nullptr));
        }
    };
}

DEFINE_TEST_CLASS(SocialManagerTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(SocialManagerTests);

private:
    // RAII class for basic SocialManager Test Setup/teardown including relevant mocks
    class SMTestEnvironment : public TestEnvironment
    {
    public:
        SMTestEnvironment() noexcept
        {
            FollowedXuids.reserve(NUM_USERS);
            for (uint64_t i = 0; i < NUM_USERS; ++i)
            {
                FollowedXuids.push_back(i + 1);
            }

            MockRtaService().SetSubscribeHandler(RTASubscribeHandler);

            // Setup default presence and peoplehub mocks
            SetPeoplehubMock();
            SetPresenceMock();
        }

        ~SMTestEnvironment() noexcept
        {
            // Remove all local users
            size_t userCount = XblSocialManagerGetLocalUserCount();
            auto users{ new XblUserHandle[userCount] };
            VERIFY_SUCCEEDED(XblSocialManagerGetLocalUsers(userCount, users));

            for (size_t i = 0; i < userCount; ++i)
            {
                VERIFY_SUCCEEDED(XblSocialManagerRemoveLocalUser(users[i]));
            }

#if ENABLE_PERF_PROFILING
            TEST_LOG(Utils::StringTFromUtf8(detail::PerfTester::Instance().FormatStats().data()).data());
#endif

            // Presence polls (and other HTTP calls) can still be in flight here. Clean up XSAPI now,
            // which waits for them, so the HttpMocks they match and the state the mock callbacks
            // capture are still alive; ~TestEnvironment's cleanup then becomes a no-op.
            Cleanup();
        }

        // CPP helpers wrapping SocialManager APIs

        // Add local user and validate the expected events are received
        void AddLocalUser(const User& user) const noexcept
        {
            LOGS_DEBUG << "Adding User to SocialManager and awaiting LocalUserAdded and RTA subscriptions";

            // As a testing convenience, await sub handshakes to complete so that tests can fire
            // RTA events and be sure that the social graph is prepared to respond to them
            struct RTASubResponder
            {
                RTASubResponder(size_t followeeCount)
                {
                    auto& rtaService{ system::MockRealTimeActivityService::Instance() };
                    rtaService.SetSubscribeHandler([=](uint32_t n, xsapi_internal_string uri)
                    {
                        SMTestEnvironment::RTASubscribeHandler(n, uri);
                        if (uri.find("https://userpresence.xboxlive.com") != xsapi_internal_string::npos)
                        {
                            ++presenceSubsComplete;
                        }
                        else if (uri.find("http://social.xboxlive.com") != xsapi_internal_string::npos)
                        {
                            socialRelationshipSubComplete = true;
                        }

                        // Graph will create two presence subscriptions for each followed User (Device & title presence)
                        if (presenceSubsComplete >= followeeCount * 2 && socialRelationshipSubComplete)
                        {
                            complete.Set();
                        }
                    });
                }

                ~RTASubResponder()
                {
                    // Reset to default handler
                    system::MockRealTimeActivityService::Instance().SetSubscribeHandler(SMTestEnvironment::RTASubscribeHandler);
                }

                size_t presenceSubsComplete{ 0 };
                bool socialRelationshipSubComplete{ false };
                Event complete{};
            } rtaResponder{ FollowedXuids.size() };

            VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(user.Handle(), XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));
            AwaitEvents({ {XblSocialManagerEventType::LocalUserAdded, 1} });
            rtaResponder.complete.Wait();
        }

        std::vector<const XblSocialManagerEvent*> DoWork() const noexcept
        {
            const XblSocialManagerEvent* events{ nullptr };
            size_t eventCount{ 0 };
            HRESULT hr = XblSocialManagerDoWork(&events, &eventCount);
            if (FAILED(hr))
            {
                VERIFY_FAIL(); // avoiding log spam
            }

            auto eventsVector = Utils::Transform<const XblSocialManagerEvent*>(events, eventCount, [](const XblSocialManagerEvent& e) { return &e; });
            LogEvents(eventsVector);
            return eventsVector;
        }

        // DoWork until the specified events happen.
        void AwaitEvents(std::map<XblSocialManagerEventType, size_t> expectedEvents) const noexcept
        {
            while (!expectedEvents.empty())
            {
                auto events{ DoWork() };
                for (auto event : events)
                {
                    auto iter{ expectedEvents.find(event->eventType) };
                    if (iter == expectedEvents.end())
                    {
                        LOGS_DEBUG << "Unexpected SocialManager Event";
                        VERIFY_FAIL();
                    }
                    else if(--iter->second == 0)
                    {
                        expectedEvents.erase(iter);
                    }
                }
            }
        }

        std::vector<const XblSocialManagerUser*> GetUsers(
            XblSocialManagerUserGroupHandle group
        ) const noexcept
        {
            const XblSocialManagerUser* const* users{ nullptr };
            size_t userCount{ 0 };
            VERIFY_SUCCEEDED(XblSocialManagerUserGroupGetUsers(group, &users, &userCount));
            return std::vector<const XblSocialManagerUser*>(users, users + userCount);
        }

        size_t GetUsersCount(
            XblSocialManagerUserGroupHandle group
        ) const noexcept
        {
            const XblSocialManagerUser* const* users{ nullptr };
            size_t userCount{ 0 };
            VERIFY_SUCCEEDED(XblSocialManagerUserGroupGetUsers(group, &users, &userCount));
            return userCount;
        }

        size_t GetTrackedUsersCount(
            XblSocialManagerUserGroupHandle group
        ) const noexcept
        {
            const uint64_t* trackedUsers{ nullptr };
            size_t userCount{ 0 };
            VERIFY_SUCCEEDED(XblSocialManagerUserGroupGetUsersTrackedByGroup(group, &trackedUsers, &userCount));
            return userCount;
        }

        // Configure Mocks used by SocialManager. PeoplehubService derives isFollowedByCaller and
        // isFollowingUser from the response's isFriend, so isFriend controls the relationship.
        void SetPeoplehubMock(
            bool online = true,
            bool isFavorite = false,
            bool isFriend = true
        ) noexcept
        {
            // No HTTP method so it matches both batch and get friends calls
            m_peoplehubMock = std::make_shared<HttpMock>("", "https://peoplehub.xboxlive.com");

            m_peoplehubMock->SetMockMatchedCallback(
                [
                    this,
                    online,
                    isFavorite,
                    isFriend
                ]
            (HttpMock* mock, xsapi_internal_string /*requestUrl*/, xsapi_internal_string requestBody)
                {
                    std::vector<uint64_t> xuids;

                    auto jsonRequest = Utils::ParseJson(requestBody.data());
                    if (jsonRequest.is_null())
                    {
                        // Empty request body => "get followed users" fetch (the call the watchdog guards).
                        ++this->m_followedUsersCallCount;
                        xuids = this->FollowedXuids;
                    }
                    else
                    {
                        auto xuidArray = jsonRequest[L"xuids"];
                        for (size_t i = 0; i < xuidArray.size(); ++i)
                        {
                            xuids.push_back(Utils::Uint64FromStringT(xuidArray[i].as_string()));
                        }
                    }

                    JsonDocument responseBody(rapidjson::kObjectType);
                    JsonDocument::AllocatorType& allocator = responseBody.GetAllocator();

                    JsonValue peopleArray(rapidjson::kArrayType);
                    for (auto& xuid : xuids)
                    {
                        JsonDocument jsonBlob(&allocator);
                        jsonBlob.Parse(defaultPeoplehubTemplate);
                        JsonUtils::SetMember(jsonBlob, "xuid", JsonValue(utils::uint64_to_internal_string(xuid).c_str(), allocator));
                        JsonUtils::SetMember(jsonBlob, "presenceState", JsonValue((online ? "Online" : "Offline"), allocator));
                        JsonUtils::SetMember(jsonBlob, "isFavorite", JsonValue(isFavorite));
                        JsonUtils::SetMember(jsonBlob, "isFriend", JsonValue(isFriend));
                        peopleArray.PushBack(jsonBlob, allocator);
                    }

                    
                    responseBody.AddMember("people", peopleArray, allocator);
                    mock->SetResponseBody(responseBody);
                });
        }

        // Peoplehub mock that fails the FIRST "followed users" fetch (the initial GetFollowedUsers,
        // which has an empty request body) with a non-retriable HTTP 404 - chosen so libHttpClient
        // does not auto-retry it and the failure surfaces as a completed error to SocialGraph - then
        // serves normal successful responses. Backs the test that a COMPLETED failure on the initial
        // fetch is retried by SocialGraph (c_failureRetryIntervalMs) and the graph recovers.
        void SetPeoplehubMockFailFirstFollowedUsersCall() noexcept
        {
            m_peoplehubMock = std::make_shared<HttpMock>("", "https://peoplehub.xboxlive.com");

            m_peoplehubMock->SetMockMatchedCallback(
                [
                    this
                ]
            (HttpMock* mock, xsapi_internal_string /*requestUrl*/, xsapi_internal_string requestBody)
                {
                    std::vector<uint64_t> xuids;

                    auto jsonRequest = Utils::ParseJson(requestBody.data());
                    if (jsonRequest.is_null())
                    {
                        // Followed-users (initial graph) fetch. Count every one (so a test can assert a
                        // retry occurred) and fail only the very first.
                        if (this->m_followedUsersCallCount.fetch_add(1) == 0)
                        {
                            mock->SetResponseHttpStatus(404);
                            mock->ClearReponseBody();
                            return;
                        }
                        xuids = this->FollowedXuids;
                    }
                    else
                    {
                        auto xuidArray = jsonRequest[L"xuids"];
                        for (size_t i = 0; i < xuidArray.size(); ++i)
                        {
                            xuids.push_back(Utils::Uint64FromStringT(xuidArray[i].as_string()));
                        }
                    }

                    JsonDocument responseBody(rapidjson::kObjectType);
                    JsonDocument::AllocatorType& allocator = responseBody.GetAllocator();

                    JsonValue peopleArray(rapidjson::kArrayType);
                    for (auto& xuid : xuids)
                    {
                        JsonDocument jsonBlob(&allocator);
                        jsonBlob.Parse(defaultPeoplehubTemplate);
                        JsonUtils::SetMember(jsonBlob, "xuid", JsonValue(utils::uint64_to_internal_string(xuid).c_str(), allocator));
                        JsonUtils::SetMember(jsonBlob, "presenceState", JsonValue("Online", allocator));
                        JsonUtils::SetMember(jsonBlob, "isFavorite", JsonValue(false));
                        JsonUtils::SetMember(jsonBlob, "isFriend", JsonValue(true));
                        peopleArray.PushBack(jsonBlob, allocator);
                    }

                    responseBody.AddMember("people", peopleArray, allocator);
                    mock->SetResponseHttpStatus(200);
                    mock->SetResponseBody(responseBody);
                });
        }

        // Remove the People Hub HttpMock so People Hub requests fall through to the libHttpClient
        // provider (see ScopedHangingPeoplehubProvider). Mocks are resolved before the provider, so
        // an installed mock would otherwise short-circuit the call before it can be hung.
        void RemovePeoplehubMock() noexcept
        {
            m_peoplehubMock.reset();
        }

        // Peoplehub mock that serves the default profile for every user, then lets 'mutator' rewrite
        // individual profiles (null, over-long, wrong-typed, or missing fields) before the response is
        // sent. Backs the tests that one malformed profile cannot block the whole social graph.
        using PeoplehubProfileMutator = std::function<void(uint64_t xuid, JsonDocument& profile)>;
        void SetPeoplehubMockWithProfileMutator(PeoplehubProfileMutator mutator) noexcept
        {
            m_peoplehubMock = std::make_shared<HttpMock>("", "https://peoplehub.xboxlive.com");

            m_peoplehubMock->SetMockMatchedCallback(
                [
                    this,
                    mutator
                ]
            (HttpMock* mock, xsapi_internal_string /*requestUrl*/, xsapi_internal_string requestBody)
                {
                    std::vector<uint64_t> xuids;

                    auto jsonRequest = Utils::ParseJson(requestBody.data());
                    if (jsonRequest.is_null())
                    {
                        ++this->m_followedUsersCallCount;
                        xuids = this->FollowedXuids;
                    }
                    else
                    {
                        auto xuidArray = jsonRequest[L"xuids"];
                        for (size_t i = 0; i < xuidArray.size(); ++i)
                        {
                            xuids.push_back(Utils::Uint64FromStringT(xuidArray[i].as_string()));
                        }
                    }

                    JsonDocument responseBody(rapidjson::kObjectType);
                    JsonDocument::AllocatorType& allocator = responseBody.GetAllocator();

                    JsonValue peopleArray(rapidjson::kArrayType);
                    for (auto& xuid : xuids)
                    {
                        JsonDocument jsonBlob(&allocator);
                        jsonBlob.Parse(defaultPeoplehubTemplate);
                        JsonUtils::SetMember(jsonBlob, "xuid", JsonValue(utils::uint64_to_internal_string(xuid).c_str(), allocator));
                        mutator(xuid, jsonBlob);
                        peopleArray.PushBack(jsonBlob, allocator);
                    }

                    responseBody.AddMember("people", peopleArray, allocator);
                    mock->SetResponseHttpStatus(200);
                    mock->SetResponseBody(responseBody);
                });
        }

        // Presence service will respond that all users are online except for those specified in offlineXuids
        void SetPresenceMock(
            const std::vector<uint64_t>& offlineXuids = {},
            uint32_t titleId = MOCK_TITLEID
        ) noexcept
        {
            m_presenceMock = std::make_shared<HttpMock>("GET", "https://userpresence.xboxlive.com");

            m_presenceMock->SetMockMatchedCallback(
                [
                    offlineXuids = std::unordered_set<uint64_t>{ offlineXuids.begin(), offlineXuids.end() },
                    titleId,
                    requestCount = m_presenceRequestCount
                ]
            (HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody) mutable
                {
                    ++(*requestCount);
                    SetPresenceResponse(mock, requestUrl, requestBody, std::move(offlineXuids), titleId);
                });
        }

        // Number of presence service requests served by the presence mock since the environment was created
        size_t PresenceRequestCount() const noexcept
        {
            return m_presenceRequestCount->load();
        }

        void FireDevicePresenceChangeRtaEvent(
            const std::vector<uint64_t>& userList,
            bool online = false
        ) const noexcept
        {
            for (auto xuid : userList)
            {
                xsapi_internal_stringstream uri;
                uri << "https://userpresence.xboxlive.com/users/xuid(" << xuid << ")/devices";

                MockRealTimeActivityService::Instance().RaiseEvent(uri.str(), online ? R"("PC:true")" : R"("PC:false")");
            }
        }

        void FireTitlePresenceChangeRtaEvent(
            const std::vector<uint64_t>& userList,
            bool ended = true
        ) const noexcept
        {
            for (auto xuid : userList)
            {
                xsapi_internal_stringstream uri;
                uri << "https://userpresence.xboxlive.com/users/xuid(" << xuid << ")/titles/1234";

                MockRealTimeActivityService::Instance().RaiseEvent(uri.str(), ended ? R"("ended")" : R"("started")");
            }
        }

        void FireSocialGraphChangedRtaEvent(
            const User& localUser,
            XblSocialNotificationType notificationType,
            uint64_t affectedXuid
        ) const noexcept
        {
            xsapi_internal_stringstream uri;
            uri << "http://social.xboxlive.com/users/xuid(" << localUser.Xuid() << ")/friends";

            rapidjson::Document eventData{ rapidjson::kObjectType };
            auto& a{ eventData.GetAllocator() };

            eventData.AddMember("NotificationType", rapidjson::Value{ EnumName(notificationType).data(), a }, a);
            rapidjson::Value eventDataXuids{ rapidjson::kArrayType };
            eventDataXuids.PushBack(rapidjson::Value{ Utils::StringFromUint64(affectedXuid).c_str(), a }.Move(), a);
            eventData.AddMember("Xuids", eventDataXuids.Move(), a);

            MockRealTimeActivityService::Instance().RaiseEvent(uri.str(), eventData);
        }

        // Xuids Local Users follow
        std::vector<uint64_t> FollowedXuids;

        // Number of "get followed users" People Hub calls (empty request body) served by the People Hub
        // mocks. Lets a test assert the watchdog does not spuriously re-issue a healthy initial fetch, or
        // that a failed initial fetch was retried.
        std::atomic<int> m_followedUsersCallCount{ 0 };
        int FollowedUsersCallCount() const noexcept { return m_followedUsersCallCount.load(); }

    private:
        static void RTASubscribeHandler(uint32_t n, xsapi_internal_string uri)
        {
            auto& rtaService{ system::MockRealTimeActivityService::Instance() };
            if (uri.find("https://userpresence.xboxlive.com") != xsapi_internal_string::npos)
            {
                rtaService.CompleteSubscribeHandshake(n, presenceOnlineRtaMessageSubscribeComplete);
            }
            else if (uri.find("http://social.xboxlive.com") != xsapi_internal_string::npos)
            {
                rtaService.CompleteSubscribeHandshake(n);
            }
        }

        static size_t SetPresenceResponse(
            HttpMock* mock,
            const xsapi_internal_string& requestUrl,
            const xsapi_internal_string& requestBody,
            std::unordered_set<uint64_t>&& offlineXuids = {},
            uint32_t titleId = MOCK_TITLEID
        )
        {
            JsonDocument userArr{};
            if (requestUrl.find("batch") != xsapi_internal_string::npos)
            {
                // for batch requests the user list is in the request body
                JsonDocument jsonRequest{ rapidjson::kObjectType };
                jsonRequest.Parse(requestBody.data());
                userArr.CopyFrom(jsonRequest["users"], userArr.GetAllocator());
                assert(userArr.IsArray());
            }
            else
            {
                assert(requestBody.empty());
                userArr.SetArray();
                auto beginIndex{ requestUrl.find("(") };
                auto endIndex{ requestUrl.find(")") };
                userArr.PushBack(JsonValue{ requestUrl.substr(beginIndex + 1, endIndex - beginIndex - 1).data(), userArr.GetAllocator() }, userArr.GetAllocator());
            }

            JsonDocument response(rapidjson::kArrayType);
            JsonDocument::AllocatorType& allocator = response.GetAllocator();
            for (uint32_t i = 0; i < userArr.Size(); ++i)
            {
                if (offlineXuids.find(utils::internal_string_to_uint64(userArr[i].GetString())) == offlineXuids.end())
                {
                    JsonDocument onlinePresenceResponseTemplateJson(&allocator);
                    onlinePresenceResponseTemplateJson.Parse(onlinePresenceResponseTemplate);
                    response.PushBack(onlinePresenceResponseTemplateJson, allocator);
                    JsonUtils::SetMember(response[i]["devices"][0]["titles"][0], allocator, "id", JsonValue(utils::uint32_to_internal_string(titleId).c_str(), allocator));
                }
                else
                {
                    JsonDocument offlinePresenceResponseTemplateJson(&allocator);
                    offlinePresenceResponseTemplateJson.Parse(offlinePresenceResponseTemplate);
                    response.PushBack(offlinePresenceResponseTemplateJson, allocator);
                }
                JsonUtils::SetMember(response[i], allocator, "xuid", userArr[i]);
            }

            mock->SetResponseBody(response);

            return userArr.Size();
        }

        void LogEvents(
            const std::vector<const XblSocialManagerEvent*>& events
        ) const noexcept
        {
            static std::unordered_map<XblSocialManagerEventType, xsapi_internal_string> eventTypesMap
            {
                { XblSocialManagerEventType::UsersAddedToSocialGraph, "UsersAddedToSocialGraph" },
                { XblSocialManagerEventType::UsersRemovedFromSocialGraph, "UsersRemovedFromSocialGraph" },
                { XblSocialManagerEventType::PresenceChanged, "PresenceChanged" },
                { XblSocialManagerEventType::ProfilesChanged, "ProfilesChanged" },
                { XblSocialManagerEventType::SocialRelationshipsChanged, "SocialRelationshipsChanged" },
                { XblSocialManagerEventType::LocalUserAdded, "LocalUserAdded" },
                { XblSocialManagerEventType::SocialUserGroupLoaded, "SocialUserGroupLoaded" },
                { XblSocialManagerEventType::SocialUserGroupUpdated, "SocialUserGroupUpdated" },
                { XblSocialManagerEventType::UnknownEvent, "UnknownEvent" }
            };

            for (auto& event : events)
            {
                xsapi_internal_stringstream ss;
                ss << "SocialManager Event: " << eventTypesMap[event->eventType] << std::endl;
                for (uint32_t i = 0; i < XBL_SOCIAL_MANAGER_MAX_AFFECTED_USERS_PER_EVENT; i++)
                {
                    if (event->usersAffected[i] != nullptr)
                    {
                        if (i == 0)
                        {
                            ss << "Users affected: " << std::endl;
                        }
                        ss << "\t" << event->usersAffected[i]->xboxUserId << std::endl;
                    }
                }
                LOGS_DEBUG << ss.str();
            }
        }

        // Default mocks used by SocialManager
        std::shared_ptr<HttpMock> m_peoplehubMock;
        std::shared_ptr<HttpMock> m_presenceMock;
        std::shared_ptr<std::atomic<size_t>> m_presenceRequestCount{ std::make_shared<std::atomic<size_t>>(0) };
    };

public:
    // Verifies every presence title record slot at or past presenceTitleRecordCount is entirely
    // zero, so callers never see residual data past the valid records.
    static void VerifyUnusedPresenceSlotsAreZeroed(
        const XblSocialManagerPresenceRecord& record
    ) noexcept
    {
        const XblSocialManagerPresenceTitleRecord zeroed{};
        for (uint32_t i = record.presenceTitleRecordCount; i < XBL_NUM_PRESENCE_RECORDS; ++i)
        {
            if (memcmp(&record.presenceTitleRecords[i], &zeroed, sizeof(zeroed)) != 0)
            {
                LOGS_DEBUG << "Presence title record slot " << i << " is not zeroed";
                VERIFY_FAIL();
                return;
            }
        }
    }

    DEFINE_TEST_CASE(TestAddLocalUser)
    {
        TEST_LOG(L"Test starting: TestAddLocalUser");

        SMTestEnvironment env{};
        auto xboxLiveContext{ env.CreateMockXboxLiveContext() };
        env.AddLocalUser(xboxLiveContext->User());
    }

    // A COMPLETED failure (HTTP error) on the initial People Hub GetFollowedUsers must be retried by
    // SocialGraph so the graph still initializes and LocalUserAdded is raised. This is the control for the
    // never-completing case (AB#63099583): a failed completion self-heals, whereas a request that never
    // completes is recovered by the watchdog (tests below).
    //
    // The invariant is asserted functionally - recovery happened AND it required more than the initial
    // (failed) fetch - via the followed-users call count, NOT via wall-clock timing. A completed failure
    // can be recovered by either the 10s failure retry (c_failureRetryIntervalMs, the intended path) or,
    // if the failing completion is delivered after the unit-test watchdog interval under load, by the 2s
    // watchdog re-issue. Both are valid recovery, so a wall-clock lower bound on recovery time is racy and
    // must not be asserted; the call count proves a retry occurred regardless of which timer fired.
    DEFINE_TEST_CASE(TestInitialFollowedUsersFailureIsRetried)
    {
        TEST_LOG(L"Test starting: TestInitialFollowedUsersFailureIsRetried");

        SMTestEnvironment env{};
        env.SetPeoplehubMockFailFirstFollowedUsersCall();

        auto xboxLiveContext{ env.CreateMockXboxLiveContext() };

        VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(
            xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));

        // Drive DoWork with a bounded deadline so a genuine stall fails the test instead of hanging.
        const auto deadline{ std::chrono::steady_clock::now() + std::chrono::seconds(30) };
        bool localUserAdded{ false };

        while (std::chrono::steady_clock::now() < deadline && !localUserAdded)
        {
            for (auto e : env.DoWork())
            {
                if (e->eventType == XblSocialManagerEventType::LocalUserAdded)
                {
                    localUserAdded = true;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        LOGS_DEBUG << "localUserAdded: " << localUserAdded
            << ", followed-users calls: " << env.FollowedUsersCallCount();

        // The graph recovered from a failed initial fetch: the failure path is self-healing.
        VERIFY_IS_TRUE(localUserAdded);

        // Recovery required a retry: the initial fetch failed (404) and at least one later fetch was
        // issued and succeeded. This proves the completed failure was retried, not that the first
        // attempt happened to succeed.
        VERIFY_IS_TRUE(env.FollowedUsersCallCount() >= 2);
    }

    DEFINE_TEST_CASE(TestInitialFollowedUsersHangIsRetriedByWatchdog)
    {
        TEST_LOG(L"Test starting: TestInitialFollowedUsersHangIsRetriedByWatchdog");

        // Empty-friends bug (AB#63099583): fault-inject People Hub GetFollowedUsers requests that
        // NEVER complete. SocialGraph only schedules a retry from the request's completion callback
        // (Failed branch), so a never-completing request relies entirely on the completion-independent
        // watchdog to make progress. The watchdog re-issues the hung request on a timer, so the
        // hung-call count keeps growing while the (still-down) service prevents initialization.

        // The hanging provider must be installed before the environment initializes XSAPI (HCInit).
        ScopedHangingPeoplehubProvider hangingProvider{}; // Every People Hub call hangs.

        {
            SMTestEnvironment env{};
            ScopedHungCallReleaser releaseHungCalls{ hangingProvider }; // Release hung calls before env teardown on every exit path.
            env.RemovePeoplehubMock(); // Unmock People Hub so its calls reach the hanging provider.

            auto xboxLiveContext{ env.CreateMockXboxLiveContext() };

            VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(
                xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));

            // Drive DoWork long enough for the watchdog (2s in unit-test builds) to fire at least once
            // and re-issue the hung request.
            const auto deadline{ std::chrono::steady_clock::now() + std::chrono::seconds(10) };
            bool localUserAdded{ false };

            while (std::chrono::steady_clock::now() < deadline && hangingProvider.HungCallCount() < 2)
            {
                for (auto e : env.DoWork())
                {
                    if (e->eventType == XblSocialManagerEventType::LocalUserAdded)
                    {
                        localUserAdded = true;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            LOGS_DEBUG << "Hung People Hub call count: " << hangingProvider.HungCallCount()
                << ", localUserAdded: " << localUserAdded;

            // The watchdog re-issued the hung request (>= 2 hung calls) instead of stalling on a single
            // never-completing request.
            VERIFY_IS_TRUE(hangingProvider.HungCallCount() >= 2);
            // The service is still down (all calls hang), so the graph has not initialized.
            VERIFY_IS_FALSE(localUserAdded);

            // Complete the outstanding hung call(s) so XblCleanup (env dtor) can drain instead of
            // blocking at teardown.
            hangingProvider.Release();

            const auto drainDeadline{ std::chrono::steady_clock::now() + std::chrono::seconds(10) };
            while (std::chrono::steady_clock::now() < drainDeadline && !localUserAdded)
            {
                for (auto e : env.DoWork())
                {
                    if (e->eventType == XblSocialManagerEventType::LocalUserAdded)
                    {
                        localUserAdded = true;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        } // env dtor -> RemoveLocalUser + XblCleanup (hung calls already released)
    }

    DEFINE_TEST_CASE(TestInitialFollowedUsersHangRecoversViaWatchdog)
    {
        TEST_LOG(L"Test starting: TestInitialFollowedUsersHangRecoversViaWatchdog");

        // Empty-friends bug (AB#63099583): the initial People Hub GetFollowedUsers request hangs
        // (never completes); the watchdog re-issues it and the retry succeeds. Confirms recovery of a
        // hung initial fetch WITHOUT any external intervention (no reboot, no 20-min refresh) - the
        // graph initializes and LocalUserAdded is raised.

        // The hanging provider must be installed before the environment initializes XSAPI (HCInit).
        ScopedHangingPeoplehubProvider hangingProvider{ /* hangOnlyFirstCall */ true };

        {
            SMTestEnvironment env{};
            ScopedHungCallReleaser releaseHungCalls{ hangingProvider }; // Release hung calls before env teardown on every exit path.
            env.RemovePeoplehubMock(); // Unmock People Hub so its calls reach the hanging provider.

            auto xboxLiveContext{ env.CreateMockXboxLiveContext() };

            VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(
                xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));

            // The first fetch hangs; the watchdog (2s in unit-test builds) re-issues it and the retry
            // succeeds, so the graph initializes shortly after the watchdog interval.
            const auto deadline{ std::chrono::steady_clock::now() + std::chrono::seconds(15) };
            bool localUserAdded{ false };

            while (std::chrono::steady_clock::now() < deadline && !localUserAdded)
            {
                for (auto e : env.DoWork())
                {
                    if (e->eventType == XblSocialManagerEventType::LocalUserAdded)
                    {
                        localUserAdded = true;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            LOGS_DEBUG << "localUserAdded: " << localUserAdded
                << ", hung: " << hangingProvider.HungCallCount()
                << ", total People Hub calls: " << hangingProvider.PeoplehubCallCount();

            // Recovery happened automatically via the watchdog-driven retry.
            VERIFY_IS_TRUE(localUserAdded);
            // Exactly the first request hung...
            VERIFY_ARE_EQUAL(size_t{ 1 }, hangingProvider.HungCallCount());
            // ...and recovery required a retry (the watchdog re-issued the fetch).
            VERIFY_IS_TRUE(hangingProvider.PeoplehubCallCount() >= 2);

            // Complete the abandoned first (hung) request so XblCleanup (env dtor) can drain cleanly.
            hangingProvider.Release();
        } // env dtor -> RemoveLocalUser + XblCleanup (hung call already released)
    }

    DEFINE_TEST_CASE(TestNormalFollowedUsersFetchDoesNotReissue)
    {
        TEST_LOG(L"Test starting: TestNormalFollowedUsersFetchDoesNotReissue");

        // Watchdog "no side effects" guard (AB#63099583). On the happy path a normal, fast, successful
        // initial People Hub GetFollowedUsers completes via its own completion callback and the watchdog
        // must NOT fire, so the fetch is issued exactly once (no duplicate service call) and only one
        // LocalUserAdded event is raised. The shared `settled` atomic guarantees the completion "wins"
        // and the later watchdog tick no-ops.

        SMTestEnvironment env{}; // Default mock: fast, succeeding People Hub fetch.
        auto xboxLiveContext{ env.CreateMockXboxLiveContext() };

        VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(
            xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));

        // Wait for initialization (the fast fetch completes in well under the 2s unit-test watchdog).
        const auto deadline{ std::chrono::steady_clock::now() + std::chrono::seconds(10) };
        int localUserAddedCount{ 0 };
        while (std::chrono::steady_clock::now() < deadline && localUserAddedCount == 0)
        {
            for (auto e : env.DoWork())
            {
                if (e->eventType == XblSocialManagerEventType::LocalUserAdded)
                {
                    ++localUserAddedCount;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        VERIFY_ARE_EQUAL_INT(1, localUserAddedCount);

        // Keep pumping DoWork past a full watchdog interval (2s in unit-test builds) to prove the
        // watchdog does not re-issue the already-completed fetch and does not raise a second event.
        const auto settleDeadline{ std::chrono::steady_clock::now() + std::chrono::seconds(4) };
        while (std::chrono::steady_clock::now() < settleDeadline)
        {
            for (auto e : env.DoWork())
            {
                if (e->eventType == XblSocialManagerEventType::LocalUserAdded)
                {
                    ++localUserAddedCount;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        LOGS_DEBUG << "followed-users calls: " << env.FollowedUsersCallCount()
            << ", LocalUserAdded events: " << localUserAddedCount;

        // The initial fetch completed on its own; the watchdog did not spuriously re-issue it.
        VERIFY_ARE_EQUAL_INT(1, env.FollowedUsersCallCount());
        // And exactly one LocalUserAdded event was raised (no duplicate handler invocation).
        VERIFY_ARE_EQUAL_INT(1, localUserAddedCount);
    } // env dtor -> RemoveLocalUser + XblCleanup

    // One profile that does not fit XSAPI's expectations (a null bool, an over-long name or URL, a
    // wrong-typed field) used to fail deserialization of the ENTIRE PeopleHub response. Because the
    // initial followed-users fetch is retried on failure and the graph only initializes on success,
    // a single such friend left the graph uninitialized forever: no LocalUserAdded and an empty
    // friends list with no error (AB#64511724). Each field must now be tolerated on its own and the
    // graph must initialize from the FIRST fetch.
    DEFINE_TEST_CASE(TestFollowedUsersToleratesInvalidProfileFields)
    {
        TEST_LOG(L"Test starting: TestFollowedUsersToleratesInvalidProfileFields");

        const std::string longGamertag(60, 'g');
        // 88 ASCII bytes + a 2-byte UTF-8 character = 90 bytes, one past the 89 usable bytes of
        // displayName, so a naive cut would split the character.
        const std::string longDisplayName{ std::string(88, 'd') + "\xC3\xA9" };
        const std::string longPicUrl{ "http://images-eds.xboxlive.com/image?url=" + std::string(1000, 'p') };

        SMTestEnvironment env{};
        env.SetPeoplehubMockWithProfileMutator([&](uint64_t xuid, JsonDocument& profile)
        {
            auto& a{ profile.GetAllocator() };
            switch (xuid)
            {
            case 1:
                JsonUtils::SetMember(profile, "isFriend", JsonValue(rapidjson::kNullType));
                JsonUtils::SetMember(profile, "isFavorite", JsonValue(rapidjson::kNullType));
                JsonUtils::SetMember(profile, "useAvatar", JsonValue(rapidjson::kNullType));
                break;
            case 2:
                JsonUtils::SetMember(profile, "gamertag", JsonValue(longGamertag.c_str(), a));
                JsonUtils::SetMember(profile, "displayName", JsonValue(longDisplayName.c_str(), a));
                break;
            case 3:
                JsonUtils::SetMember(profile, "displayPicRaw", JsonValue(longPicUrl.c_str(), a));
                JsonUtils::SetMember(profile, "gamerScore", JsonValue(9001));
                break;
            case 4:
                JsonUtils::SetMember(profile, "presenceState", JsonValue(42));
                break;
            default:
                break;
            }
        });

        auto xboxLiveContext{ env.CreateMockXboxLiveContext() };
        VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(
            xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));

        const auto deadline{ std::chrono::steady_clock::now() + std::chrono::seconds(30) };
        bool localUserAdded{ false };
        while (std::chrono::steady_clock::now() < deadline && !localUserAdded)
        {
            for (auto e : env.DoWork())
            {
                if (e->eventType == XblSocialManagerEventType::LocalUserAdded)
                {
                    VERIFY_SUCCEEDED(e->hr);
                    localUserAdded = true;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        VERIFY_IS_TRUE(localUserAdded);
        // Initialized from the first fetch - not by a retry of a response that can never succeed.
        VERIFY_ARE_EQUAL_INT(1, env.FollowedUsersCallCount());

        std::vector<uint64_t> xuids{ 1, 2, 3, 4, 5 };
        XblSocialManagerUserGroupHandle listGroup{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromList(
            xboxLiveContext->User().Handle(), xuids.data(), xuids.size(), &listGroup));

        bool groupLoaded{ false };
        while (std::chrono::steady_clock::now() < deadline && !groupLoaded)
        {
            for (auto e : env.DoWork())
            {
                groupLoaded |= e->eventType == XblSocialManagerEventType::SocialUserGroupLoaded && e->groupAffected == listGroup;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        VERIFY_IS_TRUE(groupLoaded);

        std::map<uint64_t, const XblSocialManagerUser*> users;
        for (auto user : env.GetUsers(listGroup))
        {
            users[user->xboxUserId] = user;
        }
        VERIFY_ARE_EQUAL_UINT(xuids.size(), users.size());

        // A null isFriend on the friends endpoint reads as true (being in that response is the
        // relationship); other null bools read as false. The rest of the profile is intact.
        VERIFY_IS_TRUE(users[1]->isFriend);
        VERIFY_IS_TRUE(users[1]->isFollowedByCaller);
        VERIFY_IS_FALSE(users[1]->isFavorite);
        VERIFY_IS_FALSE(users[1]->useAvatar);
        VERIFY_ARE_EQUAL_STR("TestGamerTag", users[1]->gamertag);

        // Over-long display text is truncated to fit, never splitting a UTF-8 character. An over-long
        // identifier is left empty rather than truncated into a different, wrong value.
        VERIFY_ARE_EQUAL_STR("", users[2]->gamertag);
        VERIFY_ARE_EQUAL_STR(std::string(88, 'd').c_str(), users[2]->displayName);
        VERIFY_IS_TRUE(users[2]->isFriend);

        VERIFY_ARE_EQUAL_STR("", users[3]->displayPicUrlRaw);
        // A wrong-typed field is left empty rather than discarding the profile.
        VERIFY_ARE_EQUAL_STR("", users[3]->gamerscore);
        VERIFY_ARE_EQUAL_STR("TestGamerTag", users[3]->gamertag);

        VERIFY_ARE_EQUAL_STR("TestGamerTag", users[4]->gamertag);
        VERIFY_IS_TRUE(users[4]->isFriend);

        VERIFY_ARE_EQUAL_STR("TestGamerTag", users[5]->gamertag);

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(listGroup));

        // Every friend, including the one whose isFriend was null, is in the Friends filter group.
        XblSocialManagerUserGroupHandle friendsGroup{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(), XblPresenceFilter::All, XblRelationshipFilter::Friends, &friendsGroup));

        groupLoaded = false;
        while (std::chrono::steady_clock::now() < deadline && !groupLoaded)
        {
            for (auto e : env.DoWork())
            {
                groupLoaded |= e->eventType == XblSocialManagerEventType::SocialUserGroupLoaded && e->groupAffected == friendsGroup;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        VERIFY_IS_TRUE(groupLoaded);

        auto friends{ env.GetUsers(friendsGroup) };
        VERIFY_ARE_EQUAL_UINT(env.FollowedXuids.size(), friends.size());
        VERIFY_IS_TRUE(std::any_of(friends.begin(), friends.end(), [](auto u) { return u->xboxUserId == 1; }));

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(friendsGroup));
    }

    // A batch re-poll (e.g. after a Changed relationship notification) whose isFriend is null must not
    // overwrite a relationship the graph already knows; a genuine false still applies (AB#64511724).
    DEFINE_TEST_CASE(TestBatchNullIsFriendKeepsKnownRelationship)
    {
        TEST_LOG(L"Test starting: TestBatchNullIsFriendKeepsKnownRelationship");

        SMTestEnvironment env{};
        auto xboxLiveContext{ env.CreateMockXboxLiveContext() };
        env.AddLocalUser(xboxLiveContext->User());

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(), XblPresenceFilter::All, XblRelationshipFilter::Friends, &group));
        env.AwaitEvents({ { XblSocialManagerEventType::SocialUserGroupLoaded, 1 } });
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(group));

        const uint64_t nullFriendXuid{ 1 };
        const uint64_t unfriendedXuid{ 2 };
        env.SetPeoplehubMockWithProfileMutator([&](uint64_t xuid, JsonDocument& profile)
        {
            if (xuid == nullFriendXuid)
            {
                JsonUtils::SetMember(profile, "isFriend", JsonValue(rapidjson::kNullType));
                // A real profile change, so the re-poll's result is observable as a ProfilesChanged event.
                JsonUtils::SetMember(profile, "displayName", JsonValue("RenamedFriend", profile.GetAllocator()));
            }
            else if (xuid == unfriendedXuid)
            {
                JsonUtils::SetMember(profile, "isFriend", JsonValue(false));
            }
        });
        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Changed, nullFriendXuid);
        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Changed, unfriendedXuid);

        bool nullFriendProfileChanged{ false };
        bool unfriendedRelationshipChanged{ false };
        const auto deadline{ std::chrono::steady_clock::now() + std::chrono::seconds(30) };
        while (std::chrono::steady_clock::now() < deadline && !(nullFriendProfileChanged && unfriendedRelationshipChanged))
        {
            for (auto e : env.DoWork())
            {
                for (auto user : e->usersAffected)
                {
                    if (user == nullptr)
                    {
                        break;
                    }
                    if (e->eventType == XblSocialManagerEventType::SocialRelationshipsChanged)
                    {
                        // The null isFriend must not read as an unfriend.
                        VERIFY_ARE_NOT_EQUAL(nullFriendXuid, user->xboxUserId);
                        unfriendedRelationshipChanged |= user->xboxUserId == unfriendedXuid;
                    }
                    else if (e->eventType == XblSocialManagerEventType::ProfilesChanged)
                    {
                        nullFriendProfileChanged |= user->xboxUserId == nullFriendXuid;
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        VERIFY_IS_TRUE(nullFriendProfileChanged);
        VERIFY_IS_TRUE(unfriendedRelationshipChanged);

        auto friends{ env.GetUsers(group) };
        VERIFY_ARE_EQUAL_UINT(env.FollowedXuids.size() - 1, friends.size());
        auto nullFriend{ std::find_if(friends.begin(), friends.end(), [&](auto u) { return u->xboxUserId == nullFriendXuid; }) };
        VERIFY_IS_TRUE(nullFriend != friends.end());
        VERIFY_IS_TRUE((*nullFriend)->isFriend);
        VERIFY_IS_TRUE((*nullFriend)->isFollowedByCaller);
        VERIFY_ARE_EQUAL_STR("RenamedFriend", (*nullFriend)->displayName);
        VERIFY_IS_FALSE(std::any_of(friends.begin(), friends.end(), [&](auto u) { return u->xboxUserId == unfriendedXuid; }));

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group));
    }

    // A profile that cannot be identified (missing, null, or non-object xuid) is skipped on its own;
    // every other friend still loads and the graph initializes from the first fetch (AB#64511724).
    DEFINE_TEST_CASE(TestFollowedUsersSkipsProfileWithoutXuid)
    {
        TEST_LOG(L"Test starting: TestFollowedUsersSkipsProfileWithoutXuid");

        const std::set<uint64_t> badXuids{ 2, 3, 4 };

        SMTestEnvironment env{};
        env.SetPeoplehubMockWithProfileMutator([&](uint64_t xuid, JsonDocument& profile)
        {
            switch (xuid)
            {
            case 2:
                profile.RemoveMember("xuid");
                break;
            case 3:
                JsonUtils::SetMember(profile, "xuid", JsonValue(rapidjson::kNullType));
                break;
            case 4:
                profile.SetNull();
                break;
            default:
                break;
            }
        });

        auto xboxLiveContext{ env.CreateMockXboxLiveContext() };
        VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(
            xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));

        XblSocialManagerUserGroupHandle filterGroup{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(), XblPresenceFilter::All, XblRelationshipFilter::Friends, &filterGroup));

        const auto deadline{ std::chrono::steady_clock::now() + std::chrono::seconds(30) };
        bool localUserAdded{ false };
        bool groupLoaded{ false };
        while (std::chrono::steady_clock::now() < deadline && !(localUserAdded && groupLoaded))
        {
            for (auto e : env.DoWork())
            {
                localUserAdded |= e->eventType == XblSocialManagerEventType::LocalUserAdded;
                groupLoaded |= e->eventType == XblSocialManagerEventType::SocialUserGroupLoaded && e->groupAffected == filterGroup;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        VERIFY_IS_TRUE(localUserAdded);
        VERIFY_IS_TRUE(groupLoaded);
        VERIFY_ARE_EQUAL_INT(1, env.FollowedUsersCallCount());

        auto users{ env.GetUsers(filterGroup) };
        VERIFY_ARE_EQUAL_UINT(env.FollowedXuids.size() - badXuids.size(), users.size());
        for (auto user : users)
        {
            VERIFY_IS_TRUE(badXuids.find(user->xboxUserId) == badXuids.end());
            VERIFY_IS_TRUE(user->xboxUserId != 0);
        }

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(filterGroup));
    }

    DEFINE_TEST_CASE(TestBasicCreateFilterGroup)
    {
        TEST_LOG(L"Test starting: TestBasicCreateFilterGroup");

        SMTestEnvironment env{};
        auto xboxLiveContext{ env.CreateMockXboxLiveContext() };
        env.AddLocalUser(xboxLiveContext->User());

        XblSocialManagerUserGroupHandle filterGroup{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::All,
            XblRelationshipFilter::Friends,
            &filterGroup
        ));

        bool groupLoaded{ false };
        while (!groupLoaded)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    VERIFY_IS_TRUE(event->groupAffected == filterGroup);
                    groupLoaded = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        auto users{ env.GetUsers(filterGroup) };
        VERIFY_ARE_EQUAL(env.GetTrackedUsersCount(filterGroup), users.size());

        for (auto user : users)
        {
            // profile tests
            LOGS_DEBUG << "Validating user " << user->xboxUserId;
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->displayName);
            VERIFY_ARE_EQUAL_STR("http://images-eds.xboxlive.com/image?url=mHGRD8KXEf2sp2LC58XhBQKNl2IWRp.J.q8mSURKUUeiPPf0Y7Kl7zLN7rafayiPptVaX_XIUmNOPotNmNubbx4bHmf6It7Oj1ChU5UAo9k-&background=0xababab&mode=Padding&format=png", user->displayPicUrlRaw);
            VERIFY_IS_TRUE(user->isFollowedByCaller);
            VERIFY_IS_TRUE(user->isFollowingUser);
            VERIFY_ARE_EQUAL_STR("9001", user->gamerscore);
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->gamertag);
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->modernGamertag);
            VERIFY_ARE_EQUAL_STR("", user->modernGamertagSuffix);
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->uniqueModernGamertag);
            VERIFY_IS_FALSE(user->isFavorite);
            VERIFY_IS_FALSE(user->useAvatar);

            // preferred color tests
            VERIFY_ARE_EQUAL_STR("193e91", user->preferredColor.primaryColor);
            VERIFY_ARE_EQUAL_STR("2458cf", user->preferredColor.secondaryColor);
            VERIFY_ARE_EQUAL_STR("122e6b", user->preferredColor.tertiaryColor);

            // presence record tests
            VERIFY_IS_TRUE(user->presenceRecord.presenceTitleRecordCount == 1);
            VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Online);
            VERIFY_IS_TRUE(XblSocialManagerPresenceRecordIsUserPlayingTitle(&user->presenceRecord, 1234));
            VERIFY_IS_TRUE(user->presenceRecord.presenceTitleRecords[0].isTitleActive);
            VERIFY_IS_TRUE(!user->presenceRecord.presenceTitleRecords[0].isBroadcasting);
            VERIFY_IS_TRUE(user->presenceRecord.presenceTitleRecords[0].deviceType == XblPresenceDeviceType::PC);
            VERIFY_ARE_EQUAL_STR("Home", user->presenceRecord.presenceTitleRecords[0].presenceText);

            // title history tests
            VERIFY_IS_TRUE(user->titleHistory.hasUserPlayed);
            VERIFY_IS_TRUE(VerifyTime(user->titleHistory.lastTimeUserPlayed, "2015-01-26T22:54:54.6630Z"));
            VERIFY_ARE_EQUAL_STR("8 months ago", user->titleHistory.lastTimeUserPlayedText);
        }

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(filterGroup));
    }

    DEFINE_TEST_CASE(TestPresenceRtaUpdates)
    {
        TEST_LOG(L"Test starting: TestPresenceRtaUpdates");

        SMTestEnvironment env{};
        auto xboxLiveContext{ env.CreateMockXboxLiveContext() };
        env.AddLocalUser(xboxLiveContext->User());

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::All,
            XblRelationshipFilter::Friends,
            &group
        ));

        env.AwaitEvents({ {XblSocialManagerEventType::SocialUserGroupLoaded, 1} });

        auto users{ env.GetUsers(group) };
        VERIFY_ARE_EQUAL_INT(users.size(), env.FollowedXuids.size());

        // Verify initial presence
        for (auto user : users)
        {
            VERIFY_IS_TRUE(XblSocialManagerPresenceRecordIsUserPlayingTitle(&user->presenceRecord, 1234));
            VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Online);
        }

        // Test title presence changed event
        auto presenceChangeXuids{ env.FollowedXuids };
        env.SetPresenceMock({}, 4321);
        env.FireTitlePresenceChangeRtaEvent(presenceChangeXuids);

        size_t presenceChangedEvents{ 0 };
        while (presenceChangedEvents < presenceChangeXuids.size())
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::PresenceChanged:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_FALSE(XblSocialManagerPresenceRecordIsUserPlayingTitle(&affectedUser->presenceRecord, 1234));
                            presenceChangedEvents++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }
        VERIFY_ARE_EQUAL(presenceChangedEvents, presenceChangeXuids.size());

        // Verify the group view is updated as well
        for (auto user : env.GetUsers(group))
        {
            VERIFY_IS_FALSE(XblSocialManagerPresenceRecordIsUserPlayingTitle(&user->presenceRecord, 1234));
        }

        // Test device presence changed event
        env.SetPresenceMock(presenceChangeXuids);
        env.FireDevicePresenceChangeRtaEvent(presenceChangeXuids);

        presenceChangedEvents = 0;
        while (presenceChangedEvents < presenceChangeXuids.size())
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::PresenceChanged:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_TRUE(affectedUser->presenceRecord.userState == XblPresenceUserState::Offline);
                            presenceChangedEvents++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }
        VERIFY_ARE_EQUAL(presenceChangedEvents, presenceChangeXuids.size());

        // Verify the group view is updated as well
        for (auto user : env.GetUsers(group))
        {
            VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Offline);
        }

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group));
    }

    // Regression test for spurious PresenceChanged events (found while fixing AB#63572198).
    //
    // Every RTA device/title presence event makes the graph re-poll the presence service, and the
    // result is compared with the stored record (which People Hub populated). The two sides build
    // their strings differently: strcpy_s on the presence-service path, memcpy + terminator on the
    // People Hub path. The Debug CRT's strcpy_s fills the unused tail of the buffer with 0xFE, so a
    // byte-wise memcmp of the record reported a change even when every field was identical, and every
    // re-poll raised PresenceChanged for every affected user. Presence is now compared field-wise.
    // The byte-level difference only exists with the Debug CRT, which is what the unit tests use.
    DEFINE_TEST_CASE(TestUnchangedPresenceRepollRaisesNoEvent)
    {
        TEST_LOG(L"Test starting: TestUnchangedPresenceRepollRaisesNoEvent");

        SMTestEnvironment env{};
        auto xboxLiveContext{ env.CreateMockXboxLiveContext() };
        env.AddLocalUser(xboxLiveContext->User());

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::All,
            XblRelationshipFilter::Friends,
            &group
        ));
        env.AwaitEvents({ { XblSocialManagerEventType::SocialUserGroupLoaded, 1 } });

        auto users{ env.GetUsers(group) };
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), users.size());
        for (auto user : users)
        {
            VERIFY_IS_TRUE(XblSocialManagerPresenceRecordIsUserPlayingTitle(&user->presenceRecord, MOCK_TITLEID));
        }

        // The default presence mock returns the same presence People Hub reported, so re-polling
        // must not produce any events.
        const size_t requestsBefore{ env.PresenceRequestCount() };
        env.FireDevicePresenceChangeRtaEvent(env.FollowedXuids, true);

        // Presence polls after the first are batched behind a short delay, so keep pumping for a while
        // after the first re-poll is served to give every batched result time to be applied.
        const auto deadline{ std::chrono::steady_clock::now() + std::chrono::seconds(30) };
        std::chrono::steady_clock::time_point settleUntil{};
        while (std::chrono::steady_clock::now() < deadline)
        {
            if (settleUntil == std::chrono::steady_clock::time_point{} && env.PresenceRequestCount() > requestsBefore)
            {
                settleUntil = std::chrono::steady_clock::now() + std::chrono::seconds(3);
            }
            if (settleUntil != std::chrono::steady_clock::time_point{} && std::chrono::steady_clock::now() >= settleUntil)
            {
                break;
            }

            for (auto event : env.DoWork())
            {
                LOGS_DEBUG << "Unexpected SocialManager Event " << static_cast<uint32_t>(event->eventType);
                VERIFY_FAIL(L"Re-polling unchanged presence raised a SocialManager event");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        VERIFY_IS_TRUE(env.PresenceRequestCount() > requestsBefore);
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group));
    }

    // Regression test for AB#63508132 (stack-frame reduction follow-up).
    //
    // SocialGraph::ConvertPresenceRecord used to return XblSocialManagerPresenceRecord (~3.7KB) by
    // value, which left three live copies in the /O2 frame of PresenceResultHandler - an 11,304-byte
    // frame on the same stack-constrained XTaskQueue completion thread that produced the original
    // crash. It now fills an _Out_ reference instead.
    //
    // Filling caller-owned storage means residual bytes in unused slots could survive and be handed
    // to callers. This test pins the invariant that every slot at or past presenceTitleRecordCount is
    // entirely zero - including after a transition that REDUCES the count from one to zero, which is
    // where uninitialized storage would show up.
    DEFINE_TEST_CASE(TestPresenceRecordUnusedSlotsAreZeroed)
    {
        TEST_LOG(L"Test starting: TestPresenceRecordUnusedSlotsAreZeroed");

        SMTestEnvironment env{};
        auto xboxLiveContext{ env.CreateMockXboxLiveContext() };
        env.AddLocalUser(xboxLiveContext->User());

        constexpr size_t c_trackedUserCount{ 5 };
        VERIFY_IS_TRUE(env.FollowedXuids.size() >= c_trackedUserCount);
        std::vector<uint64_t> trackedXuids{ env.FollowedXuids.begin(), env.FollowedXuids.begin() + c_trackedUserCount };

        XblSocialManagerUserGroupHandle listGroup{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromList(
            xboxLiveContext->User().Handle(),
            trackedXuids.data(),
            trackedXuids.size(),
            &listGroup
        ));

        env.AwaitEvents({ { XblSocialManagerEventType::SocialUserGroupLoaded, 1 } });

        // The default presence mock reports one active title, so exactly one slot is in use and the
        // remaining XBL_NUM_PRESENCE_RECORDS - 1 must be zeroed.
        auto users{ env.GetUsers(listGroup) };
        VERIFY_ARE_EQUAL_UINT(trackedXuids.size(), users.size());

        for (auto user : users)
        {
            VERIFY_ARE_EQUAL_UINT(1u, user->presenceRecord.presenceTitleRecordCount);
            VerifyUnusedPresenceSlotsAreZeroed(user->presenceRecord);
        }

        // Now drive those users offline. The offline presence payload has no devices, so
        // presenceTitleRecordCount drops to zero and EVERY slot must be zeroed - including the one
        // that previously held an active title.
        env.SetPresenceMock(trackedXuids);
        env.FireDevicePresenceChangeRtaEvent(trackedXuids);

        std::set<uint64_t> sawOffline;
        std::set<uint64_t> trackedXuidsSet{ trackedXuids.begin(), trackedXuids.end() };

        // Bounded by wall clock, matching the convention used by the watchdog tests above. The
        // offline transition is driven by an RTA event plus a presence poll, so what is being waited
        // on is elapsed time, not DoWork iterations - a pure iteration cap burns through 100k spins
        // in well under a second and would fail a perfectly healthy run. A genuine stall now fails
        // the test instead of spinning as a hung test host.
        const auto deadline{ std::chrono::steady_clock::now() + std::chrono::seconds(30) };
        while (std::chrono::steady_clock::now() < deadline && sawOffline.size() < trackedXuids.size())
        {
            for (auto event : env.DoWork())
            {
                if (event->eventType != XblSocialManagerEventType::PresenceChanged)
                {
                    continue;
                }

                for (uint32_t i = 0; i < XBL_SOCIAL_MANAGER_MAX_AFFECTED_USERS_PER_EVENT; ++i)
                {
                    auto affectedUser{ event->usersAffected[i] };
                    if (affectedUser == nullptr)
                    {
                        break;
                    }
                    if (trackedXuidsSet.find(affectedUser->xboxUserId) == trackedXuidsSet.end())
                    {
                        continue;
                    }

                    // The invariant under test holds for every presence record, whatever the state.
                    VerifyUnusedPresenceSlotsAreZeroed(affectedUser->presenceRecord);

                    // A presence poll already in flight when the mock was swapped can still report
                    // Online, so only count the offline transition, which is the case that reduces
                    // presenceTitleRecordCount to zero and frees a previously populated slot.
                    if (affectedUser->presenceRecord.userState == XblPresenceUserState::Offline)
                    {
                        VERIFY_ARE_EQUAL_UINT(0u, affectedUser->presenceRecord.presenceTitleRecordCount);
                        sawOffline.insert(affectedUser->xboxUserId);
                    }
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        // A timed-out wait must fail here, with a message that names what was missing, rather than
        // falling through to the group re-read below and failing on a less obvious assertion.
        VERIFY_ARE_EQUAL_UINT(trackedXuids.size(), sawOffline.size());

        // Re-read through the group so the assertion also covers the copy stored in the graph, not
        // just the event payload.
        for (auto user : env.GetUsers(listGroup))
        {
            VERIFY_ARE_EQUAL_UINT(0u, user->presenceRecord.presenceTitleRecordCount);
            VerifyUnusedPresenceSlotsAreZeroed(user->presenceRecord);
        }

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(listGroup));
    }

    DEFINE_TEST_CASE(TestMultipleLocalUsers)
    {
        TEST_LOG(L"Test starting: TestMultipleLocalUsers");

        SMTestEnvironment env{};
        auto xboxLiveContext1 = env.CreateMockXboxLiveContext();
        auto xboxLiveContext2 = env.CreateMockXboxLiveContext(202020202020, "MockLocalUser2");

        env.AddLocalUser(xboxLiveContext1->User());
        VERIFY_IS_TRUE(XblSocialManagerGetLocalUserCount() == 1);

        env.AddLocalUser(xboxLiveContext2->User());
        VERIFY_IS_TRUE(XblSocialManagerGetLocalUserCount() == 2);

        XblSocialManagerUserGroupHandle user1Group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext1->User().Handle(),
            XblPresenceFilter::All,
            XblRelationshipFilter::Friends,
            &user1Group
        ));

        // Create a list group that is a subset of user1Group
        std::vector<uint64_t> group2TrackedUsers{ 1, 2, 3, 4, 5 };

        XblSocialManagerUserGroupHandle user2Group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromList(
            xboxLiveContext2->User().Handle(),
            group2TrackedUsers.data(),
            group2TrackedUsers.size(),
            &user2Group
        ));

        size_t groupsLoaded{ 0 };
        while (groupsLoaded < 2)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    VERIFY_IS_TRUE(event->groupAffected == user1Group || event->groupAffected == user2Group);
                    groupsLoaded++;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_IS_TRUE(groupsLoaded == 2);
        auto group1Users{ env.GetUsers(user1Group) };
        auto group2Users{ env.GetUsers(user2Group) };
        VERIFY_ARE_EQUAL(group1Users.size(), env.FollowedXuids.size());
        VERIFY_ARE_EQUAL(group2Users.size(), group2TrackedUsers.size());

        // Verify initial presence
        for (auto& user : group1Users)
        {
            VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Online);
        }
        for (auto& user : group2Users)
        {
            VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Online);
        }

        // Validate presence change propogates to both user's groups and make sure we get presence
        // changed events for each local user
        env.SetPresenceMock(group2TrackedUsers);
        env.FireDevicePresenceChangeRtaEvent(group2TrackedUsers);

        size_t presenceChangedEvents{ 0 };
        size_t expectedEvents{ group2TrackedUsers.size() * 2 };

        while (presenceChangedEvents < expectedEvents)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::PresenceChanged:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            LOGS_DEBUG << affectedUser->xboxUserId;
                            VERIFY_IS_TRUE(affectedUser->presenceRecord.userState == XblPresenceUserState::Offline);
                            presenceChangedEvents++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }
        VERIFY_ARE_EQUAL(presenceChangedEvents, expectedEvents);

        std::set<uint64_t> updatedXuids{ group2TrackedUsers.begin(), group2TrackedUsers.end() };

        // Verify both group views are updated as well
        for (auto& user : env.GetUsers(user1Group))
        {
            if (updatedXuids.find(user->xboxUserId) == updatedXuids.end())
            {
                VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Online);
            }
            else
            {
                VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Offline);
            }
        }
        for (auto& user : env.GetUsers(user2Group))
        {
            VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Offline);
        }

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(user1Group));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(user2Group));
    }

    DEFINE_TEST_CASE(TestAddRemoveLocalUser)
    {
        TEST_LOG(L"Test starting: TestAddRemoveLocalUser");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        // Tests race condition in adding then removing a local user before adding is complete
        VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));
        VERIFY_SUCCEEDED(XblSocialManagerRemoveLocalUser(xboxLiveContext->User().Handle()));
        VERIFY_ARE_EQUAL(XblSocialManagerGetLocalUserCount(), 0u);
        env.AddLocalUser(xboxLiveContext->User());
        VERIFY_ARE_EQUAL(XblSocialManagerGetLocalUserCount(), 1u);
    }

    DEFINE_TEST_CASE(TestSocialRelationshipChangedRtaUpdate)
    {
        TEST_LOG(L"Test starting: TestSocialRelationshipChangedRtaUpdate");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::All,
            XblRelationshipFilter::Friends,
            &group
        ));

        size_t expectedGroupSize{ env.FollowedXuids.size() };

        env.AwaitEvents({ {XblSocialManagerEventType::SocialUserGroupLoaded, 1} });
        VERIFY_ARE_EQUAL_INT(expectedGroupSize, env.GetUsersCount(group));

        // Fire social relationship added event. Expected group size should grow by 1.
        uint64_t addedXuid{ 101 };
        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Added, addedXuid);
        expectedGroupSize++;

        bool userAddedToGraph{ false };
        while (!userAddedToGraph)
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::UsersAddedToSocialGraph:
                {
                    VERIFY_ARE_EQUAL_INT(event->usersAffected[0]->xboxUserId, addedXuid);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    userAddedToGraph = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(expectedGroupSize, env.GetUsersCount(group));

        uint64_t changedXuid{ 1 };
        env.SetPeoplehubMock(true, true);
        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Changed, changedXuid);

        auto relationshipChangedEventFound{ false };
        while (!relationshipChangedEventFound)
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialRelationshipsChanged:
                {
                    VERIFY_ARE_EQUAL_INT(event->usersAffected[0]->xboxUserId, changedXuid);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    relationshipChangedEventFound = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(expectedGroupSize, env.GetUsersCount(group));

        // Fire social relationship removed event. Expected group size should decrease by 1.
        uint64_t removedXuid{ 2 };
        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Removed, removedXuid);
        expectedGroupSize--;

        bool userRemovedFromGraph{ false };

        // Since the user is being removed from the graph, there will never be social relationship changed event in this case.
        // If the user was removed as a friend but was remaining in the graph (i.e. they are tracked by a list group), we would
        // receive the social relationship changed event instead.
        while (!userRemovedFromGraph)
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::UsersRemovedFromSocialGraph:
                {
                    // User should be removed from graph if they are removed as a friend
                    VERIFY_ARE_EQUAL_INT(event->usersAffected[0]->xboxUserId, removedXuid);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    userRemovedFromGraph = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(expectedGroupSize, env.GetUsersCount(group));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group));
    }

    DEFINE_TEST_CASE(TestListGroupWithSocialRelationshipChanged)
    {
        TEST_LOG(L"Test starting: TestListGroupWithSocialRelationshipChanged");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());

        XblSocialManagerUserGroupHandle friendsGroup{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::All,
            XblRelationshipFilter::Friends,
            &friendsGroup
        ));

        // Create a list group with a user who we aren't friends with
        uint64_t listXuid{ 101 };
        env.SetPeoplehubMock(true, false, false);

        XblSocialManagerUserGroupHandle listGroup{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromList(
            xboxLiveContext->User().Handle(),
            &listXuid,
            1,
            &listGroup
        ));

        size_t groupsLoaded{ 0 };
        while (groupsLoaded < 2)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    VERIFY_IS_TRUE(event->groupAffected == friendsGroup || event->groupAffected == listGroup);
                    groupsLoaded++;
                    break;
                }
                case XblSocialManagerEventType::UsersAddedToSocialGraph:
                {
                    VERIFY_ARE_EQUAL_INT(event->usersAffected[0]->xboxUserId, listXuid);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        auto expectedFriendsSize{ env.FollowedXuids.size() };

        VERIFY_ARE_EQUAL(2u, groupsLoaded);
        VERIFY_ARE_EQUAL(1u, env.GetUsersCount(listGroup));
        VERIFY_ARE_EQUAL(expectedFriendsSize, env.GetUsersCount(friendsGroup));

        // Fire social relationship added event. Friends group size should grow by 1.
        env.SetPeoplehubMock(true, false, true);
        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Added, listXuid);
        expectedFriendsSize++;

        bool relationshipChanged{ false };
        while (!relationshipChanged)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialRelationshipsChanged:
                {
                    VERIFY_ARE_EQUAL_INT(event->usersAffected[0]->xboxUserId, listXuid);
                    VERIFY_IS_TRUE(event->usersAffected[0]->isFollowedByCaller == true);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    relationshipChanged = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(expectedFriendsSize, env.GetUsersCount(friendsGroup));

        // Fire social relationship removed event. Expected group size should decrease by 1.
        env.SetPeoplehubMock(true, false, false);
        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Removed, listXuid);
        expectedFriendsSize--;

        relationshipChanged = false;
        while (!relationshipChanged)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialRelationshipsChanged:
                {
                    VERIFY_ARE_EQUAL_INT(event->usersAffected[0]->xboxUserId, listXuid);
                    VERIFY_IS_TRUE(event->usersAffected[0]->isFollowedByCaller == false);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    relationshipChanged = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(expectedFriendsSize, env.GetUsersCount(friendsGroup));

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(friendsGroup));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(listGroup));
    }

    DEFINE_TEST_CASE(TestFilterGroupChanges)
    {
        TEST_LOG(L"Test starting: TestFilterGroupChanges");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::TitleOnline,
            XblRelationshipFilter::Friends,
            &group
        ));

        env.AwaitEvents({ {XblSocialManagerEventType::SocialUserGroupLoaded, 1} });

        auto expectedGroupSize{ env.FollowedXuids.size() };
        VERIFY_ARE_EQUAL_INT(expectedGroupSize, env.GetUsersCount(group));

        // Make half the users go offline
        std::vector<uint64_t> usersToGoOffline{ env.FollowedXuids.begin(), env.FollowedXuids.begin() + env.FollowedXuids.size() / 2 };
        env.SetPresenceMock(usersToGoOffline);

        env.FireDevicePresenceChangeRtaEvent(usersToGoOffline);
        expectedGroupSize -= usersToGoOffline.size();

        size_t presenceChangedEvents{ 0 };
        size_t expectedEvents{ usersToGoOffline.size() };

        while (presenceChangedEvents < expectedEvents)
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::PresenceChanged:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_TRUE(affectedUser->presenceRecord.userState == XblPresenceUserState::Offline);
                            presenceChangedEvents++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(expectedEvents, presenceChangedEvents);
        VERIFY_ARE_EQUAL(expectedGroupSize, env.GetUsersCount(group));
        VERIFY_ARE_EQUAL(expectedGroupSize, env.GetTrackedUsersCount(group));
    }

    DEFINE_TEST_CASE(TestMultipleListGroups)
    {
        TEST_LOG(L"Test starting: TestMultipleListGroups");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());

        // Set mock for non-friends
        env.SetPeoplehubMock(true, false, false);

        // Create a list group that doesn't overlap with our friends at all. Five new users should be added to the graph
        std::set<uint64_t> trackedXuids1{ 101, 102, 103, 104, 105 };

        XblSocialManagerUserGroupHandle group1{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromList(
            xboxLiveContext->User().Handle(),
            std::vector<uint64_t>{ trackedXuids1.begin(), trackedXuids1.end() }.data(),
            trackedXuids1.size(),
            &group1
        ));

        // Tracked users are only reported once the group has loaded (see XblSocialManagerUserGroupGetUsersTrackedByGroup),
        // so don't query them until then. Querying a list group before it loads also permanently empties its tracked-users
        // view (AB#64300922), which would break the later assertions.

        auto groupLoaded{ false };
        size_t usersAddedToGraph{ 0 };
        while (!groupLoaded || usersAddedToGraph < trackedXuids1.size())
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    VERIFY_IS_TRUE(event->groupAffected == group1);
                    groupLoaded = true;
                    break;
                }
                case XblSocialManagerEventType::UsersAddedToSocialGraph:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_TRUE(trackedXuids1.find(affectedUser->xboxUserId) != trackedXuids1.end());
                            VERIFY_IS_TRUE(affectedUser->isFollowedByCaller == false);
                            usersAddedToGraph++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(trackedXuids1.size(), env.GetUsersCount(group1));
        VERIFY_ARE_EQUAL(trackedXuids1.size(), env.GetTrackedUsersCount(group1));

        // Create a second group with partial graph overlap. No new users should be added to graph.
        std::vector<uint64_t> trackedXuids2{ 1, 2, 3, 4, 5, 101, 102, 103, 104, 105 };

        XblSocialManagerUserGroupHandle group2{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromList(
            xboxLiveContext->User().Handle(),
            trackedXuids2.data(),
            trackedXuids2.size(),
            &group2
        ));

        groupLoaded = false;
        while (!groupLoaded)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    VERIFY_IS_TRUE(event->groupAffected == group2);
                    groupLoaded = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(trackedXuids2.size(), env.GetUsersCount(group2));

        // Validate that users remain in graph after their social status changes, until the list group is also
        // destroyed (they are also part of list group1).

        // Fire social relationship removed event.
        uint64_t removedXuid{ 1 };
        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Removed, removedXuid);

        bool socialRelationshipChanged{ false };
        while (!socialRelationshipChanged)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialRelationshipsChanged:
                {
                    VERIFY_IS_TRUE(event->usersAffected[0]->xboxUserId == removedXuid);
                    VERIFY_IS_TRUE(event->usersAffected[0]->isFollowedByCaller == false);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    socialRelationshipChanged = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        // Destroying group2 should cause 1 user to new be removed from the graph (the remainder are either
        // friends or part of group1 as well)
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group2));

        size_t usersRemoved{ 0 };
        while (usersRemoved < 1)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::UsersRemovedFromSocialGraph:
                {
                    // User should be removed from graph if they are removed as a friend
                    VERIFY_ARE_EQUAL_INT(event->usersAffected[0]->xboxUserId, removedXuid);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    usersRemoved++;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        // Test updating group1. This should cause the previous tracked users to be removed from the graph and the new ones to be added.
        std::set<uint64_t> newTrackedXuids1{ 106, 107 };
        VERIFY_SUCCEEDED(XblSocialManagerUpdateSocialUserGroup(
            group1,
            std::vector<uint64_t>{ newTrackedXuids1.begin(), newTrackedXuids1.end() }.data(),
            newTrackedXuids1.size()
        ));

        auto groupUpdated{ false };
        size_t usersRemovedFromGraph{ 0 };
        usersAddedToGraph = 0;

        while (!groupUpdated || usersAddedToGraph < newTrackedXuids1.size() || usersRemovedFromGraph < trackedXuids1.size())
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupUpdated:
                {
                    VERIFY_IS_TRUE(event->groupAffected == group1);
                    groupUpdated = true;
                    break;
                }
                case XblSocialManagerEventType::UsersAddedToSocialGraph:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_TRUE(newTrackedXuids1.find(affectedUser->xboxUserId) != newTrackedXuids1.end());
                            usersAddedToGraph++;
                        }
                    }
                    break;
                }
                case XblSocialManagerEventType::UsersRemovedFromSocialGraph:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_TRUE(trackedXuids1.find(affectedUser->xboxUserId) != trackedXuids1.end());
                            usersRemovedFromGraph++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(newTrackedXuids1.size(), env.GetUsersCount(group1));
        VERIFY_ARE_EQUAL(newTrackedXuids1.size(), env.GetTrackedUsersCount(group1));

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group1));
    }

    DEFINE_TEST_CASE(TestSocialUserGroupFromListLarge)
    {
        TEST_LOG(L"Test starting: TestSocialUserGroupFromListLarge");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());

        std::vector<uint64_t> trackedXuids;
        for (uint64_t xuid = 0; xuid < XBL_SOCIAL_MANAGER_MAX_USERS_FROM_LIST; ++xuid)
        {
            trackedXuids.push_back(xuid + 101); // Offset to make unique from our friends
        }

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromList(
            xboxLiveContext->User().Handle(),
            trackedXuids.data(),
            trackedXuids.size(),
            &group
        ));

        // Tracked users are only reported once the group has loaded; see TestMultipleListGroups.

        std::set<uint64_t> trackedXuidsSet{ trackedXuids.begin(), trackedXuids.end() };

        auto groupLoaded{ false };
        size_t usersAddedToGraph{ 0 };
        while (!groupLoaded || usersAddedToGraph < trackedXuids.size())
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    VERIFY_IS_TRUE(event->groupAffected == group);
                    groupLoaded = true;
                    break;
                }
                case XblSocialManagerEventType::UsersAddedToSocialGraph:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_TRUE(trackedXuidsSet.find(affectedUser->xboxUserId) != trackedXuidsSet.end());
                            usersAddedToGraph++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(trackedXuids.size(), env.GetUsersCount(group));
        VERIFY_ARE_EQUAL(trackedXuids.size(), env.GetTrackedUsersCount(group));

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group));
    }

    // End-to-end peak-allocation guard for AB#63480128.
    //
    // Playground reported ~8MB attributed to Social Manager for an automation account with ~500
    // friends. XblSocialManagerUser is ~5.7KB, so the graph itself is only ~2.7MB at that size; the
    // rest was redundant copies of the Peoplehub response made as it travelled through Result,
    // AsyncContext and Function. This measures peak live bytes across a full graph load and pins it
    // to a small multiple of the graph payload, so reintroducing a copy of the whole response shows
    // up as a failure rather than as partner-reported memory pressure.
    DEFINE_TEST_CASE(TestLargeGraphPeakMemory)
    {
        TEST_LOG(L"Test starting: TestLargeGraphPeakMemory");

        constexpr size_t c_trackedUsers{ XBL_SOCIAL_MANAGER_MAX_USERS_FROM_LIST };
        constexpr size_t c_graphPayloadBytes{ c_trackedUsers * sizeof(XblSocialManagerUser) };

        size_t peakBytes{ 0 };
        {
            PeakMemory::ScopedTracker tracker{};

            SMTestEnvironment env{};
            auto xboxLiveContext{ env.CreateMockXboxLiveContext() };
            env.AddLocalUser(xboxLiveContext->User());

            std::vector<uint64_t> trackedXuids;
            for (uint64_t xuid = 0; xuid < c_trackedUsers; ++xuid)
            {
                trackedXuids.push_back(xuid + 101); // Offset to make unique from our friends
            }

            XblSocialManagerUserGroupHandle group{ nullptr };
            VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromList(
                xboxLiveContext->User().Handle(),
                trackedXuids.data(),
                trackedXuids.size(),
                &group
            ));

            // Pump until the group is loaded and every tracked user has landed in the graph. Peak is
            // measured across the whole load, which is where the redundant copies used to stack up.
            bool groupLoaded{ false };
            size_t usersAddedToGraph{ 0 };
            size_t iterations{ 0 };
            while (!groupLoaded || usersAddedToGraph < trackedXuids.size())
            {
                VERIFY_IS_TRUE(++iterations < 100000);

                for (auto event : env.DoWork())
                {
                    if (event->eventType == XblSocialManagerEventType::SocialUserGroupLoaded)
                    {
                        groupLoaded = true;
                    }
                    else if (event->eventType == XblSocialManagerEventType::UsersAddedToSocialGraph)
                    {
                        for (auto affectedUser : event->usersAffected)
                        {
                            if (affectedUser)
                            {
                                ++usersAddedToGraph;
                            }
                        }
                    }
                }
            }

            VERIFY_ARE_EQUAL(c_trackedUsers, env.GetUsersCount(group));

            peakBytes = PeakMemory::PeakBytes();

            VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group));
        }

        char summary[256]{};
        sprintf_s(summary, "Graph payload %zu bytes, peak allocation %zu bytes (%.2fx payload)",
            c_graphPayloadBytes,
            peakBytes,
            static_cast<double>(peakBytes) / static_cast<double>(c_graphPayloadBytes));
        TEST_LOG(Utils::StringTFromUtf8(summary).data());

        // Measured on this fixture: 9.38x payload before the copies were removed, 5.38x after -
        // exactly four full copies of the response eliminated. 7x sits between the two with
        // headroom for allocator noise, so this fails if any one of those copies comes back.
        VERIFY_IS_TRUE(peakBytes < c_graphPayloadBytes * 7);
    }

    // Tests refresh for RTA resync
    DEFINE_TEST_CASE(TestRtaResync)
    {
        TEST_LOG(L"Test starting: TestRtaResync");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());
        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::All,
            XblRelationshipFilter::Friends,
            &group
        ));

        env.AwaitEvents({ {XblSocialManagerEventType::SocialUserGroupLoaded, 1} });
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(group));

        // Make a slight change so we know that the users were refreshed
        env.SetPeoplehubMock(true, true);
        MockRealTimeActivityService::Instance().RaiseResync();
        size_t profileChangesEvents{ 0 };
        while (profileChangesEvents < env.FollowedXuids.size())
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialRelationshipsChanged:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_ARE_EQUAL(affectedUser->isFavorite, true);
                            profileChangesEvents++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group));
    }

    DEFINE_TEST_CASE(TestSMInvalidArgs)
    {
        TEST_LOG(L"Test starting: TestSMInvalidArgs");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());

        XblUserHandle dummyHandle{ (XblUserHandle)0xFFFFFFFFFFFFFFFF };
        XblSocialManagerUserGroupHandle groupHandle{ nullptr };

#pragma warning( push )
#pragma warning( disable : 6387 )
        VERIFY_FAILED(XblSocialManagerAddLocalUser(nullptr, XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));
        VERIFY_FAILED(XblSocialManagerRemoveLocalUser(nullptr));
        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromFilters(nullptr, XblPresenceFilter::All, XblRelationshipFilter::Friends, &groupHandle));
        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromFilters(dummyHandle, XblPresenceFilter::All, XblRelationshipFilter::Friends, nullptr));
        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromList(nullptr, std::vector<uint64_t>{ 0, 1, 2 }.data(), 3, & groupHandle));
        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromList(dummyHandle, nullptr, 0, &groupHandle));
        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromList(dummyHandle, std::vector<uint64_t>{ 0, 1, 2 }.data(), 3, nullptr));
        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromList(dummyHandle, std::vector<uint64_t>(XBL_SOCIAL_MANAGER_MAX_USERS_FROM_LIST + 1, 0).data(), XBL_SOCIAL_MANAGER_MAX_USERS_FROM_LIST + 1, &groupHandle));
        VERIFY_FAILED(XblSocialManagerDestroySocialUserGroup(nullptr));
        VERIFY_FAILED(XblSocialManagerDoWork(nullptr, nullptr));
#pragma warning( pop )

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::All,
            XblRelationshipFilter::Friends,
            &group
        ));

        env.AwaitEvents({ {XblSocialManagerEventType::SocialUserGroupLoaded, 1} });

        VERIFY_FAILED(XblSocialManagerUpdateSocialUserGroup(group, std::vector<uint64_t>{ 0, 1, 2 }.data(), 3));

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group));
    }

#if 0
    DEFINE_TEST_CASE(TestImproperCallingPattern)
    {
        TEST_LOG(L"Test starting: TestImproperCallingPattern");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromFilters(xboxLiveContext->User().Handle(), XblPresenceFilter::All, XblRelationshipFilter::Friends, &group));

        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromList(xboxLiveContext->User().Handle(), std::vector<uint64_t>{ 0, 1, 2 }.data(), 3, & group));

        VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::All, nullptr));
        VERIFY_FAILED(XblSocialManagerAddLocalUser(xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::All, nullptr));

        VERIFY_SUCCEEDED(XblSocialManagerRemoveLocalUser(xboxLiveContext->User().Handle()));
        VERIFY_FAILED(XblSocialManagerRemoveLocalUser(xboxLiveContext->User().Handle()));
    }
#endif

    DEFINE_TEST_CASE(TestCreateGroupBeforeUserAddedCompletes)
    {
        TEST_LOG(L"Test starting: TestCreateGroupBeforeUserAddedCompletes");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        // Add user, and before local user is added, create and destroy a group
        VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromList(xboxLiveContext->User().Handle(), std::vector<uint64_t>{ 101, 102 }.data(), 2, & group));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group));

        bool localUserAdded{ false };
        while (!localUserAdded)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::LocalUserAdded:
                {
                    localUserAdded = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_SUCCEEDED(XblSocialManagerRemoveLocalUser(xboxLiveContext->User().Handle()));
    }

    DEFINE_TEST_CASE(TestSocialRelationshipChangedDuringInitialization)
    {
        TEST_LOG(L"Test starting: TestSocialRelationshipChangedDuringInitialization");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        // Add user, and before local user is added, receive a social relationship changed notification
        uint64_t xuidAdded{ 101 };
        env.FollowedXuids.push_back(xuidAdded);

        VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));

        XblSocialManagerUserGroupHandle friendsGroup{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(xboxLiveContext->User().Handle(), XblPresenceFilter::All, XblRelationshipFilter::Friends, &friendsGroup));

        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Added, xuidAdded);

        bool localUserAdded{ false };
        bool userAddedToGraph{ false };
        while (!localUserAdded || !userAddedToGraph)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::LocalUserAdded:
                {
                    localUserAdded = true;
                    break;
                }
                // There are two possible orderings in this scenario: if the rta event is processed before the
                // graph initialization completes, by design, we won't get a UsersAddedToSocialGraph event. However,
                // in that case, the new user should already be in the friends group when it is loaded.
                case XblSocialManagerEventType::UsersAddedToSocialGraph:
                {
                    VERIFY_IS_TRUE(event->usersAffected[0]->xboxUserId == xuidAdded);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    userAddedToGraph = true;
                    break;
                }
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    VERIFY_IS_TRUE(event->groupAffected == friendsGroup);
                    if (env.GetUsersCount(friendsGroup) == env.FollowedXuids.size())
                    {
                        userAddedToGraph = true;
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(friendsGroup));
    }

    DEFINE_TEST_CASE(TestFilters)
    {
        TEST_LOG(L"Test starting: TestFilters");
        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());
        auto user{ xboxLiveContext->User().Handle() };

        XblSocialManagerUserGroupHandle allFriends, titleOnlineFriends, allFavorites, allOfflineFriends, titleOfflineFriends, allOnlineFriends, allTitleFriends;

        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(user, XblPresenceFilter::All, XblRelationshipFilter::Friends, &allFriends));
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(user, XblPresenceFilter::TitleOnline, XblRelationshipFilter::Friends, &titleOnlineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(user, XblPresenceFilter::All, XblRelationshipFilter::Favorite, &allFavorites));
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(user, XblPresenceFilter::AllOffline, XblRelationshipFilter::Friends, &allOfflineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(user, XblPresenceFilter::TitleOffline, XblRelationshipFilter::Friends, &titleOfflineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(user, XblPresenceFilter::AllOnline, XblRelationshipFilter::Friends, &allOnlineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(user, XblPresenceFilter::AllTitle, XblRelationshipFilter::Friends, &allTitleFriends));

        env.AwaitEvents({ {XblSocialManagerEventType::SocialUserGroupLoaded, 7} });

        // Verify initial user sizes
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(allFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(titleOnlineFriends));
        VERIFY_ARE_EQUAL_INT(0, env.GetUsersCount(allFavorites));
        VERIFY_ARE_EQUAL_INT(0, env.GetUsersCount(allOfflineFriends));
        VERIFY_ARE_EQUAL_INT(0, env.GetUsersCount(titleOfflineFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(allOnlineFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(allTitleFriends));

        // Setup a mock that will affect multiple groups: Online->Offline && Followed->Favorite
        env.SetPeoplehubMock(false, true);

        uint64_t changedXuid{ 1 };

        auto userResult = User::WrapHandle(user);
        env.FireSocialGraphChangedRtaEvent(userResult.ExtractPayload(), XblSocialNotificationType::Changed, changedXuid);

        bool profileChanged{ false };
        bool presenceChanged{ false };
        while (!profileChanged || !presenceChanged)
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialRelationshipsChanged:
                {
                    VERIFY_IS_TRUE(event->usersAffected[0]->xboxUserId == changedXuid);
                    VERIFY_IS_TRUE(event->usersAffected[0]->isFavorite == true);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    profileChanged = true;
                    break;
                }
                case XblSocialManagerEventType::PresenceChanged:
                {
                    VERIFY_IS_TRUE(event->usersAffected[0]->xboxUserId == changedXuid);
                    VERIFY_IS_TRUE(event->usersAffected[0]->presenceRecord.userState == XblPresenceUserState::Offline);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    presenceChanged = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        // verify groups are affected as expected
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(allFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size() - 1, env.GetUsersCount(titleOnlineFriends));
        VERIFY_ARE_EQUAL_INT(1, env.GetUsersCount(allFavorites));
        VERIFY_ARE_EQUAL_INT(1, env.GetUsersCount(allOfflineFriends));
        VERIFY_ARE_EQUAL_INT(1, env.GetUsersCount(titleOfflineFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size() - 1, env.GetUsersCount(allOnlineFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(allTitleFriends));

        std::vector<uint64_t> offlineXuids{ 1, 2, 3, 4, 5 };
        env.SetPresenceMock(offlineXuids);
        env.FireDevicePresenceChangeRtaEvent(offlineXuids);

        size_t presenceChangedEvents{ 0 };
        while (presenceChangedEvents < offlineXuids.size())
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::PresenceChanged:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_TRUE(affectedUser->xboxUserId > 0 && affectedUser->xboxUserId < 6);
                            ++presenceChangedEvents;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(allFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size() - offlineXuids.size(), env.GetUsersCount(titleOnlineFriends));
        VERIFY_ARE_EQUAL_INT(1, env.GetUsersCount(allFavorites));
        VERIFY_ARE_EQUAL_INT(offlineXuids.size(), env.GetUsersCount(allOfflineFriends));
        VERIFY_ARE_EQUAL_INT(offlineXuids.size(), env.GetUsersCount(titleOfflineFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size() - offlineXuids.size(), env.GetUsersCount(allOnlineFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(allTitleFriends));

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(allFriends));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(titleOnlineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(allFavorites));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(allOfflineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(titleOfflineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(allOnlineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(allTitleFriends));
    }

    DEFINE_TEST_CASE(TestRichPresencePolling)
    {
        TEST_LOG(L"Test starting: TestRichPresencePolling");
        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());

        XblSocialManagerUserGroupHandle onlineFriends{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::AllOnline,
            XblRelationshipFilter::Friends,
            &onlineFriends
        ));

        env.AwaitEvents({ {XblSocialManagerEventType::SocialUserGroupLoaded, 1} });
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(onlineFriends));

        // Change presence mock so we get a PresenceChanged event when it is polled
        std::vector<uint64_t> offlineUsers{ 1, 2, 3 };
        env.SetPresenceMock(offlineUsers);
        VERIFY_SUCCEEDED(XblSocialManagerSetRichPresencePollingStatus(xboxLiveContext->User().Handle(), true));

        size_t presenceChangedEvents{ 0 };
        while (presenceChangedEvents < offlineUsers.size())
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::PresenceChanged:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            ++presenceChangedEvents;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL_INT(offlineUsers.size(), presenceChangedEvents);
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size() - offlineUsers.size(), env.GetUsersCount(onlineFriends));

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(onlineFriends));
    }

    DEFINE_TEST_CASE(TestEventUserHandleLifetime)
    {
        TEST_LOG(L"Test starting: TestEventUserHandleLifetime");

        SMTestEnvironment env{};

        uint64_t const presenceChangedXuid{ 1 };
        std::vector<const XblSocialManagerEvent*> events{};

        XblSocialManagerUserGroupHandle groupHandle{ nullptr };

        {
            // Add a user to SocialManager and create a user group but don't hang on to the user handle. 
            // Ensure the user handles returned in all SM events remain valid until the next DoWork call
            User user{ CreateMockUser(MOCK_XUID) };
            env.AddLocalUser(user);

            VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
                user.Handle(),
                XblPresenceFilter::AllOnline,
                XblRelationshipFilter::Friends,
                &groupHandle
            ));
        }

        bool groupLoaded{ false };
        while (!groupLoaded)
        {
            events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    groupLoaded = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(groupHandle));
        {
            // Ensure the user in the event is valid even after destroying the group
            auto userFromEventResult = User::WrapHandle(events[0]->user);
            VERIFY_ARE_EQUAL_UINT(MOCK_XUID, userFromEventResult.ExtractPayload().Xuid());
        }

        // Make a user go offline to trigger event
        env.SetPresenceMock({ presenceChangedXuid });
        env.FireDevicePresenceChangeRtaEvent({ presenceChangedXuid }, false);

        bool presenceChanged{ false };
        while (!presenceChanged)
        {
            events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::PresenceChanged:
                {
                    VERIFY_ARE_EQUAL_UINT(presenceChangedXuid, event->usersAffected[0]->xboxUserId);
                    presenceChanged = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        XblUserHandle userHandle{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerGetLocalUsers(1, &userHandle));
        VERIFY_SUCCEEDED(XblSocialManagerRemoveLocalUser(userHandle));

        {
            // Ensure the user handle from the event is still valid after removing the user
            auto userFromEventResult = User::WrapHandle(events[0]->user);
            VERIFY_ARE_EQUAL_UINT(MOCK_XUID, userFromEventResult.ExtractPayload().Xuid());
        }
    }

    DEFINE_TEST_CASE(CppTestBasicCreateFilterGroup)
    {
        TEST_LOG(L"Test starting: CppTestBasicCreateFilterGroup");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateLegacyMockXboxLiveContext();
        xbox_live_user_t userHandle = xboxLiveContext->user();

        auto socialManager{ social_manager::get_singleton_instance() };

        auto addUserResult = socialManager->add_local_user(
            userHandle,
            social_manager_extra_detail_level::no_extra_detail
        );
        VERIFY_IS_TRUE(!addUserResult.err());

        {
            // Wait until we get the local_user_added event
            bool localUserAdded{ false };
            while (!localUserAdded)
            {
                auto events{ socialManager->do_work() };
                for (auto event : events)
                {
                    switch (event.event_type())
                    {
                    case social_event_type::local_user_added:
                    {
                        localUserAdded = true;
                        break;
                    }
                    default:
                    {
                        LOGS_DEBUG << "Unexpected SocialManager Event";
                        VERIFY_FAIL();
                    }
                    }
                }
            }
        }

        auto createGroupResult = socialManager->create_social_user_group_from_filters(
            userHandle,
            presence_filter::all,
            relationship_filter::friends
        );

        VERIFY_IS_TRUE(!createGroupResult.err());
        auto group = createGroupResult.payload();

        bool groupLoaded{ false };
        while (!groupLoaded)
        {
            auto events{ socialManager->do_work() };
            for (auto event : events)
            {
                switch (event.event_type())
                {
                case social_event_type::social_user_group_loaded:
                {
                    auto groupLoadedArgs = static_cast<social_user_group_loaded_event_args*>(event.event_args().get());
                    VERIFY_IS_TRUE(group == groupLoadedArgs->social_user_group());
                    groupLoaded = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        auto users{ group->users() };
        VERIFY_ARE_EQUAL(group->users_tracked_by_social_user_group().size(), users.size());

        for (auto user : users)
        {
            // profile tests
            LOGS_DEBUG << "Validating user " << user->xbox_user_id();
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->display_name());
            VERIFY_ARE_EQUAL_STR("http://images-eds.xboxlive.com/image?url=mHGRD8KXEf2sp2LC58XhBQKNl2IWRp.J.q8mSURKUUeiPPf0Y7Kl7zLN7rafayiPptVaX_XIUmNOPotNmNubbx4bHmf6It7Oj1ChU5UAo9k-&background=0xababab&mode=Padding&format=png", user->display_pic_url_raw());
            VERIFY_IS_TRUE(user->is_followed_by_caller());
            VERIFY_IS_TRUE(user->is_following_user());
            VERIFY_ARE_EQUAL_STR("9001", user->gamerscore());
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->gamertag());
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->modern_gamertag());
            VERIFY_ARE_EQUAL_STR("", user->modern_gamertag_suffix());
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->unique_modern_gamertag());
            VERIFY_IS_FALSE(user->is_favorite());
            VERIFY_IS_FALSE(user->use_avatar());

            // preferred color tests
            VERIFY_ARE_EQUAL_STR("193e91", user->preferred_color().primary_color());
            VERIFY_ARE_EQUAL_STR("2458cf", user->preferred_color().secondary_color());
            VERIFY_ARE_EQUAL_STR("122e6b", user->preferred_color().tertiary_color());

            // presence record tests
            VERIFY_IS_TRUE(user->presence_record().presence_title_records().size());
            VERIFY_IS_TRUE(user->presence_record().user_state() == user_presence_state::online);
            VERIFY_IS_TRUE(user->presence_record().is_user_playing_title(1234));
            VERIFY_IS_TRUE(user->presence_record().presence_title_records()[0].is_title_active());
            VERIFY_IS_TRUE(!user->presence_record().presence_title_records()[0].is_broadcasting());
            VERIFY_IS_TRUE(user->presence_record().presence_title_records()[0].device_type() == presence_device_type::pc);
            VERIFY_ARE_EQUAL_STR("Home", user->presence_record().presence_title_records()[0].presence_text());

            // title history tests
            VERIFY_IS_TRUE(user->title_history().has_user_played());
            VERIFY_IS_TRUE(Utils::TimeTFromDatetime(user->title_history().last_time_user_played()) == utils::TimeTFromDatetime(xbox::services::datetime::from_string("2015-01-26T22:54:54.6630Z", xbox::services::datetime::date_format::ISO_8601)));
        }

        auto destroyGroupResult = socialManager->destroy_social_user_group(group);
        VERIFY_IS_TRUE(!destroyGroupResult.err());

        auto removeUserResult = socialManager->remove_local_user(userHandle);
        VERIFY_IS_TRUE(!removeUserResult.err());

        auto events{ socialManager->do_work() };
        VERIFY_ARE_EQUAL_INT(events.size(), 1);
        VERIFY_IS_TRUE(events[0].event_type() == social_event_type::local_user_removed);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END