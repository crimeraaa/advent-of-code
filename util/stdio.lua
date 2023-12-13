require("util/string")

-- C-style printing of formatted strings.
---@param fmt str String literal or C-style format string.
---@param ... str|int Arguments to C-style format string.
function printf(fmt, ...)
    io.stdout:write(fmt:format(...))
end

-- C-style printing of formatted strings.
---@param stream file* Where to write to, e.g. a file handle.
---@param fmt str String literal or C-style format string.
---@param ... str|int Arguments to C-style format string.
function fprintf(stream, fmt, ...)
    stream:write(fmt:format(...))
end

-- Opens a file with `filename` for reading and stores each line in an array.
---@param filename str 
function readfile(filename)
    local file = assert(io.open(filename, "r"))
    local contents = string.split(file:read("*a"), "\n")
    -- Only return 2nd arg if both are truthy (not `nil` and not `false`)
    return file:close() and contents
end
