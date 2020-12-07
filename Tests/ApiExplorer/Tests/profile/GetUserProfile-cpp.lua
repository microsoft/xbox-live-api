test = require 'u-test'
common = require 'common'

function GetUserProfileCpp_Handler()
    print("GetUserProfileCpp_Handler")
    ProfileServiceGetUserProfile()
end

function OnProfileServiceGetUserProfile()
    print("OnProfileServiceGetUserProfile")
    test.stopTest();
end

test.GetUserProfileCpp = function()
    common.init(GetUserProfileCpp_Handler)
end
