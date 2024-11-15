---
author: XBL
title: "HCMockCallCloseHandle"
description: "Closes a mock HTTP call handle."
kindex: HCMockCallCloseHandle
ms.author: migreen
ms.topic: reference
security: public
edited: 00/00/0000
applies-to: pc-gdk
---

# HCMockCallCloseHandle  

Closes a mock HTTP call handle.  

## Syntax  
  
```cpp
HRESULT HCMockCallCloseHandle(  
         HCMockCallHandle call  
)  
```  
  
### Parameters  
  
*call* &nbsp;&nbsp;\_In\_  
Type: HCMockCallHandle  
  
The handle of the mock HTTP call.  
  
  
### Return value  
Type: HRESULT
  
Result code for this API operation. Possible values are S_OK, E_INVALIDARG, or E_FAIL.
  
## Requirements  
  
**Header:** mock.h
  
**Library:** libHttpClient.141.GSDK.C.lib
  
## See also  
[mock](../mock_members.md)  
  
  
