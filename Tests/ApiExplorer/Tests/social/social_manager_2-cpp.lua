test = require 'u-test'
common = require 'common'

function SocialManager2Cpp_Handler()
    StartSocialManagerDoWorkLoopCpp()
    SocialManagerAddLocalUserCpp()
end

test.SocialManager2Cpp = function()
    common.init(SocialManager2Cpp_Handler)
end

function OnSocialManagerDoWorkCpp_LocalUserAddedEvent()
    SocialManagerGetLocalUsersCpp()
    Sleep(1000)
    StopSocialManagerDoWorkLoopCpp()
    test.stopTest();
end

function OnSocialManagerDoWorkCpp_LocalUserRemovedEvent()
end

function OnSocialManagerDoWorkCpp_UsersAddedToSocialGraphEvent()
end

function OnSocialManagerDoWorkCpp_UsersRemovedFromSocialGraphEvent()
end

function OnSocialManagerDoWorkCpp_PresenceChangedEvent()
end

function OnSocialManagerDoWorkCpp_ProfilesChangedEvent()
end

function OnSocialManagerDoWorkCpp_SocialRelationshipsChangedEvent()
end

function OnSocialManagerDoWorkCpp_SocialUserGroupLoadedEvent()
end

function OnSocialManagerDoWorkCpp_SocialUserGroupUpdatedEvent()
end

function OnSocialManagerDoWorkCpp_UnknownEvent()
end

function OnStartSocialManagerDoWorkLoopCppDisabled()
    test.stopTest();
end