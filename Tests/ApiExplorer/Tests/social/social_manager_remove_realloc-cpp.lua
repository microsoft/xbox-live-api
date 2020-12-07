test = require 'u-test'
common = require 'common'

function SocialManagerRemoveReallocCpp_Handler()
    print("SocialManagerListCpp_Handler")
    StartSocialManagerDoWorkLoopCpp()
    SocialManagerAddLocalUserCpp()
end

test.SocialManagerRemoveReallocCpp = function()
    common.init(SocialManagerRemoveReallocCpp_Handler)
end

function OnSocialManagerDoWorkCpp_LocalUserAddedEvent()
    group1, hr = SocialManagerCreateSocialUserGroupFromListCpp(15, 0)
    print("group1 = " .. group1)
end

local groupLoaded = 0

function OnSocialManagerDoWorkCpp_SocialUserGroupLoadedEvent()
    groupLoaded = groupLoaded + 1;
    if groupLoaded == 1 then
        print("group1 loaded")
        SocialManagerUserGroupGetUsersCpp(group1)
        SocialManagerUpdateSocialUserGroupCpp(group1, 10, 5)
    elseif groupLoaded == 2 then
        print("group2 loaded")
        print("group1 users:")
        SocialManagerUserGroupGetUsersCpp(group1)
        print("group2 users:")
        SocialManagerUserGroupGetUsersCpp(group2)
        SocialManagerDestroySocialUserGroupCpp(group1)
        SocialManagerDestroySocialUserGroupCpp(group2)
        SocialManagerRemoveLocalUserCpp()
        StopSocialManagerDoWorkLoopCpp()
        test.stopTest();
    end
end

local groupUpdate = 0;

function OnSocialManagerDoWorkCpp_SocialUserGroupUpdatedEvent()
    groupUpdate = groupUpdate + 1;
    if groupUpdate == 1 then
        group2, hr = SocialManagerCreateSocialUserGroupFromListCpp(20, 15) -- group needs to be large enough to force realloc
        print("group2 = " .. group2)
    else
        print("Unexpected GroupUpdatedEvent")
    end
end

function OnStartSocialManagerDoWorkLoopCppDisabled()
    test.stopTest();
end