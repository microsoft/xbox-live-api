// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

using HttpHeaders = Map<String, String>;

struct TokenAndSignature
{
    String token;
    String signature;
};

#if HC_PLATFORM == HC_PLATFORM_GDK
typedef XUserLocalId UserLocalId;
typedef XUserChangeEvent UserChangeType;
#else
typedef XalUserLocalId UserLocalId;
typedef XalUserChangeType UserChangeType;
#endif

using UserChangeEventHandler = Function<void(UserLocalId /*localId*/, UserChangeType /*changeType*/)>;

// RAII wrapper around XalUser
class User
{
public:
    User(XblUserHandle userHandle) noexcept;
    User(const User& other) noexcept;
    User(User&& other) noexcept;
    User& operator=(User other) noexcept;
    ~User() noexcept;

    uint64_t Xuid() const noexcept;
    uint64_t LocalId() const noexcept;
    String Gamertag() const noexcept;
    String ModernGamertag() const noexcept;
    String ModernGamertagSuffix() const noexcept;
    String UniqueModernGamertag() const noexcept;

    HRESULT GetTokenAndSignature(
        const String& httpMethod,
        const String& url,
        const HttpHeaders& headers,
        const uint8_t* requestBody,
        size_t requestBodySize,
        bool allUsers,
        AsyncContext<Result<TokenAndSignature>> async
    ) noexcept;

    HRESULT GetConstructorHR() const noexcept;

    XalUserHandle Handle() const noexcept;

    static void SetTokenExpired(uint64_t xuid) noexcept;

    static Result<XblFunctionContext> RegisterChangeEventHandler(
        UserChangeEventHandler handler
    ) noexcept;

    static void UnregisterChangeEventHandle(
        XblFunctionContext token
    ) noexcept;

private:
    String GetGamertagComponent(XalGamertagComponent component) const noexcept;
    HRESULT m_constructorHR = S_OK;
    XblUserHandle m_handle{ nullptr };
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
