test = require 'u-test'
common = require 'common'

function GetSocialRelationships_Handler()
    print("GetSocialRelationships_Handler")
    XblSocialGetSocialRelationshipsAsync()
end

function OnXblSocialGetSocialRelationshipsAsync()
    print("OnXblProfileGetUserProfileAsync")
    XblSocialRelationshipResultGetRelationships()
    XblSocialRelationshipResultCloseHandle()
    test.stopTest();
end

test.GetSocialRelationships = function()
    common.init(GetSocialRelationships_Handler)
end