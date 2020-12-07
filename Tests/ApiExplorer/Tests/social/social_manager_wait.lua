test = require 'u-test'
common = require 'common'

function SocialManagerWait_Handler()
    StartSocialManagerDoWorkLoop()
    XblSocialManagerAddLocalUser()
end

test.skip = true;
test.SocialManagerWait = function()
    common.init(SocialManagerWait_Handler)
end

function OnXblSocialManagerDoWork_LocalUserAddedEvent()
    XblSocialManagerGetLocalUsers()
    XblSocialManagerSetRichPresencePollingStatus()
    XblSocialManagerCreateSocialUserGroupFromFilters()
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
    XblSocialManagerUserGroupGetUsers()
    XblSocialManagerPresenceRecordIsUserPlayingTitle()
end

function OnXblSocialManagerDoWork_SocialUserGroupUpdatedEvent()
end

function OnXblSocialManagerDoWork_UnknownEvent()
end
