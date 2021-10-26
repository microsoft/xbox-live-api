// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( push )
#pragma warning( disable : 4365 )
#pragma warning( disable : 4061 )
#pragma warning( disable : 4996 )
#endif
#include "rapidjson/document.h"
#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( pop )
#endif
#if HC_PLATFORM == HC_PLATFORM_ANDROID
#include <thread>
#include "multidevice.h"
#include "api_explorer.h"
#include "runner.h"
#include "pal.h"
#endif
#if __has_include("apirunnercloudfns.h")
#include "apirunnercloudfns.h"
#else
#define APIRUNNER_GET_JOINABLE ""
#define APIRUNNER_HOST_SESSION ""
#define APIRUNNER_JOIN_SESSION ""
#define APIRUNNER_SET_STATE ""
#define APIRUNNER_GET_STATE ""
#endif

#define RETURN_HR_IF_FAILED(expr) { HRESULT exprResult{ expr }; if (FAILED(exprResult)) { return exprResult; } }

#if HC_PLATFORM_IS_MICROSOFT
    #define SPRINTF_XPLAT sprintf_s
#else
    #define SPRINTF_XPLAT sprintf
#endif

using namespace utility;

static std::thread s_watchThread{};

ApiRunnerMultiDeviceManager::~ApiRunnerMultiDeviceManager()
{
    // TODO: add back when HC init and cleanup is ref counted
    //if (m_isHCInitialized)
    //{
    //    HCCleanup();
    //}
}

std::string ExtractJsonString(
    _In_ const rapidjson::Document& jsonDoc,
    _In_ const std::string& stringName)
{
    if (jsonDoc.IsObject() && jsonDoc.HasMember(stringName.c_str()))
    {
        const rapidjson::Value& field = jsonDoc[stringName.c_str()];
        if (field.IsString())
        {
            return field.GetString();
        }
    }

    return "";
}

HRESULT ApiRunnerMultiDeviceManager::JoinOpenSession(
    _In_ const std::string& name,
    _In_ const std::string& xuid,
    _In_ const std::string& gamertag,
    _In_ std::function<void(HRESULT hr)> handler)
{
    auto manager = Data()->m_multiDeviceManager;
    manager->m_nextCallTime = datetime::utc_now() - datetime::from_seconds(1);
    manager->m_callGetJoinable = true;
    LogToScreen("MultiDevice: Looking for test session to join...");
    while (!manager->Joined())
    {
        int64_t delta = static_cast<int64_t>(manager->m_nextCallTime.to_interval()) - static_cast<int64_t>(datetime::utc_now().to_interval());
        if (delta < 0 && manager->m_callGetJoinable)
        {
            manager->m_callGetJoinable = false;
            manager->GetJoinable(
                name,
                [xuid, gamertag, handler](HRESULT, uint32_t, const std::string& joinableSessionId, const std::string& client1xuid, const std::string& client1gt)
                {
                    auto manager = Data()->m_multiDeviceManager;
                    if (!joinableSessionId.empty() && !xuid.empty() && !client1gt.empty())
                    {
                        LogToScreen("MultiDevice: Found test session: %s", joinableSessionId.c_str());
                        LogToScreen("MultiDevice: Remote XUID: %s GamerTag: %s", client1xuid.c_str(), client1gt.c_str());
                        manager->JoinSession(joinableSessionId, xuid, gamertag, 
                            [handler](HRESULT hr) 
                            {
                                if (SUCCEEDED(hr))
                                {
                                    handler(hr);
                                }
                                else
                                {
                                    auto manager = Data()->m_multiDeviceManager;
                                    manager->m_nextCallTime = datetime::utc_now() + datetime::from_seconds(1);
                                    manager->m_callGetJoinable = true;
                                }
                            });
                    }
                    else
                    {
                        manager->m_nextCallTime = datetime::utc_now() + datetime::from_seconds(1);
                        manager->m_callGetJoinable = true;
                    }
                });
        }

        pal::Sleep(100);
    }

    return S_OK;
}

HRESULT ApiRunnerMultiDeviceManager::GetJoinable(
    _In_ const std::string& name,
    _In_ std::function<void(HRESULT hr, uint32_t statusCode, const std::string& joinableSessionId, const std::string& xuid, const std::string& gt)> handler
)
{
    std::vector<std::vector<std::string>> headers;
    headers.push_back(std::vector<std::string>{"name", name});
    HRESULT hr = MakeCall("GET", APIRUNNER_GET_JOINABLE, headers,
        [handler](HRESULT hr, uint32_t statusCode, const std::string& responseString)
        {
            std::string sessionId;
            std::string xuid;
            std::string gt;

            if (SUCCEEDED(hr) && statusCode == 200 && responseString.length() > 0)
            {
                rapidjson::Document jsonDoc;
                jsonDoc.Parse(responseString.c_str());
                if (!jsonDoc.HasParseError())
                {
                    // Example response:
                    //{
                    //    "id": "637123016212526708",
                    //    "client1xuid" : "123",
                    //    "client1gt" : "ninja",
                    //    "client2xuid" : null,
                    //    "client2gt" : null,
                    //    "props" : null
                    //}

                    sessionId = ExtractJsonString(jsonDoc, "id");
                    xuid = ExtractJsonString(jsonDoc, "client1xuid");
                    gt = ExtractJsonString(jsonDoc, "client1gt");
                }
            }

            auto manager = Data()->m_multiDeviceManager;
            manager->m_isInitialized = true;
            if (handler)
            {
                handler(hr, statusCode, sessionId, xuid, gt);
            }
        });
    if (FAILED(hr))
    {
        LogToScreen("MultiDevice: Get joinable test session failed %0.8x", hr);
    }
    return hr;
}

HRESULT ApiRunnerMultiDeviceManager::Init(
    _In_ std::function<void(const std::string& key, const std::string& value)> onStateChangedHandler
    )
{
    m_doPoll = false;
    m_onStateChangedHandler = std::move(onStateChangedHandler);

    RETURN_HR_IF_FAILED(HCInitialize(nullptr));
    m_isHCInitialized = true;

    return S_OK;
}

HRESULT ApiRunnerMultiDeviceManager::WaitTillPeerConnects()
{
    auto manager = Data()->m_multiDeviceManager;
    manager->m_nextCallTime = datetime::utc_now() - datetime::from_seconds(1);
    manager->m_callGetJoinable = true;
    std::string curSessionId = manager->m_sessionId;
    std::string name = manager->m_sessionName;
    LogToScreen("MultiDevice: Waiting for peer to connect to test session...");
    while (!manager->Joined())
    {
        int64_t delta = static_cast<int64_t>(manager->m_nextCallTime.to_interval()) - static_cast<int64_t>(datetime::utc_now().to_interval());
        if (delta < 0)
        {
            manager->m_nextCallTime = datetime::utc_now() + datetime::from_seconds(1);
            std::string state = manager->GetSessionStateString();
            if (state.length() > 0)
            {
                rapidjson::Document jsonDoc;
                jsonDoc.Parse(state.c_str());
                if (!jsonDoc.HasParseError())
                {
                    std::string client2xuid = ExtractJsonString(jsonDoc, "client2xuid");
                    if (!client2xuid.empty())
                    {
                        std::string client2gt = ExtractJsonString(jsonDoc, "client2gt");
                        LogToScreen("MultiDevice: Connected to peer XUID: %s GamerTag: %s...", client2xuid.c_str(), client2gt.c_str());
                        manager->m_isJoined = true;
                    }
                }
            }
        }

        pal::Sleep(100);
    }

    if (manager->Abort())
    {
        return E_ABORT;
    }
    return S_OK;
}

uint64_t ApiRunnerMultiDeviceManager::GetLocalXuid()
{
    std::string xuid = IsHost() ? m_state.client1xuid : m_state.client2xuid;
    return strtoull(xuid.c_str(), nullptr, 0);
}

uint64_t ApiRunnerMultiDeviceManager::GetRemoteXuid()
{
    std::string xuid = IsHost() ? m_state.client2xuid : m_state.client1xuid;
    return strtoull(xuid.c_str(), nullptr, 0);
}

HRESULT ApiRunnerMultiDeviceManager::HostSession(
    _In_ const std::string& name,
    _In_ const std::string& xuid,
    _In_ const std::string& gamertag,
    _In_ std::function<void(HRESULT)> onResponse)
{
    LogToScreen("MultiDevice: Hosting test session as XUID: %s GamerTag: %s...", xuid.c_str(), gamertag.c_str());
    std::vector<std::vector<std::string>> headers;
    headers.push_back(std::vector<std::string>{"name", name});
    headers.push_back(std::vector<std::string>{"xuid", xuid});
    headers.push_back(std::vector<std::string>{"gt", gamertag});
    HRESULT hr = MakeCall("POST", APIRUNNER_HOST_SESSION, headers,
        [onResponse, name](HRESULT hr, uint32_t statusCode, const std::string& responseString)
        {
            auto manager = Data()->m_multiDeviceManager;
            if (SUCCEEDED(hr) && statusCode == 200 && responseString.length() > 0)
            {
                rapidjson::Document jsonDoc;
                jsonDoc.Parse(responseString.c_str());
                if (!jsonDoc.HasParseError())
                {
                    // Example response:
                    //{
                    //    "id": "637123065073534386",
                    //    "client1xuid" : "123",
                    //    "client1gt" : "ninja",
                    //    "log" : "Deleted: 1"
                    //}

                    {
                        std::lock_guard<std::mutex> lock(manager->m_mutex);
                        manager->m_sessionId = ExtractJsonString(jsonDoc, "id");
                        manager->m_sessionState = responseString;
                        manager->m_sessionName = name;
                        manager->m_hosted = true;
                        LogToScreen("MultiDevice: Hosted test session %s", manager->m_sessionId.c_str());
                    }
                    manager->StartSessionStateChangePolling();
                    if (onResponse)
                    {
                        onResponse(S_OK);
                    }
                }
            }
            else 
            {
                LogToScreen("MultiDevice: Hosting test session failed. 0x%0.8x. HTTP status: %d", hr, statusCode);
                if (onResponse)
                {
                    onResponse(E_FAIL);
                }
            }
        });
    if (FAILED(hr))
    {
        LogToScreen("MultiDevice: Hosting test session failed %0.8x", hr);
        if (onResponse)
        {
            onResponse(hr);
        }
    }
    return hr;
}

HRESULT ApiRunnerMultiDeviceManager::JoinSession(
    _In_ const std::string& sessionId,
    _In_ const std::string& xuid,
    _In_ const std::string& gamertag,
    _In_ std::function<void(HRESULT)> handler)
{
    std::vector<std::vector<std::string>> headers;
    headers.push_back(std::vector<std::string>{"id", sessionId});
    headers.push_back(std::vector<std::string>{"xuid", xuid});
    headers.push_back(std::vector<std::string>{"gt", gamertag});
    HRESULT hr = MakeCall("POST", APIRUNNER_JOIN_SESSION, headers,
        [handler](HRESULT hr, uint32_t statusCode, const std::string& responseString)
        {
            // Could return 409 if there's another client joined 

            auto manager = Data()->m_multiDeviceManager;
            if (SUCCEEDED(hr) && statusCode == 200 && responseString.length() > 0)
            {
                rapidjson::Document jsonDoc;
                jsonDoc.Parse(responseString.c_str());
                if (!jsonDoc.HasParseError())
                {
                    // Example:
                    //{
                    //    "id": "637123065073534386",
                    //    "client1xuid" : "123",
                    //    "client1gt" : "ninja",
                    //    "client2xuid" : "456",
                    //    "client2gt" : "karate",
                    //    "props" : null
                    //}

                    {
                        std::lock_guard<std::mutex> lock(manager->m_mutex);
                        manager->m_sessionId = ExtractJsonString(jsonDoc, "id");
                        manager->m_sessionState = responseString;
                        manager->m_sessionName = ExtractJsonString(jsonDoc, "name");
                        LogToScreen("MultiDevice: Joining test session %s", manager->m_sessionId.c_str());
                        manager->StartSessionStateChangePolling();
                        manager->m_isJoined = true;
                        manager->m_hosted = false;
                    }
                    if (handler)
                    {
                        handler(S_OK);
                    }
                }
            }

            if (manager->m_sessionId.empty())
            {
                LogToScreen("MultiDevice: Joining test session failed. 0x%0.8x. HTTP status: %d", hr, statusCode);
                if (handler)
                {
                    handler(E_FAIL);
                }
            }
        });
    if (FAILED(hr))
    {
        LogToScreen("MultiDevice: Joining test session failed %0.8x", hr);
        if (handler)
        {
            handler(hr);
        }
    }

    return hr;
}

HRESULT ApiRunnerMultiDeviceManager::SetSessionState(
    _In_ const std::string& key,
    _In_ const std::string& value,
    _In_ std::function<void(HRESULT)> handler)
{
    std::vector<std::vector<std::string>> headers;
    if (value.length() == 0)
    {
        LogToScreen("MultiDevice: Deleting %s key in cloud DB", key.c_str());
    }
    else
    {
        LogToScreen("MultiDevice: Setting %s key to %s in cloud DB", key.c_str(), value.c_str());
    }

    headers.push_back(std::vector<std::string>{"id", m_sessionId});
    headers.push_back(std::vector<std::string>{"key", key});
    headers.push_back(std::vector<std::string>{"value", value});
    HRESULT hr = MakeCall("POST", APIRUNNER_SET_STATE, headers,
        [](HRESULT hr, uint32_t statusCode, const std::string& responseString)
        {
            auto manager = Data()->m_multiDeviceManager;
            if (SUCCEEDED(hr) && statusCode == 200 && responseString.length() > 0)
            {
                rapidjson::Document jsonDoc;
                jsonDoc.Parse(responseString.c_str());
                if (!jsonDoc.HasParseError())
                {
                    std::lock_guard<std::mutex> lock(manager->m_mutex);
                    manager->m_sessionState = responseString;
                }
            }
            else
            {
                LogToScreen("Set test session state failed %0.8x %d", hr, statusCode);
            }
        });
    if (FAILED(hr))
    {
        LogToScreen("Set test session state failed %0.8x", hr);
    }
    return hr;
}

HRESULT ApiRunnerMultiDeviceManager::StartSessionStateChangePolling()
{
    if (!m_doPoll)
    {
        LogToScreen("Watching test session for changes");
        m_doPoll = true;
        s_watchThread = std::thread([]()
            {
                auto manager = Data()->m_multiDeviceManager;
                while (manager->IsDoPoll())
                {
                    manager->RefreshSessionState();
                    pal::Sleep(1000);
                }
                LogToScreen("Stopping watch thread");
            });
    }
    return S_OK;
}

HRESULT ApiRunnerMultiDeviceManager::StopSessionStateChangePolling()
{
    if (m_doPoll)
    {
        LogToScreen("Stopped watching for test session changes");
        m_doPoll = false;
        s_watchThread.join();
    }
    return S_OK;
}


void ApiRunnerMultiDeviceManager::ParseState(const std::string& responseString)
{
    rapidjson::Document jsonDoc;
    jsonDoc.Parse(responseString.c_str());

    ServerState oldState;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        oldState = std::move(m_state);

        m_state.client1xuid = ExtractJsonString(jsonDoc, "client1xuid");
        m_state.client1gt = ExtractJsonString(jsonDoc, "client1gt");
        m_state.client2xuid = ExtractJsonString(jsonDoc, "client2xuid");
        m_state.client2gt = ExtractJsonString(jsonDoc, "client2gt");
        m_state.id = ExtractJsonString(jsonDoc, "id");
    }

    std::string props = ExtractJsonString(jsonDoc, "props");
    rapidjson::Document jsonDocProps;
    jsonDocProps.Parse(props.c_str());
    if (!jsonDocProps.HasParseError())
    {
        for (auto& m : jsonDocProps.GetObject())
        {
            std::string key = m.name.GetString();
            std::string value = m.value.GetString();
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_state.propmap[key] = value;
            }

            if (value != oldState.propmap[key])
            {
                m_onStateChangedHandler(key, value);
            }
        }
    }
}

std::string ApiRunnerMultiDeviceManager::GetStateValueFromKey(std::string key)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_state.propmap[key];
}

HRESULT ApiRunnerMultiDeviceManager::RefreshSessionState()
{
    std::vector<std::vector<std::string>> headers;
    headers.push_back(std::vector<std::string>{"id", m_sessionId});
    HRESULT hr = MakeCall("GET", APIRUNNER_GET_STATE, headers,
        [](HRESULT hr, uint32_t statusCode, const std::string& responseString)
        {
            auto manager = Data()->m_multiDeviceManager;
            if (SUCCEEDED(hr) && statusCode == 200 && responseString.length() > 0)
            { 
                rapidjson::Document jsonDoc;
                jsonDoc.Parse(responseString.c_str());
                if (!jsonDoc.HasParseError())
                {
                    std::string oldState;
                    {
                        std::lock_guard<std::mutex> lock(manager->m_mutex);
                        oldState = std::move(manager->m_sessionState);
                        manager->m_sessionState = responseString;
                    }
                    if(oldState != responseString)
                    {
                        manager->ParseState(responseString);
                    }
                }
            }
            else
            {
                LogToScreen("Get test session state failed %0.8x %d", hr, statusCode);
            }
        });
    if (FAILED(hr))
    {
        LogToScreen("Get test session state failed %0.8x", hr);
    }
    return hr;
}

class HCCallHandleRAII
{
public:
    ~HCCallHandleRAII()
    {
        if (handle != nullptr)
        {
            HCHttpCallCloseHandle(handle);
        }
    }

    HCCallHandle handle{ nullptr };
};

struct ApiRunnerMultiDeviceManagerCallContext
{
    HCCallHandleRAII call;
    std::function<void(HRESULT hr, uint32_t statusCode, const std::string& responseString)> handler;
};

HRESULT ApiRunnerMultiDeviceManager::MakeCall(
    _In_ const std::string& method,
    _In_ const std::string& url,
    _In_ const std::vector<std::vector<std::string>>& headers,
    _In_ std::function<void(HRESULT hr, uint32_t statusCode, const std::string& responseString)> handler)
{
    auto contextPtr = std::unique_ptr<ApiRunnerMultiDeviceManagerCallContext>(
        new ApiRunnerMultiDeviceManagerCallContext{ nullptr, handler }
    );

    HRESULT hr = HCHttpCallCreate(&contextPtr->call.handle);
    if (hr == E_HC_NOT_INITIALISED)
    {
        // TODO: remove when HC init/cleanup is ref counted
        RETURN_HR_IF_FAILED(HCInitialize(nullptr));
        RETURN_HR_IF_FAILED(HCHttpCallCreate(&contextPtr->call.handle));
    }
    RETURN_HR_IF_FAILED(HCHttpCallRequestSetUrl(contextPtr->call.handle, method.c_str(), url.c_str()));

    for (auto& header : headers)
    {
        std::string headerName = header[0];
        std::string headerValue = header[1];
        RETURN_HR_IF_FAILED(HCHttpCallRequestSetHeader(contextPtr->call.handle, headerName.c_str(), headerValue.c_str(), true));
    }

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->context = contextPtr.get();
    XTaskQueueHandle queue = nullptr;
    hr = XTaskQueueCreate(
        XTaskQueueDispatchMode::ThreadPool,
        XTaskQueueDispatchMode::ThreadPool,
        &queue);
    RETURN_HR_IF_FAILED(hr);
    asyncBlock->queue = queue;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::string responseString;
        uint32_t statusCode{ 0 };
        HRESULT networkErrorCode{ S_OK };
        uint32_t platErrCode{ 0 };

        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        std::unique_ptr<ApiRunnerMultiDeviceManagerCallContext> contextPtr{ static_cast<ApiRunnerMultiDeviceManagerCallContext*>(asyncBlock->context) };
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);
        if (SUCCEEDED(hr))
        {
            hr = HCHttpCallResponseGetNetworkErrorCode(contextPtr->call.handle, &networkErrorCode, &platErrCode);
            if (SUCCEEDED(hr))
            {
                hr = HCHttpCallResponseGetStatusCode(contextPtr->call.handle, &statusCode);
                if (SUCCEEDED(hr))
                {
                    const char* response{ nullptr }; 
                    hr = HCHttpCallResponseGetResponseString(contextPtr->call.handle, &response);
                    if (response != nullptr && SUCCEEDED(hr))
                    {
                        responseString = response; // ptr memory is only alive while call handle isn't closed, so copy to std::string
                    }
                }
            }
        }
        if (FAILED(hr))
        {
            networkErrorCode = hr;
        }

        if (contextPtr->handler)
        {
            contextPtr->handler(networkErrorCode, statusCode, responseString);
        }

        // HCHttpCallCloseHandle will get called will context gets freed since it'll dtor HCCallHandleRAII
    };

    hr = HCHttpCallPerformAsync(contextPtr->call.handle, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
        contextPtr.release();
    }

    return hr;
}
