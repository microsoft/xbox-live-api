---
author: XBL
title: "HCHttpDisableAssertsForSSLValidationInDevSandboxes"
description: "Disables asserts for SSL validation in console dev sandboxes."
kindex: HCHttpDisableAssertsForSSLValidationInDevSandboxes
ms.author: migreen
ms.topic: reference
security: public
edited: 00/00/0000
applies-to: pc-gdk
---

# HCHttpDisableAssertsForSSLValidationInDevSandboxes  

Disables asserts for SSL validation in console dev sandboxes.  

## Syntax  
  
```cpp
HRESULT HCHttpDisableAssertsForSSLValidationInDevSandboxes(  
         HCConfigSetting setting  
)  
```  
  
### Parameters  
  
*setting* &nbsp;&nbsp;\_In\_  
Type: [HCConfigSetting](../enums/hcconfigsetting.md)  
  
The config settings value to be passed down.  
  
  
### Return value  
Type: HRESULT
  
HRESULT return code for this API operation.
  
## Remarks  
  
On GDK console, SSL validation is enforced on RETAIL sandboxes regardless of this setting. The asserts will not fire in RETAIL sandbox, and this setting has no affect in RETAIL sandboxes. It is best practice to not call this API, but this can be used as a temporary way to get unblocked while in early stages of game development.
  
## Requirements  
  
**Header:** httpClient.h
  
**Library:** libHttpClient.141.GSDK.C.lib
  
## See also  
[httpClient](../httpclient_members.md)  
  
  
