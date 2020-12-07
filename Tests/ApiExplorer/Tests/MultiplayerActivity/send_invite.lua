test = require 'u-test'
common = require 'common'

function TestSendInvite_Handler()
    MultiDeviceSyncAndWait("SendInvite");
    XblMultiplayerActivitySendInvitesAsync(MultiDeviceGetRemoteXuid())
end

function OnXblMultiplayerActivitySendInvitesAsync()
    MultiDeviceSyncAndWait("Complete")
    test.stopTest()
end

test.ismultidevice = true;
test.TestSendInvite = function()
    common.init(TestSendInvite_Handler)
end