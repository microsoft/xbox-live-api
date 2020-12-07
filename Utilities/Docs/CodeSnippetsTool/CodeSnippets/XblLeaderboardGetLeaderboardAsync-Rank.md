```cpp
XblLeaderboardQuery leaderboardQuery = {};
pal::strcpy(leaderboardQuery.scid, sizeof(leaderboardQuery.scid), scid.c_str());
leaderboardQuery.leaderboardName = leaderboardName.c_str();
leaderboardQuery.skipResultToRank = 100;
leaderboardQuery.maxItems = 100;
```
