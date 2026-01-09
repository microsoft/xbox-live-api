test = require 'u-test'
common = require 'common'

function SocialRelationshipChanged_Handler()
    print("SocialRelationshipChanged_Handler")
    XblSocialAddSocialRelationshipChangedHandler()
    XblSocialAddFriendRequestCountChangedHandler()
end

function OnSocialRelationshipChanged()
    print("OnSocialRelationshipChanged")
    XblSocialRemoveSocialRelationshipChangedHandler()
    test.stopTest();
end

function OnFriendRequestCountChanged()
    print("OnFriendRequestCountChanged")
    --XblSocialRemoveFriendRequestCountChangedHandler()
end

test.skip = true
test.SocialRelationshipChanged = function()
    common.init(SocialRelationshipChanged_Handler)
end