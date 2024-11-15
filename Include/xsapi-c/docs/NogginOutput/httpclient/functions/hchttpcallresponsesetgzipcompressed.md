---
author: XBL
title: "HCHttpCallResponseSetGzipCompressed"
description: "Enable GZIP compression on the expected response."
kindex: HCHttpCallResponseSetGzipCompressed
ms.author: migreen
ms.topic: reference
security: public
edited: 00/00/0000
applies-to: pc-gdk
---

# HCHttpCallResponseSetGzipCompressed  

Enable GZIP compression on the expected response.  

## Syntax  
  
```cpp
HRESULT HCHttpCallResponseSetGzipCompressed(  
         HCCallHandle call,  
         bool compress  
)  
```  
  
### Parameters  
  
*call* &nbsp;&nbsp;\_In\_  
Type: HCCallHandle  
  
The handle of the HTTP call.  
  
*compress* &nbsp;&nbsp;\_In\_  
Type: bool  
  
TBD    
  
  
### Return value  
Type: HRESULT
  
Result code for this API operation. Possible values are S_OK, E_INVALIDARG, or E_HC_NOT_INITIALISED.
  
## Remarks  
  
This must be called prior to calling HCHttpCallPerformAsync.
  
## Requirements  
  
**Header:** httpClient.h
  
**Library:** libHttpClient.141.GSDK.C.lib
  
## See also  
[httpClient](../httpclient_members.md)  
  
  
