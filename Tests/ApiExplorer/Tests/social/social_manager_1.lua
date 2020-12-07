test = require 'u-test'
common = require 'common'

function SocialManager1_Handler()
    StartSocialManagerDoWorkLoop()
    Sleep(1000)
    StopSocialManagerDoWorkLoop()
    test.stopTest();
end

test.SocialManager1 = function()
    common.init(SocialManager1_Handler)
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

function OnXblSocialManagerDoWork_LocalUserAddedEvent()
end

function OnXblSocialManagerDoWork_LocalUserRemovedEvent()
end

function OnXblSocialManagerDoWork_SocialUserGroupLoadedEvent()
end

function OnXblSocialManagerDoWork_SocialUserGroupUpdatedEvent()
end

function OnXblSocialManagerDoWork_UnknownEvent()
end
