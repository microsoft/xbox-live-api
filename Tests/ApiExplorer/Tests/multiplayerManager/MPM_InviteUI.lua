test = require 'u-test'
common = require 'common'

function MPM_InviteUITest_Handler()
    print("MPM_InviteUITest_Handler")
    MultiDeviceSyncAndWait("MPM_InviteUITest_Handler");

    XblMultiplayerManagerInitialize();
    StartDoWorkLoop();
    XblMultiplayerManagerLobbySessionAddLocalUser();
end

function OnXGameUiShowSendGameInviteAsync()
    print("OnXGameUiShowSendGameInviteAsync");
end

function OnXblMultiplayerEventType_UserAdded()
    if MultiDeviceIsHost() then 
        XblMultiplayerManagerSetJoinability();
    else
        XblMultiplayerManagerJoinGame();
    end
end

 function OnXblMultiplayerEventType_JoinabilityStateChanged()
    if MultiDeviceIsHost() then 
        XGameUiShowSendGameInviteAsyncToMPMLobby();
    end
 end

function OnXblMultiplayerEventType_JoinGameCompleted()
    MultiDeviceSyncAndWait("OnXblMultiplayerEventType_JoinGameCompleted");

    test.stopTest();
end

function OnXblMultiplayerEventType_MemberJoined()
    XblMultiplayerManagerGameSessionMembers();
end

test.skip = true
test.ismultidevice = true
test.MPM_InviteUITest = function()
    common.init(MPM_InviteUITest_Handler)
end

