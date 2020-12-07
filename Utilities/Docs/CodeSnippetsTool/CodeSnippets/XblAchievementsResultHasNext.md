```cpp
HRESULT hr = S_OK;
bool hasNext = false;
if (achievementsResult != nullptr)
{
    hr = XblAchievementsResultHasNext(achievementsResult, &hasNext);
}
```
