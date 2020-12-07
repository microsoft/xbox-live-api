```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;

HRESULT hr = XblCleanupAsync(asyncBlock.get());
if (SUCCEEDED(hr))
{
    // Synchronously wait for cleanup to complete
    hr = XAsyncGetStatus(asyncBlock.get(), true);
}
```
