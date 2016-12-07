//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {

//
// Represents the base message that comes across
//
public ref class MeshHeartbeatReceivedEvent sealed
{
public:
    property MeshConnection^ Sender { MeshConnection^ get() { return m_sender; }  }

    // Mesh unique identifier for the console
    property uint8 ConsoleId { uint8 get() { return m_consoleId; } }

internal:
    MeshHeartbeatReceivedEvent(
        uint8 consoleId,
        MeshConnection^ sender
        ) :
        m_consoleId(consoleId),
        m_sender(sender) {}

private:
    uint8 m_consoleId;
    MeshConnection^ m_sender;
};

//
// Received when a console connects
//
public ref class MeshHelloReceivedEvent sealed
{
public:
    property MeshConnection^ Sender { MeshConnection^ get() { return m_sender; }  }

    // Mesh unique identifier for the console
    property uint8 ConsoleId { uint8 get() { return m_consoleId; } }

    // The console's debug name
    property Platform::String^ ConsoleName { Platform::String^ get() { return m_consoleName; } }

    property bool RespondingToHello { bool get() { return m_respondingToHello; } }
internal:
    MeshHelloReceivedEvent(
        uint8 consoleId, 
        MeshConnection^ sender,
        Platform::String^ consoleName,
        bool respondingToHello ) :
        m_consoleId(consoleId),
        m_sender(sender),
        m_consoleName(consoleName),
        m_respondingToHello(respondingToHello)
    {}

private:
    uint8 m_consoleId;
    Platform::String^ m_consoleName;
    MeshConnection^ m_sender;
    bool m_respondingToHello;
};


//
// Received when a console connects
//
public ref class MeshAckReceivedEvent sealed
{
public:
    property MeshConnection^ Sender { MeshConnection^ get() { return m_sender; }  }

    // Mesh unique identifier for the console
    property uint8 ConsoleId { uint8 get() { return m_consoleId; } }

    // The ID of the packet which was ACK'd
    property uint16 MessageId { uint16 get() { return m_messageId; } }

internal:
    MeshAckReceivedEvent(
        uint8 consoleId, 
        MeshConnection^ sender,
        uint16 messageId
        ) :
        m_consoleId(consoleId),
        m_sender(sender),
        m_messageId(messageId)
    {
    }

private:
    uint8 m_consoleId;
    uint16 m_messageId;
    MeshConnection^ m_sender;
};






//
// Received when a audio chat packet has been received
//
public ref class MeshChatMessageReceivedEvent sealed
{
public:
    property MeshConnection^ Sender { MeshConnection^ get() { return m_sender; }  }

    // Mesh unique identifier for the console
    property uint8 ConsoleId { uint8 get() { return m_consoleId; } }

    // Buffer containing chat voice data
    property Windows::Storage::Streams::IBuffer^ Buffer { Windows::Storage::Streams::IBuffer^ get() { return m_buffer; } }

internal:
    MeshChatMessageReceivedEvent(
        uint8 consoleId, 
        MeshConnection^ sender,
        Windows::Storage::Streams::IBuffer^ buffer
        ) :
        m_consoleId(consoleId),
        m_sender(sender),
        m_buffer(buffer)
    {
    }

private:
    uint8 m_consoleId;
    Windows::Storage::Streams::IBuffer^ m_buffer;
    MeshConnection^ m_sender;
};

//
// Received when a game custom data packet has been received
//
public ref class GameCustomMessageReceivedEvent sealed
{
public:
    property MeshConnection^ Sender { MeshConnection^ get() { return m_sender; }  }

    // Mesh unique identifier for the console
    property uint8 MessageType { uint8 get() { return m_messageType; } }

    // Mesh unique identifier for the console
    property uint8 ConsoleId { uint8 get() { return m_consoleId; } }

    // Buffer containing chat voice data
    property Windows::Storage::Streams::IBuffer^ Buffer { Windows::Storage::Streams::IBuffer^ get() { return m_buffer; } }

internal:
    GameCustomMessageReceivedEvent(
        uint8 consoleId, 
        MeshConnection^ sender,
        uint8 messageType, 
        Windows::Storage::Streams::IBuffer^ buffer
        ) :
        m_consoleId(consoleId),
        m_sender(sender),
        m_messageType(messageType),
        m_buffer(buffer)
    {
    }

private:
    uint8 m_messageType;
    uint8 m_consoleId;
    Windows::Storage::Streams::IBuffer^ m_buffer;
    MeshConnection^ m_sender;
};


//
// Event for the Mesh Controller to report diagnostic and error message information
//
public ref class DebugMessageEventArgs sealed
{
public:
    property Platform::String^ Message  { Platform::String^ get() { return m_message; } }
    property int HResult { int get() { return m_hresult; } }

internal:
    DebugMessageEventArgs(Platform::String^ message, int hr)
    {
        m_message = message;
        m_hresult = hr;
    }

private:
    Platform::String^ m_message;
    int m_hresult;
};


}}}}