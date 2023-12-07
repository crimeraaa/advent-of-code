-- C-style printing of formatted strings.
---@param fmt string String literal or C-style format string.
---@param ... string|integer Arguments to C-style format string.
function printf(fmt, ...)
    io.stdout:write(fmt:format(...))
end

-- Remove leading and trailing whitespaces from the string.
-- Modifying the standard library is frowned upon but whatever :)
---@param subject string
---@nodiscard
function string.trim(subject)
    return subject:gsub("^%s+", ""):gsub("%s+$", "")
end

-- Creates a string array of based off of `subject`.
-- We separate the strings based on the chars in `separators`.
---@param subject string
---@param separators string
function string.split(subject, separators)
    ---@type string[]
    local tokens = {}
    local start = 0 -- substring start index, update as we go
    local match = 0 -- substring end index, update as we go
    -- Pictured: abuse of Lua's implementation of boolean types, ugly but works!
    while match ~= nil do
        match = subject:find(separators, start)
        local substr = subject:sub(start, match and match - 1):trim()
        table.insert(tokens, substr)
        start = (match and match + 1) or start
    end
    return tokens
end
