```cpp
const XblPresenceDeviceRecord* deviceRecords{ nullptr };
size_t deviceRecordsCount{ 0 };
HRESULT hr = XblPresenceRecordGetDeviceRecords(state.presenceRecord, &deviceRecords, &deviceRecordsCount);

for (auto i = 0u; i < deviceRecordsCount; ++i)
{
    auto& deviceRecord{ deviceRecords[i] };
    LogToScreen("Got XblDeviceRecord with device type %u and %u title records", deviceRecord.deviceType, deviceRecord.titleRecordsCount);

    for (auto j = 0u; j < deviceRecord.titleRecordsCount; ++j)
    {
        auto& titleRecord{ deviceRecord.titleRecords[j] };
        // Display rich presence string
        LogToScreen("Rich presence string for titleId %u: %s", titleRecord.titleId, titleRecord.richPresenceString);
    }
}
```
