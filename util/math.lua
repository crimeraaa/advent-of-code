-- Get the factors of `value`, stored in an integer array.
-- 
-- Note that we only search up to `value / 2`.
-- This is because anything above `value / 2` is likely not a factor.
-- 
-- Also note this will be very inefficient with large numbers.
function math.factorize(value)
    -- Keep track of this so we can correctly assign values inside of the loop.
    local sign = (value < 0 and -1) or 1 

    -- It's easier to work with positive values in a loop checking for equality.
    value = math.abs(value) 

    local factors = {} ---@type int[]
    for ii = 1, math.floor(value / 2) do
        -- Factors are defined by the property of being evenly divisible.
        if (value % ii) == 0 then
            factors[#factors + 1] = ii * sign
        end
    end

    -- The value itself is its own factor as well
    factors[#factors + 1] = value * sign
    return factors
end

---@param value int
---@param start? int First multple to get.
---@param stop? int Last multiple to get. Default: 16
---@param source? int[] Pass this if you want to modify an existing table.
function math.multiples(value, start, stop, source)
    stop = stop or 16
    local multiples = source or {}
    for i = start, stop do
        multiples[i] = value * i
    end
    return multiples
end

-- Perform binary search on a integer array.
-- Note that the search is based on equality, so floats might mess up.
---@param array int[] Some sorted array of integer.
---@param target int Value to try to find in the array.
---@param start? int Where in the array to begin searching.
---@param stop? int Where in the array to stop searching.
function math.binsearch(array, target, start, stop)
    local left = start or 1
    local right = stop or #array
    while left <= right do
        local middle = math.floor((left + right) / 2)
        if array[middle] < target then 
            left = middle + 1 -- Search right-er half of array
        elseif array[middle] > target then 
            right = middle - 1 -- Search left-er half of array
        else -- Found!
            return array[middle]
        end
    end
    return nil -- fallback
end

-- Greatest common denominator of a set of values.
function math.gcd(x, y)
    local gcd, ctrl = x, y
    while ctrl ~= 0 do
        -- For previous value of ctrl, if modulo == 0 then we have the GCD.
        gcd, ctrl = ctrl, gcd % ctrl
    end
    return gcd
end

-- Least common multiple of a set of values.
---@param x num
---@param y num
---@param ... num
function math.lcm(x, y, ...)
    if (x == 0) or (y == 0) then
        return 0
    end
    local lcm = 0
    local list = {x, y, ...}
    for i, value in ipairs(list) do
        -- try to poke at value to our right,
        local ii = i + 1 
        local right = list[ii]

        -- if list[i + 1] is `nil` don't try to access it or update it
        if right ~= nil then 
            -- LCM is effectively: abs(x * y) / gcd(x,y)
            lcm = (value * right) / math.gcd(value, right)

            print(value, right, lcm)
            
            -- update table directly so next call effectively becomes:
            --      local prev = math.lcm(list[i], list[ + 1])
            --      math.lcm(prev, list[i + 2])
            list[ii] = lcm 
        end
    end
    return lcm
end
