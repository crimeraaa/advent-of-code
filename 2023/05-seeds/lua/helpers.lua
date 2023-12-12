---@class MapLabels
---@field dst str destination category's name.
---@field src str source category's name.

---@class MapData 
---@field dst int destination category's starting number.
---@field src int source category's starting number.
---@field range int range of numbers for this specific map.

---@class Map
---@field label MapLabels Of format: `"{src}-to-{dst} map:"`
---@field data MapData[] Of format: `"{int} {int} {int}"`

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

---@param labels MapLabels
---@param data MapData
function print_mappings(labels, data)
    -- This is ugly but it works
    local col1_format = "range {size}"
    local col2_format = "src {".. labels.src .."}"
    local col3_format = "dst {".. labels.dst .."}"

    local padding = math.max(#col1_format, #col2_format, #col3_format)
    local label_format = make_padded_formatstring("s", padding, 3)
    local items_format  = make_padded_formatstring("i", padding, 3)

    printf(label_format, col1_format, col2_format, col3_format);
    for i = 0, data.range - 1 do
        printf(items_format, i + 1, i + data.src, i + data.dst)
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

---@param tbl tbl
---@param name str
---@param tabs? int
---@param pairs_fn function
---@param keyname_fn fun(key:str|int):str
local function print_table_generic(tbl, name, tabs, pairs_fn, keyname_fn)
    tabs = tabs or 0
    local indent = string.rep("\t", tabs)
    printf("%s%s = {\n", indent, name)
    for k, v in pairs_fn(tbl) do
        local item = keyname_fn(k)
        if type(v) == "table" then
            -- kinda like ternary operator in C
            local iterator_fn = (isarray(v) and ipairs) or pairs
            print_table_generic(tbl, item, tabs + 1, iterator_fn, keyname_fn)
        else
            -- Surround strings with quotes for differentiation
            v = (type(v) == "string" and '"'..v..'"') or tostring(v)
            -- Indent one more to show this element as being part of the table
            printf("\t%s%s=%s,\n", indent, k, v)
        end
    end
    printf("%s},", indent)
end

-- Print a generic key-value pair table. Can recursively print subtables!
---@param tbl tbl
---@param name str
---@param tabs? int
function print_table(tbl, name, tabs)
    tabs = tabs or 0
    local indent = string.rep("\t", tabs)
    printf("%s%s = {\n", indent, name)
    for k, v in pairs(tbl) do
        if type(v) == "table" then
            if isarray(v) then
                print_array(v, k, tabs + 1)
            else
                print_table(v, k, tabs + 1) -- Try to cram into a single line
            end
        else
            k = tostring(k)
            -- kinda like ternary operator in C.
            v = (type(v) == "string" and '"'..v..'"') or tostring(v)
            -- Indent one more to show this element as being part of the table
            printf("\t%s%s=%s,\n", indent, k, v)
        end
    end
    printf("%s},\n", indent)
end

-- Print an ordered table which is effectively an array.
---@param array any[]
---@param name str
---@param tabs? int
function print_array(array, name, tabs)
    tabs = tabs or 0
    local indent = string.rep("\t", tabs)
    printf("%s%s = {\n", indent, name)
    for i, v in ipairs(array) do
        local item = string.format("[%i]", i)
        -- Subtable names will be indented properly by `print_table`
        if type(v) == "table" then
            if isarray(v) then
                printf("\n") -- only move arrays away from caller's `{`
                print_array(v, item, tabs + 1)
            else
                print_table(v, item, tabs + 1)
            end
        else
            -- Indent one more to show this element as being part of the table
            printf("\t%s%s = %s\n", indent, item, tostring(v))
        end
    end
    printf("%s},\n", indent)
end
