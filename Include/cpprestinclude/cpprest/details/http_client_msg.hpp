#if !XSAPI_NO_PPL
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
* HTTP Library: Request and reply message definitions (client side).
*
* For the latest on this and related APIs, please see: https://github.com/Microsoft/cpprestsdk
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/
#pragma once

#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( push )
#pragma warning( disable : 26444 ) // ignore various unnamed objects
#endif

namespace web { namespace http
{

uri details::_http_request::relative_uri() const
{
    // If the listener path is empty, then just return the request URI.
    if(m_listener_path.empty() || m_listener_path == _XPLATSTR("/"))
    {
        return m_uri.resource();
    }

    utility::string_t prefix = uri::decode(m_listener_path);
    utility::string_t path = uri::decode(m_uri.resource().to_string());
    if(path.empty())
    {
        path = _XPLATSTR("/");
    }

    auto pos = path.find(prefix);
    if (pos == 0)
    {
        return uri(uri::encode_uri(path.erase(0, prefix.length())));
    }
    else
    {
        throw http_exception(_XPLATSTR("Error: request was not prefixed with listener uri"));
    }
}

uri details::_http_request::absolute_uri() const
{
    if (m_base_uri.is_empty())
    {
        return m_uri;
    }
    else
    {
        return uri_builder(m_base_uri).append(m_uri).to_uri();
    }
}

void details::_http_request::set_request_uri(const uri& relative)
{
    m_uri = relative;
}

utility::string_t details::_http_request::to_string() const
{
    utility::ostringstream_t buffer;
    buffer.imbue(std::locale::classic());
    buffer << m_method << _XPLATSTR(" ") << (this->m_uri.is_empty() ? _XPLATSTR("/") : this->m_uri.to_string()) << _XPLATSTR(" HTTP/1.1\r\n");
    buffer << http_msg_base::to_string();
    return buffer.str();
}

void details::_http_request::_record_body_data_for_retry(const concurrency::streams::istream &stream)
{
    CASABLANCA_UNREFERENCED_PARAMETER(stream);
    if (!m_bodyTextRecorded && !m_bodyVectorRecorded)
    {
        m_onlySetBodyUsingStream = true;
    }
}

void details::_http_request::_record_body_data_for_retry(const std::vector<unsigned char> &body_data)
{
    m_bodyVector = body_data;
    m_bodyVectorRecorded = true;
}

void details::_http_request::_record_body_data_for_retry(const utf8string &body_text, const utf8string &content_type)
{
    m_bodyText = body_text;
    m_contentType = content_type;
    m_bodyTextRecorded = true;
}

bool details::_http_request::_reset_body_for_retry()
{
    if (m_onlySetBodyUsingStream)
    {
        return false;
    }

    if (m_bodyTextRecorded)
    {
        set_body(concurrency::streams::bytestream::open_istream(m_bodyText), m_bodyText.size(), m_contentType);
    }
    else if (m_bodyVectorRecorded)
    {
        set_body(concurrency::streams::bytestream::open_istream(m_bodyVector), m_bodyVector.size(), "application/octet-stream");
    }

    return true;
}

utility::string_t details::_http_response::to_string() const
{
    // If the user didn't explicitly set a reason phrase then we should have it default
    // if they used one of the standard known status codes.
    auto reason_phrase = m_reason_phrase;
    if(reason_phrase.empty())
    {
        static http_status_to_phrase idToPhraseMap[] = {
#define _PHRASES
#define DAT(a,b,c) {status_codes::a, c},
#include "cpprest/details/http_constants.dat"
#undef _PHRASES
#undef DAT
        };

        for( auto iter = std::begin(idToPhraseMap); iter != std::end(idToPhraseMap); ++iter)
        {
            if( iter->id == status_code() )
            {
                reason_phrase = iter->phrase;
                break;
            }
        }
    }

    utility::ostringstream_t buffer;
    buffer.imbue(std::locale::classic());
    buffer << _XPLATSTR("HTTP/1.1 ") << m_status_code << _XPLATSTR(" ") << reason_phrase << _XPLATSTR("\r\n");

    buffer << http_msg_base::to_string();
    return buffer.str();
}

}} // namespace web::http

#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( pop )
#endif

#endif // !XSAPI_NO_PPL
