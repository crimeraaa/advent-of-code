--------------------------------------------------------------------------------
------------------------ STRING MANIPULATION FUNCTIONS -------------------------
--------------------------------------------------------------------------------

---@param subject str
---@param delimiters str
---@nodiscard
function string.split_noempty(subject, delimiters)
    ---@type str[]
    local captures = {} 
    local pattern = string.format("[^%s]+", delimiters)
    for capture in subject:gmatch(pattern) do
        captures[#captures+1] = capture
    end
    return captures
end

-- Works for strings with empty values.
-- From user bart at: https://stackoverflow.com/a/7615129
---@param subject str
---@param delimiters str
---@nodiscard
function string.split_keepempty(subject, delimiters) 
    -- 1st capture group is the user's desired match via negation charset.
    -- it may also be the empty string.
    -- 
    -- 2nd capture group is a sort of control, for `EOF` or no more lines.
    local pattern = "([^"..delimiters.."]*)("..delimiters.."?)"

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
---@param delimiters? str Defaults to "%s", which splits by whitespace.
---@param keep_empty? bool `true` = call `split_keepempty` else `split_noempty`.
---@nodiscard
function string.split(subject, delimiters, keep_empty)
    keep_empty = keep_empty or false
    local split_fn = (keep_empty and string.split_keepempty) or string.split_noempty
    return split_fn(subject, delimiters or "%s")
end

-- Creates an array of individual characters from `subject`, to mimic C/C++.
---@param subject str
---@nodiscard
function string.toarray(subject)
    local char_array = {} ---@type str[]
    -- ".", just like your usual Regex, will match 1 of literally any character.
    for c in subject:gmatch(".") do
        char_array[#char_array + 1] = c
    end
    return char_array
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
