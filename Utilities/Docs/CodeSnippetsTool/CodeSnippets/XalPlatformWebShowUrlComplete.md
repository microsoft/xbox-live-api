```cpp
HRESULT hr = XalPlatformWebShowUrlComplete(
    args->operation,
    result,
    response.c_str()
);
LogToFile("XalPlatformWebShowUrlComplete: hr=%s", ConvertHR(hr).c_str());
```
