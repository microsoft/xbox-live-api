// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/multiplayer.h"
#include "utils.h"
#include "user_context.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_session_reference::multiplayer_session_reference()
{
}

multiplayer_session_reference::multiplayer_session_reference(
    _In_ string_t serviceConfigurationId,
    _In_ string_t sessionTemplateName,
    _In_ string_t sessionName
    )
{
    XSAPI_ASSERT(!serviceConfigurationId.empty());
    XSAPI_ASSERT(!sessionTemplateName.empty());
    XSAPI_ASSERT(!sessionName.empty());

    m_serviceConfigurationId = std::move(serviceConfigurationId);
    m_sessionTemplateName = std::move(sessionTemplateName);
    m_sessionName = std::move(sessionName);
}

#ifndef DEFAULT_MOVE_ENABLED
_XSAPIIMP multiplayer_session_reference::multiplayer_session_reference(multiplayer_session_reference&& other)
{
    *this = std::move(other);
}

_XSAPIIMP multiplayer_session_reference& multiplayer_session_reference::operator= (multiplayer_session_reference&& other)
{
    if (this != &other)
    {
        m_serviceConfigurationId = std::move(other.service_configuration_id());
        m_sessionName = std::move(other.session_name());
        m_sessionTemplateName = std::move(other.session_template_name());
    }

    return *this;
}
#endif

const string_t& 
multiplayer_session_reference::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

const string_t&
multiplayer_session_reference::session_template_name() const
{
    return m_sessionTemplateName;
}

const string_t&
multiplayer_session_reference::session_name() const
{
    return m_sessionName;
}

bool 
multiplayer_session_reference::is_null() const
{
    return m_serviceConfigurationId.empty() || m_sessionTemplateName.empty() || m_sessionName.empty();
}

web::json::value 
multiplayer_session_reference::_Serialize() const
{
    web::json::value serializedObject;

    serializedObject[_T("scid")] = web::json::value::string(m_serviceConfigurationId);
    serializedObject[_T("templateName")] = web::json::value::string(m_sessionTemplateName);
    serializedObject[_T("name")] = web::json::value::string(m_sessionName);

    return serializedObject;
}

string_t 
multiplayer_session_reference::to_uri_path() const
{
    stringstream_t uriStream;
    uriStream << "/serviceconfigs/";
    uriStream << m_serviceConfigurationId;
    uriStream << "/sessiontemplates/";
    uriStream << m_sessionTemplateName;
    uriStream << "/sessions/";
    uriStream << m_sessionName;

    return uriStream.str();
}

multiplayer_session_reference
multiplayer_session_reference::parse_from_uri_path(
    _In_ const string_t& path
    )
{
    std::vector<string_t> pathComponents = utils::string_split(path, L'/');

    //   /      0       /  1   /      2        /           3           /    4    /    5
    //  /serviceconfigs/{scid}/sessiontemplates/{session-template-name}/sessions/{session-name}            
    if (pathComponents.size() > 5)  
    {
        return multiplayer_session_reference(
            pathComponents.at(1).c_str(),
            pathComponents.at(3).c_str(),
            pathComponents.at(5).c_str()
            );
    }
    else
    {
        return multiplayer_session_reference();
    }
}

xbox_live_result<multiplayer_session_reference>
multiplayer_session_reference::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null() || json.size() == 0) return xbox_live_result<multiplayer_session_reference>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto response = multiplayer_session_reference(
        utils::extract_json_string(json, _T("scid"), errc, true),
        utils::extract_json_string(json, _T("templateName"), errc, true),
        utils::extract_json_string(json, _T("name"), errc, true)
        );

    return xbox_live_result<multiplayer_session_reference>(response, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END