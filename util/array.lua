require "util/table"

-- Array manipulation library.
array = {}

-- Separate state-keeping function so that the primary `two_ipairs` function 
-- doesn't constantly create closures whenever it's called.
---@generic T
---@param _Array T[]
---@param i int
local function two_ipairs_iter(_Array, i)
    i = i + 1
    local ii = i + 1
    local vx, vy = _Array[i], _Array[ii]
    if vx and vy then
        return i, vx, ii, vy
    end
end

-- Work with 2 index-value pairs at a time from the list.
---@generic T
---@param _Array T[]
---@return fun(_Array: T[], i: int):(int, T, int, T)
---@return T[]
---@return int
function array.two_ipairs(_Array)
    -- iterator function, invariant state and control variable initial value
    return two_ipairs_iter, _Array, 0
end

-- Perform binary search on a sorted array.
-- Note that the search is based on equality, so floats might mess up.
---@param _Array int[] Some sorted array of integers.
---@param target int Value to try to find in the array.
---@param start? int Where in the array to begin searching.
---@param stop? int Where in the array to stop searching.
function array.search(_Array, target, start, stop)
    local left = start or 1
    local right = stop or #_Array
    while left <= right do
        local middle = math.floor((left + right) / 2)
        if _Array[middle] < target then 
            left = middle + 1 -- Search right-er half of array
        elseif _Array[middle] > target then 
            right = middle - 1 -- Search left-er half of array
        else -- Found!
            return _Array[middle]
        end
    end
    return nil -- fallback
end

---@generic T
---@param _Array T[]
---@param i int
local function outer_ipairs_iter(_Array, i)
    local ii = #_Array - i -- important for this to come first, is off-by-one.
    i = i + 1
    local left, right = _Array[i], _Array[ii]
    -- Ensure we stop at middle of array and indexed values exist
    if (i <= ii) and (left and right) then
        return i, left, ii, right
    end
end

-- Iterate through the array from the 2 outermost indexes going inwards.
---@generic T
---@param _Array T[]
---@return fun(_Array: T[], i: int):int, T, int ,T
---@return T[]
---@return int
function array.outer_ipairs(_Array)
    return outer_ipairs_iter, _Array, 0
end

-- Given an array (or a table with an array portion), reverse the order of elements.
-- Note that, by default, this creates a copy of the table!
---@generic T
---@param _Array T[]
---@param dontcopy? bool `true` means modify `_Array` directly, no copy made.
function array.reverse(_Array, dontcopy)
    -- If `dontcopy`, get reference/pointer to `source` itself.
    local _Target = (dontcopy and _Array) or table.copy(_Array)
    for i, left, ii, right in array.outer_ipairs(_Target) do
        -- like in Python, this compound assignment works!
        _Target[i], _Target[ii] = right, left
    end
    return _Target
end
