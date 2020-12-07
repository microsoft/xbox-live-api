test = require 'u-test'
common = require 'common'

function AddFirst_Handler()
    print("AddFirst_Handler")
    test.stopTest();
end

test.addfirst = function()
    common.init(AddFirst_Handler)
end

