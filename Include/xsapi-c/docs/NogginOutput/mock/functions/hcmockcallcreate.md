---
author: XBL
title: "HCMockCallCreate"
description: "Creates a mock HTTP call handle."
kindex: HCMockCallCreate
ms.author: migreen
ms.topic: reference
security: public
edited: 00/00/0000
applies-to: pc-gdk
---

# HCMockCallCreate  

Creates a mock HTTP call handle.  

## Syntax  
  
```cpp
HRESULT HCMockCallCreate(  
         HCMockCallHandle* call  
)  
```  
  
### Parameters  
  
*call* &nbsp;&nbsp;\_Out\_  
Type: HCMockCallHandle*  
  
The handle of the mock HTTP call.  
  
  
### Return value  
Type: HRESULT
  
Result code for this API operation. Possible values are S_OK, E_INVALIDARG, or E_FAIL.
  
## Remarks  
  
First create a HTTP handle using HCMockCallCreate(). Then call HCMockResponseSet*() to prepare the HC_MOCK_CALL_HANDLE. Then call HCMockAddMock() to add it to the system. Call HCMockRemoveMock() to remove the mock call from the system. Then call HCMockCallCloseHandle() to close the handle and clean up the mock call.
  
## Requirements  
  
**Header:** mock.h
  
**Library:** libHttpClient.141.GSDK.C.lib
  
## See also  
[mock](../mock_members.md)  
  
  
