#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

#include <atomic>
#include <stdint.h>
#include <XGame.h>
#include <XSystem.h>
#include <XUser.h>

extern "C"
{

static_assert(XalUserState_SignedIn == static_cast<uint32_t>(XUserState::SignedIn), "Xal / GDK mismatch");
static_assert(XalUserState_SigningOut == static_cast<uint32_t>(XUserState::SigningOut), "Xal / GDK mismatch");
static_assert(XalUserState_SignedOut == static_cast<uint32_t>(XUserState::SignedOut), "Xal / GDK mismatch");

static_assert(XalGamerPictureSize_Small == static_cast<uint32_t>(XUserGamerPictureSize::Small), "Xal / GDK mismatch");
static_assert(XalGamerPictureSize_Medium == static_cast<uint32_t>(XUserGamerPictureSize::Medium), "Xal / GDK mismatch");
static_assert(XalGamerPictureSize_Large == static_cast<uint32_t>(XUserGamerPictureSize::Large), "Xal / GDK mismatch");
static_assert(XalGamerPictureSize_ExtraLarge == static_cast<uint32_t>(XUserGamerPictureSize::ExtraLarge), "Xal / GDK mismatch");

static_assert(XalAgeGroup_Unknown == static_cast<uint32_t>(XUserAgeGroup::Unknown), "Xal / GDK mismatch");
static_assert(XalAgeGroup_Child == static_cast<uint32_t>(XUserAgeGroup::Child), "Xal / GDK mismatch");
static_assert(XalAgeGroup_Teen == static_cast<uint32_t>(XUserAgeGroup::Teen), "Xal / GDK mismatch");
static_assert(XalAgeGroup_Adult == static_cast<uint32_t>(XUserAgeGroup::Adult), "Xal / GDK mismatch");

static_assert(XalPrivilegeCheckDenyReasons_None == static_cast<uint32_t>(XUserPrivilegeDenyReason::None), "Xal / GDK mismatch");
static_assert(XalPrivilegeCheckDenyReasons_PurchaseRequired == static_cast<uint32_t>(XUserPrivilegeDenyReason::PurchaseRequired), "Xal / GDK mismatch");
static_assert(XalPrivilegeCheckDenyReasons_Restricted == static_cast<uint32_t>(XUserPrivilegeDenyReason::Restricted), "Xal / GDK mismatch");
static_assert(XalPrivilegeCheckDenyReasons_Banned == static_cast<uint32_t>(XUserPrivilegeDenyReason::Banned), "Xal / GDK mismatch");
static_assert(XalPrivilegeCheckDenyReasons_Unknown == static_cast<uint32_t>(XUserPrivilegeDenyReason::Unknown), "Xal / GDK mismatch");

//------------------------------------------------------------------------------
// xal.h
//------------------------------------------------------------------------------

inline
HRESULT XalInitialize(
    _In_ XalInitArgs const* /*args*/,
    _In_opt_ XTaskQueueHandle /*internalWorkQueue*/
) noexcept
{
    return S_OK;
}

inline
HRESULT XalCleanupAsync(
    _In_ XAsyncBlock* async
) noexcept
{
    HRESULT hr = XAsyncBegin(async, nullptr, &XalCleanupAsync, "XalCleanupAsync", [](
        _In_ XAsyncOp op,
        _In_ XAsyncProviderData const* data
    )
    {
        if (op == XAsyncOp::DoWork)
        {
            XAsyncComplete(data->async, S_OK, 0);
        }
        return S_OK;
    });

    if (SUCCEEDED(hr))
    {
        hr = XAsyncSchedule(async, 0);
    }
    return hr;
}

inline
HRESULT XalCleanupResult(
    _In_ XAsyncBlock* async
) noexcept
{
    return XAsyncGetStatus(async, false);
}

inline
HRESULT XalGetMaxUsers(
    _Out_ uint32_t* maxUsers
) noexcept
{
    return XUserGetMaxUsers(maxUsers);
}

inline
HRESULT XalGetTitleId(
    _Out_ uint32_t* titleId
) noexcept
{
    return XGameGetXboxTitleId(titleId);
}

inline
size_t XalGetSandboxSize() noexcept
{
    return XSystemXboxLiveSandboxIdMaxBytes;
}

inline
HRESULT XalGetSandbox(
    _In_ size_t sandboxSize,
    _Out_writes_(sandboxSize) char* sandbox,
    _Out_opt_ size_t* sandboxUsed
) noexcept
{
    return XSystemGetXboxLiveSandboxId(sandboxSize, sandbox, sandboxUsed);
}

inline
HRESULT XalTryAddDefaultUserSilentlyAsync(
    _In_ uint32_t /*userIdentifier*/, // user identifier is not used on GameCore
    _In_ XAsyncBlock* async
) noexcept
{
    return XUserAddAsync(XUserAddOptions::AddDefaultUserSilently, async);
}

inline
HRESULT XalTryAddDefaultUserSilentlyResult(
    _In_ XAsyncBlock* async,
    _Out_ XalUserHandle* newUser
) noexcept
{
    return XUserAddResult(async, newUser);
}

inline
HRESULT XalAddUserWithUiAsync(
    _In_ uint32_t /*userIdentifier*/, // user identifier is not used on GameCore
    _In_ XAsyncBlock* async
) noexcept
{
    return XUserAddAsync(XUserAddOptions::None, async);
}

inline
HRESULT XalAddUserWithUiResult(
    _In_ XAsyncBlock* async,
    _Out_ XalUserHandle* newUser
) noexcept
{
    return XUserAddResult(async, newUser);
}

inline
bool XalGetDeviceUserIsPresent() noexcept
{
    return false;
}

inline
HRESULT XalGetDeviceUser(
    _Out_ XalUserHandle* /*deviceUser*/
) noexcept
{
    return E_NOT_SUPPORTED;
}

inline
bool XalSignOutUserAsyncIsPresent() noexcept
{
    return false;
}

inline
HRESULT XalSignOutUserAsync(
    _In_ XalUserHandle /*user*/,
    _In_ XAsyncBlock* /*async*/
) noexcept
{
    return E_NOT_SUPPORTED;
}

inline
HRESULT XalSignOutUserResult(
    _In_ XAsyncBlock* /*async*/
) noexcept
{
    return E_NOT_SUPPORTED;
}

inline
HRESULT XalFindUserByLocalId(
    _In_ XalUserLocalId localId,
    _Out_ XalUserHandle* user
) noexcept
{
    return XUserFindUserByLocalId(localId, user);
}

//------------------------------------------------------------------------------
// xal_user.h
//------------------------------------------------------------------------------

inline
HRESULT XalUserDuplicateHandle(
    _In_ XalUserHandle user,
    _Out_ XalUserHandle* duplicatedUser
) noexcept
{
    return XUserDuplicateHandle(user, duplicatedUser);
}

inline
void XalUserCloseHandle(
    _In_ XalUserHandle user
) noexcept
{
    XUserCloseHandle(user);
}

inline
int32_t XalCompareUsers(
    _In_ XalUserHandle user1,
    _In_ XalUserHandle user2
) noexcept
{
    return XUserCompare(user1, user2);
}

inline
HRESULT XalUserGetId(
    _In_ XalUserHandle user,
    _Out_ uint64_t* id
) noexcept
{
    return XUserGetId(user, id);
}

inline
HRESULT XalUserGetLocalId(
    _In_ XalUserHandle user,
    _Out_ XalUserLocalId* localId
) noexcept
{
    HRESULT hr = XUserGetLocalId(user, localId);
    return hr;
}

inline
bool XalUserIsDevice(
    _In_ XalUserHandle /*user*/
) noexcept
{
    return false;
}

inline
bool XalUserIsGuest(
    _In_ XalUserHandle user
) noexcept
{
    bool isGuest = false;
    XUserGetIsGuest(user, &isGuest);
    return isGuest;
}

inline
HRESULT XalUserGetState(
    _In_ XalUserHandle user,
    _Out_ XalUserState* state
) noexcept
{
    HRESULT hr = XUserGetState(user, reinterpret_cast<XUserState*>(state));
    return hr;
}

inline
size_t XalUserGetGamertagSize(
    _In_ XalUserHandle /*user*/,
    _In_ XalGamertagComponent component
) noexcept
{
    switch (component)
    {
        case XalGamertagComponent_Classic:
            return XUserGamertagComponentClassicMaxBytes;
        case XalGamertagComponent_Modern:
            return XUserGamertagComponentModernMaxBytes;
        case XalGamertagComponent_ModernSuffix:
            return XUserGamertagComponentModernSuffixMaxBytes;
        case XalGamertagComponent_UniqueModern:
            return XUserGamertagComponentUniqueModernMaxBytes;
        default:
            return 0;
    }
}

inline
HRESULT XalUserGetGamertag(
    _In_ XalUserHandle user,
    _In_ XalGamertagComponent component,
    _In_ size_t gamertagSize,
    _Out_writes_(gamertagSize) char* gamertag,
    _Out_opt_ size_t* gamertagUsed
) noexcept
{
    return XUserGetGamertag(user, static_cast<XUserGamertagComponent>(component), gamertagSize, gamertag, gamertagUsed);
}

inline
HRESULT XalUserGetGamerPictureAsync(
    _In_ XalUserHandle user,
    _In_ XalGamerPictureSize pictureSize,
    _In_ XAsyncBlock* async
) noexcept
{
    return XUserGetGamerPictureAsync(user, static_cast<XUserGamerPictureSize>(pictureSize), async);
}

inline
HRESULT XalUserGetGamerPictureResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* bufferSize
) noexcept
{
    return XUserGetGamerPictureResultSize(async, bufferSize);
}

inline
HRESULT XalUserGetGamerPictureResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_(bufferSize) void* buffer
) noexcept
{
    return XUserGetGamerPictureResult(async, bufferSize, buffer, nullptr);
}

inline
HRESULT XalUserGetAgeGroup(
    _In_ XalUserHandle user,
    _Out_ XalAgeGroup* ageGroup
) noexcept
{
    HRESULT hr = XUserGetAgeGroup(user, reinterpret_cast<XUserAgeGroup*>(ageGroup));
    return hr;
}

inline
HRESULT XalUserCheckPrivilege(
    _In_ XalUserHandle user,
    _In_ XalPrivilege privilege,
    _Out_ bool* hasPrivilege,
    _Out_opt_ XalPrivilegeCheckDenyReasons* reasons
) noexcept
{
    HRESULT hr = XUserCheckPrivilege(
        user,
        XUserPrivilegeOptions::None,
        static_cast<XUserPrivilege>(privilege),
        hasPrivilege,
        reinterpret_cast<XUserPrivilegeDenyReason*>(reasons)
    );

    return hr;
}

inline
bool XalUserResolvePrivilegeWithUiIsPresent() noexcept
{
    return true;
}

inline
HRESULT XalUserResolveUserPrivilegeWithUiAsync(
    _In_ XalUserHandle user,
    _In_ XalPrivilege privilege,
    _In_ XAsyncBlock* async
) noexcept
{
    return XUserResolvePrivilegeWithUiAsync(
        user,
        XUserPrivilegeOptions::None,
        static_cast<XUserPrivilege>(privilege),
        async
    );
}

inline
HRESULT XalUserResolveUserPrivilegeWithUiResult(
    _In_ XAsyncBlock* async
) noexcept
{
    return XUserResolvePrivilegeWithUiResult(async);
}

inline
HRESULT XalUserGetTokenAndSignatureSilentlyAsync(
    _In_ XalUserHandle user,
    _In_ XalUserGetTokenAndSignatureArgs const* args,
    _In_ XAsyncBlock* async
) noexcept
{
    static_assert(sizeof(XalHttpHeader) == sizeof(XUserGetTokenAndSignatureHttpHeader), "Xal / GDK mismatch");

    XUserGetTokenAndSignatureOptions opts = XUserGetTokenAndSignatureOptions::None;
    if (args->forceRefresh)
    {
        opts |= XUserGetTokenAndSignatureOptions::ForceRefresh;
    }
    if (args->allUsers)
    {
        opts |= XUserGetTokenAndSignatureOptions::AllUsers;
    }

    return XUserGetTokenAndSignatureAsync(
        user,
        opts,
        args->method,
        args->url,
        args->headerCount,
        reinterpret_cast<const XUserGetTokenAndSignatureHttpHeader*>(args->headers),
        args->bodySize,
        args->body,
        async
    );
}

inline
HRESULT XalUserGetTokenAndSignatureSilentlyResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* bufferSize
) noexcept
{
    return XUserGetTokenAndSignatureResultSize(async, bufferSize);
}

inline
HRESULT XalUserGetTokenAndSignatureSilentlyResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XalUserGetTokenAndSignatureData** result,
    _Out_opt_ size_t* bufferUsed
) noexcept
{
    static_assert(sizeof(XalUserGetTokenAndSignatureData) == sizeof(XUserGetTokenAndSignatureData), "Xal / GDK mismatch");

    return XUserGetTokenAndSignatureResult(
        async,
        bufferSize,
        buffer,
        reinterpret_cast<XUserGetTokenAndSignatureData**>(result),
        bufferUsed
    );
}

inline
HRESULT XalUserResolveIssueWithUiAsync(
    _In_ XalUserHandle user,
    _In_opt_z_ char const* url,
    _In_ XAsyncBlock* async
) noexcept
{
    return XUserResolveIssueWithUiAsync(user, url, async);
}

inline
HRESULT XalUserResolveIssueWithUiResult(
    _In_ XAsyncBlock* async
) noexcept
{
    return XUserResolveIssueWithUiResult(async);
}

inline
HRESULT XalUserRegisterChangeEventHandler(
    _In_opt_ XTaskQueueHandle queue,
    _In_opt_ void* context,
    _In_ XalUserChangeEventHandler* handler,
    _Out_ XalRegistrationToken* token
) noexcept
{
    static_assert(sizeof(XalRegistrationToken) == sizeof(XTaskQueueRegistrationToken), "Xal / GDK mismatch");
    static_assert(sizeof(XalRegistrationToken::token) == sizeof(XTaskQueueRegistrationToken::token), "Xal / GDK mismatch");

    return XUserRegisterForChangeEvent(
        queue,
        context,
        handler,
        reinterpret_cast<XTaskQueueRegistrationToken*>(token)
    );
}

inline
void XalUserUnregisterChangeEventHandler(
    _In_ XalRegistrationToken token
) noexcept
{
    XTaskQueueRegistrationToken gdkToken{ token.token };
    XUserUnregisterForChangeEvent(gdkToken, true);
}

inline
HRESULT XalUserGetSignoutDeferral(
    _Out_ XalSignoutDeferralHandle* deferral
) noexcept
{
    return XUserGetSignOutDeferral(deferral);
}

inline
void XalUserCloseSignoutDeferral(
    _In_ XalSignoutDeferralHandle deferral
) noexcept
{
    XUserCloseSignOutDeferralHandle(deferral);
}

}
