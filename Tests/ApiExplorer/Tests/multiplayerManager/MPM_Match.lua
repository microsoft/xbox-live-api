test = require 'u-test'
common = require 'common'

function MPM_Match_Handler()
    print("MPM_Match")
    MultiDeviceSyncAndWait("MPM_Match");

    XblMultiplayerManagerInitialize();
    StartDoWorkLoop();
    XblMultiplayerManagerLobbySessionAddLocalUser();
end

function OnXblMultiplayerEventType_UserAdded()
    MultiDeviceSyncAndWait("UserAdded");
    XblMultiplayerManagerFindMatch("PlayerSkillNoQoS");
end

function OnXblMultiplayerEventType_FindMatchCompleted()
    VerifyMPMGameSessionProperites();
    VerifyMPMLobbySessionProperites();
    MultiDeviceSyncAndWait("FindMatch");

    XblMultiplayerManagerLeaveGame()
end

function OnXblMultiplayerEventType_JoinGameCompleted()
    MultiDeviceSyncAndWait("JoinGameCompleted");

    test.stopTest();
end

function OnXblMultiplayerEventType_LeaveGameCompleted()
    MultiDeviceSyncAndWait("LeaveGameCompleted");

    test.stopTest();
end

test.ismultidevice = true
test.MPM_Match = function()
    common.init(MPM_Match_Handler)
end

