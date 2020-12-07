test = require 'u-test'
common = require 'common'

function GetUserProfilesCpp_Handler()
    print("GetUserProfilesCpp_Handler")
    ProfileServiceGetUserProfiles()
end

function OnProfileServiceGetUserProfiles()
    print("OnProfileServiceGetUserProfiles")
    test.stopTest();
end

test.GetUserProfilesCpp = function()
    common.init(GetUserProfilesCpp_Handler)
end
