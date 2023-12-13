--------------------------------------------------------------------------------
------------------------ STRING MANIPULATION FUNCTIONS -------------------------
--------------------------------------------------------------------------------

---@param subject str
---@param delimiter str
---@nodiscard
function string.split_noempty(subject, delimiter)
    ---@type str[]
    local captures = {} 
    local pattern = string.format("[^%s]+", delimiter or "%s")
    for capture in subject:gmatch(pattern) do
        captures[#captures+1] = capture
    end
    return captures
end

-- Works for strings with empty values.
-- From user bart at: https://stackoverflow.com/a/7615129
---@param subject str
---@param delimiter str
---@nodiscard
function string.split_keepempty(subject, delimiter) 
    delimiter = delimiter or '%s' -- Fallback to splitting by whitespace chars

    -- 1st capture group is the user's desired match via negation charset.
    -- it may also be the empty string.
    -- 
    -- 2nd capture group is a sort of control, for `EOF` or no more lines.
    local pattern = "([^"..delimiter.."]*)("..delimiter.."?)"

    -- local pattern = string.format("([^%s]*)(%s?)", delimiter, delimiter)
    ---@type str[]
    local captures = {}

    -- Since we speciifed 2 capture groups, expect 2 return values.
    for capture, control in subject:gmatch(pattern) do 
        captures[#captures + 1] = capture
        -- "({delimiter}?)"" may match and capture empty string, in this case
        -- it probably means end of file or no more lines to parse!
        if control == "" then 
            return captures 
        end 
    end
    return captures
end

-- Creates a string array of based off of `subject`.
-- We separate the strings based on the chars in `separators`.
---@param subject str
---@param delimiters str
---@param keep_empty_strings? bool 
---@nodiscard
function string.split(subject, delimiters, keep_empty_strings)
    keep_empty_strings = keep_empty_strings or false
    return keep_empty_strings and subject:split_keepempty(delimiters) 
        or subject:split_noempty(delimiters)
end

--[[ 
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
]]

-- Remove leading and trailing whitespaces from the string.
-- Modifying the standard library is frowned upon but whatever :)
---@param subject string
---@nodiscard
function string.trim(subject)
    return subject:gsub("^%s+", ""):gsub("%s+$", "")
end
