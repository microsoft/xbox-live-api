#pragma once

#include <memory>
#include <string>
#include <wrl.h>

namespace Xal
{

namespace Platform
{

namespace Grts
{

/// <summary>
/// Handles the lifetime of an impersonation operation by ensuring we only continue
/// impersonating for as long as the object lives and stop impersonating after that.
/// This guard is not thread safe.
/// </summary>
class ImpersonationGuard
{
public:
    ImpersonationGuard() noexcept;
    ImpersonationGuard(ImpersonationGuard const& other) = delete;
    ImpersonationGuard(ImpersonationGuard&& other) noexcept;
    ~ImpersonationGuard() noexcept;

    ImpersonationGuard& operator=(ImpersonationGuard const& other) = delete;
    ImpersonationGuard& operator=(ImpersonationGuard&& other) noexcept;

    HRESULT StartImpersonating(HANDLE impersonationToken) noexcept;

private:
    DWORD m_impersonatingThread;
};

/// <summary>
/// Wrapper for an impersonation guard that also sets the proxy blanket 
/// to make calls with dynamic cloaking, allowing the API's we are calling
/// to use the impersonation token to identify us, instead of our process' token.
/// </summary>
class TransitiveImpersonationGuard
{
public:
    TransitiveImpersonationGuard() noexcept;
    TransitiveImpersonationGuard(TransitiveImpersonationGuard const& other) = delete;
    TransitiveImpersonationGuard(TransitiveImpersonationGuard&& other) noexcept;
    ~TransitiveImpersonationGuard() noexcept;

    TransitiveImpersonationGuard& operator=(TransitiveImpersonationGuard const& other) = delete;
    TransitiveImpersonationGuard& operator=(TransitiveImpersonationGuard&& other) noexcept;

    HRESULT StartImpersonating(HANDLE impersonationToken, IUnknown* allowedInterface) noexcept;

private:
    ImpersonationGuard m_impersonationGuard;
    Microsoft::WRL::ComPtr<IUnknown> m_allowedInterface;
    DWORD m_pwAuthnSvc;
    DWORD m_pAuthzSvc;
    PWSTR m_pServerPrincName;
    DWORD m_pAuthnLevel;
    DWORD m_pImpLevel;
    RPC_AUTH_IDENTITY_HANDLE m_pAuthInfo;
    DWORD m_pCapabilites;
};

/// <summary>
/// Wrapper that holds a HANDLE and closes it when it is destroyed.
/// </summary>
class ImpersonationHandle
{
public:
    ImpersonationHandle() noexcept;
    ~ImpersonationHandle() noexcept;

    ImpersonationHandle(ImpersonationHandle const& other) = delete;
    ImpersonationHandle(ImpersonationHandle&& other) = delete;
    ImpersonationHandle& operator=(ImpersonationHandle const& other) = delete;
    ImpersonationHandle& operator=(ImpersonationHandle&& other) = delete;

    HANDLE* GetAddressOfHandle() noexcept;
    HANDLE GetHandle() const noexcept;
    bool IsNull() const noexcept;

    HRESULT GetUserSid(std::string& sid) const noexcept;
    void SetUserSid(std::string&& sid) noexcept;

private:
    HANDLE m_handle;
    std::string m_ntUserSid;
};

/// <summary>
/// Context for impersonation, handles getting a user impersonation token on
/// initialization and uses it to impersonate when Impersonate() is called. Init must
/// be called before trying to impersonate.
/// </summary>
class ImpersonationCtx
{
public:
    ImpersonationCtx() noexcept;

    HRESULT Init() noexcept;
    HRESULT Impersonate(_Out_ ImpersonationGuard& impersonationGuard) const noexcept;
    HRESULT ImpersonateTransitively(_In_ IUnknown* allowedInterface, _Out_ TransitiveImpersonationGuard& impersonationGuard) const noexcept;

    HRESULT GetUserSid(std::string& sid) const noexcept;

private:
    std::shared_ptr<ImpersonationHandle> m_impersonationToken;
};

}

}

}
