test = require 'u-test'
common = require 'common'

function TestMatchmaking_Handler()
    print("TestMP_Handler")
    HCTraceSetTraceToDebugger()
    HCSettingsSetTraceLevel()

    XblMultiplayerSessionReferenceCreate();
    XblMultiplayerSessionReferenceIsValid();

    XblMultiplayerSessionCreateHandle()

    XblMultiplayerSessionJoin()
    XblMultiplayerWriteSessionAsync()
end

local writeCount = 0;
function OnXblMultiplayerWriteSessionAsync()    
    writeCount = writeCount + 1
    if writeCount == 1 then
        -- triggers from TestMatchmaking_Handler
        print("OnXblMultiplayerWriteSessionAsync 1")
        XblMatchmakingCreateTicket();
        XblMultiplayerSessionSetMatchmakingResubmit()
        XblMultiplayerSessionSetServerConnectionStringCandidates() 
        XblMultiplayerSessionSetMatchmakingServerConnectionPath()
        XblMultiplayerSessionSetMatchmakingTargetSessionConstantsJson()
        XblMultiplayerWriteSessionAsync()
    elseif writeCount == 2 then
        -- triggers from OnXblMultiplayerWriteSessionAsync when writeCount == 1
        print("OnXblMultiplayerWriteSessionAsync 2")
        XblMultiplayerSessionHostCandidates()
        XblMultiplayerSessionMatchmakingServer()
    elseif writeCount == 3 then
        -- triggers from OnXblMatchmakingDeleteMatchTicket
        print("OnXblMultiplayerWriteSessionAsync 3")
        local hr = GetLastError()
        SetCheckHR(1)
        if hr ~= 0 and hr ~= -2147023084 then -- --2147023084 == 0x80070714 == __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND)
            test.equal(hr, 0);
        end
        XblMultiplayerSessionCloseHandle()
        test.stopTest();
    end
end
function OnXblMatchmakingCreateTicket()
    print("OnXblMatchmakingCreateTicket")
    XblMatchmakingGetMatchTicketDetails();
end

function OnXblMatchmakingGetMatchTicketDetails()
    print("OnXblMatchmakingGetMatchTicketDetails")
    XblMatchmakingGetHopperStatistics();
end

function OnXblMatchmakingGetHopperStatistics()
    print("OnXblMatchmakingGetHopperStatistics")
    XblMatchmakingDeleteMatchTicket();
end

function OnXblMatchmakingDeleteMatchTicket()
    print("OnXblMatchmakingGetHopperStatistics")
    XblMultiplayerSessionLeave()
    XblMultiplayerWriteSessionAsync()
    SetCheckHR(0)
end

test.TestMatchmaking_SingleDevice = function()
    common.init(TestMatchmaking_Handler)
end