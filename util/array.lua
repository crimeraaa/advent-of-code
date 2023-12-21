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

function array.tostring(source)
    local output = {}
    for _, v in ipairs(source) do
        output[#output+1] = tostring(v)
    end
    return "{".. table.concat(output, ", ") .."}"
end

-- Given an array (or table w/ array portion), reverse the order of elements.
-- Note that, by default, this creates a copy of the table!
---@generic T
---@param source T[]
---@param nocopy? bool `true` means modify `source` directly, no copy made.
function array.reverse(source, nocopy)
    -- If `dontcopy`, get reference/pointer to `source` itself.
    local target = (nocopy and source) or table.copy(source)
    for i, left, ii, right in array.ipairs.outer(target) do
        -- like in Python, this compound assignment works!
        target[i], target[ii] = right, left
    end
    return target
end

-- Perform binary search on a sorted array.
-- Note that the search is based on equality, so floats might mess up.
---@param source int[] Some sorted array of integers.
---@param target int Value to try to find in the array.
---@param start? int Where in the array to begin searching.
---@param stop? int Where in the array to stop searching.
function array.search(source, target, start, stop)
    local left = start or 1
    local right = stop or #source
    while left <= right do
        local middle = math.floor((left + right) / 2)
        if source[middle] < target then 
            left = middle + 1 -- Search right-er half of array
        elseif source[middle] > target then 
            right = middle - 1 -- Search left-er half of array
        else -- Found!
            return source[middle]
        end
    end
    return nil -- fallback
end

--------------------------------------------------------------------------------
------------------------------ ITERATOR FUNCTIONS ------------------------------
--------------------------------------------------------------------------------

-- Iterate through the list from the last index going to the start.
---@generic T
---@param source T[]
---@return fun(_Array: T[], i: int):int, T, int, T
---@return T[]
---@return int
function array.ipairs.reverse(source)
    return array.__iter.reverse, source, #source + 1
end

-- When `i` is returned as the first value, generic `for` uses it to update.
-- 
-- So it ends up like `for`'s internal iterator gets subtracted 1.
-- 
-- https://stackoverflow.com/a/41350070
---@generic T
---@param source T[]
---@param i int
function array.__iter.reverse(source, i)
    i = i - 1 
    local vx = source[i]
    if vx then
        return i, vx
    end
end

-- Work with 2 index-value pairs at a time from the list.
---@generic T
---@param source T[]
---@return fun(_Array: T[], i: int):int, T, int, T
---@return T[]
---@return int
function array.ipairs.two(source)
    -- iterator function, invariant state and control variable initial value
    return array.__iter.two, source, 0
end

---@generic T
---@param source T[]
---@param i int
function array.__iter.two(source, i)
    i = i + 1
    local ii = i + 1
    local vx, vy = source[i], source[ii]
    if vx and vy then
        return i, vx, ii, vy
    end
end

-- Iterate through the array from the 2 outermost indexes going inwards.
---@generic T
---@param source T[]
---@return fun(_Array: T[], i: int):int, T, int ,T
---@return T[]
---@return int
function array.ipairs.outer(source)
    return array.__iter.outer, source, 0
end

---@generic T
---@param source T[]
---@param i int
function array.__iter.outer(source, i)
    local ii = #source - i -- important for this to come first, is off-by-one.
    i = i + 1
    local left, right = source[i], source[ii]
    -- Ensure we stop at middle of array and indexed values exist
    if (i <= ii) and (left and right) then
        return i, left, ii, right
    end
end


