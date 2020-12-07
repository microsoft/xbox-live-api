test = require 'u-test'
common = require 'common'

function MPM_JoinLobbyViaActivity_Handler()
    print("MPM_JoinLobbyViaActivity")
    MultiDeviceSyncAndWait("MPM_JoinLobbyViaActivity");

    XblMultiplayerManagerInitialize();
    StartDoWorkLoop();

    if MultiDeviceIsHost() > 0 then 
        XblMultiplayerManagerLobbySessionAddLocalUser();
        XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress("1111111111111111");
    else
        MultiDeviceWaitTillRemoteState("stage", "JoinabilityStateChanged");
        print("Joining lobby of remote player. NOTE: Ensure remote is a friend or join will fail")
        XblMultiplayerManagerJoinLobbyViaActivity();
        XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress("2222222222222222");
    end
end

function OnXblMultiplayerEventType_UserAdded()
    if MultiDeviceIsHost() > 0 then 
        XblMultiplayerManagerSetJoinability(); -- sets it to XblMultiplayerJoinability::JoinableByFriends
    end
end

function OnXblMultiplayerEventType_JoinabilityStateChanged()
    XblMultiplayerManagerJoinability(); -- not needed just for coverage
    if MultiDeviceIsHost() > 0 then 
        XblMultiplayerManagerLobbySessionSetSynchronizedHost();
    end
end

function OnXblMultiplayerEventType_SynchronizedHostWriteCompleted()
    MultiDeviceSetLocalState("stage", "JoinabilityStateChanged");
    -- waiting till we get a OnXblMultiplayerEventType_JoinLobbyCompleted
end

function OnXblMultiplayerEventType_JoinLobbyCompleted()
    MultiDeviceSyncAndWait("MemberJoinedLobby");
    XblMultiplayerManagerLobbySessionIsHost();
    XblMultiplayerManagerLobbySessionHost();
end

function OnXblMultiplayerEventType_MemberJoined()
    MultiDeviceSyncAndWait("MemberJoinedLobby");
    XblMultiplayerManagerLobbySessionIsHost();
    XblMultiplayerManagerLobbySessionHost();

    print("XblMultiplayerManagerLobbySessionSetProperties as host")
    XblMultiplayerManagerLobbySessionSetProperties();
end

function OnXblMultiplayerEventType_LocalMemberPropertyWriteCompleted()
    print('OnXblMultiplayerEventType_LocalMemberPropertyWriteCompleted');

    print("Joining game from lobby")
    XblMultiplayerManagerJoinGameFromLobby();
end

function OnXblMultiplayerEventType_MemberPropertyChanged()
    print('OnXblMultiplayerEventType_MemberPropertyChanged');

    print("Joining game from lobby")
    XblMultiplayerManagerJoinGameFromLobby();
end

function OnXblMultiplayerEventType_SessionPropertyWriteCompleted()
    print('OnXblMultiplayerEventType_SessionPropertyWriteCompleted');
    MultiDeviceSyncAndWait("MemberJoinedLobbyPropsChanged");

    print("XblMultiplayerManagerLobbySessionSetLocalMemberProperties")
    XblMultiplayerManagerLobbySessionSetLocalMemberProperties();
end

function OnXblMultiplayerEventType_LobbySessionPropertyChanged()
    print('OnXblMultiplayerEventType_LobbySessionPropertyChanged');
    MultiDeviceSyncAndWait("MemberJoinedLobbyPropsChanged");
end

function OnXblMultiplayerEventType_MemberLeft()
    if MultiDeviceIsHost() > 0 then 
        print("OnXblMultiplayerEventType_MemberLeft as host")
        MultiDeviceSyncAndWait("PeerUserRemoved");
        print("Removing local user from MPM")
        XblMultiplayerManagerLobbySessionRemoveLocalUser();
    else 
        print("OnXblMultiplayerEventType_MemberLeft as client")
    end
    
end

function OnXblMultiplayerEventType_JoinGameCompleted()
    MultiDeviceSyncAndWait("JoinGameCompleted");
	
    if MultiDeviceIsHost() == 0 then 
        print("Removing local user from MPM")
        XblMultiplayerManagerLobbySessionRemoveLocalUser();
    end
end

function OnXblMultiplayerEventType_UserRemoved()
    if MultiDeviceIsHost() > 0 then 
        test.stopTest();
    else
        MultiDeviceSyncAndWait("PeerUserRemoved");
        test.stopTest();
    end
end

test.ismultidevice = true
test.MPM_JoinLobbyViaActivity = function()
    common.init(MPM_JoinLobbyViaActivity_Handler)
end

