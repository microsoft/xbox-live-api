test = require 'u-test'
common = require 'common'

function SocialServiceGetSocialRelationships_Handler()
    print("SocialServiceGetSocialRelationships_Handler")
    SocialServiceGetSocialRelationships()
	--OnSocialServiceGetSocialRelationships()
end

function OnSocialServiceGetSocialRelationships()
    hr, hasNext = SocialRelationshipResultHasNextCpp()
    print("hasNext " .. hr)
    print("hr " .. hr)
    if hasNext ~= 0 then
        SocialRelationshipResultGetNextCpp()
    else
        SocialRelationshipResultCloseHandleCpp()
        test.stopTest();
    end
end

function OnSocialRelationshipGetNextCpp()
    print("OnSocialRelationshipGetNextCpp")
    OnSocialServiceGetSocialRelationships()
end

test.GetSocialRelationshipsCpp = function()
    common.init(SocialServiceGetSocialRelationships_Handler)
end