test = require 'u-test'
common = require 'common'

function SocialRelationshipChanged_Handler()
    print("SocialRelationshipChanged_Handler")
    XblSocialAddSocialRelationshipChangedHandler()
end

function OnSocialRelationshipChanged()
    print("OnSocialRelationshipChanged")
    XblSocialRemoveSocialRelationshipChangedHandler()
    test.stopTest();
end

test.skip = true
test.SocialRelationshipChanged = function()
    common.init(SocialRelationshipChanged_Handler)
end