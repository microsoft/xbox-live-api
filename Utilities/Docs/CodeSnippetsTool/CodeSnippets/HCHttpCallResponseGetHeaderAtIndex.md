```cpp
const char* headerName = nullptr;
const char* headerValue = nullptr;
HRESULT hr = HCHttpCallResponseGetHeaderAtIndex(httpCall, headerIndex, &headerName, &headerValue);
```
