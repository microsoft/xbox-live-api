test = require 'u-test'
common = require 'common'

function TestMatchmakingCpp_Handler()
    print("TestMatchmakingCpp_Handler")
    HCTraceSetTraceToDebugger()
    HCSettingsSetTraceLevel()

    MultiplayerSessionReferenceCreateCpp();
    MultiplayerSessionReferenceIsValidCpp();

    MultiplayerSessionCreateCpp()

    MultiplayerSessionJoinCpp()
    MultiplayerServiceWriteSession()
end

local writeCount = 0;
function OnMultiplayerServiceWriteSession()    
    writeCount = writeCount + 1
    if writeCount == 1 then
        -- triggers from TestMatchmaking_Handler
        print("OnMultiplayerServiceWriteSession 1")
        MatchmakingServiceCreateTicket();
        MultiplayerSessionSetMatchmakingResubmitCpp()
        MultiplayerSessionSetServerConnectionStringCandidatesCpp() 
        MultiplayerSessionSetMatchmakingServerConnectionPathCpp()
        MultiplayerSessionSetMatchmakingTargetSessionConstantsJsonCpp()
        MultiplayerServiceWriteSession()
    elseif writeCount == 2 then
        -- triggers from OnXblMultiplayerWriteSessionAsync when writeCount == 1
        print("OnMultiplayerServiceWriteSession 2")
        MultiplayerSessionHostCandidatesCpp()
        MultiplayerSessionMatchmakingServerCpp()
    elseif writeCount == 3 then
        -- triggers from OnXblMatchmakingDeleteMatchTicket
        print("OnMultiplayerServiceWriteSession 3")
        local hr = GetLastError()
        SetCheckHR(1)
        if hr ~= 0 and hr ~= -2147023084 then -- --2147023084 == 0x80070714 == __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND)
            test.equal(hr, 0);
        end
        test.stopTest();
    end
end
function OnMatchmakingServiceCreateTicket()
    print("OnMatchmakingServiceCreateTicket")
    MatchmakingServiceGetMatchTicketDetails();
end

function OnMatchmakingServiceGetMatchTicketDetails()
    print("OnMatchmakingServiceGetMatchTicketDetails")
    MatchmakingServiceGetHopperStatistics();
end

function OnMatchmakingServiceGetHopperStatistics()
    print("OnMatchmakingServiceGetHopperStatistics")
    MatchmakingServiceDeleteMatchTicket();
end

function OnMatchmakingServiceDeleteMatchTicket()
    print("OnMatchmakingServiceDeleteMatchTicket")
    MultiplayerSessionLeaveCpp()
    MultiplayerServiceWriteSession()
    SetCheckHR(0)
end

test.TestMatchmaking_SingleDevice_Cpp = function()
    common.init(TestMatchmakingCpp_Handler)
end