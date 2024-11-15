---
author: XBL
title: "HCHttpCallRequestGetTimeoutWindow"
description: "Sets the HTTP timeout window in seconds."
kindex: HCHttpCallRequestGetTimeoutWindow
ms.author: migreen
ms.topic: reference
security: public
edited: 00/00/0000
applies-to: pc-gdk
---

# HCHttpCallRequestGetTimeoutWindow  

Sets the HTTP timeout window in seconds.  

## Syntax  
  
```cpp
HRESULT HCHttpCallRequestGetTimeoutWindow(  
         HCCallHandle call,  
         uint32_t* timeoutWindowInSeconds  
)  
```  
  
### Parameters  
  
*call* &nbsp;&nbsp;\_In\_opt\_  
Type: HCCallHandle  
  
The handle of the HTTP call. Pass nullptr to get the default for future calls.  
  
*timeoutWindowInSeconds* &nbsp;&nbsp;\_Out\_  
Type: uint32_t*  
  
The timeout window in seconds.  
  
  
### Return value  
Type: HRESULT
  
Result code for this API operation. Possible values are S_OK, E_INVALIDARG, or E_FAIL.
  
## Remarks  
  
This controls how long to spend attempting to retry idempotent service calls before failing.<br /> The default is 20 seconds.<br /><br /> Idempotent service calls are retried when a network error occurs or the server responds <br /> with one of these HTTP status codes:<br /> 408 (Request Timeout)<br /> 429 (Too Many Requests)<br /> 500 (Internal Server Error)<br /> 502 (Bad Gateway)<br /> 503 (Service Unavailable)<br /> 504 (Gateway Timeout)<br />
  
## Requirements  
  
**Header:** httpProvider.h
  
**Library:** libHttpClient.141.GSDK.C.lib
  
## See also  
[httpProvider](../httpprovider_members.md)  
  
  
