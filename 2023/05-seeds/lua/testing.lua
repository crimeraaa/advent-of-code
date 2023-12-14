require("util/common")
require("util/stdio")
require("util/string")
require("util/prettyprint")

require("newhelpers")

local PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("/2023/05-seeds/")

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

-- Your starting `query` can beginning "seed" or maybe do some smart recursion.
---@param query str Used to query into `database`.
---@param database tbl<str, NewMap> All possible {thing}-to-{thing} mappings.
---@param seed NewRange Current seed range we're checking for.
---@param tab? int Index into the global `INDENT` table, can be 0.
local function query_maps(query, database, seed, tab)
    tab = tab or 0
    local handle = database[query] ---@type NewMap
    -- For now, dump mapping ranges
    while handle do
        printf("%s%s-to-%s map:\n", INDENT[tab], handle.label.src, handle.label.dst)
        -- This map may have 1 or more src-to-data relationships
        for _, range in ipairs(handle.ranges) do
            local _SrcRange, _DstRange = make_rangestr(range.src), make_rangestr(range.dst)
            local _Size = get_map_rangesize(range)
            local _Offset = range.dst.start - range.src.start

            -- 1 indent higher cuz part of mapping block
            printf("%s%-9s => %-9s %s\n", INDENT[tab+1], _SrcRange, _DstRange, _Size)

            -- Determine where how our seed range fits in this mapping
            if seed.start >= range.src.start and seed.start <= range.src.endpt then
                local _Start, _Endpt = seed.start + _Offset, seed.endpt + _Offset
                printf("%s^seed.start: %.0f => %.0f\n", INDENT[tab+2], seed.start, _Start)
                printf("%s^seed.endpt: %.0f => %.0f ", INDENT[tab+2], seed.endpt, _Endpt)
                seed.start = _Start
                if seed.endpt >= range.src.start and seed.endpt <= range.src.endpt then
                    seed.endpt = _Endpt
                    printf("\n")
                else
                    -- TODO: determine values of child tables first
                    printf("intersects multiple maps!\n")
                    -- TODO: recurse? use return to end control flow here 
                    -- in which case should prolly make a new `Range` object
                    
                end
            end
        end
        query = handle.label.dst -- next query is this mapping's destination
        handle = database[query]
    end
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
        -- Always start by querying the "seed" (not "seeds") table first!
        query_maps("seed", maps_db, seed, 1)
    end

end

return main(#arg, arg)
