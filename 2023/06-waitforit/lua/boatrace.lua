---@class BoatRace
---@field duration num How long the race lasts, in milliseconds.
---@field distance num Record for longest distance, in millimeters.

-- Extracts each of the number literals in the text and stores them in an array.
---@param subject str
function extract_numbers(subject)
    local numbers = {} ---@type num[]
    for num in subject:gmatch("%d+") do
        numbers[#numbers + 1] = tonumber(num)
    end
    return numbers
end

-- Combines all of the string's number literals into one giant number,
-- note that this occurs from left-to-right, 
-- 
-- e.g string `"1, 9, and 93"` is converted to the number 1993 and not 3991.
---@param subject str
function make_compoundnumber(subject)
    local collection = extract_numbers(subject)
    return tonumber(table.concat(collection))
end
