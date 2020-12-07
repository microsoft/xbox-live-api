```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock };

    XblMultiplayerSessionHandle sessionHandle;
    HRESULT hr = XblMultiplayerWriteSessionResult(asyncBlock, &sessionHandle);
    if (SUCCEEDED(hr))
    {
        // Process multiplayer session handle
    }
    else
    {
        // Handle failure
    }
};

XblMultiplayerSessionReference ref;
pal::strcpy(ref.Scid, sizeof(ref.Scid), SCID);
pal::strcpy(ref.SessionTemplateName, sizeof(ref.SessionTemplateName), SESSION_TEMPLATE_NAME);
pal::strcpy(ref.SessionName, sizeof(ref.SessionName), SESSION_NAME);

XblMultiplayerSessionInitArgs args = {};

XblMultiplayerSessionHandle sessionHandle = XblMultiplayerSessionCreateHandle(XUID, &ref, &args);

auto hr = XblMultiplayerWriteSessionAsync(xblContextHandle, sessionHandle, XblMultiplayerSessionWriteMode::CreateNew, asyncBlock.get());
if (SUCCEEDED(hr))
{
    asyncBlock.release();
}
```
