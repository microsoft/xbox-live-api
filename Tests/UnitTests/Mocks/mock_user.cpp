// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

struct XalUserImpl
{
    uint64_t const xuid;
    std::string const gamertag;
    std::string const modernGamertag;
    std::string const modernGamertagSuffix;
    std::string const uniqueModernGamertag;
    uint64_t const localId;
};

struct XalUser
{
    std::shared_ptr<XalUserImpl> userImpl;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

User CreateMockUser(
    uint64_t xuid,
    const std::string& gamertag,
    uint64_t localId
)
{
    auto xalUserImpl = std::shared_ptr<XalUserImpl>{ new XalUserImpl{ xuid, gamertag, "", "", "", localId } };
    auto xalUser = std::unique_ptr<XalUser>{ new XalUser{ xalUserImpl } };
    auto userResult = User::WrapHandle(xalUser.get());
    return userResult.ExtractPayload();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

STDAPI XalUserDuplicateHandle(
    _In_ XalUserHandle user,
    _Out_ XalUserHandle* duplicatedUser
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(user == nullptr || duplicatedUser == nullptr);
    *duplicatedUser = new XalUser{ user->userImpl };
    return S_OK;
}

STDAPI_(void) XalUserCloseHandle(
    _In_ XalUserHandle user
) noexcept
{
    delete user;
}

STDAPI XalUserGetId(
    _In_ XalUserHandle user,
    _Out_ uint64_t* id
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(user == nullptr || id == nullptr);
    *id = user->userImpl->xuid;
    return S_OK;
}

STDAPI XalUserGetLocalId(
    _In_ XalUserHandle user,
    _Out_ XalUserLocalId* localId
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(localId);
    *localId = XalUserLocalId{ user->userImpl->localId};
    return S_OK;
}

STDAPI_(size_t) XalUserGetGamertagSize(
    _In_ XalUserHandle user,
    _In_ XalGamertagComponent component
) noexcept
{
    switch (component)
    {
    case XalGamertagComponent_Classic:
    {
        return user->userImpl->gamertag.size() + 1;
    }
    case XalGamertagComponent_Modern:
    {
        return user->userImpl->modernGamertag.size() + 1;
    }
    case XalGamertagComponent_ModernSuffix:
    {
        return user->userImpl->modernGamertagSuffix.size() + 1;
    }
    case XalGamertagComponent_UniqueModern:
    {
        return user->userImpl->uniqueModernGamertag.size() + 1;
    }
    default:
    {
        assert(false);
        return 0;
    }
    }
}

STDAPI XalUserGetGamertag(
    _In_ XalUserHandle user,
    _In_ XalGamertagComponent component,
    _In_ size_t gamertagSize,
    _Out_writes_(gamertagSize) char* gamertag,
    _Out_opt_ size_t* gamertagUsed
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(user == nullptr || gamertag == nullptr);

    const std::string* gamertagComponent{ nullptr };
    switch (component)
    {
    case XalGamertagComponent_Classic:
    {
        gamertagComponent = &user->userImpl->gamertag;
        break;
    }
    case XalGamertagComponent_Modern:
    {
        gamertagComponent = &user->userImpl->modernGamertag;
        break;
    }
    case XalGamertagComponent_ModernSuffix:
    {
        gamertagComponent = &user->userImpl->modernGamertagSuffix;
        break;
    }
    case XalGamertagComponent_UniqueModern:
    {
        gamertagComponent = &user->userImpl->uniqueModernGamertag;
        break;
    }
    default:
    {
        assert(false);
        return E_UNEXPECTED;
    }
    }

    if (gamertagSize < gamertagComponent->size() + 1)
    {
        return E_INVALIDARG;
    }

    memcpy(gamertag, gamertagComponent->data(), gamertagComponent->size() + 1);

    if (gamertagUsed)
    {
        *gamertagUsed = gamertagComponent->size() + 1;
    }
    return S_OK;
}

STDAPI XalUserGetTokenAndSignatureSilentlyAsync(
    _In_ XalUserHandle user,
    _In_ XalUserGetTokenAndSignatureArgs const* args,
    _In_ XAsyncBlock* async
) noexcept
{
    UNREFERENCED_PARAMETER(user);
    UNREFERENCED_PARAMETER(args);

    static std::string token{ "MockToken" };
    static std::string signature{ "MockSignature" };

    return RunAsync(async, __FUNCTION__,
        [&](XAsyncOp op, const XAsyncProviderData* data)
        {
            switch (op)
            {
            case XAsyncOp::DoWork:
            {
                size_t resultSize{ sizeof(XalUserGetTokenAndSignatureData) };
                resultSize += token.size() + 1;
                resultSize += signature.size() + 1;
                XAsyncComplete(data->async, S_OK, resultSize);
                return S_OK;
            }
            case XAsyncOp::GetResult:
            {
                auto tokenBuffer = static_cast<char*>(data->buffer) + sizeof(XalUserGetTokenAndSignatureData);
                auto signatureBuffer = tokenBuffer + token.size() + 1;
                new (data->buffer) XalUserGetTokenAndSignatureData{ token.size() + 1, signature.size() + 1, tokenBuffer, signatureBuffer };

                memcpy(tokenBuffer, token.data(), token.size() + 1);
                memcpy(signatureBuffer, signature.data(), signature.size() + 1);
                return S_OK;
            }
            default:
            {
                return S_OK;
            }
            }
        });
}

STDAPI XalUserGetTokenAndSignatureSilentlyResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* bufferSize
) noexcept
{
    return XAsyncGetResultSize(async, bufferSize);
}

STDAPI XalUserGetTokenAndSignatureSilentlyResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XalUserGetTokenAndSignatureData** result,
    _Out_opt_ size_t* bufferUsed
) noexcept
{
    size_t sizeUsed{ 0 };
    HRESULT hr = XAsyncGetResult(async, nullptr, bufferSize, buffer, &sizeUsed);

    *result = static_cast<XalUserGetTokenAndSignatureData*>(buffer);
    if (bufferUsed)
    {
        *bufferUsed = sizeUsed;
    }

    return hr;
}

STDAPI XalUserRegisterChangeEventHandler(
    _In_ XTaskQueueHandle queue,
    _In_opt_ void* context,
    _In_ XalUserChangeEventHandler* handler,
    _Out_ XalRegistrationToken* token
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(token);

    UNREFERENCED_PARAMETER(queue);
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(handler);

    static uint64_t nextToken{ 1 };
    token->token = nextToken++;

    return S_OK;
}

STDAPI_(void) XalUserUnregisterChangeEventHandler(
    _In_ XalRegistrationToken token
) noexcept
{
    UNREFERENCED_PARAMETER(token);
}
