---
author: XBL
title: "HCWebSocketGetBinaryMessageFragmentEventFunction"
description: "Gets the WebSocket binary message fragment handler."
kindex: HCWebSocketGetBinaryMessageFragmentEventFunction
ms.author: migreen
ms.topic: reference
security: public
edited: 00/00/0000
applies-to: pc-gdk
---

# HCWebSocketGetBinaryMessageFragmentEventFunction  

Gets the WebSocket binary message fragment handler.  

## Syntax  
  
```cpp
HRESULT HCWebSocketGetBinaryMessageFragmentEventFunction(  
         HCWebsocketHandle websocket,  
         HCWebSocketBinaryMessageFragmentFunction* binaryMessageFragmentFunc,  
         void** functionContext  
)  
```  
  
### Parameters  
  
*websocket* &nbsp;&nbsp;\_In\_  
Type: HCWebsocketHandle  
  
The handle of the websocket.  
  
*binaryMessageFragmentFunc* &nbsp;&nbsp;\_Out\_  
Type: HCWebSocketBinaryMessageFragmentFunction*  
  
Returned binaryMessageFragmentFunc.  
  
*functionContext* &nbsp;&nbsp;\_Out\_  
Type: void**  
  
Client context to pass to callback function.  
  
  
### Return value  
Type: HRESULT
  
Result code for this API operation. Possible values are S_OK, E_INVALIDARG, E_HC_NOT_INITIALISED, or E_FAIL.
  
## Requirements  
  
**Header:** httpClient.h
  
**Library:** libHttpClient.141.GSDK.C.lib
  
## See also  
[httpClient](../httpclient_members.md)  
  
  
