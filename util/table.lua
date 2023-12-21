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

---@param source {}
function table.isarray(source)
    local arraysize = #source -- `#` operator only considers numeric keys
    local totalsize = 0 -- non-numeric keys, e.g. strings, are not yet counted
    -- No need keys or values, just need number of iterations
    for _ in pairs(source) do
        totalsize = totalsize + 1
    end
    return arraysize == totalsize
end

-- Convert the given table key `k` for prettier output.
-- Numerical indexes are converted to `"[%i]"`, everything else is `tostring`'d.
local function get_keyname(key)
    local is_num = (type(key) == "number") 
    local is_str = (type(key) == "string")
    local fmt = (is_num and "[%.0f]") or (is_str and "[\"%s\"]") or "[%s]"
    local name = (is_num and tonumber(key)) or tostring(key)
    return fmt:format(name)
end

local function get_valuestring(k, v, tab)
    v = (type(v) == "string" and string.format("\"%s\"", v)) or tostring(v)
    return string.format("%sK: %s\tV: %s", INDENT[tab], k, v)
end

-- Somewhat neatly dump all values in the table, including metatables.
---@param tbl {}
---@param name? str
---@param iter_fn? function
---@param visited? { [tbl]: bool}
---@param tab? int
function table.dump(tbl, name, iter_fn, visited, tab)
    visited = visited or {}
    if visited[tbl] then
        return string.format("Already visited table %s. Skipping...", tostring(tbl))
    end
    visited[tbl] = true -- need to do it already else may infinite loop

    name = name or "(table)"
    tab = tab or 0
    iter_fn = iter_fn or (table.isarray(tbl) and ipairs or pairs)

    local items = {} ---@type str[]
    items[#items+1] = string.format("%sK: %s\tV: %s", INDENT[tab], name, tostring(tbl))
    
    -- printf("%s%s = {\n", INDENT[tab], name)
    for k, v in iter_fn(tbl) do
        local keyname = get_keyname(k)
        if type(v) == "table" then
            if not visited[v] then
                local fn = iter_fn or (table.isarray(v) and ipairs or pairs)
                local output = table.dump(v, keyname, fn, visited, tab+1)
                if output then
                    items[#items+1] = output
                end
                visited[v] = true
            end
        else
            items[#items+1] = get_valuestring(keyname, v, tab+1)
        end
    end
    local mt = getmetatable(tbl)
    if mt and not visited[mt] then
        local output = table.dump(mt, "[metatable]", pairs, visited, tab+1)
        if output then
            items[#items+1] = output
        end
        visited[mt] = true
    end
    return table.concat(items, "\n")
end

-- Create a simple single-line string of all the iteratable elements.
-- 
-- Do note this can get very ugly with nested tables.
---@param tbl {}
---@param visited? { [tbl]: bool }
function table.tostring(tbl, visited)
    visited = visited or {}
    if visited[tbl] then
        return ""
    end
    visited[tbl] = true -- mark this table as visited so future calls skip it
    local items = {}
    local iter = table.isarray(tbl) and ipairs or pairs
    for k, value in iter(tbl) do
        local output
        if type(value) == "table" then
            output = table.tostring(value, visited)
        elseif type(value) == "string" then
            output = string.format("\"%s\"", value)
        else
            output = tostring(value)
        end
        -- Don't include [] for numerical indexes as can get very ugly
        if type(k) ~= "number" then
            output = string.format("%s = %s", get_keyname(k), output)
        end
        items[#items+1] = output
    end
    return "{".. table.concat(items, ", ") .."}"
end
