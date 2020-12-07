test = require 'u-test'
common = require 'common'

function TestStringVerifyCpp_Handler()
    print("TestStringVerifyCpp_Handler")
    StringServiceVerifyString()
end

function OnStringServiceVerifyString()
    print("OnStringServiceVerifyString")
    StringServiceVerifyStrings()
end

function OnStringServiceVerifyStrings()
    print("OnStringServiceVerifyStrings")
    test.stopTest();
end

test.TestStringVerifyCpp = function()
    common.init(TestStringVerifyCpp_Handler)
end

