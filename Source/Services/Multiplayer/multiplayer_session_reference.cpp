// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_internal.h"

using namespace xbox::services;
using namespace xbox::services::legacy;
using namespace xbox::services::multiplayer;

STDAPI XblMultiplayerSessionReferenceParseFromUriPath(
    _In_ const char* path,
    _Out_ XblMultiplayerSessionReference* ref
) XBL_NOEXCEPT
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(ref);

    //   /      0       /  1   /      2        /           3           /    4    /    5
    //  /serviceconfigs/{scid}/sessiontemplates/{session-template-name}/sessions/{session-name}

    xsapi_internal_vector<xsapi_internal_string> pathComponents = utils::string_split(xsapi_internal_string(path), '/');
    if (pathComponents.size() < 6)
    {
        return E_INVALIDARG;
    }

    memset(ref, 0, sizeof(XblMultiplayerSessionReference));
    utils::strcpy(ref->Scid, sizeof(ref->Scid), pathComponents.at(1).data());
    utils::strcpy(ref->SessionTemplateName, sizeof(ref->SessionTemplateName), pathComponents.at(3).data());
    utils::strcpy(ref->SessionName, sizeof(ref->SessionName), pathComponents.at(5).data());

    return S_OK;
}

STDAPI XblMultiplayerSessionReferenceToUriPath(
    _In_ const XblMultiplayerSessionReference* sessionReference,
    _Out_ XblMultiplayerSessionReferenceUri* sessionReferenceUri
) XBL_NOEXCEPT
{
    RETURN_HR_INVALIDARGUMENT_IF(sessionReference == nullptr || sessionReferenceUri == nullptr);

    xsapi_internal_stringstream uriStream;
    uriStream << "/serviceconfigs/";
    uriStream << sessionReference->Scid;
    uriStream << "/sessiontemplates/";
    uriStream << sessionReference->SessionTemplateName;
    uriStream << "/sessions/";
    uriStream << sessionReference->SessionName;

    memset(sessionReferenceUri, 0, sizeof(XblMultiplayerSessionReferenceUri));
    utils::strcpy(sessionReferenceUri->value, sizeof(sessionReferenceUri->value), uriStream.str().data());

    return S_OK;
}

/// <summary>
/// Checks whether an XblMultiplayerSessionReference is well formed. It is considered well formed if none of the
/// fields are empty strings.
/// </summary>
STDAPI_(bool) XblMultiplayerSessionReferenceIsValid(
    _In_ const XblMultiplayerSessionReference* sessionReference
) XBL_NOEXCEPT
{
    if (sessionReference == nullptr)
    {
        return false;
    }
    return sessionReference->Scid[0] != 0 && sessionReference->SessionTemplateName[0] != 0 && sessionReference->SessionName[0] != 0;
}

/// <summary>
/// Creates an XblMultiplayerSessionReference from a scid, session template name, and session name.
/// </summary>
STDAPI_(XblMultiplayerSessionReference) XblMultiplayerSessionReferenceCreate(
    _In_z_ const char* scid,
    _In_z_ const char* sessionTemplateName,
    _In_z_ const char* sessionName
) XBL_NOEXCEPT
{
    XblMultiplayerSessionReference out{};
    if (scid != nullptr)
    {
        utils::strcpy(out.Scid, sizeof(out.Scid), scid);
    }
    if (sessionTemplateName != nullptr)
    {
        utils::strcpy(out.SessionTemplateName, sizeof(out.SessionTemplateName), sessionTemplateName);
    }
    if (sessionName != nullptr)
    {
        utils::strcpy(out.SessionName, sizeof(out.SessionName), sessionName);
    }
    return out;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

bool operator==(const XblMultiplayerSessionReference& lhs, const XblMultiplayerSessionReference& rhs)
{
    return utils::str_icmp(lhs.Scid, rhs.Scid) == 0 &&
           utils::str_icmp(lhs.SessionName, rhs.SessionName) == 0 &&
           utils::str_icmp(lhs.SessionTemplateName, rhs.SessionTemplateName) == 0;
}

#if !XSAPI_NO_PPL
namespace legacy
{
    multiplayer_session_reference::multiplayer_session_reference() : m_reference{}
    {
    }

    multiplayer_session_reference::multiplayer_session_reference(
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& sessionTemplateName,
        _In_ const string_t& sessionName
    )
    {
        Utf8FromCharT(serviceConfigurationId.data(), m_reference.Scid, sizeof(m_reference.Scid));
        Utf8FromCharT(sessionTemplateName.data(), m_reference.SessionTemplateName, sizeof(m_reference.SessionTemplateName));
        Utf8FromCharT(sessionName.data(), m_reference.SessionName, sizeof(m_reference.SessionName));
    }

    multiplayer_session_reference::multiplayer_session_reference(
        _In_ const XblMultiplayerSessionReference& reference
    )
        : m_reference(reference)
    {
    }

    string_t multiplayer_session_reference::service_configuration_id() const
    {
        return StringTFromUtf8(m_reference.Scid);
    }

    string_t multiplayer_session_reference::session_template_name() const
    {
        return StringTFromUtf8(m_reference.SessionTemplateName);
    }

    string_t multiplayer_session_reference::session_name() const
    {
        return StringTFromUtf8(m_reference.SessionName);
    }

    bool multiplayer_session_reference::is_null() const
    {
        return m_reference.Scid[0] == 0 ||
            m_reference.SessionName[0] == 0 ||
            m_reference.SessionTemplateName[0] == 0;
    }

    string_t multiplayer_session_reference::to_uri_path() const
    {
        XblMultiplayerSessionReferenceUri uri{};
        XblMultiplayerSessionReferenceToUriPath(&m_reference, &uri);

        return StringTFromUtf8(uri.value);
    }

    multiplayer_session_reference multiplayer_session_reference::parse_from_uri_path(_In_ const string_t& path)
    {
        XblMultiplayerSessionReference reference;
        XblMultiplayerSessionReferenceParseFromUriPath(StringFromStringT(path).data(), &reference);
        return multiplayer_session_reference(reference);
    }
}
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END