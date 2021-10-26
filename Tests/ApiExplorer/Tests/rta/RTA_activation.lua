test = require 'u-test'
common = require 'common'

-- Test RTA connection state based on various states of SM, Client handlers, and Legacy Activation

function RTAActivation_Handler()
    print("RTAActivation_Handler")

    -- Test isn't valid on all platforms (i.e. on Win32 we set up a notification service subscription by default so the RTA connection
    -- won't be torn down just by removing the social relationship changed handler).
    isGdk = IsGDKPlatform()
    if isGdk then
        StartSocialManagerDoWorkLoop()
        XblRealTimeActivityAddConnectionStateChangeHandler()
        XblSocialAddSocialRelationshipChangedHandler()
    else
        test.stopTest()
    end
end

function OnXblRealTimeActivityAddConnectionStateChangeHandler_Connecting()
    print("RTA Connecting");
end

connectCount = 0
function OnXblRealTimeActivityAddConnectionStateChangeHandler_Connected()
    print("RTA Connected");
    connectCount = connectCount + 1

    if connectCount == 1 then
        -- Removing the only handler should cause disconnection
        XblSocialRemoveSocialRelationshipChangedHandler()
    elseif connectCount == 2 then
        XblSocialManagerRemoveLocalUser()
    elseif connectCount == 3 then
        -- User will be removed from SM in response to the LocalUserAdded event, which should cause a disconnect
        XblSocialManagerAddLocalUser()
        XblRealTimeActivityDeactivate()
    end
end

disconnectCount = 0
function OnXblRealTimeActivityAddConnectionStateChangeHandler_Disconnected()
    print("RTA Disconnected")
    disconnectCount = disconnectCount + 1

    if disconnectCount == 1 then
        -- Adding user to SM will cause connection to be established again. There is not a guaranteed ordering
        -- between the RTA connected event and the SM local user added event. That said, we can always remove the local
        -- from SM when the RTA connected event happens, because of the fact that it is valid to remove a user from 
        -- SocialManager even before the LocalUserAdded event is received
        XblSocialManagerAddLocalUser()
    elseif disconnectCount == 2 then
        XblRealTimeActivityActivate()
    elseif disconnectCount == 3 then
        test.stopTest()
    end
end

function OnXblSocialManagerDoWork_LocalUserAddedEvent()
    print("SocialManager LocalUserAdded")
    if connectCount == 3 then
        XblSocialManagerRemoveLocalUser()
    end
end

test.RTAActivation = function()
    common.init(RTAActivation_Handler)
end