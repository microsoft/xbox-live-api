test = require 'u-test'
common = require 'common'

function SocialManagerFilter_Handler()
    StartSocialManagerDoWorkLoop()
    XblSocialManagerAddLocalUser()
end

test.SocialManagerFilter = function()
    common.init(SocialManagerFilter_Handler)
end

function OnXblSocialManagerDoWork_LocalUserAddedEvent()
    XblSocialManagerGetLocalUsers()
    XblSocialManagerSetRichPresencePollingStatus()
    group, hr = XblSocialManagerCreateSocialUserGroupFromFilters()
    XblSocialManagerUserGroupGetUsers()
    print("group " .. group)
end

function OnXblSocialManagerDoWork_SocialUserGroupLoadedEvent()
    XblSocialManagerUserGroupGetUsers()
    XblSocialManagerPresenceRecordIsUserPlayingTitle()
    XblSocialManagerDestroySocialUserGroup(group)
    XblSocialManagerRemoveLocalUser()
    StopSocialManagerDoWorkLoop()
    test.stopTest();
end
