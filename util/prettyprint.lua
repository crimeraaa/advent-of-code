require "util/stdio"
require "util/string"

---@param tabs? int If not specified, returns an empty string.
function make_indent(tabs)
    return (tabs and string.rep("\t", tabs)) or ""
end

-- Index into this so you can quickly set indent "levels" in printouts.
-- index 0 is a sort of base case: just an empty string, no indentation.
---@type str[]
INDENT = {}
for i = 0, 16 do
    INDENT[i] = make_indent(i)
end

setmetatable(INDENT, {
    -- For non-existent indent indexes, we'll create them as needed
    __index = function(tbl, index) 
        INDENT[index] = make_indent(index)
        return INDENT[index]
    end
})

-- Create an evenly spaced format string: `"| {label} | {label} | ... |"`
---@param padding int How much to space out by.
---@param reps? int How many cells to create. Default: `1`
function make_padded_format(padding, reps)
    reps = reps or 1
    local _FormatString = "| %-".. padding .."s "
    return _FormatString:rep(reps) .. "|\n"
end

--------------------------------------------------------------------------------
----------------------- PRETTY TABLE PRINTOUT FUNCTIONS ------------------------
--------------------------------------------------------------------------------

-- Convert the given table key `k` for prettier output.
-- Numerical indexes are converted to `"[%i]"`, everything else is `tostring`'d.
local function convert_key(k)
    local _IsNum = (type(k) == "number") 
    local _IsStr = (type(k) == "string")
    local _Format = (_IsNum and "[%.0f]") or (_IsStr and "[\"%s\"]") or "[%s]"
    local _Keyname = (_IsNum and tonumber(k)) or tostring(k)
    return _Format:format(_Keyname)
end

-- Surround strings with quotes for differentiation or just convert non-strings.
local function convert_value(v)
    return (type(v) == "string" and string.format("\"%s\"", v)) or tostring(v)
end

-- Usually, you want to pass `tabs + 1` to show this element as being
-- under the scope of a table.
local function print_entry(tab, key, value, endl)
    printf("%s%s=%s,%s", INDENT[tab], key, value, endl)
end

-- Print a generic key-value pair table. Can recursively print subtables!
---@param tbl tbl
---@param name str
---@param tab? int How much to indent by: `0` (default)
---@param iterator_fn? function Pass one of: `ipairs` or `pairs` (default)
---@param recurse? int Limit to stop recursing at: `8` (default)
function print_table(tbl, name, tab, iterator_fn, recurse)
    tab = tab or 0
    recurse = recurse or 8
    if recurse and (recurse <= 0) then
        return
    end
    iterator_fn = iterator_fn or pairs

    -- local endl = (isarray(tbl) and "\n") or ""
    local _Endl = "\n"
    printf("%s%s = {%s", INDENT[tab], name, _Endl)

    -- Dump information of 0-based tables, Lua iterators don't catch these
    -- In case we have an __index metamethod, don't invoke it else we may
    -- loop infinitely!
    local index_zero = rawget(tbl, 0)
    if index_zero then
        print_entry(tab + 1, convert_key(0), convert_value(index_zero), _Endl)
    end

    for k, v in iterator_fn(tbl) do
        local _ElemName = convert_key(k)
        if type(v) == "table" then
            --! Prolly infinite loop if 2/more tables point back to each other.
            local _PairsFn = (table.isarray(v) and ipairs) or pairs
            print_table(v, _ElemName, tab + 1, _PairsFn, recurse - 1)
        else
            print_entry(tab + 1, _ElemName, convert_value(v), _Endl)
        end
    end
    -- For generic K-V tables, don't indent the last element as there's no newline
    -- we want to cram them into one line.
    -- local closingbracket_startl = (isarray(tbl) and indent) or ""

    -- In 0 indent scope (e.g. primary table), don't add comma after the bracket
    local _BraceEnd = (tab > 0 and ",") or ""
    printf("%s}%s%s", INDENT[tab], _BraceEnd, _Endl)
end

-- Print an ordered table which is effectively an array.
-- If index 0 exists, there is a special case to print it as well.
---@generic T
---@param array T[]
---@param name str
---@param tabs? int
function print_array(array, name, tabs)
    print_table(array, name, tabs or 0, ipairs)
end
