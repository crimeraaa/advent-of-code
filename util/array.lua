require "util/table"

-- Array manipulation library.
array = {} 

-- Various helper functions to help you iterate through arrays in strange ways.
array.ipairs = {}

-- Collection of separate state-keeping functions so pair functions don't have
-- to constantly create new closures everytime.
-- 
-- Note: This is internal implementation, don't mess with this kthxbai
-- 
-- Also: function  "forward" declarations works when using tables
-- thanks to the power of indirection that pointers/references give!
array.__iter = {} 

--------------------------------------------------------------------------------
------------------------------ GENERIC FUNCTIONS -------------------------------
--------------------------------------------------------------------------------

-- Quickly create a new 1D array of the given length with initialized values.
---@generic T
---@param length int How many elements in the array.
---@param value T Initializer value.
---@return T[]
function array.new(length, value)
    local target = {} 
    for i = 1, length do
        target[i] = value
    end
    return target
end

-- Given an array (or table w/ array portion), reverse the order of elements.
-- Note that, by default, this creates a copy of the table!
---@generic T
---@param _Array T[]
---@param dontcopy? bool `true` means modify `_Array` directly, no copy made.
function array.reverse(_Array, dontcopy)
    -- If `dontcopy`, get reference/pointer to `source` itself.
    local target = (dontcopy and _Array) or table.copy(_Array)
    for i, left, ii, right in array.ipairs.outer(target) do
        -- like in Python, this compound assignment works!
        target[i], target[ii] = right, left
    end
    return target
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

--------------------------------------------------------------------------------
------------------------------ ITERATOR FUNCTIONS ------------------------------
--------------------------------------------------------------------------------

-- Iterate through the list from the last index going to the start.
---@generic T
---@param _Array T[]
---@return fun(_Array: T[], i: int):int, T, int, T
---@return T[]
---@return int
function array.ipairs.reverse(_Array)
    return array.__iter.reverse, _Array, #_Array + 1
end

-- When `i` is returned as the first value, generic `for` uses it to update.
-- 
-- So it ends up like `for`'s internal iterator gets subtracted 1.
-- 
-- https://stackoverflow.com/a/41350070
---@generic T
---@param _Array T[]
---@param i int
function array.__iter.reverse(_Array, i)
    i = i - 1 
    local vx = _Array[i]
    if vx then
        return i, vx
    end
end

-- Work with 2 index-value pairs at a time from the list.
---@generic T
---@param _Array T[]
---@return fun(_Array: T[], i: int):int, T, int, T
---@return T[]
---@return int
function array.ipairs.two(_Array)
    -- iterator function, invariant state and control variable initial value
    return array.__iter.two, _Array, 0
end

---@generic T
---@param _Array T[]
---@param i int
function array.__iter.two(_Array, i)
    i = i + 1
    local ii = i + 1
    local vx, vy = _Array[i], _Array[ii]
    if vx and vy then
        return i, vx, ii, vy
    end
end

-- Iterate through the array from the 2 outermost indexes going inwards.
---@generic T
---@param _Array T[]
---@return fun(_Array: T[], i: int):int, T, int ,T
---@return T[]
---@return int
function array.ipairs.outer(_Array)
    return array.__iter.outer, _Array, 0
end

---@generic T
---@param _Array T[]
---@param i int
function array.__iter.outer(_Array, i)
    local ii = #_Array - i -- important for this to come first, is off-by-one.
    i = i + 1
    local left, right = _Array[i], _Array[ii]
    -- Ensure we stop at middle of array and indexed values exist
    if (i <= ii) and (left and right) then
        return i, left, ii, right
    end
end


