
test = require 'u-test'
common = require 'common'

function GetUserProfile_Handler()
    print("GetUserProfile_Handler")
    XblProfileGetUserProfileAsync()
end

function OnXblProfileGetUserProfileAsync()
    print("OnXblProfileGetUserProfileAsync")
    test.stopTest();
end

test.GetUserProfile = function()
    common.init(GetUserProfile_Handler)
end
