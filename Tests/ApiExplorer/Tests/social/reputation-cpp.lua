test = require 'u-test'
common = require 'common'

function ReputationServiceSubmitReputationFeedback_Handler()
    print("ReputationServiceSubmitReputationFeedback_Handler")
    ReputationServiceSubmitReputationFeedback()
end

function OnReputationServiceSubmitReputationFeedback()
    print("OnReputationServiceSubmitReputationFeedback")
    ReputationServiceSubmitBatchReputationFeedback()
end

function OnReputationServiceSubmitBatchReputationFeedback()
    print("OnReputationServiceSubmitBatchReputationFeedback")
    test.stopTest()
end

test.SubmitReputationFeedbackCpp = function()
    common.init(ReputationServiceSubmitReputationFeedback_Handler)
end