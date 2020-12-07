test = require 'u-test'
common = require 'common'

function SocialManagerList_Handler()
    print("SocialManagerList_Handler")
    StartSocialManagerDoWorkLoop()
    XblSocialManagerAddLocalUser()
end

test.SocialManagerList = function()
    common.init(SocialManagerList_Handler)
end

function OnXblSocialManagerDoWork_LocalUserAddedEvent()
    print("OnXblSocialManagerDoWork_LocalUserAddedEvent")
    XblPresenceSetPresenceAsync()
end

function OnXblPresenceSetPresenceAsync()
    print("OnXblPresenceSetPresenceAsync")
    XblSocialManagerGetLocalUsers()
    group, hr = XblSocialManagerCreateSocialUserGroupFromList()
    print("group " .. group)
end

function OnXblSocialManagerDoWork_SocialUserGroupLoadedEvent()
    print("OnXblSocialManagerDoWork_SocialUserGroupLoadedEvent")
    XblSocialManagerUserGroupGetUsers()
    XblSocialManagerUpdateSocialUserGroup()
end

function OnXblSocialManagerDoWork_SocialUserGroupUpdatedEvent()
    print("OnXblSocialManagerDoWork_SocialUserGroupUpdatedEvent")
    XblSocialManagerUserGroupGetUsersTrackedByGroup()
    XblSocialManagerDestroySocialUserGroup(group)
    XblSocialManagerRemoveLocalUser()
    StopSocialManagerDoWorkLoop()
    print("stopping test")
    test.stopTest();
end
