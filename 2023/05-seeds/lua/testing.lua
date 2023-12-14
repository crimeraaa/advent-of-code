require("util/common")
require("util/stdio")
require("util/string")
require("util/prettyprint")

require("newhelpers")

local PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("2023/05-seeds/")

local FALLBACK = PROJECT_DIR .. "sample.txt"

---@param seedsline str
local function make_seeds_db(seedsline)
    local db = {} ---@type NewRange[]
    for first, second in seedsline:gmatch("(%d+) (%d+)") do
        db[#db + 1] = NewRange.new(first, second) -- calls tonumber internally
    end
    return db
end

---@param lines str[]
local function make_maps_db(lines)
    local db = {} ---@type tbl<str, NewMap> Hashtable that we can "query"

    -- Hold string matches of current map's source and target category names.
    local label = {src="", dst=""} ---@type NewLabel

    -- hold string matches of mapping ranges, need convert to numbers later
    -- when updating the current map's list of possible ranges.
    local ranges = {src="",dst="",length=""} 

    -- Keep a pointer outside of loop scope so we can poke at the current map.
    ---@type NewMap
    local handle 

    for _, line in ipairs(lines) do
        ---@type str?, str?
        label.src, label.dst = line:match("(%w+)%-to%-(%w+) map:")

        -- Watch the order! Remember format is: {dst} {src} {range}.
        ---@type str?, str?, str?
        ranges.dst, ranges.src, ranges.length = line:match("(%d+) (%d+) (%d+)")

        -- Main assumption: label and data don't exist on the same line!
        if label.src and label.dst then
            -- New category, so hash it!
            ---@type NewMap
            db[label.src] = {
                label = copy_table(label), -- copy by value, not pointer
                ranges = {}
            }
            handle = db[label.src]
        elseif ranges.dst and ranges.src and ranges.length then
            -- Update current category's possible mapping/s, there can be many
            handle.ranges[#handle.ranges + 1] = {
                src = NewRange.new(ranges.src, ranges.length),
                dst = NewRange.new(ranges.dst, ranges.length)
            }
        end
    end
    return db
end

-- Check if `range.src.length` and `range.dst.length` are the same.
---@param range NewMapRanges
local function get_map_rangesize(range)
    local _Src, _Dst = range.src.length, range.dst.length
    return (_Src == _Dst) and make_lengthstr(_Src) or "TODO: range lengths inconsistent!"
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)

    -- the "seeds: %d %d..." line, but only the numbers part!
    -- `table.remove` takes care of rearranging the input table for us. We do so
    -- because we want to parse only the ranges afterwards.
    -- 
    -- Note that `string.trim` is a custom function in `utils/string.lua`.
    ---@type str
    local seedsline = table.remove(lines, 1):match("[%d%s]+"):trim()
    local seeds_db = make_seeds_db(seedsline)
    local maps_db = make_maps_db(lines)

    -- Dump seed ranges
    for _, seed in ipairs(seeds_db) do
        printf("seeds%s %s\n", make_rangestr(seed), make_lengthstr(seed.length))
    end

    -- Always start by querying the "seed" table first!
    local maphandle = maps_db["seed"] ---@type NewMap 

    -- Dump mapping ranges
    while maphandle do
        printf("%s-to-%s map:\n", maphandle.label.src, maphandle.label.dst)
        for _, range in ipairs(maphandle.ranges) do
            local _SrcRange, _DstRange = make_rangestr(range.src), make_rangestr(range.dst)
            local _Size = get_map_rangesize(range)
            printf("\t%-9s => %-9s %s\n", _SrcRange, _DstRange, _Size)
        end
        maphandle = maps_db[maphandle.label.dst]
    end
end

return main(#arg, arg)
