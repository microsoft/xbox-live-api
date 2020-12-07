test = require 'u-test'
common = require 'common'

function TestMPSearchCpp_Handler()
    print("TestMPSearchCpp_Handler")
    MultiplayerSessionReferenceCreateCpp()
    MultiplayerSessionCreateCpp()
    MultiplayerSessionJoinCpp()
    MultiplayerSessionSetSessionCustomPropertyJsonCpp()
    MultiplayerServiceWriteSession()
end

function OnMultiplayerServiceWriteSession()
    MultiplayerServiceSetSearchHandle()
end

function OnMultiplayerServiceSetSearchHandle()
    MultiplayerServiceGetSearchHandles()
end

function OnMultiplayerServiceGetSearchHandles()
    MultiplayerSearchHandleDetailsSessionOwnerXuids()
    MultiplayerSearchHandleDetailsTags()
    MultiplayerSearchHandleDetailsStringsMetadata()
    MultiplayerSearchHandleDetailsNumbersMetadata()
    MultiplayerSearchHandleDetailsVisibility()
    MultiplayerSearchHandleDetailsJoinRestriction()
    MultiplayerSearchHandleDetailsClosed()
    MultiplayerSearchHandleDetailsMemberCounts()
    MultiplayerSearchHandleDetailsHandleCreationTime()
    MultiplayerSearchHandleDetailsCustomSessionPropertiesJson()
    MultiplayerSearchHandleDetailsCloseHandle()
    test.stopTest();
end

test.TestMPSearchCpp = function()
    common.init(TestMPSearchCpp_Handler)
end