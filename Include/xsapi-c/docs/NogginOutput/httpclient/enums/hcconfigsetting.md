---
author: XBL
title: "HCConfigSetting"
description: "Defines the config settings value that is passed to the below API's."
kindex: HCConfigSetting
ms.author: migreen
ms.topic: reference
security: public
edited: 00/00/0000
applies-to: pc-gdk
---

# HCConfigSetting  

Defines the config settings value that is passed to the below API's.    

## Syntax  
  
```cpp
enum class HCConfigSetting  : uint32_t  
{  
    SSLValidationEnforcedInRetailSandbox = 1  
}  
```  
  
## Constants  
  
| Constant | Description |
| --- | --- |
| SSLValidationEnforcedInRetailSandbox | Only passed to the below API's to warn callers that this SSL validation is enforced RETAIL sandboxes regardless of this setting |  
  
## Requirements  
  
**Header:** httpClient.h
  
## See also  
[httpClient](../httpclient_members.md)  
  
  
