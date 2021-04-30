// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

struct XblHttpCall;

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

class User;

template<>
class Result<User>;

// RAII wrapper around XalUser
class User
{
public:
    User(const User& other) = delete;
    User(User&& other) noexcept;
    User& operator=(User&& other) noexcept;
    ~User() noexcept;

    static Result<User> WrapHandle(XblUserHandle userHandle) noexcept;
    Result<User> Copy() const noexcept;

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
        AsyncContext<Result<TokenAndSignature>>&& async
    ) noexcept;

    XalUserHandle Handle() const noexcept;

    static void SetTokenExpired(uint64_t xuid) noexcept;

    static Result<uint64_t> RegisterChangeEventHandler(
        UserChangeEventHandler handler
    ) noexcept;

    static void UnregisterChangeEventHandle(
        uint64_t token
    ) noexcept;

private:
    User() noexcept = default;
    User(XblUserHandle userHandle) noexcept;

    HRESULT InitializeUser() noexcept;
    Result<String> GetGamertagComponent(XalGamertagComponent component) const noexcept;	
    XblUserHandle m_handle{ nullptr };
    mutable uint64_t m_xuid;
    mutable XalUserLocalId m_localId;
    mutable Map<XalGamertagComponent, String> m_gamertags;

    friend class Result<User>;
};

template<>
class Result<User>
{
public:
    Result(User&& user) : m_payload{ std::move(user) } {}
    Result(User&& user, HRESULT error) : m_result{ error }, m_payload{ std::move(user) } {}
    Result(Result&& other) = default;
    Result(const Result& other) = delete;

    HRESULT Hresult() const noexcept
    {
        return m_result;
    }

    const User& Payload() const noexcept
    {
        return m_payload;
    }

    User&& ExtractPayload() noexcept
    {
        return std::move(m_payload);
    }

private:
    HRESULT m_result;
    User m_payload;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
