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
---@param left str|num
---@param right str|num
---@param dont_calculate? bool `true` = assign `right` to `endpt` as is.
function NewRange.new(left, right, dont_calculate)
    local _Start, _Endpt = tonumber(left), tonumber(right)
    assert(_Start and _Endpt, "what") -- yell at me so i can figure it out
    local _Length = (dont_calculate and _Endpt - _Start + 1) or _Endpt
    ---@type NewRange
    return {
        start = _Start,
        endpt = (dont_calculate and _Endpt) or (_Start + _Endpt - 1),
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

-- Check if `range.src.length` and `range.dst.length` are the same.
---@param range NewMapRanges
function get_map_rangesize(range)
    local _Src, _Dst = range.src.length, range.dst.length
    return (_Src == _Dst) and make_lengthstr(_Src) or "TODO: range lengths inconsistent!"
end

---@param seed NewRange
---@param tab? int
function print_seed(seed, tab)
    tab = tab or 0
    printf(INDENT[tab].."seeds%s %s\n", make_rangestr(seed), make_lengthstr(seed.length))
end
