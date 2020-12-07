-- Writes some events while offline, regains connection and uploads the events

test = require 'u-test'
common = require 'common'

function TestOfflineEvents_Handler()
    print("TestOfflineEvents_Handler")
    XblLeaderboardGetLeaderboardAsync()
end

local count = 0;

function OnXblLeaderboardGetLeaderboardAsync()
    print("OnXblLeaderboardGetLeaderboardAsync")
    count = count + 1
    if count == 1 then
        UploadEventsAndWait(1)
        XblLeaderboardGetLeaderboardAsync()
    elseif count == 2 then
        SetupEventUploadMock()
        UploadEventsAndWait(9)
        HCMockClearMocks()
        --Clear mocks and events should upload automatically
        Sleep(5000)
        XblLeaderboardGetLeaderboardAsync()
    elseif count == 3 then
        test.stopTest()
    end
end

function SetupEventUploadMock()
    --Force event uploads to fail
    HCMockCallCreate()
    HCMockResponseSetNetworkErrorCode()
    HCMockAddMock("POST", "https://vortex.data.microsoft.com/collect/v1")
end

function UploadEventsAndWait(count)
    while count > 0 do
        XblEventsWriteInGameEvent()
        count = count - 1
    end
    Sleep(5000)
end

test.TestOfflineEvents = function()
    common.init(TestOfflineEvents_Handler)
end