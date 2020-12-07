#pragma once

#include <map>
#include <memory>
#include <shared_mutex>
#include <Xal/grts_base/grts_account_manager.h>
#include <Xal/grts_base/impersonation_ctx.h>

namespace Xal
{

namespace Platform
{

namespace Grts
{

// NtUserContext holds state that is shared across all Xal instances for games
// running under the same nt user
class NtUserContext
{
public:
    static NtUserContext Empty() noexcept;
    static NtUserContext Initialized(ImpersonationCtx&& impersonationCtx);

    bool IsEmpty() const noexcept;

    std::shared_lock<std::shared_mutex> GetCredentialManagerReadLock() const;
    std::unique_lock<std::shared_mutex> GetCredentialManagerWriteLock() const;

    GrtsAccountManager& AccountManager() const;

private:
    NtUserContext() = default;

    struct Data;

    std::shared_ptr<Data> m_data;
};

// NtUserManager is the central directory where we store NtUserContexts for
// later lookup
class NtUserManager
{
public:
    HRESULT GetContext(std::string const& userSid, ImpersonationCtx const& impersonationCtx, NtUserContext& ctx) noexcept;

private:
    std::unique_lock<std::mutex> Lock() const;

    mutable std::mutex m_mutex;
    std::map<std::string, NtUserContext> m_users;
};

}

}

}
