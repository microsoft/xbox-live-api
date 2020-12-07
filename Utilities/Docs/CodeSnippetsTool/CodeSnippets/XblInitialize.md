```cpp
XblInitArgs args = { };
args.queue = queue;
#if !(HC_PLATFORM == HC_PLATFORM_XDK || HC_PLATFORM == HC_PLATFORM_UWP)
args.scid = "00000000-0000-0000-0000-000076029b4d";
#endif
#if HC_PLATFORM == HC_PLATFORM_WIN32
char pathArray[MAX_PATH + 1];
GetCurrentDirectoryA(MAX_PATH + 1, pathArray);
auto pathString = std::string{ pathArray } + '\\';
args.localStoragePath = pathString.data();
#endif
#if HC_PLATFORM == HC_PLATFORM_IOS

#endif
HRESULT hr = XblInitialize(&args);
```
