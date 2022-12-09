local xblContext = 0

function TestFunc()
    XblMultiplayerSetSubscriptionsEnabled(1)
    XblMultiplayerSessionReferenceCreate()
    XblMultiplayerSessionCreateHandle()
    XblMultiplayerSessionJoin()
    XblMultiplayerWriteSessionAsync()
end

local writeCount = 0;

function OnXblMultiplayerWriteSessionAsync()
    writeCount = writeCount + 1
    if writeCount == 1 then
        print("OnXblMultiplayerWriteSessionAsync 1")
        -- first write made using default XblContext should complete successfully
        -- change the session and attempt to write with second XblContext
        xblContext = XblContextCreateHandle()
        XblMultiplayerSetSubscriptionsEnabled(1, xblContext)
        XblMultiplayerSessionCurrentUserSetStatus()
        XblMultiplayerWriteSessionAsync(0, xblContext)
    elseif writeCount == 2 then
        print("OnXblMultiplayerWriteSessionAsync 2")
        XblMultiplayerSessionCloseHandle()
        XblContextCloseHandle(xblContext)
        test.stopTest();
    end
end

test.TestMPMultipleContexts = function()
    common.init(TestFunc)
end