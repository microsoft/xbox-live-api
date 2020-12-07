
test = require 'u-test'
common = require 'common'

function LuaSetupHandler()
    local hr = GetLastError()
    print("LuaSetupHandler: hr=" .. hr)
    SetCheckHR(1)
    if hr ~= 0 then
        print("LuaSetupHandler: hr != 0. Calling UwpXblSigninAsync")
        common.doCallbackOnError = false
        UwpXblSigninAsync()
    else
        print("LuaSetupHandler: stopping test")
        common.doCallbackOnError = false
        test.stopTest();
    end
end

test.StartSetupTest = function()
    SetCheckHR(0)
    common.doCallbackOnError = true
    common.init(LuaSetupHandler)
end

function OnUwpXblSigninAsync()
    local hr = GetLastError()
    print("OnUwpXblSigninAsync: hr " .. hr )
    if hr == 0 then
        UwpXblContextCreateHandle()
    end
    test.stopTest();
end
