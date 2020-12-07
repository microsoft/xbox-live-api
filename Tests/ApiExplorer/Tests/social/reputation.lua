
test = require 'u-test'
common = require 'common'

function SubmitReputationFeedback_Handler()
    print("GetUserProfile_Handler")
    XblSocialSubmitReputationFeedbackAsync()
end

function OnXblSocialSubmitReputationFeedbackAsync()
    print("OnXblProfileGetUserProfileAsync")
    XblSocialSubmitBatchReputationFeedbackAsync()
end

function OnXblSocialSubmitBatchReputationFeedbackAsync()
    print("OnXblSocialSubmitBatchReputationFeedbackAsync")
    test.stopTest()
end

test.SubmitReputationFeedback = function()
    common.init(SubmitReputationFeedback_Handler)
end