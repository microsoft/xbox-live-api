local common = {}

function common.init(fn)
    common.functionAddFirstAsync = fn
    XalPlatformWebSetEventHandler()
    XalPlatformStorageSetEventHandlers()
    XalInitialize()
    XblInitialize()
    XblGetScid()
    XalTryAddFirstUserSilentlyAsync()
end

function common.cleanup()
    XblContextCloseHandle();
    XalUserCloseHandle()
    XblCleanupAsync()
    XalCleanupAsync()
    XTaskQueueCloseHandle()
end

function common.OnXalTryAddFirstUserSilentlyAsync()
    local hr = GetLastError()
    local handle = 0
    if hr == 0 then
        print("SignInSilently Succeeded. Creating XblContext")
        handle, hr = XblContextCreateHandle()
        print("hr " .. hr)
        print("handle " .. handle)
        if hr == 0 then
            XalUserGetGamertag()
            XalUserGetId()
        else
            if GetCheckHR() == true then 
                test.stopTest();
            else
                SetCheckHR(1)
            end
        end
        common.functionAddFirstAsync()
    else 
        print("SignInSilently Failed")
        if GetCheckHR() == true then 
            test.stopTest();
        else
            SetCheckHR(1)
            common.functionAddFirstAsync()
        end
    end
end

return common
