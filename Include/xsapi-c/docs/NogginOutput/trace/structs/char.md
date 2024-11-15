---
author: XBL
title: "char"
description: "TBD  "
kindex: char
ms.author: migreen
ms.topic: reference
security: public
edited: 00/00/0000
applies-to: pc-gdk
---

# char  

TBD    

## Syntax  
  
```cpp
typedef struct char {  
<member>
<name>#area(verbosity)</name>
<datatype></datatype>
<desc>
<p>Creates a new instance of a #area, initializing the following parameters:</p>
<p>verbosity</p>
</desc>
</member>
<member>
<name>HCTraceSetPlatformCallbacks(HCTracePlatformThisThreadIdCallback* threadIdCallback, void* threadIdContext, HCTracePlatformWriteMessageToDebuggerCallback* writeToDebuggerCallback, void* writeToDebuggerContext)</name>
<datatype></datatype>
<desc>
<p>Creates a new instance of a HCTraceSetPlatformCallbacks, initializing the following parameters:</p>
<p>HCTracePlatformThisThreadIdCallback* threadIdCallback</p><p>void* threadIdContext</p><p>HCTracePlatformWriteMessageToDebuggerCallback* writeToDebuggerCallback</p><p>void* writeToDebuggerContext</p>
</desc>
</member>
<member>
<name>HC_PRIVATE_TRACE_AREA_NAME(area)</name>
<datatype>#define</datatype>
<desc>
<p>Specifies a function that initializes the following parameters:</p>
<p>area</p>
</desc>
</member>
<member>
<name>HCTraceImplSetAreaVerbosity(HCTraceImplArea* area, HCTraceLevel verbosity)</name>
<datatype>void</datatype>
<desc>
<p>Specifies a function that initializes the following parameters:</p>
<p>HCTraceImplArea* area</p><p>HCTraceLevel verbosity</p>
</desc>
</member>
<member>
<name>HCTraceImplGetAreaVerbosity(HCTraceImplArea* area)</name>
<datatype>EXTERN_C</datatype>
<desc>
<p>Specifies a function that initializes the following parameters:</p>
<p>HCTraceImplArea* area</p>
</desc>
</member>
<member>
<name>HCTraceImplMessage()</name>
<datatype>void</datatype>
<desc>
<p>Specifies a function that ... TBD.</p>
</desc>
</member>
    HCTraceImplArea ;  
} char  
```
  
### Members  
  
**  
Type: HCTraceImplArea  
  
Sets the Platform Callbacks.
  
## Requirements  
  
**Header:** trace.h
  
## See also  
[trace](../trace_members.md)  
  
  
