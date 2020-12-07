```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    HRESULT hr = XAsyncGetStatus(asyncBlock, false);

    if (SUCCEEDED(hr))
    {
        size_t resultSize{ 0 };
        hr = XblMultiplayerGetActivitiesWithPropertiesForUsersResultSize(asyncBlock, &resultSize);
        if (SUCCEEDED(hr))
        {
            size_t count{ 0 };
            std::vector<char> buffer(resultSize);
            XblMultiplayerActivityDetails* activityDetails{};
            if (resultSize > 0)
            {
                hr = XblMultiplayerGetActivitiesWithPropertiesForUsersResult(asyncBlock, resultSize, buffer.data(), &activityDetails, &count, nullptr);
                if (SUCCEEDED(hr))
                {
                        std::string handleIdStr = activityDetails[0].HandleId;
                        LogToScreen("Joining lobby via handle %s", handleIdStr.c_str());
                }
            }
            else
            {
                if (m_multiDeviceManager->GetRemoteXuid() != 0)
                {
                    LogToScreen("No activity handle to join.  Failing...");
                    hr = E_FAIL;
                }
            }
        }
    }

};

uint64_t xuids[1] = {};
xuids[0] = xuid1;
size_t xuidsCount = 1;

HRESULT hr = XblMultiplayerGetActivitiesWithPropertiesForUsersAsync(
    xboxLiveContext,
    scid,
    xuids,
    xuidsCount,
    asyncBlock.get());
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
