---@alias int integer
---@alias str string
---@alias bool boolean

---@class Symbol
---@field lineno int 
---@field column int Index into the line.
---@field value str|int Symbol character or constructed part number.


PEEK_LEFT = -1
PEEK_RIGHT = 1

-- C-style printing of formatted strings.
---@param fmt str String literal or C-style format string.
---@param ... str|int Arguments to C-style format string.
function printf(fmt, ...)
    io.stdout:write(fmt:format(...))
end

-- Use absolute path for Lua debug in VSCode cause it uses workspace directory
local fallback = "C:/Users/crimeraaa/repos/advent-of-code/2023/03-gear-ratios/"

-- Opens a file with `filename` for reading and dumps each line, char by char.
---@param filename str 
function readfile(filename)
    local file, errmsg = io.open(filename, "r")
    if not file then
        printf("%s, using fallback directory %s\n", errmsg, fallback)
        file, errmsg = io.open(fallback..filename, "r")
    end
    assert(file, errmsg)
    local matrix = {} ---@type string[][]
    local lineno = 1
    for line in file:lines() do
        matrix[lineno] = {} -- line of "chars" (as best as Lua can, anyway)
        for char in line:gmatch(".") do
            table.insert(matrix[lineno], char)
        end
        lineno = lineno + 1
    end
    file:close()
    return matrix
end
