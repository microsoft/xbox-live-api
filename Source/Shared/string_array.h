// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

// RAII class wrapping an array C-Strings
class UTF8StringArray
{
public:
    UTF8StringArray(const xsapi_internal_vector<xsapi_internal_string>& vector)
    {
        std::transform(vector.begin(), vector.end(), std::back_inserter(m_strings), 
            [](const xsapi_internal_string& in)
            {
                return Make(in);
            });
    }

    UTF8StringArray(const UTF8StringArray& other)
    {
        std::transform(other.m_strings.begin(), other.m_strings.end(), std::back_inserter(m_strings),
            [](const char* in)
            {
                return Make(in);
            });
    }

    UTF8StringArray(UTF8StringArray&& other) noexcept
        : m_strings{ std::move(other.m_strings) }
    {
    }

    UTF8StringArray& operator=(UTF8StringArray other)
    {
        std::swap(other.m_strings, m_strings);
        return *this;
    }

    ~UTF8StringArray() noexcept
    {
        for (auto string : m_strings)
        {
            Delete(string);
        }
    }

    const char** Data() noexcept
    {
        return m_strings.data();
    }

    size_t Size() const noexcept
    {
        return m_strings.size();
    }

private:
    xsapi_internal_vector<const char*> m_strings;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END