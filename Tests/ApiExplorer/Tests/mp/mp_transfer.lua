
test = require 'u-test'
common = require 'common'

function TestMPTransfer_Handler()
    print("TestMPSearch_Handler")
    session1, hr = XblMultiplayerSessionCreateHandle("00000000-0000-0000-0000-000076029b4d", "MinGameSession", "", 0)
    session2, hr = XblMultiplayerSessionCreateHandle("00000000-0000-0000-0000-000076029b4d", "MinGameSession", "", 1)
    XblMultiplayerSessionJoin(session1)
    XblMultiplayerSessionJoin(session2)
    XblMultiplayerWriteSessionAsync(session1)
    XblMultiplayerWriteSessionAsync(session2)
end

local sessionsWritten = 0

function OnXblMultiplayerWriteSessionAsync()
    sessionsWritten = sessionsWritten + 1;
    if sessionsWritten == 2 then
        XblMultiplayerSetTransferHandleAsync(session1, session2)
    end
end

function OnXblMultiplayerSetTransferHandleAsync()
    XblMultiplayerSessionCloseHandle(session1)
    XblMultiplayerSessionCloseHandle(session2)
    test.stopTest();
end

test.TestMPTransfer = function()
    common.init(TestMPTransfer_Handler)
end