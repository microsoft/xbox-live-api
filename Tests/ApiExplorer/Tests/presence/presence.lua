test = require 'u-test'
common = require 'common'

function TestPresenceHandler()
    print("TestPresenceHandler")
    XblPresenceSetPresenceAsync()
end

function OnXblPresenceSetPresenceAsync()
    XblPresenceGetPresenceAsync()
end

function OnXblPresenceGetPresenceAsync()
    XblPresenceRecordGetXuid()
    XblPresenceRecordGetUserState()
    XblPresenceRecordGetDeviceRecords()
    XblPresenceRecordCloseHandle()
    XblPresenceGetPresenceForSocialGroupAsync()
end

function OnXblPresenceGetPresenceForSocialGroupAsync()
    XblPresenceGetPresenceForMultipleUsersAsync()
end

function OnXblPresenceGetPresenceForMultipleUsersAsync()
    test.stopTest();
end

test.TestPresence = function()
    common.init(TestPresenceHandler)
end

