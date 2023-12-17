-- Array manipulation library.
array = {}

-- Separate state-keeping function so that the primary `two_ipairs` function 
-- doesn't constantly create closures whenever it's called.
---@generic Type
---@param _Array Type[]
---@param i int
function array.two_ipairs_iter(_Array, i)
    i = i + 1
    local ii = i + 1
    local vx, vy = _Array[i], _Array[ii]
    if vx and vy then
        return i, vx, ii, vy
    end
end

-- Work with 2 index-value pairs at a time from the list.
---@generic Type
---@param _Array Type[]
---@return fun(_Array: Type[], i: int):(int, Type, int, Type)
---@return Type[]
---@return int
function array.two_ipairs(_Array)
    -- iterator function, invariant state and control variable initial value
    return array.two_ipairs_iter, _Array, 0
end

-- Perform binary search on an array.
-- Note that the search is based on equality, so floats might mess up.
---@param _Array int[] Some sorted array of integer.
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
