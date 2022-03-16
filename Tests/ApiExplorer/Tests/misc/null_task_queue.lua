test = require 'u-test'

-- Ensure we properly handle a null process queue
-- XblInitialize should fail with E_NO_TASK_QUEUE if the process task queue is set to nullptr
-- and we try and get a default queue (XblInitArgs queue is set to nullptr)

test.nullTaskQueue = function()
    local taskQueue = XTaskQueueGetCurrentProcessTaskQueue()
    XTaskQueueSetCurrentProcessTaskQueue(0)
    XalPlatformWebSetEventHandler()
    XalPlatformStorageSetEventHandlers()
    XalInitialize()
    SetCheckHR(0)
    XblInitialize(0)
    local hr = GetLastError()
    print("XblInitialize: hr=" .. hr)
    print("Expected:          -2147024469 (E_NO_TASK_QUEUE)")
    SetCheckHR(1)
    test.equal(hr, -2147024469)
    if hr ~= -2147024469 then -- -2147024469 == 0x800701AB == E_NO_TASK_QUEUE
        print("Failure. hr=" .. hr)
    else
        print("Succeeded.")
    end
    XTaskQueueSetCurrentProcessTaskQueue(taskQueue)
    test.stopTest();
end