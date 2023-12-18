--[[------------------------------ NOTICE --------------------------------------
In order to require modules from this folder, please make sure that the LUA_PATH
environment variable is set up. This modifiers the interpreter's `package.path`.

Make sure the LUA_PATH environment variable is set to something like:
    .\?.lua;
    C:\Users\crimeraaa\repos\?.lua;
    C:\Users\crimeraaa\repos\advent-of-code\?.lua

Double semicolons ';;' may be important if you are entering this in a script.
On Unix systems, you will need to use colons ':' instead.
https://stackoverflow.com/a/31626271 

In Windows, if you are entering these from the Set Environment Variables GUI, 
you may not need to include the semicolons ';'whenever you make a new entry. 

Default `package.path` if LUA_PATH is not set:
    .\?.lua;
    C:\Program Files (x86)\Lua\bin\lua\?.lua;
    C:\Program Files (x86)\Lua\bin\lua\?\init.lua;
    C:\Program Files (x86)\Lua\bin\?.lua;
    C:\Program Files (x86)\Lua\bin\?\init.lua

However, these defaults are overwritten when you set LUA_PATH yourself!
------------------------------------------------------------------------------]]

-- Fully qualified path of caller's current working directory, not the script's.
-- Ugly but works: https://stackoverflow.com/a/6036884
---@type str
CWD = os.getenv("PWD") or io.popen("cd", "r"):read("*l") 

-- The workspace's main directory. Note that it may not have the trailing slash!
---@type str
WORKSPACE_DIR = CWD:match("(.-advent%-of%-code)") 

-- Taken from: https://stackoverflow.com/a/23535333
function script_path()
    ---@type str
    local source = debug.getinfo(2, "S").source:sub(2)
    return source:match("(.*[/\\])") or "./" 
 end

-- Used to determine which slash direction to use, that's literally it.
-- Taken from: https://stackoverflow.com/a/14425862
function is_windows()
    return package.config:sub(1,1) == '\\'
end

-- Transforms `path` to the version containing the OS's preferred backslashes.
-- This assumes that only Windows is the weird one using \ slashes.
---@param path str
function convert_OSpath(path)
    local slashtype = (is_windows() and '\\') or '/'
    return path:gsub("[/\\]", slashtype)
end

-- On a Unix system, try os.capture 'uname' where os.capture is defined below.
-- This will help on all flavors of unix and on Mac OSX. 
-- If it fails, you might be on a Windows system? Or check os.getenv 'HOME'.
-- https://stackoverflow.com/a/326677
--  function os.capture(cmd, raw)
--     local f = assert(io.popen(cmd, 'r'))
--     local s = assert(f:read('*a'))
--     f:close()
--     if raw then 
--         return s 
--     end
--     s = string.gsub(s, '^%s+', '')
--     s = string.gsub(s, '%s+$', '')
--     s = string.gsub(s, '[\n\r]+', ' ')
--     return s
-- end
