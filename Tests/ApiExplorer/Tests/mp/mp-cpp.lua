test = require 'u-test'
common = require 'common'

function TestMPCpp_Handler()
    print("TestMPCpp_Handler")
    HCTraceSetTraceToDebugger()
    HCSettingsSetTraceLevel()

    MultiplayerSessionReferenceCreateCpp()
    MultiplayerSessionReferenceIsValidCpp()

    MultiplayerSessionCreateCpp()

    RealTimeActivityServiceActivate()
    MultiplayerServiceMultiplayerSubscriptionsEnabled()
    MultiplayerServiceEnableMultiplayerSubscriptions()
    MultiplayerServiceMultiplayerSubscriptionsEnabled()

    MultiplayerSessionCreateCpp()

    MultiplayerServiceAddMultiplayerSessionChangedHandler()
    MultiplayerServiceAddMultiplayerSubscriptionLostHandler()
    MultiplayerServiceAddMultiplayerConnectionIdChangedHandler()
    MultiplayerServiceRemoveMultiplayerSessionChangedHandler()
    MultiplayerServiceRemoveMultiplayerSubscriptionLostHandler()
    MultiplayerServiceRemoveMultiplayerConnectionIdChangedHandler()

    MultiplayerServiceAddMultiplayerSessionChangedHandler()
    MultiplayerServiceAddMultiplayerSubscriptionLostHandler()

    MultiplayerSessionReferenceParseFromUriPathCpp()

    MultiplayerSessionJoinCpp()
    MultiplayerServiceWriteSession()
end

local writeCount = 0;

function OnMultiplayerServiceWriteSession()
    writeCount = writeCount + 1
    if writeCount == 1 then
        print("OnMultiplayerServiceWriteSession 1")
        MultiplayerSessionCurrentUserCpp()
        MultiplayerSessionSetCurrentUserMemberCustomPropertyJsonCpp()
        MultiplayerSessionCurrentUserSetEncountersCpp()
        MultiplayerSessionCurrentUserSetGroupsCpp()
        MultiplayerSessionSetCurrentUserStatusCpp()
        MultiplayerSessionSetCurrentUserSecureDeviceAddressBase64Cpp()
        MultiplayerServiceWriteSession()
    elseif writeCount == 2 then
        print("OnMultiplayerServiceWriteSession 2")
        MultiplayerSessionDeleteCurrentUserMemberCustomPropertyJsonCpp()

        MultiplayerSessionSetClosedCpp()
        MultiplayerSessionSetSessionCustomPropertyJsonCpp()
        MultiplayerSessionSetAllocateCloudComputeCpp()
        MultiplayerSessionSetHostDeviceTokenCpp()
        MultiplayerSessionSetLockedCpp()
        MultiplayerSessionSetSessionChangeSubscriptionCpp()
        
        MultiplayerSessionPropertiesSetJoinRestrictionCpp()
        MultiplayerSessionPropertiesSetReadRestrictionCpp()
        MultiplayerSessionPropertiesSetKeywordsCpp()

        MultiplayerServiceWriteSession()
    elseif writeCount == 3 then
        print("OnMultiplayerServiceWriteSession 3")
        MultiplayerSessionDeleteSessionCustomPropertyJsonCpp()

        MultiplayerSessionEtagCpp()
        MultiplayerSessionGetInfoCpp()
        MultiplayerSessionGetInitializationInfoCpp()
        MultiplayerSessionGetMemberCpp()
        MultiplayerSessionMembersCpp()
        MultiplayerSessionMembersAcceptedCpp()
        MultiplayerSessionServersJsonCpp()
        MultiplayerSessionRoleTypesCpp()
        MultiplayerSessionSessionConstantsCpp()
        MultiplayerSessionSessionPropertiesCpp()
        MultiplayerSessionSessionReferenceCpp()
        MultiplayerSessionSubscribedChangeTypesCpp()
        MultiplayerSessionTimeOfSessionCpp()

        MultiplayerServiceWriteSession()
    elseif writeCount == 4 then
        print("OnMultiplayerServiceWriteSession 4")
        MultiplayerSessionLeaveCpp()
        MultiplayerServiceWriteSession()
        SetCheckHR(0)
    elseif writeCount == 5 then
        print("OnMultiplayerServiceWriteSession 5")
        local hr = GetLastError()
        SetCheckHR(1)
        if hr ~= 0 and hr ~= -2147023084 then -- --2147023084 == 0x80070714 == __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND)
            test.equal(hr, 0);
        end
        MultiplayerSessionCreateCpp()
        MultiplayerSessionJoinCpp()
        MultiplayerServiceWriteSession()
    elseif writeCount == 6 then
        print("OnMultiplayerServiceWriteSession 6")
        MultiplayerServiceWriteSession()
    elseif writeCount == 7 then
        print("OnMultiplayerServiceWriteSession 7")
        MultiplayerServiceSetActivity()
    elseif writeCount == 8 then
        print("OnMultiplayerServiceWriteSession 8")
        local hr = GetLastError()
        SetCheckHR(1)
        if hr ~= 0 and hr ~= -2147023084 then -- --2147023084 == 0x80070714 == __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND)
            test.equal(hr, 0);
        end
        MultiplayerServiceRemoveMultiplayerSessionChangedHandler()
        MultiplayerServiceRemoveMultiplayerSubscriptionLostHandler()
        test.stopTest();
    end
end

function OnMultiplayerServiceSetActivity()
    print("OnMultiplayerServiceSetActivity")
    MultiplayerServiceClearActivity()
end

function OnMultiplayerServiceClearActivity()
    print("OnMultiplayerServiceClearActivity")
    MultiplayerServiceGetCurrentSession()
end

function OnMultiplayerServiceGetCurrentSession()
    print("OnMultiplayerServiceGetCurrentSession")

    MultiplayerSessionHostCandidatesCpp()
    MultiplayerSessionMatchmakingServerCpp()
    MultiplayerSessionEtagCpp()
    MultiplayerSessionGetInfoCpp()
    MultiplayerSessionGetInitializationInfoCpp()
    MultiplayerSessionGetMemberCpp()
    MultiplayerSessionMembersCpp()
    MultiplayerSessionMembersAcceptedCpp()
    MultiplayerSessionServersJsonCpp()
    MultiplayerSessionRoleTypesCpp()
    MultiplayerSessionSessionConstantsCpp()
    MultiplayerSessionSessionPropertiesCpp()
    MultiplayerSessionSessionReferenceCpp()
    MultiplayerSessionSubscribedChangeTypesCpp()
    MultiplayerSessionTimeOfSessionCpp()

    MultiplayerSessionCompareCpp()

    MultiplayerServiceGetSessions()
end

function OnMultiplayerServiceGetSessions()
    print('OnMultiplayerServiceGetSessions')
    MultiplayerServiceGetActivitiesForUsers()
end

function OnMultiplayerServiceGetActivitiesForUsers()
    print('OnMultiplayerServiceGetActivitiesForUsers')
    MultiplayerServiceGetActivitiesForSocialGroup()
end

function OnMultiplayerServiceGetActivitiesForSocialGroup()
    print('OnMultiplayerServiceGetActivitiesForSocialGroup')
    --MultiplayerServiceWriteSessionByHandle()
	MultiplayerServiceSendInvites()
end

function OnMultiplayerServiceWriteSessionByHandle()
    print('OnMultiplayerServiceWriteSessionByHandle')
    MultiplayerServiceGetCurrentSessionByHandle()
end

function OnMultiplayerServiceGetCurrentSessionByHandle()
    print('OnMultiplayerServiceGetCurrentSessionByHandle')
    MultiplayerServiceSendInvites()
end

function OnMultiplayerServiceSendInvites()
    print('OnMultiplayerServiceSendInvites')
    MultiplayerSessionLeaveCpp()
    MultiplayerServiceWriteSession()
    SetCheckHR(0)
end

test.TestMPCpp = function()
    common.init(TestMPCpp_Handler)
end