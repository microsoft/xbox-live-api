test = require 'u-test'
common = require 'common'

function PrivacyTestsCpp_Handler()
    print("PrivacyTestsCpp_Handler")
    PrivacyServiceGetAvoidList()
end

function OnPrivacyServiceGetAvoidList()
    print("OnPrivacyServiceGetAvoidList")
    PrivacyServiceCheckPermissionWithTargetUser()
end

function OnPrivacyServiceCheckPermissionWithTargetUser()
    print("OnPrivacyServiceCheckPermissionWithTargetUser")
    PrivacyServiceCheckMultiplePermissionsWithMultipleTargetUsers()
end

function OnPrivacyServiceCheckMultiplePermissionsWithMultipleTargetUsers()
    print("OnPrivacyServiceCheckMultiplePermissionsWithMultipleTargetUsers")
    PrivacyServiceGetMuteList()
end

function OnPrivacyServiceGetMuteList()
    print("OnPrivacyServiceGetMuteList")
    PrivacyServiceGetAvoidOrMuteList()
end

function OnPrivacyServiceGetAvoidOrMuteList()
    print("OnPrivacyServiceGetAvoidOrMuteList")
    test.stopTest();
end

test.PrivacyTestsCpp = function()
    common.init(PrivacyTestsCpp_Handler)
end