```cpp

XalInitArgs xalInitArgs = {};
#if HC_PLATFORM == HC_PLATFORM_UWP

xalInitArgs.titleId = titleId;
xalInitArgs.packageFamilyName = u8"41336MicrosoftATG.XboxLiveE2E_dspnxghe87tn0";
//xalInitArgs.correlationVector; // optional
//xalInitArgs.flags; // optional
//xalInitArgs.launchUser; // optional
//xalInitArgs.mainWindow; // optional

#elif HC_PLATFORM == HC_PLATFORM_GDK || HC_PLATFORM == HC_PLATFORM_XDK
// No args on GDK / XDK

#else

// Args for iOS / Android / Win32 
xalInitArgs.clientId = clientId.c_str();
xalInitArgs.titleId = titleId;
xalInitArgs.sandbox = sandbox.c_str();
#if HC_PLATFORM == HC_PLATFORM_IOS
// Extra args on iOS
std::string redirectUri{ "ms-xal-" + clientId + "://auth" };
xalInitArgs.redirectUri = redirectUri.data();
#endif

#endif

HCTraceSetTraceToDebugger(true);
HCSettingsSetTraceLevel(HCTraceLevel::Verbose);

HRESULT hr = XalInitialize(&xalInitArgs, queue);
```
