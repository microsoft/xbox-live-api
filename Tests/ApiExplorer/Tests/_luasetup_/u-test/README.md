# u-test
[![License](http://img.shields.io/badge/License-MIT-green.svg)](LICENSE.txt)

**u-test** is a sane and simple unit testing framework for Lua. It has all essential unit test framework features:
defining test cases, test suites, set of build-in assertions, configurable tests output, protected calls and etc.

### Top features that are not present in other lua test frameworks
1. Nice command line interface (like gtest).
1. Backtrace in failed assertions.
1. Ordered test execution (as written in source file). 
1. Support 5.1/5.2/5.3 and LuaJIT.
1. Select particular tests with regexp.

### How to install
#### GitHub
Just copy `u-test.lua` to your projct or add this repo as submodule.
```
$ git clone git://github.com/iudalov/u-test
```
#### [LuaRocks](https://luarocks.org)
Just run:
```
$ luarocks install u-test
```

### How to use
```lua
local test = require 'u-test'

-- You can use 'assert' to check invariants.
test.hello_world = function ()
    test.assert(true)
    test.assert(1 ~= 2)
end

-- This is how you can crete your first test case 
test.addition = function ()
    test.equal(1 + 1, 2)
    test.not_equal("1 + 1", "2")
    test.almost_equal(1 + 1, 2.1, 0.2)
end

-- You can enable custom start_up and tear_down actions 
-- Thse actions will be invoked:
-- start_up - before test case
-- tear_down - after test case
local global_state = 0
test.start_up = function () global_state = 1 end
test.tear_down = function () global_state = 0 end

test.dummy1 = function()
    test.equal(global_state, 1)
    test.is_number(global_state)
end

-- You can separate tests by test suites
test.string.format = function ()
    test.equal(string.format("%d + %d", 1, 1), "1 + 1")
    test.not_equal(string.format("Sparky %s", "bark"), "Fluffy bark")
end

test.string.find = function ()
    test.is_nil(string.find("u-test", "banana"))
    test.is_not_nil(string.find("u-test", "u"))
end

-- You can declare test case with parameters
test.string.starts_with = function (str, prefix)
    test.equal(string.find(str, prefix), 1)
end

-- Then, run it with multiple parameters
test.string.starts_with("Lua rocks", "Lua")
test.string.starts_with("Wow", "Wow")

local global_table = {}

-- Each test suite can be customized by start_up and tear_down
test.table.start_up = function ()
    global_table = { 1, 2, "three", 4, "five" }
end
test.table.tear_down = function () 
    global_table = {}
end

test.table.concat = function ()
    test.equal(table.concat(global_table, ", "), "1, 2, three, 4, five")
end

-- you can disabe broken test case like this
test.broken.skip = true
test.broken.bad_case = function ()
    test.equal(1, 2)
    there_is_no_such_function()
end

-- obtain total number of tests and numer of failed tests
local ntests, nfailed = test.result()

-- this code prints tests summary and invokes os.exit with 0 or 1
test.summary()
```

![Output](https://raw.githubusercontent.com/iudalov/u-test/master/res/ui.png)

### List of all assertions
```lua
test.assert(true)
test.equal(1, 1)
test.not_equal(1, 2)
test.is_false(false)
test.is_true(true)
test.is_not_nil("Something")
test.is_nil(nil)
test.is_boolean(true)
test.is_boolean(false)
test.is_string("I am string! look at me!")
test.is_number(3)
test.is_table({"I am table now"})
test.is_function(function () end)
test.is_userdata(userdata_value)
```
