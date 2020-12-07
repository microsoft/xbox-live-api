test = require 'u-test'
common = require 'common'

test.simpletest = function()
    test.assert(2 == 2)
    test.stopTest();
end

