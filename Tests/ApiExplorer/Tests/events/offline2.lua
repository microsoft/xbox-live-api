-- Write 10 events while offline. An offline events file with 10 events should be leftover after this test.

test = require 'u-test'
common = require 'common'

function TestOfflineEvents_Handler()
    print("TestOfflineEvents_Handler")
    SetupEventUploadMock()
    XblEventsWriteInGameEvent()
    XblEventsWriteInGameEvent()
    XblEventsWriteInGameEvent()
    XblEventsWriteInGameEvent()
    XblEventsWriteInGameEvent()
    XblEventsWriteInGameEvent()
    XblEventsWriteInGameEvent()
    XblEventsWriteInGameEvent()
    XblEventsWriteInGameEvent()
    XblEventsWriteInGameEvent()
    XblContextCloseHandle()
    Sleep(2000)
    ValidateOfflineEventsDirectoryFileExistsAndDelete()
    test.stopTest()
end

function SetupEventUploadMock()
    --Force event uploads to fail
    HCMockCallCreate()
    HCMockResponseSetNetworkErrorCode()
    HCMockAddMock("POST", "https://vortex.data.microsoft.com/collect/v1")
end

test.TestOfflineEvents2 = function()
    common.init(TestOfflineEvents_Handler)
end