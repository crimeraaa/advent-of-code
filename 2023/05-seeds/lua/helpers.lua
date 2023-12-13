--------------------------------------------------------------------------------
----------------------------- STRUCT "DEFINITIONS" -----------------------------
--------------------------------------------------------------------------------

---@class MapLabels
---@field dst str destination category's name.
---@field src str source category's name.

---@class MapRange 
---@field dst num destination category's starting number.
---@field src num source category's starting number.
---@field range num range of numbers for this specific map.

---@class Map
---@field label MapLabels Of format: `"{src}-to-{dst} map:"`
---@field data MapRange[] Of format: `"{int} {int} {int}"`

---@class SeedPair
---@field start num Number with which to start `range` by.
---@field range num Up to how many seeds, offset from `start`, to check against.

--------------------------------------------------------------------------------
-------------------------------- IO FUNCTIONS ----------------------------------
--------------------------------------------------------------------------------

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

--------------------------------------------------------------------------------
------------------------- VARIOUS PRINT DATA FUNCTIONS -------------------------
--------------------------------------------------------------------------------

---@param tabs? int If not specified, returns an empty string.
function make_indent(tabs)
    return (tabs and string.rep("\t", tabs)) or ""
end

-- Creates format string with padding cuz Lua doesn't support `"%.*{spec}"`.
---@param fmtspec str
---@param pad int
---@param indent str
---@param reps? int
local function make_padded_formatstring(fmtspec, pad, indent, reps)
    reps = reps or 1
    local escaped = indent .. "| %%-%i" .. fmtspec .. " "
    -- double % (the %%- fmtspec) is an escape, to be formatted by the caller
    return string.format(escaped, pad):rep(reps) .. "|\n"
end

---@param label MapLabels
---@param data MapRange
---@param tab? int
function print_mapdata(label, data, tab)
    local indent = make_indent(tab)
    -- This is ugly but it works
    local col1_format = "range {size}"
    local col2_format = "src {".. label.src .."}"
    local col3_format = "dst {".. label.dst .."}"

    local padding = math.max(#col1_format, #col2_format, #col3_format)
    local label_format = make_padded_formatstring("s", padding, indent, 3)
    local items_format = make_padded_formatstring("i", padding, indent, 3)

    printf(label_format, col1_format, col2_format, col3_format);
    for i = 0, data.range - 1 do
        printf(items_format, i + 1, i + data.src, i + data.dst)
    end
end

---@param label MapLabels
---@param mapped int[]
---@param count int
---@param tab? int
function print_mapped(label, mapped, count, tab)
    local indent = make_indent(tab)
    local col1_format = "src {".. label.src .."}"
    local col2_format = "dst {".. label.dst .."}"

    local padding = math.max(#col1_format, #col2_format)
    local label_format = make_padded_formatstring("s", padding, indent, 2)
    local items_format = make_padded_formatstring("i", padding, indent, 2)

    printf("%s-to-%s map: (%i elements)\n", label.src, label.dst, count)
    printf(label_format, col1_format, col2_format);

    -- Lua numeric `for` loops are inclusive of condition, so need subtract 1
    for i = 0, count - 1 do
        printf(items_format, i, mapped[i])
    end
end

---@param start num
---@param stop num
local function make_range_string(start, stop)
    return string.format("[%.0f...%.0f]", start, stop)
end

-- Print out the current map's src and dst value ranges.
---@param label MapLabels
---@param data MapRange
---@param limit tbl
---@param tabs? int
function print_mapping_ranges(label, data, limit, tabs)
    local indent = make_indent(tabs)
    local ranges = {
        src = make_range_string(data.src, limit.src),
        dst = make_range_string(data.dst, limit.dst),
    }
    printf("%s[\"%s-to-%s map\"] = ", indent, label.src, label.dst)
    printf("\"%s=>%s\",\n", ranges.src, ranges.dst)
end

---@param label MapLabels
---@param source int
---@param mapped int
---@param tab? int
function print_final_mapping(label, source, mapped, tab)
    local indent = make_indent(tab)
    printf("%s--%s %s => %s: %.0f\n", indent, label.src, source, label.dst, mapped)
end

--------------------------------------------------------------------------------
-------------------------- PRINT TABLEDATA FUNCTIONS ---------------------------
--------------------------------------------------------------------------------

function isarray(tbl)
    local arraysize = #tbl -- `#` operator only considers numeric keys
    local actualsize = 0 -- non-numeric keys are counted in `arraysize`
    for _ in pairs(tbl) do
        actualsize = actualsize + 1
    end
    return (arraysize == actualsize)
end

-- Convert the given table key `k` for prettier output.
-- Numerical indexes are converted to `"[%i]"`, everything else is `tostring`'d.
local function convert_key(indent, k)
    if type(k) == "number" then
        return string.format("[%.0f]", k)
    else
        return tostring(k)
    end
end

-- Surround strings with quotes for differentiation.
local function convert_value(v)
    return (type(v) == "string" and '"'..v..'"') or tostring(v)
end

-- Usually, you want to indent one more to `tabs` to show this element as being
-- under the scope of a table.
local function print_entry(tabs, key, value, endl)
    local indent = make_indent(tabs)
    printf("%s%s=%s,%s", indent, key, value, endl)
end

-- Print a generic key-value pair table. Can recursively print subtables!
---@param tbl tbl
---@param name str
---@param tabs? int How much to indent by: `0` (default)
---@param iterator_fn? function Pass one of: `ipairs` or `pairs` (default)
---@param recurse? int Limit to stop recursing at: `8` (default)
function print_table(tbl, name, tabs, iterator_fn, recurse)
    tabs = tabs or 0
    recurse = recurse or 8
    if recurse and (recurse <= 0) then
        return
    end
    iterator_fn = iterator_fn or pairs

    -- local endl = (isarray(tbl) and "\n") or ""
    local endl = "\n"
    local indent = make_indent(tabs)
    printf("%s%s = {%s", indent, name, endl)

    -- Dump information of 0-based tables, Lua iterators don't catch these
    if tbl[0] then
        print_entry(tabs + 1, convert_key(indent, 0), tbl[0], endl)
    end

    for elem_key, elem_value in iterator_fn(tbl) do
        local elem_name = convert_key(indent, elem_key)
        if type(elem_value) == "table" then
            --! Prolly infinite loop if 2/more tables point back to each other
            print_table(
                elem_value,
                elem_name,
                tabs + 1, 
                (isarray(elem_value) and ipairs) or pairs,
                recurse - 1
            )
        else
            print_entry(tabs + 1, elem_name, convert_value(elem_value), endl)
        end
    end
    -- For generic K-V tables, don't indent the last element as there's no newline
    -- we want to cram them into one line.
    -- local closingbracket_startl = (isarray(tbl) and indent) or ""

    -- In 0 indent scope (e.g. primary table), don't add comma after the bracket
    local brace_endl = (tabs > 0 and ",") or ""
    printf("%s}%s%s", indent, brace_endl, endl)
end

-- Print an ordered table which is effectively an array.
-- If index 0 exists, there is a special case to print it as well.
---@param array any[]
---@param name str
---@param tabs? int
function print_array(array, name, tabs)
    print_table(array, name, tabs or 0, ipairs)
end
