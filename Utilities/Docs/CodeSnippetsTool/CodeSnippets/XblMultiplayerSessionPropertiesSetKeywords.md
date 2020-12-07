```cpp
const char* keywords[2] = {};
keywords[0] = key1.c_str();
keywords[1] = key2.c_str();
size_t keywordsCount = 2;

HRESULT hr = XblMultiplayerSessionPropertiesSetKeywords(
    sessionHandle,
    keywords,
    keywordsCount);
```
