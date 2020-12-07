test = require 'u-test'
common = require 'common'

function TestStringVerify_Handler()
    print("TestStringVerify_Handler")
    XblStringVerifyStringAsync()
end

function OnTestStringVerify()
    XblStringVerifyStringsAsync()
    test.stopTest()
end

test.TestStringVerify = function()
    common.init(TestStringVerify_Handler)
end

