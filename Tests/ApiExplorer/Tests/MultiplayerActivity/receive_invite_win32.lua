test = require 'u-test'
common = require 'common'

function TestReceiveInvite_Handler()
    XblGameInviteRegisterForEventAsync()
end

function OnXblGameInviteRegisterForEventAsync()
    XblMultiplayerActivityAddInviteHandler();
    MultiDeviceSyncAndWait("SendInvite");
end

function OnMultiplayerActivityGameInvite()
    XblMultiplayerActivityRemoveInviteHandler()
    XblGameInviteUnregisterForEventAsync()
    MultiDeviceSyncAndWait("Complete");
    test.stopTest();
end

test.ismultidevice = true;
test.TestReceiveInvite = function()
    common.init(TestReceiveInvite_Handler)
end