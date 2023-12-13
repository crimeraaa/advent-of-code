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
------------------------- VARIOUS PRINT DATA FUNCTIONS -------------------------
--------------------------------------------------------------------------------

-- Creates format string with padding cuz Lua doesn't support `"%.*{spec}"`.
---@param fmtspec str
---@param pad int
---@param indent str
---@param reps? int
---@nodiscard
local function make_padded(fmtspec, pad, indent, reps)
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
    local label_format = make_padded("s", padding, indent, 3)
    local items_format = make_padded("i", padding, indent, 3)

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
    local label_format = make_padded("s", padding, indent, 2)
    local items_format = make_padded("i", padding, indent, 2)

    printf("%s-to-%s map: (%i elements)\n", label.src, label.dst, count)
    printf(label_format, col1_format, col2_format);

    -- Lua numeric `for` loops are inclusive of condition, so need subtract 1
    for i = 0, count - 1 do
        printf(items_format, i, mapped[i])
    end
end

-- Get the inclusive endpoints of a range. They are subtracted by 1, as in 
-- `data.src=98`, `data.range=2` means the source's endpoint is `99`, not `100`.
---@param data MapRange
---@nodiscard
function get_maprange_limits(data)
    local offset = data.range - 1
    return data.src + offset, data.dst + offset
end

-- Creates the correct range as a string, e.g. given `start=98` and `range=2`,
-- the final range should be `"[98...99]"` as there are 2 elements: 98 and 99.
-- 
-- Notice how we didn't include 100 even though 98 + 2 is 100, as 100 would mean
-- that there's 3 elements: 98, 99, 100. Hence we subtract 1 for the right side.
---@param start num Value to start the range by.
---@param range num How many elements are in the current range.
---@param dont_calculate? bool If `true`, don't use `range` in calulating the right side.
---@nodiscard
function make_rangestr(start, range, dont_calculate)
    local right = (dont_calculate and range) or (start + range - 1)
    -- Don't use %i as may not fit in int, our values are bigger than 32 bits!
    -- Instead, use Lua's "%.0f" specifier for maximum precision.
    return string.format("[%.0f...%.0f]", start, right)
end

---@param data MapRange
---@param dont_calculate? bool If `true`, don't use `data.range` in calulating the right side.
---@nodiscard
function make_map_rangestrs(data, dont_calculate)
    return make_rangestr(data.src, data.range, dont_calculate),
        make_rangestr(data.dst, data.range, dont_calculate)
end

-- Print out the current map's src and dst value ranges.
---@param label MapLabels
---@param data MapRange
---@param limit tbl
---@param tabs? int
function print_mapping_ranges(label, data, limit, tabs)
    local indent = make_indent(tabs)
    local ranges = {
        src = make_rangestr(data.src, limit.src),
        dst = make_rangestr(data.dst, limit.dst),
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


