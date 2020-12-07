test = require 'u-test'
common = require 'common'

function SignOut_Handler()
    print("SignOut_Handler")
    if XalSignOutUserAsyncIsPresent then
        XalSignOutUserAsync();
    else
        test.stopTest();
    end
end

function OnXalSignOutUserAsync()
    print("OnXalSignOutUserAsync")
    XblContextCloseHandle();
    XalUserCloseHandle();
    Sleep(1000);
    XalAddUserWithUiAsync();
end

function OnXalAddUserWithUiAsync()
    print("OnXalAddUserWithUiAsync")
    test.stopTest();
end

test.skip = true
test.signOut = function()
    common.init(SignOut_Handler)
end

