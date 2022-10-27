
test = require 'u-test'
common = require 'common'

function TestMPInviteFlow_Handler()
    print("TestMPInviteFlow_Handler")
    XblMultiplayerSessionReferenceCreate();
    session, hr = XblMultiplayerSessionCreateHandle("00000000-0000-0000-0000-000076029b4d", "MinGameSession", "", 0)
    XblMultiplayerSessionJoin(session)
    XblMultiplayerWriteSessionAsync(session)
end

function OnXblMultiplayerWriteSessionAsync()
    XblMultiplayerSendInvitesAsync()
end

function OnXblMultiplayerSendInvitesAsync()
    XblMultiplayerGetSessionByHandleAsync()
end

function OnXblMultiplayerGetSessionByHandleAsync()
    XblMultiplayerSessionCloseHandle()
    test.stopTest();
end

test.TestMPInviteFlow = function()
    common.init(TestMPInviteFlow_Handler)
end