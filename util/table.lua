-- Copies table by value as table assignment is just moving pointers around.
-- 
-- By default, it just each value as is, no conversion at all. You can pass a 
-- function `convert_fn` which takes an iterated value are returns a conversion.
---@param source tbl
---@param convert_fn? fun(v: any):any `tostring`, `tonumber` or custom function.
function table.copy(source, convert_fn)
    local _Target = {}
    for k, v in pairs(source) do
        -- Invoke `copy_fn` only if exists, otherwise assign value as-is
        _Target[k] = (convert_fn and convert_fn(v)) or v
    end
    return _Target
end

-- Given a key-value pair table `source`, create a table based off of it where
-- the original keys are the new values and the original values are the keys.
function table.invert(source)
    local _Target = {}
    for k, v in pairs(source) do
        _Target[v] = k
    end
    return _Target
end

-- Create a copy of `source` array that contains only the values that 
-- successfully pass through the filter function.
-- 
-- Currently this only works properly on arrays.
---@generic T
---@param _Array T[]
---@param filter_fn fun(v:T):bool `true` = append to filtered table.
function table.filter(_Array, filter_fn)
    ---@generic T
    ---@type T[]
    local filtered = {} 
    for _, v in ipairs(_Array) do
        if filter_fn(v) then
            -- Don't do filtered[k] = v, as you'll have holes in the array!
            filtered[#filtered + 1] = v
        end
    end
    return filtered
end
