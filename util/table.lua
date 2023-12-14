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

-- Given an array (or a table with an array portion), reverse the order of elements.
-- Note that, by default, this creates a copy of the table!
---@param source any[]
---@param dontcopy? bool `true` = modify `source` directly.
function table.reverse(source, dontcopy)
    -- If `dontcopy` is passed, is a reference/pointer to `source` itself.
    local _Target = (dontcopy and source) or table.copy(source)
    local i, ii = 1, #_Target
    while (i < ii) do
        -- like in Python, this compound assignment works as you want it to!
        _Target[i], _Target[ii] = _Target[ii], _Target[i]
        i = i + 1
        ii = ii - 1
    end
    return _Target
end
