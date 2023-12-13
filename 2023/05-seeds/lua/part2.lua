require("helpers")

---@param line str
local function make_seedtable(line)
    local seeds = {} ---@type SeedPair[]
    local i = 0
    -- Match a pair of numbers, remember gmatch returns explicit capture pairs!
    for start, range in line:gmatch("(%d+) (%d+)") do
        i = i + 1
        ---@type SeedPair
        seeds[i] = {start = tonumber(start), range = tonumber(range)} 
    end
    return seeds
end

-- Create a new `Map` with the given labels and an empty `data` table.
local function make_mapcategory(matched_src, matched_dst)
    ---@type Map
    return {
        label = {src = matched_src, dst = matched_dst}, 
        data = {}
    }
end

-- Creates a copy of table `data` where all values are converted to numbers.
---@param source tbl
---@param copy_fn? fun(v: any):any Custom function to convert values if needed.
local function copy_table(source, copy_fn)
    local target = {}
    for k, v in pairs(source) do
        -- Invoke `copy_fn` only if exists
        target[k] = (copy_fn and copy_fn(v)) or v
    end
    return target
end

-- Define here so function not created everytime it's needed in the loop.
local function copy_tonumber(val)
    return tonumber(val)
end

-- Parse our file's stored lines and divide them into our seeds and map tables.
---@param lines str[]
local function make_seedmap_table(lines)
    local seeds = {} ---@type SeedPair[]
    -- Query source category strings to get a source-destination mapping.
    local database = {} ---@type tbl<str, Map>

    local category = "" -- exist outside callee scope so can index into `maps`.
    local index = 0 -- number of `data` elements for current map.

    local labels = {} -- matched category names in the line
    local values = {} -- matched mapping values

    -- Explicit capture groups are the only return values of string.match.
    -- otherwise, if no capture groups specified, it'll return the whole match.
    for _, line in pairs(lines) do
        -- Format: `{src-name}-to-{dst-name} map:`
        ---@type string?, string?
        labels.src, labels.dst = line:match("(%w+)%-to%-(%w+) map:")

        -- Format: `{dst-start} {src-start} {range}`
        --! Watch the order! Remember dst comes before src in the input.
        ---@type string?, string?, string?
        values.dst, values.src, values.range = line:match("(%d+) (%d+) (%d+)")
        if line:find("seeds: [%d%s]+") then
            seeds = make_seedtable(line)
        elseif labels.src and labels.dst then 
            category = labels.src -- found a whole new map category
            database[category] = make_mapcategory(labels.src, labels.dst)
            index = 0
        elseif values.dst and values.src and values.range then 
            -- found new data line for the current map
            index = index + 1 
            database[category].data[index] = copy_table(values, copy_tonumber)
        end
    end
    return seeds, database
end

-- Attempts to creating destination mapping from `source` within `map`.
---@param map Map
---@param source int
---@param tabs? int
local function map_source(map, source, tabs)
    local limit = {}
    local label = {src = map.label.src, dst = map.label.dst} ---@type MapLabels
    -- If mapping doesn't exist this will remain the same as the source value.
    local mapped = source
    for i, data in pairs(map.data) do
        limit.src = data.src + data.range - 1 -- -1 excludes {src+range} itself
        limit.dst = data.dst + data.range - 1 -- src=97,range=2: 97...98, not 99
        -- print_mapping_ranges(label, data, limit, tabs)
        -- Determine if current value is in range
        if (source >= data.src) and (source <= limit.src) then
            mapped = source + (data.dst - data.src) -- difference = offset
            -- break -- No need to match anymore so get outta here!
        end
    end
    return mapped
end

-- Try to make connections for `seed` until we get a `location` key-value.
---@param seed int
---@param database tbl<str, Map>
---@param tabs? int
local function find_location(seed, database, tabs)
    local indent = make_indent(tabs)
    local source = seed
    local mapped = 0 -- need outside of loop so can poke at afterwards
    local category = "seed" -- used to query `maps`, first query is "seeds"
    local map = database[category] ---@type Map
    -- Last category should be the humidity-to-location map
    while map do
        mapped = map_source(map, source, tabs + 1)
        source = mapped -- need for the next comparison
        category = map.label.dst -- query next map
        map = database[category]
    end
    return mapped
end

-- Tries to evenly divide your table into 4 columns.
local function split_columns(i)
    if (i ~= 0) and (math.fmod(i + 1, 4) == 0) then
        printf("\n")
        return true
    end
    return false
end

local function print_lookup_table(lookup_t, count, tabs)
    local indent = make_indent(tabs)
    local last_split = false
    -- Need index 0 also, as a result we need to sub from `count` as well.
    for i = 0, count - 1 do
        -- If index doesn't exist, use index itself as the mapping
        printf("%s[%.0f] = %.0f,", indent, i, lookup_t[i] or i)
        last_split = split_columns(i)
    end
    if not last_split then
        printf("\n")
    end
end

---@param seed int
---@param database tbl<str, Map>
---@param tabs? int
local function dump_all_mappings(seed, database, tabs)
    tabs = tabs or 0
    local indent = make_indent(tabs)
    local source = seed
    local mapped = 0
    local category = "seed"
    local map = database[category] ---@type Map
    local lookup_t = {} ---@type num[]
    -- printf("<SEED> %.0f:\n", source)
    while map do
        local target = map.label.dst
        local count = nil
        -- Populate lookup table
        for i, data in ipairs(map.data) do
            local limit = data.src + data.range
            if (not count) or (limit > count) then
                count = limit
            end
            for ii = 0, data.range - 1 do
                lookup_t[data.src + ii] = data.dst + ii
            end
        end
        -- Dump lookup table
        printf("%s-- %s-to-%s map: %.0f elements\n", indent, category, target, count)
        printf("%s%s = {\n", indent, category)
        print_lookup_table(lookup_t, count, tabs + 1)
        printf("%s},\n", indent)
        
        mapped = map_source(map, source, tabs + 1)
        source = mapped
        category = target
        map = database[category]
    end
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or "../sample.txt")
    local seeds, database = make_seedmap_table(lines)
    -- local lowest = {}
    printf("-- <DUMPING MAP DATABASE>\n")
    print_table(database, "database")

    -- printf("\n-- <DUMPING SEED DATABASE>\n")
    -- print_table(seeds, "seeds")
    
    printf("-- <DUMPING SEED MAPS>\n")
    printf("local seeds = {}\n\n")
    
    for i, seed in ipairs(seeds) do
        local name = string.format("seeds[%i]", i)
        local tabs = 0
        local indent = make_indent(tabs)
        printf("%s%s = {start = %.0f, range = %.0f}\n", indent, name, seed.start, seed.range)
        name = name .. ".mappings"
        -- sub 1 so that given start=79 && range = 14: `79...92`, not `79...93`
        printf("%s%s = {\n", indent, name)
        dump_all_mappings(seed.start, database, tabs + 1)
        printf("}\n")
        -- for ii = 0, seed.range - 1 do
        --     local offset = seed.start + ii -- use iterator as offset
        --     --! Bottleneck! Will take forever with large ranges. 
        --     local mapped = find_location(offset, database, tabs + 1)
        --     if (not lowest.location) or mapped < lowest.location then
        --         lowest.location = mapped
        --         lowest.seed = offset
        --     end
        -- end
        printf("\n")
        -- printf("lowest: seed = %.0f, location = %.0f\n", lowest.seed, lowest.location)
    end
    return 0
end

return main(#arg, arg)
