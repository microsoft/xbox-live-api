---
author: XBL
title: "HCTraceInit"
description: "Initialize tracing for the library."
kindex: HCTraceInit
ms.author: migreen
ms.topic: reference
security: public
edited: 00/00/0000
applies-to: pc-gdk
---

# HCTraceInit  

Initialize tracing for the library.  

## Syntax  
  
```cpp
void HCTraceInit(  
)  
```  
  
### Parameters  
  
  
### Return value  
Type: void
  
TBD  
  
## Remarks  
  
This function is implicitly called during HCInitialize. Initialization is reference counted, and multiple calls to HCTraceInit and HCTraceCleanup will not interfere with each other as long as each call to HCTraceInit is paired with exactly one call to HCTraceCleanup.
  
## Requirements  
  
**Header:** trace.h
  
**Library:** libHttpClient.141.GSDK.C.lib
  
## See also  
[trace](../trace_members.md)  
  
  
