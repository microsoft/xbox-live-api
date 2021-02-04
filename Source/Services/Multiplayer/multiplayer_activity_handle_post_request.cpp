// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

MultiplayerActivityHandlePostRequest::MultiplayerActivityHandlePostRequest(
    _In_ XblMultiplayerSessionReference sessionReference
) :
    m_sessionReference(std::move(sessionReference))
{
    XSAPI_ASSERT(XblMultiplayerSessionReferenceIsValid(&m_sessionReference));
}

const XblMultiplayerSessionReference&
MultiplayerActivityHandlePostRequest::SessionReference() const
{
    return m_sessionReference;
}

void
MultiplayerActivityHandlePostRequest::Serialize(_Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator) const
{
    json.SetObject();

    json.AddMember("type", "activity", allocator);
    JsonValue sessionRefJson;
    Serializers::SerializeSessionReference(m_sessionReference, sessionRefJson, allocator);
    json.AddMember("sessionRef", sessionRefJson, allocator);
    json.AddMember("version", MULTIPLAYER_HANDLE_VERSION, allocator);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END