```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    XblMultiplayerSessionHandleId id{};
    auto hr = XblMultiplayerSetTransferHandleResult(asyncBlock, &id);
    if (SUCCEEDED(hr))
    {
        LogToFile("Sucessfully set transfer handle, ID = %s", id.value);
    }
};

auto targetReference = XblMultiplayerSessionSessionReference(MPState()->sessionHandles[static_cast<uint32_t>(targetIndex)]);
auto originReference = XblMultiplayerSessionSessionReference(MPState()->sessionHandles[static_cast<uint32_t>(originIndex)]);

HRESULT hr = S_OK;
if (originReference != nullptr && targetReference != nullptr)
{
    hr = XblMultiplayerSetTransferHandleAsync(
        xboxLiveContext,
        *targetReference,
        *originReference,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
}

// CODE SNIP END

LogToFile("XblMultiplayerSetTransferHandleAsync: hr=%s", ConvertHR(hr).data());
return LuaReturnHR(L, hr);


XblMatchmakingCreateTicket_Lua(lua_State* L)

// Params:
// 1) matchmaking hopper name
// 2) attributes json
// 3) SCID
// 4) session template name
// 5) session name
// 6) timeout in seconds
auto hopperName{ GetStringFromLua(L, 1, "PlayerSkillNoQoS") };
auto attributesJson{ GetStringFromLua(L, 2, "{}") };
uint32_t timeoutInSeconds = GetUint32FromLua(L, 6, 100);
CreateQueueIfNeeded();

// CODE SNIPPET START: XblMatchmakingCreateMatchTicketAsync
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    XblCreateMatchTicketResponse result{};

    auto hr = XblMatchmakingCreateMatchTicketResult(asyncBlock, &result);
    if (SUCCEEDED(hr))
    {

    }

};

HRESULT hr = XblMatchmakingCreateMatchTicketAsync(
    xboxLiveContext,
    MPState()->sessionRef,
    scid,
    hopperName.c_str(),
    timeoutInSeconds,
    XblPreserveSessionMode::Never,
    attributesJson.c_str(),
    asyncBlock.get()
);
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
