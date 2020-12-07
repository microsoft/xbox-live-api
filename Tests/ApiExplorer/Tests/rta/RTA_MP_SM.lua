test = require 'u-test'
common = require 'common'
firstConnect = true

-- Creates a MPSD session and ensures client can update RTA connection ID to prevent 
-- getting kicked from the session. To force a connection ID changed you can disconnect network
-- or wait for RTA timeout
function CreateSessionAndWait_Handler()
    StartSocialManagerDoWorkLoop();
    XblSocialManagerAddLocalUser();
    XblRtaMultiplayerInit();
end

test.skip = true
test.CreateSessionAndWait = function()
    common.init(CreateSessionAndWait_Handler)
end