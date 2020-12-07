```cpp
XblMultiplayerSessionTag tag{ "SessionTag" };
XblMultiplayerSessionTag* tags = &tag;
size_t tagsCount = 1;

XblMultiplayerSessionNumberAttribute numberAttribute{ "numberattributename", 1.1 };
XblMultiplayerSessionNumberAttribute* numberAttributes = &numberAttribute;
size_t numberAttributesCount = 1;

XblMultiplayerSessionStringAttribute strAttribute{ "stringattributename", "string attribute value" };
XblMultiplayerSessionStringAttribute* strAttributes = &strAttribute;
size_t strAttributesCount = 1;

auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    XblMultiplayerSearchHandle searchHandle{ nullptr };
    HRESULT hr = XblMultiplayerCreateSearchHandleResult(asyncBlock, &searchHandle);

    if (SUCCEEDED(hr))
    {
        const char* handleId{ nullptr };
        XblMultiplayerSearchHandleGetId(searchHandle, &handleId);
    }
};

HRESULT hr = XblMultiplayerCreateSearchHandleAsync(
    xblContextHandle,
    &xblMultiplayerSessionReference,
    tags,
    tagsCount,
    numberAttributes,
    numberAttributesCount,
    strAttributes,
    strAttributesCount,
    asyncBlock.get()
);

if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
