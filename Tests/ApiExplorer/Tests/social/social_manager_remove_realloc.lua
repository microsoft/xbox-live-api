test = require 'u-test'
common = require 'common'

function SocialManagerRemoveRealloc_Handler()
    print("SocialManagerList_Handler")
    StartSocialManagerDoWorkLoop()
    XblSocialManagerAddLocalUser()
end

test.SocialManagerRemoveRealloc = function()
    common.init(SocialManagerRemoveRealloc_Handler)
end

function OnXblSocialManagerDoWork_LocalUserAddedEvent()
    group1, hr = XblSocialManagerCreateSocialUserGroupFromList(15, 0)
    print("group1 = " .. group1)
end

local groupLoaded = 0

function OnXblSocialManagerDoWork_SocialUserGroupLoadedEvent()
    groupLoaded = groupLoaded + 1;
    if groupLoaded == 1 then
        print("group1 loaded")
        XblSocialManagerUserGroupGetUsers(group1)
        XblSocialManagerUpdateSocialUserGroup(group1, 10, 5)
    elseif groupLoaded == 2 then
        print("group2 loaded")
        print("group1 users:")
        XblSocialManagerUserGroupGetUsers(group1)
        print("group2 users:")
        XblSocialManagerUserGroupGetUsers(group2)
        XblSocialManagerDestroySocialUserGroup(group1)
        XblSocialManagerDestroySocialUserGroup(group2)
        XblSocialManagerRemoveLocalUser()
        StopSocialManagerDoWorkLoop()
        test.stopTest();
    end
end

local groupUpdate = 0;

function OnXblSocialManagerDoWork_SocialUserGroupUpdatedEvent()
    groupUpdate = groupUpdate + 1;
    if groupUpdate == 1 then
        group2, hr = XblSocialManagerCreateSocialUserGroupFromList(20, 15) -- group needs to be large enough to force realloc
        print("group2 = " .. group2)
    else
        print("Unexpected GroupUpdatedEvent")
    end
end
