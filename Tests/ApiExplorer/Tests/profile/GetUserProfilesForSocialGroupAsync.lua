
test = require 'u-test'
common = require 'common'

function GetUserProfilesForSocialGroupAsync_Handler()
    print("GetUserProfilesForSocialGroupAsync_Handler")
    XblProfileGetUserProfilesForSocialGroupAsync()
end

function OnXblProfileGetUserProfilesForSocialGroupAsync()
    print("OnXblProfileGetUserProfilesForSocialGroupAsyncRetry")
    XblProfileGetUserProfilesForSocialGroupAsync()
end

function OnXblProfileGetUserProfilesForSocialGroupAsync()
    print("OnXblProfileGetUserProfilesForSocialGroupAsync")
    test.stopTest();
end

test.GetUserProfilesForSocialGroupAsync = function()
    common.init(GetUserProfilesForSocialGroupAsync_Handler)
end
