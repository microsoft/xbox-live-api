-- COMMAND LINE ----------------------------------------------------------------

print = function(...)
    LogToScreen(...)
end

local quiet, grey, test_regex

-- UTILS -----------------------------------------------------------------------
local function red(str)    return grey and str or "" .. str .. " " end
local function blue(str)   return grey and str or "" .. str .. " " end
local function green(str)  return grey and str or "" .. str .. " " end
local function yellow(str) return grey and str or "" .. str .. " " end

local tab_tag      = blue   "[----------]"
local done_tag     = blue   "[==========]"
local run_tag      = blue   "[ RUN      ]"
local ok_tag       = green  "[       OK ]"
local fail_tag     = red    "[      FAIL]"
local disabled_tag = yellow "[ DISABLED ]"
local passed_tag   = green  "[  PASSED  ]"
local failed_tag   = red    "[  FAILED  ]"

local ntests = 0
local failed = false
local failed_list = {}

local function log(msg)
    if not quiet then
        LogToScreen(msg)
    end
end

local function trace(start_frame)
    log("Trace:")
    local frame = start_frame
    while true do
        local info = debug.getinfo(frame, "Sl")
        if not info then break end
        if info.what == "C" then
            -- log("??????")
        else
            log(info.short_src .. ":" .. info.currentline .. " ")
        end
        frame = frame + 1
    end
end

local function fail(msg, start_frame)
    failed = true
    log("Fail: " .. msg)
    trace(start_frame or 4)
end

local function fail_no_log(msg, start_frame)
    failed = true
end

local function stringize_var_arg(varg, ...)
    if varg then
        local rest = stringize_var_arg(...)
        if rest ~= "" then
            return tostring(varg) .. ", ".. rest
        else
            return tostring(varg)
        end
    else
        return ""
    end
end

local function test_pretty_name(suite_name, test_name)
    if suite_name == "__root" then
        return test_name
    else
        return suite_name .. "." .. test_name
    end
end

-- PUBLIC API -----------------------------------------------------------------
local api = 
{ 
    test_suite_name = "__root", 

    enablebvts = false, 
    enablemultidevice = false, 
    skipOverride = false, 

    skip = false, 
    isbvt = false,
    ismultidevice = false
}

api.assert = function (cond)
    if not cond then
        fail("assertion " .. tostring(cond) .. " failed")
    end
end

api.equal_no_log = function (l, r)
    if l ~= r then
        fail_no_log(tostring(l) .. " ~= " .. tostring(r))
    end
end

api.equal = function (l, r)
    if l ~= r then
        fail(tostring(l) .. " ~= " .. tostring(r))
    end
end

api.not_equal = function (l, r)
    if l == r then
        fail(tostring(l) .. " == " .. tostring(r))
    end
end

api.almost_equal = function (l, r, diff)
    if require("math").abs(l - r) > diff then
        fail("|" .. tostring(l) .. " - " .. tostring(r) .."| > " .. tostring(diff))
    end
end

api.is_false = function (maybe_false)
    if maybe_false or type(maybe_false) ~= "boolean" then
        fail("got " .. tostring(maybe_false) .. " instead of false")
    end
end

api.is_true = function (maybe_true)
    if not maybe_true or type(maybe_true) ~= "boolean" then
        fail("got " .. tostring(maybe_true) .. " instead of true")
    end
end

api.is_not_nil = function (maybe_not_nil)
    if type(maybe_not_nil) == "nil" then
        fail("got nil")
    end
end

local function skip_test(test_suite)
    failed = false

    -- return back to default for future tests
    test_suite.ismultidevice = false
    test_suite.isbvt = false 
    test_suite.skip = false 
    SetTestWasSkipped()
    api.stopTest()
end

local last_test_suite
local function run_test(test_suite, test_name, test_function, ...)
    local suite_name = test_suite.test_suite_name
    full_test_name = test_pretty_name(suite_name, test_name)

    if test_regex and not string.match(full_test_name, test_regex) then
        return
    end

    start = os.time()  

    -- uncomment for extra debug logs
    -- if api.skipOverride then log("skipOverride true") else log("skipOverride false") end
    -- if api.enablemultidevice then log("enablemultidevice true") else log("enablemultidevice false") end
    -- if api.enablebvts then log("enablebvts true") else log("enablebvts false") end
    -- if test_suite.skip then log("skip true") else log("skip false") end
    -- if test_suite.ismultidevice then log("ismultidevice true") else log("ismultidevice false") end
    -- if test_suite.isbvt then log("isbvt true") else log("isbvt false") end

    if test_suite.skip and not api.skipOverride then
        log(disabled_tag .. " " .. full_test_name)
        skip_test(test_suite) -- doesnt match filter, so stop test early
        return
    end

    if api.enablemultidevice then
        if not test_suite.ismultidevice then
            skip_test(test_suite) -- doesnt match filter, so stop test early
            return
        end
    else
        if test_suite.ismultidevice and not api.skipOverride then
log("spot3")
            skip_test(test_suite) -- doesnt match filter, so stop test early
            return
        end
    end
    
    if api.enablebvts then
        if not test_suite.isbvt then
            skip_test(test_suite) -- doesnt match filter, so stop test early
            return
        end
    end

    -- return back to default for future tests
    test_suite.ismultidevice = false
    test_suite.isbvt = false 

    if suite_name ~= last_test_suite then
        log(tab_tag)
        last_test_suite = suite_name
    end

    ntests = ntests + 1
    failed = false

    log(run_tag .. " " .. full_test_name)
    
    status = true

    for _, f in ipairs({test_suite.start_up,  test_function, test_suite.tear_down}) do
        status, err = pcall(f, ...)
        if status == nil then
          status = true
        end        
        if not status then
            failed = true
            log(tostring(err))
        end
    end    
end

api.stopTest = function()
    local stop = os.time()

    local is_test_failed = not status or failed
    log(string.format("%s %s %d sec",
                            is_test_failed and fail_tag or ok_tag,
                            full_test_name,
                            os.difftime(stop, start)))

    if is_test_failed then
        table.insert(failed_list, full_test_name)
    end

    StopTestFile();
end

api.summary = function ()
    log(done_tag)
    local nfailed = #failed_list
    if nfailed == 0 then
        log(passed_tag .. " " .. ntests .. " test(s)")
    else
        log(failed_tag .. " " .. nfailed .. " out of " .. ntests .. ":")
        for _, test_name in ipairs(failed_list) do
            log(failed_tag .. "\t" .. test_name)
        end
    end
end

api.result = function ( ... )
    return ntests, #failed_list
end

local default_start_up = function () end
local default_tear_down = function () collectgarbage() end

api.start_up = default_start_up
api.tear_down = default_tear_down

local all_test_cases = { __root = {} }
local function handle_new_test(suite, test_name, test_function)
    local suite_name = suite.test_suite_name
    if not all_test_cases[suite_name] then
        all_test_cases[suite_name] = {}
    end
    all_test_cases[suite_name][test_name] = test_function

    local info = debug.getinfo(test_function)
    if info.nparams == nil and
            string.sub(test_name, #test_name - 1, #test_name) ~= "_p"
            or info.nparams == 0 then
        run_test(suite, test_name, test_function)
    end
end

local function lookup_test_with_params(suite, test_name)
    local suite_name = suite.test_suite_name

    if all_test_cases[suite_name] and all_test_cases[suite_name][test_name] then
        return function (...)
            run_test(suite
                , test_name .. "(" .. stringize_var_arg(...) .. ")"
                , all_test_cases[suite_name][test_name], ...)
        end
    else
        local full_test_name = test_pretty_name(suite_name, test_name)
        table.insert(failed_list, full_test_name)
        ntests = ntests + 1
        log(fail_tag .. " No " .. full_test_name .. " parametrized test case!")
    end
end

local function new_test_suite(_, name)
    local test_suite = {
        test_suite_name = name,
        start_up = default_start_up,
        tear_down = default_tear_down,
        skip = false,
        isbvt = false,
        ismultidevice = false }

    setmetatable(test_suite, {
        __newindex = handle_new_test,
        __index = lookup_test_with_params })
    return test_suite
end

local test_suites = {}
setmetatable(api, {
    __index = function (tbl, name)
        if all_test_cases.__root[name] then
            return lookup_test_with_params(tbl, name)
        end

        if not test_suites[name] then
            test_suites[name] = new_test_suite(tbl, name)
        end
        return test_suites[name]
    end,
    __newindex = handle_new_test
})

return api