// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "user.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

User::User(XblUserHandle userHandle) noexcept
{
    auto hr = XalUserDuplicateHandle(userHandle, &m_handle);
    if (FAILED(hr))
    {
        m_handle = nullptr;
        m_constructorHR = hr;
        LOGS_ERROR << "User being used is currently unavailable. Probably because the title is still in suspended mode. Try again later";
    }
}

User::User(const User& other) noexcept
{
    auto hr = XalUserDuplicateHandle(other.m_handle, &m_handle);
    if (FAILED(hr))
    {
        m_handle = nullptr;
        m_constructorHR = hr;
        LOGS_ERROR << "User being used is currently unavailable. Probably because the title is still in suspended mode. Try again later";
    }
}

User::User(User&& other) noexcept
    : m_handle{ other.m_handle }
{
    other.m_handle = nullptr;
    m_constructorHR = other.m_constructorHR;
}

User& User::operator=(User other) noexcept
{
    std::swap(other.m_handle, m_handle);
    m_constructorHR = other.m_constructorHR;
    return *this;
}

User::~User() noexcept
{
    if (m_handle)
    {
        XalUserCloseHandle(m_handle);
    }
}

uint64_t User::Xuid() const noexcept
{

    uint64_t xuid{ 0 };
    if (m_handle != nullptr) 
    {
        auto hr = XalUserGetId(m_handle, &xuid);
        UNREFERENCED_PARAMETER(hr);
    }

    return xuid;
}

uint64_t User::LocalId() const noexcept
{
    XalUserLocalId localId{ 0 };
    if (m_handle != nullptr)
    {
        auto hr = XalUserGetLocalId(m_handle, &localId);
        UNREFERENCED_PARAMETER(hr);
    }
    return localId.value;
}

xsapi_internal_string User::Gamertag() const noexcept
{
    return GetGamertagComponent(XalGamertagComponent_Classic);
}

xsapi_internal_string User::ModernGamertag() const noexcept
{
    return GetGamertagComponent(XalGamertagComponent_Modern);
}

xsapi_internal_string User::ModernGamertagSuffix() const noexcept
{
    return GetGamertagComponent(XalGamertagComponent_ModernSuffix);
}

xsapi_internal_string User::UniqueModernGamertag() const noexcept
{
    return GetGamertagComponent(XalGamertagComponent_UniqueModern);
}

HRESULT User::GetTokenAndSignature(
    const xsapi_internal_string& httpMethod,
    const xsapi_internal_string& url,
    const HttpHeaders& headers,
    const uint8_t* requestBody,
    size_t requestBodySize,
    bool allUsers,
    AsyncContext<Result<TokenAndSignature>> async
) noexcept
{
    if (!m_handle)
    {
        return m_constructorHR;
    }

    bool forceRefresh{ false };

    auto state{ GlobalState::Get() };
    if (state)
    {
        if (state->EraseUserExpiredToken(Xuid()) > 0)
        {
            forceRefresh = true;
        }
    }

    XalUserGetTokenAndSignatureArgs tokenAndSigArgs{
        httpMethod.data(),
        url.data(),
        static_cast<uint32_t>(headers.size()),
        nullptr,
        requestBodySize,
        (requestBodySize == 0) ? nullptr : requestBody, // XUser requires nullptr body if body is 0 size
        forceRefresh,
        allUsers
    };

    xsapi_internal_vector<XalHttpHeader> xalHttpHeaders{};
    if (headers.size() > 0)
    {
        xalHttpHeaders.reserve(tokenAndSigArgs.headerCount);
        for (const auto& header : headers)
        {
            xalHttpHeaders.push_back(XalHttpHeader{ header.first.data(), header.second.data() });
        }

        tokenAndSigArgs.headers = xalHttpHeaders.data();
    }

    auto asyncBlock{ Make<XAsyncBlock>() };

    asyncBlock->queue = async.Queue().GetHandle();
    asyncBlock->context = Make<AsyncContext<Result<TokenAndSignature>>>(std::move(async));
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        auto async{ static_cast<AsyncContext<Result<TokenAndSignature>>*>(asyncBlock->context) };

        size_t bufferSize{ 0 };
        HRESULT hr = XalUserGetTokenAndSignatureSilentlyResultSize(asyncBlock, &bufferSize);
        TokenAndSignature payload;

        if (SUCCEEDED(hr))
        {
            auto buffer{ MakeArray<uint8_t>(bufferSize) };
            XalUserGetTokenAndSignatureData* xalTokenSignatureData{ nullptr };

            hr = XalUserGetTokenAndSignatureSilentlyResult(asyncBlock, bufferSize, buffer, &xalTokenSignatureData, nullptr);
            assert(SUCCEEDED(hr));

            payload.token = xsapi_internal_string{ xalTokenSignatureData->token, xalTokenSignatureData->tokenSize };
            payload.signature = xsapi_internal_string{ xalTokenSignatureData->signature, xalTokenSignatureData->signatureSize };

            DeleteArray(buffer, bufferSize);
        }
        else if (hr == E_XAL_NOTOKENREQUIRED)
        {
            // Consider this a success
            hr = S_OK;
        }

        async->Complete(Result<TokenAndSignature>{ payload, hr });

        Delete(asyncBlock);
        Delete(async);
    };

    return XalUserGetTokenAndSignatureSilentlyAsync(
        m_handle,
        &tokenAndSigArgs,
        asyncBlock
    );
}

HRESULT User::GetConstructorHR() const noexcept
{
    return m_constructorHR;
}

XalUserHandle User::Handle() const noexcept
{
    return m_handle;
}

void User::SetTokenExpired(uint64_t xuid) noexcept
{
    auto state{ GlobalState::Get() };
    if (state)
    {
        state->InsertUserExpiredToken(xuid);
    }
    
}

Result<XblFunctionContext> User::RegisterChangeEventHandler(
    UserChangeEventHandler handler
) noexcept
{
    XalRegistrationToken token{};
    auto context{ MakeShared<UserChangeEventHandler>(std::move(handler)) };

    auto hr = XalUserRegisterChangeEventHandler(
        TaskQueue().GetHandle(),
        context.get(),
        [](void* context, UserLocalId userId, UserChangeType change)
        {
            auto handler{ static_cast<UserChangeEventHandler*>(context) };
            (*handler)(userId, static_cast<UserChangeType>(change));
        },
        &token
    );

    if (SUCCEEDED(hr))
    {
        auto state{ GlobalState::Get() };
        if (state)
        {
            state->SetUserChangeHandler(token.token, context);
        }
    }
    return Result<XblFunctionContext>{ static_cast<XblFunctionContext>(token.token), hr };
}

void User::UnregisterChangeEventHandle(
    XblFunctionContext token
) noexcept
{
    XalUserUnregisterChangeEventHandler(XalRegistrationToken{ static_cast<uint64_t>(token) });
    auto state{ GlobalState::Get() };
    if (state)
    {
        state->EraseUserChangeHandler(token);
    }
}

xsapi_internal_string User::GetGamertagComponent(
    XalGamertagComponent component
) const noexcept
{
    if (m_handle != nullptr)
    {
        size_t size = XalUserGetGamertagSize(m_handle, component);

        std::vector<char> gamertagComponent(size, char{});
        auto hr = XalUserGetGamertag(m_handle, component, size, &gamertagComponent[0], nullptr);
        if (FAILED(hr)) 
        {
            return "";
        }
        else
        {
            return gamertagComponent.data();
        }
    }
    else 
    {
        return "";
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
