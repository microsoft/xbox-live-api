#pragma once

#include <mutex>
#include <optional>
#include <vector>
#include <winreg.h>

#include "impersonation_ctx.h"

namespace Xal
{

namespace Platform
{

namespace Grts
{

struct IAccountManagerListener
{
    virtual HRESULT OnUserSignOut(std::string const& webAccountId) noexcept = 0;

protected:
    ~IAccountManagerListener() noexcept = default;
};

/// <summary>
/// Class that handles the synchronization of Xbox IDP/AuthMan and all the instances of XAL in GRTS.
/// Makes sure they share the same gamer account and if there is a user change, it gets propagated.
/// </summary>
/// <remarks>
/// This should be tied to a single NT User and only have one instance per NT User context.
/// Init has to be called when constructed to start watching the AuthMan registry for user changes.
/// </remarks>
class GrtsAccountManager
{
public:
    GrtsAccountManager(ImpersonationCtx&& impersonationCtx) noexcept;
    ~GrtsAccountManager() noexcept;

    HRESULT Init() noexcept;

    HRESULT GetDefaultUser(_Out_ std::string& webAccountId) noexcept;
    void UserSignedIn(_In_ IAccountManagerListener* caller, std::string&& webAccountId, uint64_t xuid) noexcept;
    void UserSignedOut(_In_ IAccountManagerListener* caller) noexcept;

    HRESULT RegisterForNotification(_In_ IAccountManagerListener* listener) noexcept;
    void UnregisterForNotification(_In_ IAccountManagerListener* listener) noexcept;

private:
    std::unique_lock<std::mutex> Lock();

    static void CALLBACK RegistryChangeCallback(
        PTP_CALLBACK_INSTANCE instance,
        PVOID context,
        PTP_WAIT wait,
        TP_WAIT_RESULT waitResult
    ) noexcept;
    void OnRegistryChange() noexcept;
    HRESULT ResetRegistryNotification() noexcept;

    void SignOutListeners(_In_ IAccountManagerListener* caller) noexcept;
    HRESULT UpdateAuthMan(std::string const& webAccountId, std::string const& xuid) noexcept;

    HRESULT ReadStringFromRegistry(LPCWSTR lpSubKey, LPCWSTR lpValueName, std::string& value) noexcept;
    HRESULT WriteStringToRegistry(LPCWSTR lpSubKey, LPCWSTR lpValueName, std::string& value) noexcept;
    HRESULT DeleteFromRegistry(LPCWSTR lpSubKey, LPCWSTR lpValueName) noexcept;

    std::vector<IAccountManagerListener*>::const_iterator Find(std::vector<IAccountManagerListener*> const& list, _In_ IAccountManagerListener* listener) const noexcept;

private:
    mutable std::mutex m_mutex;

    std::optional<std::string> m_currentWebAccount;
    ImpersonationCtx m_impersonationCtx;
    std::vector<IAccountManagerListener*> m_listeners; // not owned by GrtsAccountManager
    std::vector<IAccountManagerListener*> m_expectedSignOuts; // not owned by GrtsAccountManager

    bool m_resetRegistryNotification;
    HKEY m_authManSubkey;
    PTP_WAIT m_pwait;
    HANDLE m_regEventHandle;
};

}

}

}
