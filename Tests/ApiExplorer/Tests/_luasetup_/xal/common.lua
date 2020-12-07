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
end

function common.OnXalTryAddFirstUserSilentlyAsync()
    local hr = GetLastError()
    if hr == 0 then
        XblContextCreateHandle()
        XalUserGetGamertag()
        XalUserGetId()
    end
    common.functionAddFirstAsync()
end

return common
