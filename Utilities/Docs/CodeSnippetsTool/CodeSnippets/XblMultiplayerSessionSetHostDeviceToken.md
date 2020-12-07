```cpp
XblDeviceToken hostDeviceToken = {};
pal::strcpy(hostDeviceToken.Value, sizeof(hostDeviceToken.Value), host.c_str());
XblMultiplayerSessionSetHostDeviceToken(sessionHandle, hostDeviceToken);
```
