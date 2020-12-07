test = require 'u-test'
common = require 'common'

function TestPresenceCppHandler()
    print("TestPresenceCppHandler")
  
    PresenceServiceSetPresence()
end

function OnPresenceServiceSetPresence()
    PresenceServiceGetPresence()
end

function OnPresenceServiceGetPresence()
    PresenceRecordGetXuidCpp()
    PresenceRecordGetUserStateCpp()
    PresenceRecordGetDeviceRecordsCpp()
	PresenceRecordCloseHandleCpp()
    PresenceServiceGetPresenceForSocialGroup()
end

function OnPresenceServiceGetPresenceForSocialGroup()
    PresenceServiceGetPresenceForMultipleUsers()
end

function OnPresenceServiceGetPresenceForMultipleUsers()
    test.stopTest();
end

test.TestPresenceCpp = function()
    common.init(TestPresenceCppHandler)
end

