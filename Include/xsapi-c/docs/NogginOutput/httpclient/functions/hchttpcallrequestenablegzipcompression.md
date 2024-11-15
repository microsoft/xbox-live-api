---
author: XBL
title: "HCHttpCallRequestEnableGzipCompression"
description: "Enable GZIP compression on the provided body payload."
kindex: HCHttpCallRequestEnableGzipCompression
ms.author: migreen
ms.topic: reference
security: public
edited: 00/00/0000
applies-to: pc-gdk
---

# HCHttpCallRequestEnableGzipCompression  

Enable GZIP compression on the provided body payload.  

## Syntax  
  
```cpp
HRESULT HCHttpCallRequestEnableGzipCompression(  
         HCCallHandle call,  
         HCCompressionLevel level  
)  
```  
  
### Parameters  
  
*call* &nbsp;&nbsp;\_In\_  
Type: HCCallHandle  
  
The handle of the HTTP call.  
  
*level* &nbsp;&nbsp;\_In\_  
Type: [HCCompressionLevel](../enums/hccompressionlevel.md)  
  
The desired compression level.  
  
  
### Return value  
Type: HRESULT
  
Result code for this API operation. Possible values are S_OK, E_INVALIDARG, E_OUTOFMEMORY, or E_FAIL.
  
## Remarks  
  
This must be called prior to calling HCHttpCallPerformAsync.
  
## Requirements  
  
**Header:** httpClient.h
  
**Library:** libHttpClient.141.GSDK.C.lib
  
## See also  
[httpClient](../httpclient_members.md)  
  
  
