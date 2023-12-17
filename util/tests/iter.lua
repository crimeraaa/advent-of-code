require "util/stdio"

local array = {}

function array.values(tbl)
    local i = 0
    local length = #tbl
    return function()
        i = i + 1
        if i <= length then
            return tbl[i]
        end
    end
end

---@param _Array any[]
function array.twopairs_stateful(_Array)
    local i, ii = 0, 1 -- internal closure interators
    local _Length = #_Array
    ---@return any?, any?
    return function()
        i = i + 1
        ii = ii + 1
        if ii <= _Length then
            return _Array[i], _Array[ii]
        end
    end
end

function array.twopairs_iterator(_Array, ii)
    ii = ii + 1
    local vx, vy = _Array[ii], _Array[ii + 1]
    if vx and vy then
        return ii, vx, vy
    end
end

-- (1) iterator function. called in the form fn(_Array, _Index)
--      `_Index` is a sort of internal state kept by generic for itself.
-- (2) invariant/unchanging state
-- (3) initial value for the control variable, our iterator index.
function array.twopairs_stateless(_Array)
    return array.twopairs_iterator, _Array, 0
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local t = {1, 2, 3, 4, 5}
    for first, second in array.twopairs_stateful(t) do
        print(first, second)
    end
    for index, first, second in array.twopairs_stateless(t) do
        print(index, first, second)
    end
end

return main(#arg, arg)
