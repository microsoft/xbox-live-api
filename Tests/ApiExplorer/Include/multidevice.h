#pragma once

#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( push )
#pragma warning( disable : 4365 )
#pragma warning( disable : 4266 )
#endif
#include "cpprest/http_msg.h"
#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( pop )
#endif

struct ServerState 
{
    std::string client1xuid;
    std::string client1gt;
    std::string client2xuid;
    std::string client2gt;
    std::string id;
    std::map<std::string, std::string> propmap;
};

class ApiRunnerMultiDeviceManager
{
private:
    std::mutex m_mutex;
    ServerState m_state;
    std::string m_sessionId;
    std::string m_sessionName;
    bool m_hosted{ false };
    bool m_isInitialized{ false };
    bool m_isHCInitialized{ false };
    bool m_isJoinable{ false };
    std::string m_sessionState;
    std::function<void(const std::string& key, const std::string& value)> m_onStateChangedHandler;
    std::atomic<bool> m_doPoll;
    std::atomic<bool> m_isJoined;
    std::atomic<bool> m_isAbort;
    utility::datetime m_nextCallTime;
    bool m_callGetJoinable;

public:
    ~ApiRunnerMultiDeviceManager();
    HRESULT Init(_In_ std::function<void(const std::string& key, const std::string& value)> onStateChangedHandler);

    bool IsInitialized() { return m_isInitialized; }
    bool IsJoinable() { return m_isJoinable; }
    bool Joined() { return m_isJoined; }
    bool Abort() { return m_isAbort; }
    std::string SessionId() { return m_sessionId; }
    const ServerState& GetSessionState() { return m_state; }
    bool IsHost() { return m_hosted; }
    uint64_t GetLocalXuid();
    uint64_t GetRemoteXuid();

    HRESULT HostSession(
        _In_ const std::string& name,
        _In_ const std::string& xuid,
        _In_ const std::string& gamertag,
        _In_ std::function<void(HRESULT hr)> onResponse);

    HRESULT WaitTillPeerConnects();

    HRESULT JoinOpenSession(
        _In_ const std::string& name,
        _In_ const std::string& xuid,
        _In_ const std::string& gamertag,
        _In_ std::function<void(HRESULT hr)> handler);

    HRESULT GetJoinable(
        _In_ const std::string& name,
        _In_ std::function<void(HRESULT hr, uint32_t statusCode, const std::string& joinableSessionId, const std::string& xuid, const std::string& gt)> handler
        );

    HRESULT JoinSession(
        _In_ const std::string& sessionId,
        _In_ const std::string& xuid,
        _In_ const std::string& gamertag,
        _In_ std::function<void(HRESULT hr)> handler);

    HRESULT SetSessionState(
        _In_ const std::string& key,
        _In_ const std::string& value,
        _In_ std::function<void(HRESULT)> handler);

    HRESULT StartSessionStateChangePolling();
    HRESULT StopSessionStateChangePolling();
    HRESULT RefreshSessionState();
    std::string GetSessionStateString() { return m_sessionState; }
    std::string GetStateValueFromKey(std::string key);
    bool IsDoPoll() { return m_doPoll; }
    void ParseState(const std::string& responseString);

    HRESULT MakeCall(
        _In_ const std::string& method,
        _In_ const std::string& url,
        _In_ const std::vector<std::vector<std::string>>& headers,
        _In_ std::function<void(HRESULT hr, uint32_t statusCode, const std::string& responseString)> handler);
};

