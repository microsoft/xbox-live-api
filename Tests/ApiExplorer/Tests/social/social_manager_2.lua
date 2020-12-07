test = require 'u-test'
common = require 'common'

function SocialManager2_Handler()
    StartSocialManagerDoWorkLoop()
    XblSocialManagerAddLocalUser()
end

test.SocialManager2 = function()
    common.init(SocialManager2_Handler)
end

function OnXblSocialManagerDoWork_LocalUserAddedEvent()
    XblSocialManagerGetLocalUsers()
    Sleep(1000)
    StopSocialManagerDoWorkLoop()
    test.stopTest();
end

function OnXblSocialManagerDoWork_LocalUserRemovedEvent()
end

function OnXblSocialManagerDoWork_UsersAddedToSocialGraphEvent()
end

function OnXblSocialManagerDoWork_UsersRemovedFromSocialGraphEvent()
end

function OnXblSocialManagerDoWork_PresenceChangedEvent()
end

function OnXblSocialManagerDoWork_ProfilesChangedEvent()
end

function OnXblSocialManagerDoWork_SocialRelationshipsChangedEvent()
end

function OnXblSocialManagerDoWork_SocialUserGroupLoadedEvent()
end

function OnXblSocialManagerDoWork_SocialUserGroupUpdatedEvent()
end

function OnXblSocialManagerDoWork_UnknownEvent()
end
