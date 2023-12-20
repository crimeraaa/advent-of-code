-- Copies table by value as table assignment is just moving pointers around.
-- 
-- By default, it just each value as is, no conversion at all. You can pass a 
-- function `convert_fn` which takes an iterated value are returns a conversion.
---@param source {}
---@param convert_fn? fun(v: any):any `tostring`, `tonumber` or custom function.
---@param nocopy? bool Pass `true` if you want to modify `source` directly.
function table.copy(source, convert_fn, nocopy)
    local target = (nocopy and source) or {}
    for k, v in pairs(source) do
        -- Need explicit check as (fn and fn(v)) or v messes up w/ bools.
        if convert_fn then
            target[k] = convert_fn(v)
        else
            target[k] = v -- no conversion function specified so assign as-is.
        end
    end
    return target
end

-- Given a key-value pair table `source`, create a table where the key-value
-- relationship is reversed. e.g. passing the following table:
-- 
--      weekdays = {"M", "T", "W", "TH", "F"}
-- 
-- Would create a copy that looks like:
-- 
--      weekdays = {["M"]=1, ["T"]=2, ["W"]=3, ["TH"]=4, ["F"]=5}
---@param source {}
function table.invert(source)
    local target = {}
    for k, v in pairs(source) do
        target[v] = k
    end
    return target
end

-- Create a copy of `source` array that contains only the values that 
-- successfully pass through the filter function.
-- 
-- Currently this only works properly on arrays.
---@generic T
---@param _Array T[]
---@param filter_fn fun(v: T):bool return value `true` means append to filtered table.
---@param convert_fn? fun(v: T):any Optional. By default we assign values as-is.
---@return T[]
function table.filter(_Array, filter_fn, convert_fn)
    local filtered = {}
    for _, v in ipairs(_Array) do
        if filter_fn(v) then
            -- Don't do filtered[k] = v, as you'll have holes in the array!
            -- Also don't do `fn and fn(v) or v` as it messes w/ bools.
            if convert_fn then
                filtered[#filtered + 1] = convert_fn(v)
            else
                filtered[#filtered + 1] = v
            end
        end
    end
    return filtered
end
