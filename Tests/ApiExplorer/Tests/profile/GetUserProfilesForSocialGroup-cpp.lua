test = require 'u-test'
common = require 'common'

function GetUserProfilesForSocialGroupCpp_Handler()
    print("GetUserProfilesForSocialGroupCpp_Handler")
    ProfileServiceGetUserProfilesForSocialGroup()
end

function OnProfileServiceGetUserProfilesForSocialGroup()
    print("OnProfileServiceGetUserProfilesForSocialGroup")
    test.stopTest();
end

test.GetUserProfilesForSocialGroupCpp = function()
    common.init(GetUserProfilesForSocialGroupCpp_Handler)
end
