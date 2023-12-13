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

---@param value num
---@param range_start num
---@param range_stop num
local function is_in_range(value, range_start, range_stop)
    return (value >= range_start) and (value <= range_stop)
end

---@param indent str[]
---@param handle SeedPair
---@param data MapRange
local function update_thismap(indent, handle, data)
    -- Given `{dst=50,src=98,range=2}` so offset = `dst-src`.
    -- 
    -- We then get `50 - 98 = -48` as our value to map by.
    local offset = data.dst - data.src

    -- These are inclusive.
    local limit_src, limit_dst = get_maprange_limits(data)
    local range_src, range_dst = make_map_rangestrs(data)

    printf("%s%s => %s", indent[2], range_src, range_dst)
    printf("%soffset: %.0f\n", indent[1], offset, limit_src, limit_dst)

    if is_in_range(handle.start, data.src, limit_src) then
        local old = handle.start -- keep around, need it quite a bit
        handle.start = old + offset
        printf("%sRemap handle.start: %.0f=>%.0f\n", indent[2], old, handle.start)
        if not is_in_range(handle.start + handle.range, data.src, limit_src) then
            printf("%sTODO: Remap handle.range: %.0f values\n", indent[2], handle.range)
            local lclip_src = make_rangestr(old, limit_src, true)
            local rclip_src = make_rangestr(limit_src + 1, old + handle.range, true)
            printf("%sNeed submap for src%s.\n", indent[3], lclip_src)
            printf("%sNeed submap for src%s.\n", indent[3], rclip_src)
        else
            printf("%sNo need remap handle.range: %.0f\n", indent[2], handle.range)
        end
        -- Implied else, no need to change handle.range!
    end
    -- Implied else is we keep the previous mappings.
end

---@param seeds SeedPair
---@param database tbl<str, Map>
local function find_location(seeds, database)
    local seedrange = make_rangestr(seeds.start, seeds.range)
    local map = database["seed"] ---@type Map we start by querying key "seed".

    -- We will definitely have multiple mappings that are not contiguous.
    local mapped = {} ---@type SeedPair[][] is 2D so we can have multimaps.
    local ii, jj = 1, 1 -- jj is the index of which multimap we're poking at
    mapped[ii] = {
        {start = seeds.start, range = seeds.range}
    }
    local handle = mapped[ii][jj]
    printf("Seeds: %s\n", seedrange)

    -- Indent levels so we don't construct the strings over and over.
    local indent = {
        make_indent(1),
        make_indent(2),
        make_indent(3),
        make_indent(4)
    }
    
    while map do
        local rangestr = make_rangestr(handle.start, handle.range)
        printf("%smapped[%i][%i]: %s (%i elements)\n", indent[1], ii, jj, rangestr, handle.range)
        printf("%s%s-to-%s map:\n", indent[1], map.label.src, map.label.dst)
        ii = ii + 1
        mapped[ii] = {}
        mapped[ii][jj] = copy_table(handle) -- start of with previous values
        handle = mapped[ii][jj]
        for _, data in ipairs(map.data) do
            update_thismap(indent, handle, data) 
        end
        printf("\n")
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
