test = require 'u-test'
common = require 'common'

function TestMPTransferCpp_Handler()
    print("TestMPTransferCpp_Handler")
    session1, hr = MultiplayerSessionCreateCpp("00000000-0000-0000-0000-000076029b4d", "MinGameSession", "", 0)
    session2, hr = MultiplayerSessionCreateCpp("00000000-0000-0000-0000-000076029b4d", "MinGameSession", "", 1)
    MultiplayerSessionJoinCpp(session1)
    MultiplayerSessionJoinCpp(session2)
    MultiplayerServiceWriteSession(session1)
    MultiplayerServiceWriteSession(session2)
end

local sessionsWritten = 0

function OnMultiplayerServiceWriteSession()
    sessionsWritten = sessionsWritten + 1;
    if sessionsWritten == 2 then
        MultiplayerServiceSetTransferHandle(session1, session2)
    end
end

function OnMultiplayerServiceSetTransferHandle()
    test.stopTest();
end

test.TestMPTransferCpp = function()
    common.init(TestMPTransferCpp_Handler)
end