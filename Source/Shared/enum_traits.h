// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#define DEFAULT_ENUM_MIN 0u
#define DEFAULT_ENUM_MAX 30u
#define ENUM_RANGE_MAX 1000u

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

namespace detail
{

template<size_t n>
class StaticString
{
public:
    DISABLE_WARNING_PUSH;
    SUPPRESS_WARNING_UNINITIALIZED_MEMBER;
    constexpr StaticString(const char* name) noexcept
        : StaticString{ name, std::make_index_sequence<n>{} }
    {
    }
    DISABLE_WARNING_POP;

    constexpr operator const char*() const noexcept
    {
        return chars;
    }

private:
    template<std::size_t... I>
    constexpr StaticString(const char* name, std::index_sequence<I...>) noexcept
        : chars{ name[I]..., 0 }
    {
    }

    const char chars[n + 1]{};
};

template<>
class StaticString<0>
{
public:
    constexpr StaticString(const char*) noexcept {};
    constexpr operator const char*() const noexcept
    {
        return nullptr;
    }
};

using StringView = std::pair<const char*, size_t>;

constexpr StringView ParseName(const char* funcName) noexcept
{
#if defined(_MSC_VER)
    constexpr auto delim{ '>' };
#elif defined(__clang__)
    constexpr auto delim{ ']' };
#endif
    size_t end{ 0 };
    for (; funcName[end] && funcName[end] != delim; ++end) {}

    size_t begin{ end };
    for (; begin > 0; --begin)
    {
        if (!((funcName[begin - 1] >= '0' && funcName[begin - 1] <= '9') ||
            (funcName[begin - 1] >= 'a' && funcName[begin - 1] <= 'z') ||
            (funcName[begin - 1] >= 'A' && funcName[begin - 1] <= 'Z') ||
            (funcName[begin - 1] == '_')))
        {
            break;
        }
    }

    // Symbol names cannot begin with a number
    if (funcName[begin] >= '0' && funcName[begin] <= '9')
    {
        // Invalid enum value
        return StringView{ nullptr, 0 };
    }

    return StringView{ funcName + begin, end - begin };
}

template<typename E, E V>
static constexpr auto N() noexcept
{
    static_assert(std::is_enum<E>::value, "E must be an enum type.");
#if defined(_MSC_VER)
    constexpr auto name = ParseName(__FUNCSIG__);
#elif defined(__clang__)
    constexpr auto name = ParseName(__PRETTY_FUNCTION__);
#else
    static_assert(false, "Unrecognized platform");
#endif
    return StaticString<name.second>{ name.first };
}

template<typename E, E V>
constexpr auto EnumValueName = N<E, V>();

template<typename E, uint32_t MIN_E, uint32_t MAX_E>
class EnumTraits
{
private:
    static_assert(std::is_enum<E>::value, "EnumTraits can only be instantiated for enum types.");
    static_assert(std::is_same<std::underlying_type_t<E>, uint32_t>::value, "EnumTraits can only be instantiated for enums with uint32_t as their underlying type");

    static constexpr uint32_t range = MAX_E - MIN_E + 1;
    static_assert(range < ENUM_RANGE_MAX, "EnumTraits only supports a maximum value range of ENUM_RANGE_MAX");

    template<std::size_t... I>
    static constexpr auto Names(std::index_sequence<I...>) noexcept
    {
        return std::array<const char*, sizeof...(I)>{ { EnumValueName<E, static_cast<E>(I + MIN_E)>... } };
    }
    static constexpr std::array<const char*, range> names = Names(std::make_index_sequence<range>());

public:
    static String Name(E v) noexcept
    {
#if HC_PLATFORM == HC_PLATFORM_ANDROID
        // This gets around the ODR-use rule on clang when compiling without C++17 support
        constexpr auto names_{ names };
#else 
        auto& names_{ names };
#endif
        auto i{ static_cast<size_t>(v) };
        if (i < MIN_E || i > MAX_E)
        {
            // Value outside the specified range. Return empty string.
            return String{};
        }
        return String{ names_[i - MIN_E] };
    }

    static E Value(const char* n) noexcept
    {
#if HC_PLATFORM == HC_PLATFORM_ANDROID
        constexpr auto names_{ names };
#else 
        auto& names_{ names };
#endif

        for (size_t i = 0; i < names_.size(); ++i)
        {
            if (names_[i] && xbox::services::legacy::Stricmp(n, names_[i]) == 0)
            {
                return static_cast<E>(i + MIN_E);
            }
        }
        // Provided string didn't map to an enum value. Return default value.
        return E{};
    }
};

}

template<typename E, size_t MIN_E = DEFAULT_ENUM_MIN, size_t MAX_E = DEFAULT_ENUM_MAX>
constexpr auto EnumName(E value) noexcept -> std::enable_if_t<std::is_enum<E>::value, String>
{
    return detail::EnumTraits<E, MIN_E, MAX_E>::Name(value);
}

template<typename E, size_t MIN_E = DEFAULT_ENUM_MIN, size_t MAX_E = DEFAULT_ENUM_MAX>
constexpr auto EnumValue(const char* name) noexcept -> std::enable_if_t<std::is_enum<E>::value, E>
{
    return detail::EnumTraits<E, MIN_E, MAX_E>::Value(name);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
