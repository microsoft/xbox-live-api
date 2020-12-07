test = require 'u-test'
common = require 'common'

function TestReceiveInvite_Handler()
    XGameInviteRegisterForEvent();
    MultiDeviceSyncAndWait("SendInvite");
end

function OnXGameInviteRegisterForEvent()
    MultiDeviceSyncAndWait("Complete");
    test.stopTest();
end

test.ismultidevice = true
test.TestReceiveInvite = function()
    common.init(TestReceiveInvite_Handler)
end
