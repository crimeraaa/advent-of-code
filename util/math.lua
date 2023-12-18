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

-- Create a math function that can work with 2 or more Lua `number`s.
-- It should also be able to iteratively get the result for a list.
---@param fn fun(x: num, y: num):num
---@param ini? num Start value of result variable for list algorithm. Default: 1
function math.fn_factory(fn, ini)
    -- For lists, e.g. using the `math.gcd` as `fn`:
    -- 
    --      fn(16, 12, 8)
    -- 
    -- Is effectively evaluated as:
    -- 
    --      fn(fn(16, 12), 4)
    -- 
    -- Which in turns evaluates to:
    -- 
    --      fn(fn(fn(ini or 1, 16), 12), 4)
    ---@param x num
    ---@param y num
    ---@param ... num
    return function(x, y, ...)
        -- Recurse case/base case to avoid creating unnecessary tables
        if select("#", x, y, ...) == 2 then
            return fn(x, y)
        end
        -- Previous result, ideally get first list value as is via algorithm
        -- e.g. first GCD in list be gcd(1, list[1])
        local result = ini or 1
        local list = {x, y, ...}
        for _, value in ipairs(list) do
            result = fn(result, value)
        end
        return result
    end
end

-- Greatest common denominator of 2 values/set of values. 
math.gcd = math.fn_factory(function(x, y) 
    local gcd, rem = x, y
    -- (gcd=12,rem=8)=>(gcd=8,rem=4)=>(gcd=4,rem=0): return gcd
    while rem ~= 0 do
        local tmp = gcd
        gcd = rem
        rem = tmp % rem
    end
    return gcd
end)

-- Least common multiple of 2 values/set of values. 
math.lcm = math.fn_factory(function(x, y)
    -- LCM is just abs(x*y)/gcd(x,y), also works for x=0 and/or y=0
    return (x * y) / math.gcd(x, y)
end)
