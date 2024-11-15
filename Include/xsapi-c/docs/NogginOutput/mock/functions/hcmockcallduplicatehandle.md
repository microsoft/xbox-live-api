---
author: XBL
title: "HCMockCallDuplicateHandle"
description: "Duplicates a mock HTTP call handle."
kindex: HCMockCallDuplicateHandle
ms.author: migreen
ms.topic: reference
security: public
edited: 00/00/0000
applies-to: pc-gdk
---

# HCMockCallDuplicateHandle  

Duplicates a mock HTTP call handle.  

## Syntax  
  
```cpp
HCMockCallHandle HCMockCallDuplicateHandle(  
         HCMockCallHandle call  
)  
```  
  
### Parameters  
  
*call* &nbsp;&nbsp;\_In\_  
Type: HCMockCallHandle  
  
The handle of the mock HTTP call.  
  
  
### Return value  
Type: HCMockCallHandle
  
Returns the duplicated handle.
  
## Remarks  
  
Call HCMockCallCloseHandle to close the new handle.
  
## Requirements  
  
**Header:** mock.h
  
**Library:** libHttpClient.141.GSDK.C.lib
  
## See also  
[mock](../mock_members.md)  
  
  
