```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t resultSize;
    HRESULT hr = XblLeaderboardGetLeaderboardResultSize(asyncBlock, &resultSize);

    if (SUCCEEDED(hr))
    {
        leaderboardBuffer.resize(resultSize);
        XblLeaderboardResult* leaderboard{};

        hr = XblLeaderboardGetLeaderboardResult(asyncBlock, resultSize, leaderboardBuffer.data(), &leaderboard, nullptr);

        if (SUCCEEDED(hr))
        {
            // Use XblLeaderboardResult in result
            for (auto row = 0u; row < leaderboard->rowsCount; ++row)
            {
                std::stringstream rowText;
                rowText << leaderboard->rows[row].xboxUserId << "\t";

                for (auto column = 0u; column < leaderboard->rows[row].columnValuesCount; ++column)
                {
                    rowText << leaderboard->rows[row].columnValues[column] << "\t";
                }
            }
        }
    }
};

XblLeaderboardQuery leaderboardQuery = {}; 
pal::strcpy(leaderboardQuery.scid, sizeof(leaderboardQuery.scid), scid.c_str()); 
leaderboardQuery.leaderboardName = leaderboardName.c_str(); 
// See below on more options in XblLeaderboardQuery

HRESULT hr = XblLeaderboardGetLeaderboardAsync(
    xboxLiveContext,
    leaderboardQuery,
    asyncBlock.get());
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
