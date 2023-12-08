-- C-style printing of formatted strings.
---@param fmt string String literal or C-style format string.
---@param ... string|integer Arguments to C-style format string.
function printf(fmt, ...)
    io.stdout:write(fmt:format(...))
end

-- Opens a file with `filename` for reading and dumps each line, char by char.
---@param filename string 
function readfile(filename)
    local file = assert(io.open(filename), "r")
    local matrix = {} ---@type string[][]
    local lineno = 1
    for line in file:lines("*l") do
        matrix[lineno] = {} -- new line to split up into chars
        for char in line:gmatch(".") do
            table.insert(matrix[lineno], char)
        end
        lineno = lineno + 1
    end
    file:close()
    return matrix
end
