test = require 'u-test'
common = require 'common'

function SocialManager1Cpp_Handler()
    StartSocialManagerDoWorkLoopCpp()
    Sleep(1000)
    StopSocialManagerDoWorkLoopCpp()
    test.stopTest();
end

test.SocialManager1Cpp = function()
    common.init(SocialManager1Cpp_Handler)
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

function OnSocialManagerDoWorkCpp_LocalUserAddedEvent()
end

function OnSocialManagerDoWorkCpp_LocalUserRemovedEvent()
end

function OnSocialManagerDoWorkCpp_SocialUserGroupLoadedEvent()
end

function OnSocialManagerDoWorkCpp_SocialUserGroupUpdatedEvent()
end

function OnSocialManagerDoWorkCpp_UnknownEvent()
end
