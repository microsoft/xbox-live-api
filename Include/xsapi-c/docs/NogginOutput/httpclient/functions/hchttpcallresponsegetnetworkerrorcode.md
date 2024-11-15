---
author: XBL
title: "HCHttpCallResponseGetNetworkErrorCode"
description: "Get the network error code of the HTTP call."
kindex: HCHttpCallResponseGetNetworkErrorCode
ms.author: migreen
ms.topic: reference
security: public
edited: 00/00/0000
applies-to: pc-gdk
---

# HCHttpCallResponseGetNetworkErrorCode  

Get the network error code of the HTTP call.  

## Syntax  
  
```cpp
HRESULT HCHttpCallResponseGetNetworkErrorCode(  
         HCCallHandle call,  
         HRESULT* networkErrorCode,  
         uint32_t* platformNetworkErrorCode  
)  
```  
  
### Parameters  
  
*call* &nbsp;&nbsp;\_In\_  
Type: HCCallHandle  
  
The handle of the HTTP call.  
  
*networkErrorCode* &nbsp;&nbsp;\_Out\_  
Type: HRESULT*  
  
The network error code of the HTTP call. Possible values are S_OK, or E_FAIL.  
  
*platformNetworkErrorCode* &nbsp;&nbsp;\_Out\_  
Type: uint32_t*  
  
The platform specific network error code of the HTTP call to be used for tracing / debugging.  
  
  
### Return value  
Type: HRESULT
  
Result code for this API operation. Possible values are S_OK, E_INVALIDARG, or E_FAIL.
  
## Remarks  
  
This can only be called after calling HCHttpCallPerformAsync when the HTTP task is completed. On the GDK Platform, HTTP calls that fail due to the title being suspended will have platformNetworkErrorCode set to HRESULT_FROM_WIN32(PROCESS_SUSPEND_RESUME).
  
## Requirements  
  
**Header:** httpClient.h
  
**Library:** libHttpClient.141.GSDK.C.lib
  
## See also  
[httpClient](../httpclient_members.md)  
  
  
