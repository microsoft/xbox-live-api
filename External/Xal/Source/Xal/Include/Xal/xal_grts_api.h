#pragma once

#include <memory>
#include <string>
#include <Xal/xal_grts_platform.h>
#include <Xal/xal_internal_telemetry.h>
#include <Xal/xal_internal_web_account.h>
#include <Xal/xal_types.h>

namespace Xal
{

namespace State
{

class State;

}

namespace Grts
{

class User
{
public:
    User() noexcept;
    User(User&& o) noexcept;
    User& operator=(User&& o) noexcept;
    HRESULT Initialize(_In_ XalUserHandle h) noexcept; // takes ownership
    HRESULT InitializeAsCopyOf(User const& o) noexcept;

    ~User() noexcept;

    XalUserHandle Handle() const noexcept;

    HRESULT GetId(uint64_t& id) const noexcept;
    HRESULT GetLocalId(XalUserLocalId& localId) const noexcept;
    HRESULT GetGamertag(XalGamertagComponent component, std::string& gamertag) const noexcept;
    HRESULT GetWebAccountId(std::string& webAccountId) const noexcept;
    HRESULT GetAgeGroup(XalAgeGroup& ageGroup) const noexcept;
    HRESULT CheckPrivilege(
        uint32_t privilege,
        bool& result,
        XalPrivilegeCheckDenyReasons& reasons
    ) const noexcept;

private:
    User(User const&) = delete;
    User& operator=(User const&) = delete;

    XalUserHandle m_user;
};

// this class firewalls Xal internals from the rest of the system
class State
{
public:
    State() noexcept;
    HRESULT Initialize(XalInitArgs const& args) noexcept;

    State(State const& o) noexcept;
    State(State&& o) noexcept;
    State& operator=(State const& o) noexcept;
    State& operator=(State&& o) noexcept;

    ~State() noexcept;

    HRESULT CleanupAsync(_In_ XAsyncBlock* async) noexcept;

    HRESULT GetMaxUsers(uint32_t& maxUsers) const noexcept;

    bool SignOutUserAsyncIsPresent() const noexcept;

    HRESULT GetTitleId(uint32_t& titleId) const noexcept;

    size_t GetSandboxSize() const noexcept;
    HRESULT GetSandbox(
        size_t sandboxSize,
        _Out_writes_(sandboxSize) char* sandbox,
        size_t& sandboxUsed
    ) noexcept;

    bool GetDeviceUserIsPresent() const noexcept;
    HRESULT GetDeviceUser(User& deviceUser) noexcept;

    HRESULT TryAddFirstUserSilentlyAsync(
        _In_ XAsyncBlock* async,
        _In_opt_ void* userContext
    ) noexcept;
    HRESULT AddUserWithUiAsync(
        _In_ XAsyncBlock* async,
        _In_opt_ void* userContext
    ) noexcept;
    HRESULT AddUserWithUrl(
        _In_ XAsyncBlock* async,
        _In_opt_ void* userContext,
        std::string const& url
    ) noexcept;

    HRESULT SignOutUserAsync(
        _In_ XAsyncBlock* async,
        User const& oldUser
    ) noexcept;

    HRESULT GetWebAccountTokenAsync(
        _In_ XAsyncBlock* async,
        User const& user,
        XalUserGetWebAccountTokenArgs const& args,
        bool allowUi
    ) noexcept;

    HRESULT GetGamerPictureAsync(
        _In_ XAsyncBlock* async,
        User const& user,
        XalGamerPictureSize size
    ) noexcept;

    HRESULT CheckPrivilege(
        User const& user,
        XalPrivilege privilege,
        bool& result,
        XalPrivilegeCheckDenyReasons& reasons
    ) const noexcept;

    HRESULT GetTokenAndSignatureSilentlyAsync(
        _In_ XAsyncBlock* async,
        User const& user,
        XalUserGetTokenAndSignatureArgs const& args
    ) noexcept;

    HRESULT ResolveUserIssueWithUiAsync(
        _In_ XAsyncBlock* async,
        User const& user,
        std::string const& url
    ) noexcept;

    HRESULT RegisterUserChangeEventHandler(
        _In_opt_ XTaskQueueHandle queue,
        _In_opt_ void* context,
        _In_ XalUserChangeEventHandler* handler,
        XalRegistrationToken& token
    ) noexcept;

    void UnregisterUserChangeEventHandler(XalRegistrationToken token) noexcept;

    HRESULT GetSignOutDeferral(XalSignoutDeferralHandle& deferral) noexcept;

    HRESULT CloseSignOutDeferral(XalSignoutDeferralHandle deferral) noexcept;

    HRESULT PlatformWebShowUrlComplete(
        XalPlatformOperation operation,
        XalPlatformOperationResult result,
        std::string const& url
    ) noexcept;

    HRESULT PlatformShowMsaWamUiComplete(
        _In_ XalPlatformOperation operation,
        XalPlatformOperationResult result,
        _In_ HRESULT hresult,
        _In_ std::string const& webAccountId
    ) noexcept;

    HRESULT PlatformStorageWriteComplete(
        XalPlatformOperation operation,
        XalPlatformOperationResult result
    ) noexcept;

    HRESULT PlatformStorageReadComplete(
        XalPlatformOperation operation,
        XalPlatformOperationResult result,
        size_t dataSize,
        _In_reads_bytes_opt_(dataSize) void const* data
    ) noexcept;

    HRESULT PlatformStorageClearComplete(
        XalPlatformOperation operation,
        XalPlatformOperationResult result
    ) noexcept;

    HRESULT PlatformRemoteConnectCancelPrompt(XalPlatformOperation operation) noexcept;

    HRESULT SendTelemetryEvent(
        User const& user,
        std::string const& iKey,
        std::string const& name,
        std::string const& data,
        uint32_t ticketCount,
        _In_reads_(ticketCount) XalTelemetryTicket* tickets,
        XalTelemetryLatency latency,
        XalTelemetryPersistence persistence,
        XalTelemetrySensitivity sensitivity,
        XalTelemetrySampleRate sampleRate
    ) noexcept;

public: // internal utils
    HRESULT UserFromLocalId(XalUserLocalId localId, User& user) noexcept;

private:
    void Cleanup() noexcept;

    Xal::State::State* m_state;
};

}

}
