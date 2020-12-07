// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include <cassert>
#include <exception>
#include <string>
#include "xsapi_utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

template<typename T>
class Result
{
public:
    Result() = default;
    Result(const Result&) = default;
    Result(Result&& other) = default;
    ~Result() = default;

    Result& operator=(const Result& rhs) = default;

    Result(HRESULT hr, String errorMessage = {}) : m_result{ hr }, m_errorMessage{ std::move(errorMessage) } {}

    Result(T payload, HRESULT hr = S_OK, String errorMessage = {}) : m_result{ hr }, m_payload{ payload }, m_errorMessage{ std::move(errorMessage) } {}

    Result(std::error_code errc, String errorMessage = {}) : m_errorMessage{ std::move(errorMessage) }
    {
        m_result = utils::convert_xbox_live_error_code_to_hresult(errc);
    }

    Result(T payload, std::error_code errc, String errorMessage = {}) : m_payload{ payload }, m_errorMessage{ std::move(errorMessage) }
    {
        m_result = utils::convert_xbox_live_error_code_to_hresult(errc);
    }

    HRESULT Hresult() const noexcept
    {
        return m_result;
    }

    const char* ErrorMessage() const noexcept
    {
        return m_errorMessage.c_str();
    }

    T& Payload() noexcept
    {
        return m_payload;
    }

    const T& Payload() const noexcept
    {
        return m_payload;
    }

    T&& ExtractPayload()
    {
        return std::move(m_payload);
    }

private:
    HRESULT m_result{ S_OK };
    T m_payload{};
    String m_errorMessage{};
};

template<>
class Result<void>
{
public:
    Result() = default;
    Result(const Result<void>&) = default;

    template<typename T>
    Result(const Result<T>& other) : m_result{ other.Hresult() }, m_errorMessage{ other.ErrorMessage() } {}

    Result(HRESULT hr, String errorMessage = {}) : m_result{ hr }, m_errorMessage{ std::move(errorMessage) } {}

    Result(std::error_code errc, String errorMessage = {}) : m_errorMessage{ std::move(errorMessage) }
    {
        m_result = utils::convert_xbox_live_error_code_to_hresult(errc);
    }

    HRESULT Hresult() const noexcept
    {
        return m_result;
    }

    const char* ErrorMessage() const noexcept
    {
        return m_errorMessage.c_str();
    }

private:
    HRESULT m_result{ S_OK };
    String m_errorMessage{};
};

template<typename T>
bool Succeeded(const Result<T>& result)
{
    return SUCCEEDED(result.Hresult());
}

template<typename T>
bool Failed(const Result<T>& result)
{
    return FAILED(result.Hresult());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END