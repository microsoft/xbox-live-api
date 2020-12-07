```cpp
HRESULT networkErrorCode = S_OK;
uint32_t platErrorCode = 0;
HRESULT hr = HCHttpCallResponseGetNetworkErrorCode(httpCall, &networkErrorCode, &platErrorCode);
```
