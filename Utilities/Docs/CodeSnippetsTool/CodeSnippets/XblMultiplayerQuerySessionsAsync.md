```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t sessionCount = 0;
    HRESULT hr = XblMultiplayerQuerySessionsResultCount(asyncBlock, &sessionCount);
    LogToFile("sessionCount %d", sessionCount);
    if (SUCCEEDED(hr))
    {
        std::vector<XblMultiplayerSessionQueryResult> sessions(sessionCount);
        hr = XblMultiplayerQuerySessionsResult(asyncBlock, sessionCount, sessions.data());
    }

};

XblMultiplayerSessionQuery sessionQuery = {};
pal::strcpy(sessionQuery.Scid, sizeof(sessionQuery.Scid), Scid.c_str());
sessionQuery.MaxItems = MaxItems;
sessionQuery.IncludePrivateSessions = IncludePrivateSessions;
sessionQuery.IncludeReservations = IncludeReservations;
sessionQuery.IncludeInactiveSessions = IncludeInactiveSessions;
sessionQuery.KeywordFilter = KeywordFilter.c_str();
pal::strcpy(sessionQuery.SessionTemplateNameFilter, sizeof(sessionQuery.SessionTemplateNameFilter), SessionTemplateNameFilter.c_str());
sessionQuery.VisibilityFilter = VisibilityFilter;
sessionQuery.ContractVersionFilter = ContractVersionFilter;
//sessionQuery.XuidFilters
//sessionQuery.XuidFiltersCount 

HRESULT hr = XblMultiplayerQuerySessionsAsync(
    xboxLiveContext,
    &sessionQuery,
    asyncBlock.get());
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
