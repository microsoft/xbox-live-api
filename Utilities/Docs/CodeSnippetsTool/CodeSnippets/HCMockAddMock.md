```cpp
HRESULT hr = HCMockAddMock(
    mockHttpCall,
    method.empty() ? nullptr : method.data(),
    url.empty() ? nullptr : url.data(),
    nullptr,
    0
);
```
