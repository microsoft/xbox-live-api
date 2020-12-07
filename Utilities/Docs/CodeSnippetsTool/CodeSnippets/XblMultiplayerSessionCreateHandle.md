```cpp
XblMultiplayerSessionReference ref;
pal::strcpy(ref.Scid, sizeof(ref.Scid), scid.c_str());
pal::strcpy(ref.SessionTemplateName, sizeof(ref.SessionTemplateName), sessionTemplateName.c_str());
pal::strcpy(ref.SessionName, sizeof(ref.SessionName), sessionName.c_str());

XblMultiplayerSessionInitArgs args = {};

XblMultiplayerSessionHandle sessionHandle = XblMultiplayerSessionCreateHandle(xuid, &ref, &args);
```
