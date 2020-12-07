test = require 'u-test'
common = require 'common'

function MPM_JoinFixedGameSession_Handler()
    print("MPM_JoinFixedGameSession")
    MultiDeviceSyncAndWait("MPM_JoinFixedGameSession");

    XblMultiplayerManagerInitialize();
    StartDoWorkLoop();
    XblMultiplayerManagerLobbySessionAddLocalUser();
end

function OnXblMultiplayerEventType_UserAdded()
    if MultiDeviceIsHost() > 0 then 
        local sessionName = MultiDeviceGetRemoteXuid() .. "-session";
        MultiDeviceSetLocalState("session", sessionName);
        MultiDeviceSyncAndWait("SessionName");
        XblMultiplayerManagerJoinGame(sessionName);
    else
        MultiDeviceSyncAndWait("SessionName");
        local sessionName = MultiDeviceGetRemoteState("session");
        XblMultiplayerManagerJoinGame(sessionName);
    end    
end

function OnXblMultiplayerEventType_MemberJoined()
    MultiDeviceSyncAndWait("MemberJoinedLobby");
    
    test.stopTest();
end

test.ismultidevice = true
test.MPM_JoinFixedGameSession = function()
    common.init(MPM_JoinFixedGameSession_Handler)
end

