-- Write 10 events while offline. An offline events file with 10 events should be written during this test, and then deleted at the end of this test.

test = require 'u-test'
common = require 'common'

function TestOfflineEventsCpp_Handler()
    print("TestOfflineEventsCpp_Handler")
    SetupEventUploadMock()
    EventsServiceWriteInGameEvent()
    EventsServiceWriteInGameEvent()
    EventsServiceWriteInGameEvent()
    EventsServiceWriteInGameEvent()
    EventsServiceWriteInGameEvent()
    EventsServiceWriteInGameEvent()
    EventsServiceWriteInGameEvent()
    EventsServiceWriteInGameEvent()
    EventsServiceWriteInGameEvent()
    EventsServiceWriteInGameEvent()
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

test.TestOfflineEvents2Cpp = function()
    common.init(TestOfflineEventsCpp_Handler)
end