
test = require 'u-test'
common = require 'common'

function GetUserProfiles_Handler()
    print("GetUserProfiles_Handler")
    XblProfileGetUserProfilesAsync()
end

function OnXblProfileGetUserProfilesAsync()
    print("OnXblProfileGetUserProfilesAsync")
    test.stopTest();
end

test.GetUserProfiles = function()
    common.init(GetUserProfiles_Handler)
end
