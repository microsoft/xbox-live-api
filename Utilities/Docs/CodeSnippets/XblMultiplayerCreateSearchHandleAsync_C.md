```cpp
size_t tagsCount = 1;
XblMultiplayerSessionTag tags[1] = {};
tags[0] = XblMultiplayerSessionTag{ "SessionTag" };

size_t numberAttributesCount = 1;
XblMultiplayerSessionNumberAttribute numberAttributes[1] = {};
numberAttributes[0] = XblMultiplayerSessionNumberAttribute{ "numberattributename", 1.1 };

size_t strAttributesCount = 1;
XblMultiplayerSessionStringAttribute strAttributes[1] = {};
strAttributes[0] = XblMultiplayerSessionStringAttribute{ "stringattributename", "string attribute value" };

auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock };
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
    asyncBlock.release();
}
```
