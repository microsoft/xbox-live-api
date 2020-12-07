// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

MultiplayerTransferHandlePostRequest::MultiplayerTransferHandlePostRequest(
    _In_ XblMultiplayerSessionReference targetSessionReference,
    _In_ XblMultiplayerSessionReference originSessionReference
) :
    m_originSessionReference(std::move(originSessionReference)),
    m_targetSessionReference(std::move(targetSessionReference))
{
    XSAPI_ASSERT(XblMultiplayerSessionReferenceIsValid(&m_targetSessionReference));
    XSAPI_ASSERT(XblMultiplayerSessionReferenceIsValid(&m_originSessionReference));
}

const XblMultiplayerSessionReference&
MultiplayerTransferHandlePostRequest::OriginSessionReference() const
{
    return m_originSessionReference;
}

const XblMultiplayerSessionReference&
MultiplayerTransferHandlePostRequest::TargetSessionReference() const
{
    return m_targetSessionReference;
}

void
MultiplayerTransferHandlePostRequest::Serialize(_Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator) const
{
    json.SetObject();

    json.AddMember("type", "transfer", allocator);
    JsonValue targetSessionJson;
    Serializers::SerializeSessionReference(m_targetSessionReference, targetSessionJson, allocator);
    json.AddMember("sessionRef", targetSessionJson, allocator);
    json.AddMember("version", MULTIPLAYER_HANDLE_VERSION, allocator);
    JsonValue originSessionJson;
    Serializers::SerializeSessionReference(m_originSessionReference, originSessionJson, allocator);
    json.AddMember("originSessionRef", originSessionJson, allocator);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END