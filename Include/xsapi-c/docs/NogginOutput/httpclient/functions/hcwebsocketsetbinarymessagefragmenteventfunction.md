---
author: XBL
title: "HCWebSocketSetBinaryMessageFragmentEventFunction"
description: "Set the binary message fragment handler. The client functionContext passed to HCWebSocketCreate will also be passed to this handler."
kindex: HCWebSocketSetBinaryMessageFragmentEventFunction
ms.author: migreen
ms.topic: reference
security: public
edited: 00/00/0000
applies-to: pc-gdk
---

# HCWebSocketSetBinaryMessageFragmentEventFunction  

Set the binary message fragment handler. The client functionContext passed to HCWebSocketCreate will also be passed to this handler.  

## Syntax  
  
```cpp
HRESULT HCWebSocketSetBinaryMessageFragmentEventFunction(  
         HCWebsocketHandle websocket,  
         HCWebSocketBinaryMessageFragmentFunction binaryMessageFragmentFunc  
)  
```  
  
### Parameters  
  
*websocket* &nbsp;&nbsp;\_In\_  
Type: HCWebsocketHandle  
  
The handle of the websocket.  
  
*binaryMessageFragmentFunc* &nbsp;&nbsp;\_In\_  
Type: HCWebSocketBinaryMessageFragmentFunction  
  
A pointer to the binary message fragment handling callback to use, or a null pointer to remove.  
  
  
### Return value  
Type: HRESULT
  
Result code for this API operation. Possible values are S_OK, E_INVALIDARG, or E_FAIL.
  
## Remarks  
  
If this handler is not set, messages larger than the configured buffer size may still be broken down and passed to the HCWebSocketBinaryMessageFunction, but there will be no indication that they are partial messages. If large WebSocket messages are expected, it is recommended to either set this handler OR set a receive buffer large enough to hold the entire message.
  
## Requirements  
  
**Header:** httpClient.h
  
**Library:** libHttpClient.141.GSDK.C.lib
  
## See also  
[httpClient](../httpclient_members.md)  
  
  
