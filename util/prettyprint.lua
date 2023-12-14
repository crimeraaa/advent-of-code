require("util/stdio")
require("util/string")

---@param tabs? int If not specified, returns an empty string.
function make_indent(tabs)
    return (tabs and string.rep("\t", tabs)) or ""
end

-- Index into this so you can quickly set indent "levels" in printouts.
-- index 0 is a sort of base case: just an empty string, no indentation.
INDENT = {}
for i = 0, 100 do
    INDENT[i] = make_indent(i)
end

-- Create an evenly spaced format string: `"| {label} | {label} | ... |"`
---@param padding int How much to space out by.
---@param reps? int How many cells to create. Default: `1`
function make_padded_format(padding, reps)
    reps = reps or 1
    local _FormatString = "| %-".. padding .."s "
    return string.rep(_FormatString, reps) .. "|\n"
end

--------------------------------------------------------------------------------
----------------------- PRETTY TABLE PRINTOUT FUNCTIONS ------------------------
--------------------------------------------------------------------------------

local function is_array(tbl)
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

-- Surround strings with quotes for differentiation or just convert non-strings.
local function convert_value(v)
    return (type(v) == "string" and '"'..v..'"') or tostring(v)
end

-- Usually, you want to pass `tabs + 1` to show this element as being
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
                (is_array(elem_value) and ipairs) or pairs,
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
