/***
* ==++==
*
* Copyright (c) Microsoft Corporation. All rights reserved.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* Protocol independent support for URIs.
*
* For the latest on this and related APIs, please see: https://github.com/Microsoft/cpprestsdk
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

#pragma once

using namespace xbox::services::cppresturi::utility::conversions;

#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( push )
#pragma warning( disable : 26444 ) // ignore various unnamed objects
#pragma warning( disable : 26812 )  // enum instead of enum class
#endif

namespace xbox { namespace services { namespace cppresturi {

namespace web { namespace details
{
xsapi_internal_string uri_components::join()
{
    // canonicalize components first

    // convert scheme to lowercase
    std::transform(m_scheme.begin(), m_scheme.end(), m_scheme.begin(), [](char c) {
        return (char)tolower(c);
    });

    // convert host to lowercase
    std::transform(m_host.begin(), m_host.end(), m_host.begin(), [](char c) {
        return (char)tolower(c);
    });

    // canonicalize the path to have a leading slash if it's a full uri
    if (!m_host.empty() && m_path.empty())
    {
        m_path = "/";
    }
    else if (!m_host.empty() && m_path[0] != '/')
    {
        m_path.insert(m_path.begin(), 1, '/');
    }

    xsapi_internal_string ret;

#if (defined(_MSC_VER) && (_MSC_VER >= 1800))
    if (!m_scheme.empty())
    {
        ret.append(m_scheme).append({ ':' });
    }

    if (!m_host.empty())
    {
        ret.append("//");

        if (!m_user_info.empty())
        {
            ret.append(m_user_info).append({ '@' });
        }

        ret.append(m_host);

        if (m_port > 0)
        {
            char buf[16] = { 0 };
            sprintf_s(buf, sizeof(buf), ":%d", m_port);
            ret.append(buf);
        }
    }

    if (!m_path.empty())
    {
        // only add the leading slash when the host is present
        if (!m_host.empty() && m_path.front() != '/')
        {
            ret.append({ '/' });
        }

        ret.append(m_path);
    }

    if (!m_query.empty())
    {
        ret.append({ '?' }).append(m_query);
    }

    if (!m_fragment.empty())
    {
        ret.append({ '#' }).append(m_fragment);
    }

    return ret;
#else
    xsapi_internal_ostringstream_t os;
    os.imbue(std::locale::classic());

    if (!m_scheme.empty())
    {
        os << m_scheme << ':';
    }

    if (!m_host.empty())
    {
        os << "//";

        if (!m_user_info.empty())
        {
            os << m_user_info << '@';
        }

        os << m_host;

        if (m_port > 0)
        {
            os << ':' << m_port;
        }
    }

    if (!m_path.empty())
    {
        // only add the leading slash when the host is present
        if (!m_host.empty() && m_path.front() != '/')
        {
            os << '/';
        }
        os << m_path;
    }

    if (!m_query.empty())
    {
        os << '?' << m_query;
    }

    if (!m_fragment.empty())
    {
        os << '#' << m_fragment;
    }

    return os.str();
#endif
}
}

using namespace xbox::services::cppresturi::web::details;

uri::uri(const details::uri_components &components) : m_components(components)
{
    m_uri = m_components.join();
    if (!details::uri_parser::validate(m_uri))
    {
        throw uri_exception(""); // provided uri is invalid : " + utility::conversions::to_utf8string(m_uri));
    }
}

uri::uri(const xsapi_internal_string &uri_string)
{
    if (!details::uri_parser::parse(uri_string, m_components))
    {
        throw uri_exception(""); // provided uri is invalid : " + utility::conversions::to_utf8string(uri_string));
    }
    m_uri = m_components.join();
}

uri::uri(const char *uri_string): m_uri(uri_string)
{
    if (!details::uri_parser::parse(uri_string, m_components))
    {
#if 0 // this returns a non-mem hooked string which can't be changed so commenting it out since its not really needed
        throw uri_exception("provided uri is invalid: " + utility::conversions::to_utf8string_internal(uri_string));
#else
        throw uri_exception(std::string());
#endif
    }
    m_uri = m_components.join();
}

xsapi_internal_string uri::encode_impl(const xsapi_internal_string &utf8raw, const std::function<bool(int)>& should_encode)
{
    const char * const hex = "0123456789ABCDEF";
    xsapi_internal_string encoded;
    for (auto iter = utf8raw.begin(); iter != utf8raw.end(); ++iter)
    {
        // for utf8 encoded string, char ASCII can be greater than 127.
        int ch = static_cast<unsigned char>(*iter);
        // ch should be same under both utf8 and utf16.
        if(should_encode(ch))
        {
            encoded.push_back('%');
            encoded.push_back(hex[(ch >> 4) & 0xF]);
            encoded.push_back(hex[ch & 0xF]);
        }
        else
        {
            // ASCII don't need to be encoded, which should be same on both utf8 and utf16.
            encoded.push_back((char)ch);
        }
    }
    return encoded;
}

/// <summary>
/// Encodes a string by converting all characters except for RFC 3986 unreserved characters to their
/// hexadecimal representation.
/// </summary>
xsapi_internal_string uri::encode_data_string(const xsapi_internal_string &raw)
{
    return uri::encode_impl(raw, [](int ch) -> bool
    {
        return !uri_parser::is_unreserved(ch);
    });
}

xsapi_internal_string uri::encode_uri(const xsapi_internal_string &raw, uri::components::component component)
{
    // Note: we also encode the '+' character because some non-standard implementations
    // encode the space character as a '+' instead of %20. To better interoperate we encode
    // '+' to avoid any confusion and be mistaken as a space.
    switch(component)
    {
    case components::user_info:
        return uri::encode_impl(raw, [](int ch) -> bool
        {
            return !uri_parser::is_user_info_character(ch)
                || ch == '%' || ch == '+';
        });
    case components::host:
        return uri::encode_impl(raw, [](int ch) -> bool
        {
            // No encoding of ASCII characters in host name (RFC 3986 3.2.2)
            return ch > 127;
        });
    case components::path:
        return uri::encode_impl(raw, [](int ch) -> bool
        {
            return !uri_parser::is_path_character(ch)
                || ch == '%' || ch == '+';
        });
    case components::query:
        return uri::encode_impl(raw, [](int ch) -> bool
        {
            return !uri_parser::is_query_character(ch)
                || ch == '%' || ch == '+';
        });
    case components::fragment:
        return uri::encode_impl(raw, [](int ch) -> bool
        {
            return !uri_parser::is_fragment_character(ch)
                || ch == '%' || ch == '+';
        });
    case components::full_uri:
    default:
        return uri::encode_impl(raw, [](int ch) -> bool
        {
            return !uri_parser::is_unreserved(ch) && !uri_parser::is_reserved(ch);
        });
    };
}

/// <summary>
/// Helper function to convert a hex character digit to a decimal character value.
/// Throws an exception if not a valid hex digit.
/// </summary>
static int hex_char_digit_to_decimal_char(int hex)
{
    int decimal;
    if(hex >= '0' && hex <= '9')
    {
        decimal = hex - '0';
    }
    else if(hex >= 'A' && hex <= 'F')
    {
        decimal = 10 + (hex - 'A');
    }
    else if(hex >= 'a' && hex <= 'f')
    {
        decimal = 10 + (hex - 'a');
    }
    else
    {
        throw uri_exception("Invalid hexadecimal digit");
    }
    return decimal;
}

xsapi_internal_string uri::decode(const xsapi_internal_string &encoded)
{
    xsapi_internal_string utf8raw;
    for(auto iter = encoded.begin(); iter != encoded.end(); ++iter)
    {
        if(*iter == '%')
        {
            if(++iter == encoded.end())
            {
                throw uri_exception("Invalid URI string, two hexadecimal digits must follow '%'");
            }
            int decimal_value = hex_char_digit_to_decimal_char(static_cast<int>(*iter)) << 4;
            if(++iter == encoded.end())
            {
                throw uri_exception("Invalid URI string, two hexadecimal digits must follow '%'");
            }
            decimal_value += hex_char_digit_to_decimal_char(static_cast<int>(*iter));

            utf8raw.push_back(static_cast<char>(decimal_value));
        }
        else
        {
            // encoded string has to be ASCII.
            utf8raw.push_back(reinterpret_cast<const char &>(*iter));
        }
    }
    return utf8raw;
}

std::vector<xsapi_internal_string> uri::split_path(const xsapi_internal_string &path)
{
    std::vector<xsapi_internal_string> results;
    xsapi_internal_istringstream iss(path);
    iss.imbue(std::locale::classic());
    xsapi_internal_string s;

    while (std::getline(iss, s, '/'))
    {
        if (!s.empty())
        {
            results.push_back(s);
        }
    }

    return results;
}

std::map<xsapi_internal_string, xsapi_internal_string> uri::split_query(const xsapi_internal_string &query)
{
    std::map<xsapi_internal_string, xsapi_internal_string> results;

    // Split into key value pairs separated by '&'.
    size_t prev_amp_index = 0;
    while(prev_amp_index != xsapi_internal_string::npos)
    {
        size_t amp_index = query.find_first_of('&', prev_amp_index);
        if (amp_index == xsapi_internal_string::npos)
            amp_index = query.find_first_of(';', prev_amp_index);

        xsapi_internal_string key_value_pair = query.substr(
            prev_amp_index,
            amp_index == xsapi_internal_string::npos ? query.size() - prev_amp_index : amp_index - prev_amp_index);
        prev_amp_index = amp_index == xsapi_internal_string::npos ? xsapi_internal_string::npos : amp_index + 1;

        size_t equals_index = key_value_pair.find_first_of('=');
        if(equals_index == xsapi_internal_string::npos)
        {
            continue;
        }
        else if (equals_index == 0)
        {
            xsapi_internal_string value(key_value_pair.begin() + equals_index + 1, key_value_pair.end());
            results[""] = value;
        }
        else
        {
            xsapi_internal_string key(key_value_pair.begin(), key_value_pair.begin() + equals_index);
            xsapi_internal_string value(key_value_pair.begin() + equals_index + 1, key_value_pair.end());
            results[key] = value;
        }
    }

    return results;
}

bool uri::validate(const xsapi_internal_string &uri_string)
{
    return uri_parser::validate(uri_string);
}

uri uri::authority() const
{
    return uri_builder().set_scheme(this->scheme()).set_host(this->host()).set_port(this->port()).set_user_info(this->user_info()).to_uri();
}

uri uri::resource() const
{
    return uri_builder().set_path(this->path()).set_query(this->query()).set_fragment(this->fragment()).to_uri();
}

bool uri::operator == (const uri &other) const
{
    // Each individual URI component must be decoded before performing comparison.
    // TFS # 375865

    if (this->is_empty() && other.is_empty())
    {
        return true;
    }
    else if (this->is_empty() || other.is_empty())
    {
        return false;
    }
    else if (this->scheme() != other.scheme())
    {
        // scheme is canonicalized to lowercase
        return false;
    }
    else if(uri::decode(this->user_info()) != uri::decode(other.user_info()))
    {
        return false;
    }
    else if (uri::decode(this->host()) != uri::decode(other.host()))
    {
        // host is canonicalized to lowercase
        return false;
    }
    else if (this->port() != other.port())
    {
        return false;
    }
    else if (uri::decode(this->path()) != uri::decode(other.path()))
    {
        return false;
    }
    else if (uri::decode(this->query()) != uri::decode(other.query()))
    {
        return false;
    }
    else if (uri::decode(this->fragment()) != uri::decode(other.fragment()))
    {
        return false;
    }

    return true;
}

}

} } }

#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( pop )
#endif
