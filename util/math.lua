require "util/array"

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

-- Greatest common denominator of a set of values.
---@param x num
---@param y num
---@param ... num
function math.gcd(x, y, ...)
    if select("#", x, y, ...) == 2 then
        local gcd, rem = x, y
        -- (gcd=12,rem=8)=>(gcd=8,rem=4)=>(gcd=4,rem=0): return gcd
        while rem ~= 0 do
            local tmp = gcd
            gcd = rem
            rem = tmp % rem
        end
        return gcd
    end
    local list = {x, y, ...}
    local gcd = 0
    for i, vx, ii, vy in array.two_ipairs(list) do
        gcd = math.gcd(vx, vy)
        -- gcd(16,12,8)=>gcd(16,12): 4, resolves to gcd(4,8): 4
        -- effectively the same as: gcd(gcd(16, 12), 8)
        list[ii] = gcd
    end
    return gcd
end

-- Least common multiple of a set of values.
---@param x num
---@param y num
---@param ... num
function math.lcm(x, y, ...)
    if select("#", x, y, ...) == 2 then
        -- LCM is just abs(x * y) / gcd(x,y), also works for x=0 or y=0
        return (x * y) / math.gcd(x, y)
    end
    local list = {x, y, ...}
    local lcm = 0
    for i, vx, ii, vy in array.two_ipairs(list) do
        lcm = math.lcm(vx, vy)
        -- lcm(2,3,5)=>lcm(2,3): 6, resolves to lcm(6,5): 30
        -- effectively the same as: lcm(lcm(2, 3), 5)
        list[ii] = lcm
    end
    return lcm
end
