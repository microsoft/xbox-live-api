// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/multiplayer.h"
#include "xsapi/matchmaking.h"
#include "MatchTicketDetailsResponse_WinRT.h"

using namespace Platform::Collections;
using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services::matchmaking;
using namespace Microsoft::Xbox::Services::Multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_BEGIN

MatchTicketDetailsResponse::MatchTicketDetailsResponse(
    _In_ match_ticket_details_response cppObj
    ):
    m_cppObj(std::move(cppObj))
{
    if (!m_cppObj.ticket_session().is_null())
    {
        m_ticketSession = ref new MultiplayerSessionReference(m_cppObj.ticket_session());
    }

    if (!m_cppObj.target_session().is_null())
    {
        m_targetSession = ref new MultiplayerSessionReference(m_cppObj.target_session());
    }

    m_ticketAttributes = ref new String(m_cppObj.ticket_attributes().serialize().c_str());
}

MultiplayerSessionReference^
MatchTicketDetailsResponse::TicketSession::get()
{
    return m_ticketSession;
}

MultiplayerSessionReference^
MatchTicketDetailsResponse::TargetSession::get()
{
    return m_targetSession;
}

Platform::String^
MatchTicketDetailsResponse::TicketAttributes::get()
{
    return m_ticketAttributes;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_END