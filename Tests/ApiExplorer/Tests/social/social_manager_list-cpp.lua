test = require 'u-test'
common = require 'common'

function SocialManagerListCpp_Handler()
    print("SocialManagerListCpp_Handler")
    StartSocialManagerDoWorkLoopCpp()
    SocialManagerAddLocalUserCpp()
end

test.skip = true
test.SocialManagerListCpp = function()
    common.init(SocialManagerListCpp_Handler)
end

function OnSocialManagerDoWorkCpp_LocalUserAddedEvent()
    print("OnSocialManagerDoWorkCpp_LocalUserAddedEvent")
    PresenceServiceSetPresence()
end

function OnPresenceServiceSetPresence()
    print("OnPresenceServiceSetPresence")
    SocialManagerGetLocalUsersCpp()
    group, hr = SocialManagerCreateSocialUserGroupFromListCpp()
    print("group " .. group)
end

function OnSocialManagerDoWorkCpp_SocialUserGroupLoadedEvent()
    print("OnSocialManagerDoWorkCpp_SocialUserGroupLoadedEvent")
    SocialManagerUserGroupGetUsersCpp()
    SocialManagerUpdateSocialUserGroupCpp()
end

function OnSocialManagerDoWorkCpp_SocialUserGroupUpdatedEvent()
    print("OnSocialManagerDoWorkCpp_SocialUserGroupUpdatedEvent")
    SocialManagerUserGroupGetUsersTrackedByGroupCpp()
    SocialManagerDestroySocialUserGroupCpp(group)
    SocialManagerRemoveLocalUserCpp()
    StopSocialManagerDoWorkLoopCpp()
    print("stopping test")
    test.stopTest();
end

function OnStartSocialManagerDoWorkLoopCppDisabled()
    test.stopTest();
end
