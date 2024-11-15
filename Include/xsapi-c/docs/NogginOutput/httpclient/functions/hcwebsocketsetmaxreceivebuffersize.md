---
author: XBL
title: "HCWebSocketSetMaxReceiveBufferSize"
description: "Configures how large the WebSocket receive buffer is allowed to grow before passing messages to clients. If a single message exceeds the maximum buffer size, the message will be broken down and passed to clients via multiple calls to the HCWebSocketMessageFunction. The default value is 20kb."
kindex: HCWebSocketSetMaxReceiveBufferSize
ms.author: migreen
ms.topic: reference
security: public
edited: 00/00/0000
applies-to: pc-gdk
---

# HCWebSocketSetMaxReceiveBufferSize  

Configures how large the WebSocket receive buffer is allowed to grow before passing messages to clients. If a single message exceeds the maximum buffer size, the message will be broken down and passed to clients via multiple calls to the HCWebSocketMessageFunction. The default value is 20kb.  

## Syntax  
  
```cpp
HRESULT HCWebSocketSetMaxReceiveBufferSize(  
         HCWebsocketHandle websocket,  
         size_t bufferSizeInBytes  
)  
```  
  
### Parameters  
  
*websocket* &nbsp;&nbsp;\_In\_  
Type: HCWebsocketHandle  
  
The handle of the WebSocket  
  
*bufferSizeInBytes* &nbsp;&nbsp;\_In\_  
Type: size_t  
  
Maximum size (in bytes) for the WebSocket receive buffer.  
  
  
### Return value  
Type: HRESULT
  
Result code for this API operation. Possible values are S_OK, E_INVALIDARG, or E_FAIL.
  
## Requirements  
  
**Header:** httpClient.h
  
**Library:** libHttpClient.141.GSDK.C.lib
  
## See also  
[httpClient](../httpclient_members.md)  
  
  
