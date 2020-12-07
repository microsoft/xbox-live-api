test = require 'u-test'
common = require 'common'

function PrivacyTests_Handler()
    print("PrivacyTests_Handler")
    XblPrivacyGetAvoidListAsync()
end

function OnXblPrivacyGetAvoidListAsync()
    print("OnXblPrivacyGetAvoidListAsync")
    XblPrivacyCheckPermissionAsync()
end

function OnXblPrivacyCheckPermissionAsync()
    print("OnXblPrivacyCheckPermissionAsync")
    XblPrivacyCheckPermissionForAnonymousUserAsync()
end

function OnXblPrivacyCheckPermissionForAnonymousUserAsync()
    print("OnOnXblPrivacyCheckPermissionForAnonymousUserAsync")
    XblPrivacyBatchCheckPermissionAsync()
end

function OnXblPrivacyBatchCheckPermissionAsync()
    print("OnXblPrivacyBatchCheckPermissionAsync")
    test.stopTest();
end

test.PrivacyTests = function()
    common.init(PrivacyTests_Handler)
end