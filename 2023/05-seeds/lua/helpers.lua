---@class MapLabels
---@field dst str destination category's name.
---@field src str source category's name.

---@class MapRange 
---@field dst int destination category's starting number.
---@field src int source category's starting number.
---@field range int range of numbers for this specific map.

---@class Map
---@field label MapLabels Of format: `"{src}-to-{dst} map:"`
---@field data MapRange[] Of format: `"{int} {int} {int}"`

-- Fully qualified path of caller's current working director, not the script's.
-- Ugly but works: https://stackoverflow.com/a/6036884
-- CWD = os.getenv("PWD") or io.popen("cd", "r"):read("*l")

-- C-style printing of formatted strings.
---@param fmt str String literal or C-style format string.
---@param ... str|int Arguments to C-style format string.
function printf(fmt, ...)
    io.stdout:write(fmt:format(...))
end

-- Use absolute path for Lua debug in VSCode cause it uses workspace directory
local fallback = "C:/Users/crimeraaa/repos/advent-of-code/2023/05-seeds/"

-- Opens a file with `filename` for reading and stores each line in an array.
---@param filename str 
function readfile(filename)
    local file, errmsg = io.open(filename, "r")
    if not file then
        printf("%s, using fallback directory %s\n", errmsg, fallback)
        -- Remove ./ or ../ from the start of the filename
        file, errmsg = io.open(fallback..filename:gsub("^%.+[/\\]", ""), "r")
    end
    assert(file, errmsg)
    local lines = {} ---@type string[]
    local lineno = 1
    for line in file:lines() do
        lines[lineno] = line
        lineno = lineno + 1
    end
    file:close()
    return lines
end

-- Creates format string with padding cuz Lua doesn't support `"%.*{spec}"`.
---@param fmtspec str
---@param pad int
---@param reps? int
local function make_padded_formatstring(fmtspec, pad, reps)
    reps = reps or 1
    local escaped = "| %%-%i" .. fmtspec .. " "
    -- double % (the %%- fmtspec) is an escape, to be formatted by the caller
    return string.format(escaped, pad):rep(reps) .. "|\n"
end

---@param label MapLabels
---@param data MapRange
function print_mapdata(label, data)
    -- This is ugly but it works
    local col1_format = "range {size}"
    local col2_format = "src {".. label.src .."}"
    local col3_format = "dst {".. label.dst .."}"

    local padding = math.max(#col1_format, #col2_format, #col3_format)
    local label_format = make_padded_formatstring("s", padding, 3)
    local items_format = make_padded_formatstring("i", padding, 3)

    printf(label_format, col1_format, col2_format, col3_format);
    for i = 0, data.range - 1 do
        printf(items_format, i + 1, i + data.src, i + data.dst)
    end
end

---@param label MapLabels
---@param mapped int[]
---@param count int
function print_mapped(label, mapped, count)
    local col1_format = "src {".. label.src .."}"
    local col2_format = "dst {".. label.dst .."}"

    local padding = math.max(#col1_format, #col2_format)
    local label_format = make_padded_formatstring("s", padding, 2)
    local items_format = make_padded_formatstring("i", padding, 2)

    printf("%s-to-%s map: (%i elements)\n", label.src, label.dst, count)
    printf(label_format, col1_format, col2_format);

    -- Lua numeric `for` loops are inclusive of condition, so need subtract 1
    for i = 0, count - 1 do
        printf(items_format, i, mapped[i])
    end
end

function isarray(tbl)
    local arraysize = #tbl -- `#` operator only considers numeric keys
    local actualsize = 0 -- non-numeric keys are counted in `arraysize`
    for k in pairs(tbl) do
        actualsize = actualsize + 1
    end
    return (arraysize == actualsize)
end

-- Lookup table to determine how to convert table keys to strings for output.
-- I still find it funny that non-string and non-integer keys are valid
---@type table<function, fun(key:str|int):str>
local key_fns = {
    [ipairs] = function(key)
        return string.format("[%i]", key)
    end,
    [pairs] = function(key)
        return tostring(key)
    end
}

---@param item_isarray bool
local function determine_fns(item_isarray)
    local pairs_fn = (item_isarray and ipairs) or pairs
    local key_fn = (item_isarray and key_fns[ipairs]) or key_fns[pairs_fn]
    return pairs_fn, key_fn
end

-- Print a generic key-value pair table. Can recursively print subtables!
---@param tbl tbl
---@param name str
---@param tabs? int Default to 0.
---@param pairs_fn? function Default to `pairs`.
---@param key_fn? fun(key:str|int):str Default to fn returning `tostring(key)`.
function print_table(tbl, name, tabs, pairs_fn, key_fn)
    tabs = tabs or 0
    pairs_fn = pairs_fn or pairs
    key_fn = key_fn or key_fns[pairs_fn]

    local indent = string.rep("\t", tabs)
    printf("%s%s = {\n", indent, name)

    -- Dump information of 0-based tables, Lua iterators don't catch these
    if tbl[0] then
        printf("\t%s%s=%s,\n", indent, key_fn(0), tbl[0])
    end

    for key, item in pairs_fn(tbl) do
        local itemname = key_fn(key)
        if type(item) == "table" then
            -- Don't overwrite caller's functions, you may screw something up!
            local _pairs_fn, _key_fn = determine_fns(isarray(item))
            -- lol recursion
            print_table(item, itemname, tabs + 1, _pairs_fn, _key_fn)
        else
            -- Surround strings with quotes for differentiation
            item = (type(item) == "string" and '"'..item..'"') or tostring(item)
            -- Indent one more to show this element as under scope of table
            printf("\t%s%s=%s,\n", indent, itemname, item)
        end
    end
    printf("%s},\n", indent)
end

-- Print an ordered table which is effectively an array.
-- If index 0 exists, there is a special case to print it as well.
---@param array any[]
---@param name str
---@param tabs? int
function print_array(array, name, tabs)
    print_table(array, name, tabs or 0, ipairs)
end
