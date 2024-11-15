---
author: XBL
title: "HCCompressionLevel"
description: "Defines the compression level that will be used on the compression algorithm. Lower levels perform less compression but have the highest speed in the compression and higher levels perform better compression but have the slowest speed in the compression."
kindex: HCCompressionLevel
ms.author: migreen
ms.topic: reference
security: public
edited: 00/00/0000
applies-to: pc-gdk
---

# HCCompressionLevel  

Defines the compression level that will be used on the compression algorithm. Lower levels perform less compression but have the highest speed in the compression and higher levels perform better compression but have the slowest speed in the compression.    

## Syntax  
  
```cpp
enum class HCCompressionLevel  : uint32_t  
{  
    None = 0,  
    Low = 1,  
    Medium = 6,  
    High = 9  
}  
```  
  
## Constants  
  
| Constant | Description |
| --- | --- |
| None | A value of "None" indicates that no compression will be made. |  
| Low | A value of "Low" indicates that compression level 1 will be made. |  
| Medium | A value of "Medium" indicates that compression level 6 will be made. |  
| High | A value of "High" indicates that compression level 9 will be made. |  
  
## Requirements  
  
**Header:** httpClient.h
  
## See also  
[httpClient](../httpclient_members.md)  
  
  
