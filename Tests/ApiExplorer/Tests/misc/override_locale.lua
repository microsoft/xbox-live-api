test = require 'u-test'
common = require 'common'

-- Ensure we properly handle overriding locales
function OverrideLocale()
    print("OverrideLocale");
    XblSetOverrideLocale();
    test.stopTest();
end

test.OverrideLocale = function()
    common.init(OverrideLocale)
end
