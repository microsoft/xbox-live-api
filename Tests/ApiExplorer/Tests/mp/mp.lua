test = require 'u-test'
common = require 'common'

function TestMP_Handler()
    print("TestMP_Handler")
    HCTraceSetTraceToDebugger()
    HCSettingsSetTraceLevel()
    XblFormatSecureDeviceAddress()

    XblMultiplayerSessionReferenceCreate()
    XblMultiplayerSessionReferenceIsValid()

    XblMultiplayerSessionCreateHandle()
    XblMultiplayerSessionDuplicateHandle()
    XblMultiplayerSessionCloseHandle()

    --XblRealTimeActivityActivate()
    XblMultiplayerSubscriptionsEnabled()
    --XblMultiplayerSetSubscriptionsEnabled()
    XblMultiplayerSubscriptionsEnabled()

    XblMultiplayerSessionCreateHandle()

    -- XblMultiplayerSessionSetInitializationSucceeded() --The session's 'initializationSucceeded' property can only be set during the 'evaluating' stage of initialization.
    -- XblMultiplayerSessionSetRawServersJson() -- The request body can't contain a 'servers' collection because the authentication principal doesn't include a server.

    -- The constant field **** is specified in the request, but the value conflicts with the one in the session.
    --XblMultiplayerSessionConstantsSetMaxMembersInSession()
    --XblMultiplayerSessionConstantsSetCapabilities()
    --XblMultiplayerSessionConstantsSetCloudComputePackageJson()
    --XblMultiplayerSessionConstantsSetMeasurementServerAddressesJson()
    --XblMultiplayerSessionConstantsSetMemberInitialization()
    --XblMultiplayerSessionConstantsSetPeerToHostRequirements()
    --XblMultiplayerSessionConstantsSetPeerToPeerRequirements()
    --XblMultiplayerSessionConstantsSetQosConnectivityMetrics()
    --XblMultiplayerSessionConstantsSetTimeouts()
    --XblMultiplayerSessionConstantsSetVisibility()

    XblMultiplayerAddSessionChangedHandler()
    XblMultiplayerAddSubscriptionLostHandler()
    XblMultiplayerRemoveSessionChangedHandler()
    XblMultiplayerRemoveSubscriptionLostHandler()

    XblMultiplayerAddSessionChangedHandler()
    XblMultiplayerAddSubscriptionLostHandler()

    XblMultiplayerSessionReferenceParseFromUriPath()

    XblMultiplayerSessionJoin()
    XblMultiplayerWriteSessionAsync()
end

local writeCount = 0;

function OnXblMultiplayerWriteSessionAsync()
    writeCount = writeCount + 1
    if writeCount == 1 then
        print("OnXblMultiplayerWriteSessionAsync 1")

        XblMultiplayerSessionCurrentUser()
        XblMultiplayerSessionCurrentUserSetCustomPropertyJson()
        XblMultiplayerSessionCurrentUserSetEncounters()
        XblMultiplayerSessionCurrentUserSetGroups()
        XblMultiplayerSessionCurrentUserSetStatus()
        XblMultiplayerSessionCurrentUserSetMembersInGroup()
        XblMultiplayerSessionCurrentUserSetSecureDeviceAddressBase64()
        XblMultiplayerWriteSessionAsync()
    elseif writeCount == 2 then
        print("OnXblMultiplayerWriteSessionAsync 2")
        XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson()

        XblMultiplayerSessionSetClosed()
        XblMultiplayerSessionSetCustomPropertyJson()
        XblMultiplayerSessionSetAllocateCloudCompute()
        XblMultiplayerSessionSetHostDeviceToken()
        XblMultiplayerSessionSetLocked()
        XblMultiplayerSessionSetSessionChangeSubscription()
        
        XblMultiplayerSessionPropertiesSetJoinRestriction()
        XblMultiplayerSessionPropertiesSetReadRestriction()
        XblMultiplayerSessionPropertiesSetKeywords()
        -- XblMultiplayerSessionCurrentUserSetRoles() -- A member's 'roles' section contains a role type with an invalid name: roleTypeName1
        -- XblMultiplayerSessionSetMutableRoleSettings() -- need properly formated JSON to call
        -- XblMultiplayerSessionPropertiesSetReadRestriction() -- Invalid session 'readRestriction' provided, cannot be set to none on sessions with the 'userAuthorizationStyle' capability.
        -- XblMultiplayerSessionPropertiesSetTurnCollection() -- The session's 'turn' property must be an array of numbers representing the indicies of the member(s) in the session whose turn it is.
        --XblMultiplayerSessionCurrentUserSetQosMeasurements() -- Invalid member 'measurements' provided, measurements can only be uploaded if session 'metrics' are configured.
        --XblMultiplayerSessionCurrentUserSetServerQosMeasurements() -- Invalid member property serverMeasurements 'measurements1' provided, must be a JSON object.

        XblMultiplayerWriteSessionAsync()
    elseif writeCount == 3 then
        print("OnXblMultiplayerWriteSessionAsync 3")

        XblMultiplayerSessionSetSessionChangeSubscription(0)

        XblMultiplayerWriteSessionAsync()
    elseif writeCount == 4 then
        print("OnXblMultiplayerWriteSessionAsync 4")

        XblMultiplayerSessionDeleteCustomPropertyJson()

        XblMultiplayerSessionEtag()
        XblMultiplayerSessionGetInfo()
        XblMultiplayerSessionGetInitializationInfo()
        XblMultiplayerSessionGetMember()
        XblMultiplayerSessionGetRoleByName()
        XblMultiplayerSessionMembers()
        XblMultiplayerSessionMembersAccepted()
        XblMultiplayerSessionRawServersJson()
        XblMultiplayerSessionRoleTypes()
        XblMultiplayerSessionSessionConstants()
        XblMultiplayerSessionSessionProperties()
        XblMultiplayerSessionSessionReference()
        XblMultiplayerSessionSubscribedChangeTypes()
        XblMultiplayerSessionTimeOfSession()

        XblMultiplayerWriteSessionAsync()
    elseif writeCount == 5 then
        print("OnXblMultiplayerWriteSessionAsync 5")
        XblMultiplayerSessionLeave()
        XblMultiplayerWriteSessionAsync()
        SetCheckHR(0)
    elseif writeCount == 6 then
        local hr = GetLastError()
        SetCheckHR(1)
        if hr ~= 0 and hr ~= -2147023084 then -- --2147023084 == 0x80070714 == __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND)
            test.equal(hr, 0);
        end
        XblMultiplayerSessionCloseHandle()
        print("OnXblMultiplayerWriteSessionAsync 6")
        XblMultiplayerSessionCreateHandle()
        XblMultiplayerSessionJoin()
        XblMultiplayerWriteSessionAsync()
    elseif writeCount == 7 then
        print("OnXblMultiplayerWriteSessionAsync 7")
        -- XblMultiplayerSessionAddMemberReservation() -- The requested session cannot be accessed. The calling user must have the multiplayer privilege and must be a member of the session if the session either isn't open or has a join restriction on it that the user doesn't satisfy, multi-user requests aren't allowed for large sessions, banned xuids can't access the session, users must have the communicate permission to access sessions that require it, users must qualify to access club sessions based on the club rules, and devices other than an Xbox One can only access sessions with user-style authorization.
        XblMultiplayerWriteSessionAsync()
    elseif writeCount == 8 then
        print("OnXblMultiplayerWriteSessionAsync 8")
        XblMultiplayerSetActivityAsync()
    elseif writeCount == 9 then
        print("OnXblMultiplayerWriteSessionAsync 9")
        local hr = GetLastError()
        SetCheckHR(1)
        if hr ~= 0 and hr ~= -2147023084 then -- --2147023084 == 0x80070714 == __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND)
            test.equal(hr, 0);
        end
        XblMultiplayerSessionCloseHandle()
        XblMultiplayerRemoveSessionChangedHandler()
        XblMultiplayerRemoveSubscriptionLostHandler()
        test.stopTest();
    end
end

function OnXblMultiplayerSetActivityAsync()
    print("OnXblMultiplayerSetActivityAsync")
    XblMultiplayerClearActivityAsync()
end

function OnXblMultiplayerClearActivityAsync()
    print("OnXblMultiplayerClearActivityAsync")
    XblMultiplayerGetSessionAsync()
end

function OnXblMultiplayerGetSessionAsync()
    print("OnXblMultiplayerGetSessionAsync")

    XblMultiplayerSessionHostCandidates()
    XblMultiplayerSessionMatchmakingServer()
    XblMultiplayerSessionEtag()
    XblMultiplayerSessionGetInfo()
    XblMultiplayerSessionGetInitializationInfo()
    XblMultiplayerSessionGetMember()
    XblMultiplayerSessionGetRoleByName()
    XblMultiplayerSessionMembers()
    XblMultiplayerSessionMembersAccepted()
    XblMultiplayerSessionRawServersJson()
    XblMultiplayerSessionRoleTypes()
    XblMultiplayerSessionSessionConstants()
    XblMultiplayerSessionSessionProperties()
    XblMultiplayerSessionSessionReference()
    XblMultiplayerSessionSubscribedChangeTypes()
    XblMultiplayerSessionTimeOfSession()

    XblMultiplayerSessionCompare();

    XblMultiplayerQuerySessionsAsync()
end

function OnXblMultiplayerQuerySessionsAsync()
    print('OnXblMultiplayerQuerySessionsAsync')
    XblMultiplayerGetActivitiesForUsersAsync()
end

function OnXblMultiplayerGetActivitiesForUsersAsync()
    print('OnXblMultiplayerGetActivitiesForUsersAsync')
    XblMultiplayerGetActivitiesWithPropertiesForUsersAsync()
end

function OnXblMultiplayerGetActivitiesWithPropertiesForUsersAsync()
    print('OnXblMultiplayerGetActivitiesWithPropertiesForUsersAsync')
    XblMultiplayerGetActivitiesForSocialGroupAsync()
end

function OnXblMultiplayerGetActivitiesForSocialGroupAsyncRetry()
    print('OnXblMultiplayerGetActivitiesForSocialGroupAsyncRetry')
    XblMultiplayerGetActivitiesForSocialGroupAsync()
end

function OnXblMultiplayerGetActivitiesForSocialGroupAsync()
    print('OnXblMultiplayerGetActivitiesForSocialGroupAsync')
    XblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync()
end

function OnXblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync()
    print('OnXblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync')
    XblMultiplayerSendInvitesAsync()
    -- XblMultiplayerWriteSessionByHandleAsync() -- The request URI contains an invalid handle ID. It may contain invalid characters or be all zeroes.
end

function OnXblMultiplayerWriteSessionByHandleAsync()
    print('OnXblMultiplayerWriteSessionByHandleAsync')
    XblMultiplayerGetSessionByHandleAsync()
end

function OnXblMultiplayerGetSessionByHandleAsync()
    print('OnXblMultiplayerGetSessionByHandleAsync')
    XblMultiplayerSendInvitesAsync()
end

function OnXblMultiplayerSendInvitesAsync()
    print('OnXblMultiplayerSendInvitesAsync')
    XblMultiplayerSessionLeave()
    XblMultiplayerWriteSessionAsync()
    SetCheckHR(0)
end

test.TestMP = function()
    common.init(TestMP_Handler)
end