//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#pragma once

namespace Microsoft { namespace Xbox { namespace Samples { namespace NetworkMesh {

public enum class MessageTypeEnum
{
    GAME_HEARTBEAT_DATA = 1, // Heartbeat
    GAME_HELLO_DATA = 2, // First message sent between clients
    GAME_CHAT_DATA = 3, // Sending chat data
    GAME_ACK = 4, // Sending ACK packet 
    GAME_CUSTOM_DATA = 64 // Message type 64 or higher is custom data as defined by the game
};

// Set data alignment to be 1 byte
#pragma pack(push) 
#pragma pack(1) 

struct MeshPacketHeader
{
    uint16 messageId; // To keep track of packets being sent/received to detect number of packets dropped.
    uint8 messageType; // Type of message (MessageTypeEnum)
    uint8 consoleId; // Mesh unique identifier of the console who created packet
    uint16 messageSize; // Total number of bytes in the packet
};  

struct MeshPacketHelloMessageHeader
{
    uint8 respondingToHello; // Set to 1 if this packet is responding to a hello message
    uint16 consoleNameLength; // Length is number of characters, NOT including any null termination
};

// Store data alignment
#pragma pack(pop)  

}}}}

