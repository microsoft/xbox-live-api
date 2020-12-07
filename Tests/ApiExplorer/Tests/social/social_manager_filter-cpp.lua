test = require 'u-test'
common = require 'common'

function SocialManagerFilterCpp_Handler()
    StartSocialManagerDoWorkLoopCpp()
    SocialManagerAddLocalUserCpp()
end

test.SocialManagerFilterCpp = function()
    common.init(SocialManagerFilterCpp_Handler)
end

function OnSocialManagerDoWorkCpp_LocalUserAddedEvent()
    SocialManagerGetLocalUsersCpp()
    SocialManagerSetRichPresencePollingStatusCpp()
    group, hr = SocialManagerCreateSocialUserGroupFromFiltersCpp()
    print("group " .. group)
end

function OnSocialManagerDoWorkCpp_SocialUserGroupLoadedEvent()
    SocialManagerUserGroupGetUsersCpp()
    SocialManagerPresenceRecordIsUserPlayingTitleCpp()
    SocialManagerDestroySocialUserGroupCpp(group)
    SocialManagerRemoveLocalUserCpp()
    StopSocialManagerDoWorkLoopCpp()
    test.stopTest();
end

function OnStartSocialManagerDoWorkLoopCppDisabled()
    test.stopTest();
end
