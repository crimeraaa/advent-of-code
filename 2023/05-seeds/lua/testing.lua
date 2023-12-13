require("util/common")
require("util/stdio")
require("util/prettyprint")

require("helpers")

local PROJECT_DIR = WORKSPACE_DIR .. convert_to_ospath("2023/05-seeds/")

local FALLBACK = PROJECT_DIR .. "sample.txt"

---@param lines str[]
local function parse_lines(lines)
    local seeds = {} ---@type SeedPair[]
    local database = {} ---@type tbl<str, Map>

    -- Keep handle in memory across loop scope so we can modify it,
    -- as `label_` variables may be nil values whenever they're matched.
    local maphandle ---@type Map

    local label = {src="",dst=""} ---@type MapLabels
    local value = {src=0,dst=0,range=0} ---@type MapRange

    for _, line in ipairs(lines) do
        -- I'm assuming the labels and values can't exist at the same time.
        label.src, label.dst = line:match("(%w+)%-to%-(%w+) map:")

        -- Watch the order, remember format is: {dst} {src} {range}.
        value.dst, value.src, value.range = line:match("(%d+) (%d+) (%d+)")

        if line:find("seeds: [%d%s]+") then
            for start, range in line:gmatch("(%d+) (%d+)") do
                seeds[#seeds + 1] = {
                    start = tonumber(start),
                    range = tonumber(range)
                }
            end
        elseif label.src then
            ---@type Map New category key found so hash it.
            database[label.src] = {label = copy_table(label), data = {}}
            -- Assign so we can use it when values are found
            maphandle = database[label.src] ---@type Map
        elseif value.dst then
            maphandle.data[#maphandle.data + 1] = copy_table(value, tonumber)
        end
    end
    return seeds, database
end

---@param handle SeedPair
---@param data MapRange
---@param indent str[]
---@param tab int
local function loop_submaps(handle, data, indent, tab)
    -- Given `{dst=50,src=98,range=2}` so offset = `dst-src`.
    -- 
    -- We then get `50 - 98 = -48` as our value to map by.
    local offset = data.dst - data.src

    -- These are inclusive.
    local limit_src, limit_dst = get_maprange_limits(data)
    local range_src, range_dst = make_map_rangestrs(data)

    printf("%s%s => %s", indent[tab], range_src, range_dst)
    printf("%soffset: %.0f\n", indent[tab], offset, limit_src, limit_dst)

    if is_in_range(handle.start, data.src, limit_src) then
        local old = handle.start -- keep around, need it quite a bit.
        local new = old + offset

        printf("%sRemap handle.start: %.0f=>%.0f\n", indent[tab], old, new)
        if not is_in_range(new + handle.range, data.src, limit_src) then
            printf("%sRemap handle.range: %.0f\n", indent[tab], handle.range)
            ---@type SeedPair
            local left_src = {
                start = old, 
                range = limit_src - old + 1 -- range is exclusive so add 1 to offset
            }
            local left_dst = {
                start = new,
                range = left_src.range
            }
            printf(
                "%ssubmap: src%s=>dst%s\n", 
                indent[tab+1], 
                make_rangestr(left_src.start, left_src.range),
                make_rangestr(left_dst.start, left_dst.range)
            )

            ---@type SeedPair
            local right_src = {
                start = limit_src + 1, -- add 1 for next element, 
                range = (old + handle.range - 1) - limit_src -- sub 1 for endpoint.
            }
            printf(
                "%ssubmap: src%s=>TODO: get outside mapping\n", 
                indent[tab+1], 
                make_rangestr(right_src.start, right_src.range)
            )
        else
            printf("%sNo need remap handle.range: %.0f\n", indent[tab], handle.range)
        end
        handle.start = new
    end 
end

---@param seeds SeedPair
---@param database tbl<str, Map>
local function find_location(seeds, database)
    local seedrange = make_rangestr(seeds.start, seeds.range)
    local map = database["seed"] ---@type Map we start by querying key "seed".

    -- We will definitely have multiple mappings that are not contiguous.
    -- It's 2D so we can have multimaps.
    ---@type SeedPair[][] 
    local mapped = {} 

    -- sub_i is the index of which multimap we're poking at
    local main_i, sub_i = 1, 1 

    mapped[main_i] = {
        {start = seeds.start, range = seeds.range}
    }
    printf("Seeds: %s\n", seedrange)

    local tab = 1
    -- Indent levels so we don't construct the strings over and over.
    local indent = {
        make_indent(tab+0),
        make_indent(tab+1),
        make_indent(tab+2),
        make_indent(tab+3)
    }
    
    while map do
        print_handles(mapped, main_i, sub_i, indent, tab)
        printf("%s%s-to-%s map:\n", indent[tab], map.label.src, map.label.dst)
        main_i = main_i + 1
        mapped[main_i] = make_handle_array(mapped, main_i - 1, sub_i)
        for loop_i = 1, sub_i do
            local handle = mapped[main_i][loop_i]
            for _, data in pairs(map.data) do
                loop_submaps(handle, data, indent, tab + 1)
                -- local left, right = loop_submaps(handle, data, indent, tab + 1)
                -- if left and right then
                --     mapped[main_i][sub_i] = left -- Replace the original map
                --     mapped[main_i][sub_i + 1] = right -- Newly split map!
                --     sub_i = sub_i + 1
                -- end
            end
            printf("\n")
        end
        map = database[map.label.dst]
    end
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    
    local seedpairs, database = parse_lines(lines)

    for _, seeds in ipairs(seedpairs) do
        find_location(seeds, database)
    end
end

return main(#arg, arg)
