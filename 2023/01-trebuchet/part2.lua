local file = assert(io.open(arg[1] or "./part2.txt", "r"))

local SPELLINGS = {
    ["one"] = 1,
    ["two"] = 2,
    ["three"] = 3,
    ["four"] = 4,
    ["five"] = 5,
    ["six"] = 6,
    ["seven"] = 7,
    ["eight"] = 8,
    ["nine"] = 9,
}

---@param fmt string
---@param ... string|number
local function printf(fmt, ...)
    io.stdout:write(fmt:format(...))
end

-- Checks if any part of `capture` is found in the global `SPELLINGS` table.
-- 
-- Prolly causes our algorithm to grow quadratically in time taken :) 
local function is_number_spelling(capture)
    for spelling, value in pairs(SPELLINGS) do
        if string.match(capture, spelling) then
            return spelling, value
        end
    end
    return nil
end

-- Match digits or string literals representing numbers.
-- We check on the left and right extremes of the input string `line`.
---@param line string
local function match_numbers(line)
    ---@type number[]
    local captures = {}
    local test = ""
    -- Loop through each individual character in the string
    for letter in line:gmatch("%w") do 
        if tonumber(letter) then
            table.insert(captures, letter)
            test = ""
        else
            -- Build our test string letter by letter
            test = test .. letter
            local spelling, value = is_number_spelling(test)
            -- Update captures table and reset test string for next checks
            if (spelling and value) then
                table.insert(captures, value)
                -- Set to be the last letter in case we wanna match a collison
                -- e.g. "twone": test = "two" =>  test = "o"
                test = test:sub(test:find("$") - 1)
            end
        end
        -- printf("%c", letter:byte())
    end
    -- ! DEBUG PURPOSES ONLY
    -- for i, v in ipairs(captures) do
    --     printf("\tMatch: %i\tValue: %i\n", i, v)
    -- end

    -- Create a compound number, e.g. 1 and 2 becomes 12, 7 and 7 becomes 77
    return (captures[1] * 10) + captures[#captures]
end

local sum = 0
local count = 0
for line in file:lines("*l") do
    -- ! DEBUG PURPOSES ONLY
    -- printf("%i : '%s'\n", count, line)
    sum = sum + match_numbers(line)
    count = count + 1
end
printf("Calibration Value: %i\n", sum)
file:close()
