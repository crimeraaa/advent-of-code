-- C-style printing of formatted strings.
---@param fmt str String literal or C-style format string.
---@param ... str|int Arguments to C-style format string.
function printf(fmt, ...)
    io.stdout:write(fmt:format(...))
end

-- Use absolute path for Lua debug in VSCode cause it uses workspace directory
local fallback = "C:/Users/crimeraaa/repos/advent-of-code/2023/04-scratchcards/"

-- Opens a file with `filename` for reading and dumps each line, char by char.
---@param filename str 
function get_cardlist(filename)
    local file, errmsg = io.open(filename, "r")
    if not file then
        printf("%s, using fallback directory %s\n", errmsg, fallback)
        file, errmsg = io.open(fallback..filename, "r")
    end
    assert(file, errmsg)
    local lines = {} ---@type string[]
    local index = 1
    for line in file:lines() do
        lines[index] = line -- supposedly faster than calling `#` operator alot
        index = index + 1 -- tbh I just like this more for some reason
    end
    file:close()
    return lines
end
