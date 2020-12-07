-- Writes some events while offline, regains connection and uploads the events

test = require 'u-test'
common = require 'common'

function TestOfflineEventsCpp_Handler()
    print("TestOfflineEventsCpp_Handler")
    LeaderboardServiceGetLeaderboard()
end

local count = 0;

function OnLeaderboardServiceGetLeaderboard()
    print("OnLeaderboardServiceGetLeaderboard")
    count = count + 1
    if count == 1 then
        UploadEventsAndWait(1)
        LeaderboardServiceGetLeaderboard()
    elseif count == 2 then
        SetupEventUploadMock()
        UploadEventsAndWait(9)
        HCMockClearMocks()
        --Clear mocks and events should upload automatically
        Sleep(5000)
        LeaderboardServiceGetLeaderboard()
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
        EventsServiceWriteInGameEvent()
        count = count - 1
    end
    Sleep(5000)
end

test.TestOfflineEventsCpp = function()
    common.init(TestOfflineEventsCpp_Handler)
end