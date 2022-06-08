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
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(ref);

    //   /      0       /  1   /      2        /           3           /    4    /    5
    //  /serviceconfigs/{scid}/sessiontemplates/{session-template-name}/sessions/{session-name}

    xsapi_internal_vector<xsapi_internal_string> pathComponents = utils::string_split_internal(xsapi_internal_string(path), '/');
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
CATCH_RETURN()

STDAPI XblMultiplayerSessionReferenceToUriPath(
    _In_ const XblMultiplayerSessionReference* sessionReference,
    _Out_ XblMultiplayerSessionReferenceUri* sessionReferenceUri
) XBL_NOEXCEPT
try
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
CATCH_RETURN()

/// <summary>
/// Checks whether an XblMultiplayerSessionReference is well formed. It is considered well formed if none of the
/// fields are empty strings.
/// </summary>
STDAPI_(bool) XblMultiplayerSessionReferenceIsValid(
    _In_ const XblMultiplayerSessionReference* sessionReference
) XBL_NOEXCEPT
try
{
    if (sessionReference == nullptr)
    {
        return false;
    }
    return sessionReference->Scid[0] != 0 && sessionReference->SessionTemplateName[0] != 0 && sessionReference->SessionName[0] != 0;
}
CATCH_RETURN()

/// <summary>
/// Creates an XblMultiplayerSessionReference from a scid, session template name, and session name.
/// </summary>
STDAPI_(XblMultiplayerSessionReference) XblMultiplayerSessionReferenceCreate(
    _In_z_ const char* scid,
    _In_z_ const char* sessionTemplateName,
    _In_z_ const char* sessionName
) XBL_NOEXCEPT
try
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
CATCH_RETURN_WITH({})

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

bool operator==(const XblMultiplayerSessionReference& lhs, const XblMultiplayerSessionReference& rhs)
{
    return utils::str_icmp(lhs.Scid, rhs.Scid) == 0 &&
           utils::str_icmp(lhs.SessionName, rhs.SessionName) == 0 &&
           utils::str_icmp(lhs.SessionTemplateName, rhs.SessionTemplateName) == 0;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END