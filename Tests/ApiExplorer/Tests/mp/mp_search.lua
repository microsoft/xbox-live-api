
test = require 'u-test'
common = require 'common'

function TestMPSearch_Handler()
    print("TestMPSearch_Handler")
    XblMultiplayerSessionReferenceCreate()
    XblMultiplayerSessionCreateHandle()
    XblMultiplayerSessionJoin()
    XblMultiplayerSessionSetCustomPropertyJson()
    XblMultiplayerWriteSessionAsync()
end

function OnXblMultiplayerWriteSessionAsync()
    XblMultiplayerCreateSearchHandleAsync()
end

function OnXblMultiplayerCreateSearchHandleAsync()
    XblMultiplayerSearchHandleGetSessionOwnerXuids()
    XblMultiplayerSearchHandleGetTags()
    XblMultiplayerSearchHandleGetStringAttributes()
    XblMultiplayerSearchHandleGetNumberAttributes()
    XblMultiplayerSearchHandleGetVisibility()
    XblMultiplayerSearchHandleGetJoinRestriction()
    XblMultiplayerSearchHandleGetSessionClosed()
    XblMultiplayerSearchHandleGetMemberCounts()
    XblMultiplayerSearchHandleGetCreationTime()
    XblMultiplayerSearchHandleGetCustomSessionPropertiesJson()
    XblMultiplayerGetSearchHandlesAsync()
end

function OnXblMultiplayerGetSearchHandlesAsync()
    XblMultiplayerDeleteSearchHandleAsync()
end

function OnXblMultiplayerDeleteSearchHandleAsync()
    XblMultiplayerSearchHandleCloseHandle()
    XblMultiplayerSessionCloseHandle()
    test.stopTest();
end

test.TestMPSearch = function()
    common.init(TestMPSearch_Handler)
end