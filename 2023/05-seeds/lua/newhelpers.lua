---@class NewRange
---@field start num First value in range.
---@field endpt num Last value in range (inclusive).
---@field length num Number of values contained in range.

NewRange = {}

-- Remember: in `{dst=50,src=98,range=2}`, the actual mapping ranges are
-- `[98...99]=>[50...51]` and not `[98...100]=>[50...52]`!
-- 
-- This is because `range` is the number of values in the map.
-- `[98...99]` is a range of 2 values, but `[98...100]` is a range of 3 values.
---@param start str|num
---@param length str|num
function NewRange.new(start, length)
    local _Start, _Length = tonumber(start), tonumber(length)
    assert(_Start and _Length, "what") -- yell at me so i can figure it out
    ---@type NewRange
    return {
        start = _Start,
        endpt = _Start + _Length - 1,
        length = _Length
    }
end

---@class NewLabel
---@field src str Source category's name.
---@field dst str Destination category's name.

---@class NewMapRanges
---@field src NewRange Source category's mappings.
---@field dst NewRange Destination category's mappings.

---@class NewMap
---@field label NewLabel
---@field ranges NewMapRanges[] List of 1 or more src-dst data relationships.

---@param range NewRange
function make_rangestr(range)
    return string.format("[%.0f...%.0f]", range.start, range.endpt)
end

-- Create a "({length} elements)" string.
---@param length num
function make_lengthstr(length)
    return string.format("(%.0f elements)", length)
end
