// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "user.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

User::User(XblUserHandle userHandle) noexcept
    : m_handle(userHandle)
{}

User::User(User&& other) noexcept
    : m_handle{ other.m_handle }, m_xuid{ other.m_xuid }, m_localId { std::move(other.m_localId) }
{
    Map<XalGamertagComponent, String>::iterator it = other.m_gamertags.begin();

    while(it != other.m_gamertags.end())
    {
        m_gamertags[it->first] = std::move(it->second);
        it++;
    }

    other.m_gamertags.clear();
    other.m_handle = nullptr;
}

User& User::operator=(User&& other) noexcept
{
    std::swap(other.m_handle, m_handle);

    m_localId = std::move(other.m_localId);
    m_xuid = other.m_xuid;

    Map<XalGamertagComponent, String>::iterator it = other.m_gamertags.begin();

    while (it != other.m_gamertags.end())
    {
        m_gamertags[it->first] = std::move(it->second);
        it++;
    }

    other.m_gamertags.clear();
    return *this;
}

User::~User() noexcept
{
    if (m_handle)
    {
        XalUserCloseHandle(m_handle);
    }
}

/*static*/ Result<User> User::WrapHandle(XblUserHandle userHandle) noexcept
{
    if (userHandle == nullptr)
    {
        return Result<User>{ User(nullptr), E_INVALIDARG };
    }

    XalUserHandle copiedHandle;
    auto hr = XalUserDuplicateHandle(userHandle, &copiedHandle);
    if (FAILED(hr))
    {
        LOGS_ERROR << "Copying user failed: User failed to duplicate.";
        return Result<User>{ User(nullptr), hr };
    }
    else
    {
        User user{ copiedHandle };
        hr = user.InitializeUser();

        if (FAILED(hr))
        {
            LOGS_ERROR << "Copying user failed: User failed to duplicate.";
            return Result<User>{ User(nullptr), hr };
        }

        return Result<User>{ std::move(user) , S_OK };
    }

}

HRESULT User::InitializeUser() noexcept
{
    auto hr = XalUserGetId(m_handle, &m_xuid);
    if (FAILED(hr))
    {
        LOGS_ERROR << "Failed to get User ID with HRESULT: " << hr;
        return hr;
    }

    hr = XalUserGetLocalId(m_handle, &m_localId);
    if (FAILED(hr))
    {
        LOGS_ERROR << "Failed to get User LocalID with HRESULT: " << hr;
        return hr;
    }

    auto gamertagComponentResult = GetGamertagComponent(XalGamertagComponent_Classic);
    if (FAILED(gamertagComponentResult.Hresult()))
    {
        LOGS_ERROR << "Failed to get Gamertag Component" << XalGamertagComponent_Classic << " with HRESULT: " << hr;
        return hr;
    }

    m_gamertags[XalGamertagComponent_Classic] = gamertagComponentResult.ExtractPayload();

    gamertagComponentResult = GetGamertagComponent(XalGamertagComponent_Modern);
    if (FAILED(gamertagComponentResult.Hresult()))
    {
        LOGS_ERROR << "Failed to get Gamertag Component" << XalGamertagComponent_Modern << " with HRESULT: " << hr;
        return hr;
    }

    m_gamertags[XalGamertagComponent_Modern] = gamertagComponentResult.ExtractPayload();

    gamertagComponentResult = GetGamertagComponent(XalGamertagComponent_ModernSuffix);
    if (FAILED(gamertagComponentResult.Hresult()))
    {
        LOGS_ERROR << "Failed to get Gamertag Component" << XalGamertagComponent_ModernSuffix << " with HRESULT: " << hr;
        return hr;
    }

    m_gamertags[XalGamertagComponent_ModernSuffix] = gamertagComponentResult.ExtractPayload();

    gamertagComponentResult = GetGamertagComponent(XalGamertagComponent_UniqueModern);
    if (FAILED(gamertagComponentResult.Hresult()))
    {
        LOGS_ERROR << "Failed to get Gamertag Component" << XalGamertagComponent_UniqueModern << " with HRESULT: " << hr;
        return hr;
    }

    m_gamertags[XalGamertagComponent_UniqueModern] = gamertagComponentResult.ExtractPayload();

    return S_OK;
}

Result<User> User::Copy() const noexcept
{
    XalUserHandle copiedHandle;
    auto hr = XalUserDuplicateHandle(this->m_handle, &copiedHandle);
    if (FAILED(hr))
    {
        LOGS_ERROR << "Copying user failed: User failed to duplicate.";
        return Result<User>{ User(nullptr), hr};
    }
    else
    {
        User copiedUser{ copiedHandle };
        hr = copiedUser.InitializeUser();
        return Result<User>{std::move(copiedUser), hr};
    }
}

uint64_t User::Xuid() const noexcept
{
    return m_xuid;
}

uint64_t User::LocalId() const noexcept
{
    XalUserLocalId localId{ 0 };
    if (m_handle != nullptr)
    {
        auto hr = XalUserGetLocalId(m_handle, &localId);
        if (SUCCEEDED(hr))
        {
            m_localId = localId;
        }
    }

    return m_localId.value;
}

xsapi_internal_string User::Gamertag() const noexcept
{
    auto result = GetGamertagComponent(XalGamertagComponent_Classic);
    if (Failed(result))
    {
        return m_gamertags[XalGamertagComponent_Classic];
    }
    else
    {
        return result.ExtractPayload();
    }
}

xsapi_internal_string User::ModernGamertag() const noexcept
{
    auto result = GetGamertagComponent(XalGamertagComponent_Modern);
    if (Failed(result))
    {
        return m_gamertags[XalGamertagComponent_Modern];
    }
    else
    {
        return result.ExtractPayload();
    }
}

xsapi_internal_string User::ModernGamertagSuffix() const noexcept
{
    auto result = GetGamertagComponent(XalGamertagComponent_ModernSuffix);
    if (Failed(result))
    {
        return m_gamertags[XalGamertagComponent_ModernSuffix];
    }
    else
    {
        return result.ExtractPayload();
    }
}

xsapi_internal_string User::UniqueModernGamertag() const noexcept
{
    auto result = GetGamertagComponent(XalGamertagComponent_UniqueModern);
    if (Failed(result))
    {
        return m_gamertags[XalGamertagComponent_UniqueModern];
    }
    else
    {
        return result.ExtractPayload();
    }
}

HRESULT User::GetTokenAndSignature(
    const String& httpMethod,
    const String& url,
    const HttpHeaders& headers,
    const uint8_t* requestBody,
    size_t requestBodySize,
    bool allUsers,
    AsyncContext<Result<TokenAndSignature>>&& async
) noexcept
{
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

    Vector<XalHttpHeader> xalHttpHeaders{};
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
            if (SUCCEEDED(hr))
            {
                payload.token = String{ xalTokenSignatureData->token, xalTokenSignatureData->tokenSize };
                payload.signature = String{ xalTokenSignatureData->signature, xalTokenSignatureData->signatureSize };
            }

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
            (*handler)(std::move(userId), std::move(static_cast<UserChangeType>(change)));
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

Result<String> User::GetGamertagComponent(
    XalGamertagComponent component
) const noexcept
{
    if (m_handle != nullptr)
    {
        size_t size = XalUserGetGamertagSize(m_handle, component);

        std::vector<char> gamertagComponent(size, char{});
        auto hr = XalUserGetGamertag(m_handle, component, size, &gamertagComponent[0], nullptr);
        if (SUCCEEDED(hr))
        {
            m_gamertags[component] = gamertagComponent.data();
        }
        else 
        {
            LOGS_ERROR << "Getting Gamertag failed with HR: "<< hr ;
        }

        return Result<String>{m_gamertags[component], hr };
    }

    return  E_UNEXPECTED;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
