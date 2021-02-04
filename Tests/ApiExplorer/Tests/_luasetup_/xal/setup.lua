
test = require 'u-test'
common = require 'common'

function SetupTest_Handler()
    print("SetupTest_Handler")
    local hr = GetLastError()
    SetCheckHR(1)
    if hr ~= 0 then
        print("Calling XalAddUserWithUiAsync")
        XalAddUserWithUiAsync()
    else
        test.stopTest();
    end
end

test.StartSetupTest = function()
    print("Running shared test.StartSetupTest")
    SetCheckHR(0)
    common.init(SetupTest_Handler)
end

function OnXalAddUserWithUiAsync()
    print("In OnXalAddUserWithUiAsync")
    XblContextCreateHandle()
    XalUserGetGamertag()
    XalUserGetId()
    test.stopTest();
end
