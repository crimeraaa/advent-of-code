require("helpers")

---@param line str
local function make_seedtable(line)
    local seeds = {} ---@type int[]
    local i = 0
    for seed in line:gmatch("%d+") do
        i = i + 1
        seeds[i] = tonumber(seed)
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
    local seeds = {} ---@type int[]
    -- Map source name strings to destination, source and range tables.
    local maps = {} ---@type tbl<str, Map>

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
            -- found a whole new map
            category = labels.src
            maps[category] = make_mapcategory(labels.src, labels.dst)
            index = 0
        elseif values.dst and values.src and values.range then 
            -- found new data line for the current map
            index = index + 1 
            maps[category].data[index] = copy_table(values, copy_tonumber)
        end
    end
    return seeds, maps
end

-- Attempts to creating destination mapping from `source` within `map`.
---@param map Map
---@param source int
local function map_source(map, source)
    ---@type MapLabels
    local label = {src = map.label.src, dst = map.label.dst}
    local limit = {}
    -- If mapping doesn't exist this will remain the same as the source value.
    local mapped = source
    for _, data in pairs(map.data) do
        limit.src = data.src + data.range - 1 -- -1 excludes {src+range} itself
        limit.dst = data.dst + data.range - 1 -- src=97,range=2: 97...98, not 99
        print_mapdata(map.label, data, 1)
        -- print_mapping_ranges(label, data, limit)
        -- Determine if current value is in range
        if (source >= data.src) and (source <= limit.src) then
            mapped = source + (data.dst - data.src) -- difference = offset
            break -- No need to match anymore so get outta here!
        end
    end
    print_final_mapping(label, source, mapped)
    return mapped
end

-- Try to make connections for `seed` until we get a `location` key-value.
---@param seed int
---@param maps tbl<str, Map[]>
local function find_location(seed, maps)
    local source = seed
    local mapped = 0 -- need outside of loop so can poke at afterwards
    local category = "seed" -- used to query `maps`
    local map = maps[category] ---@type Map
    printf("<SEED> %.0f:\n", seed)
    while map do
        mapped = map_source(map, source)
        source = mapped -- need for the next comparison
        category = map.label.dst -- query next map
        map = maps[category]
    end
    printf("\t<LOCATION> %.0f\n", mapped)
    return mapped
end

---@param seeds int[]
---@param maps tbl<str, Map[]>
local function get_lowest_location(seeds, maps)
    -- Probably from `humidity-to-location` map.
    local lowest = nil -- Start `nil`, not 0 some less-than comparison can pass!
    for _, seed in ipairs(seeds) do
        local mapped = find_location(seed, maps)
        if (not lowest) or (mapped < lowest) then
            lowest = mapped
        end
    end
    return lowest
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or "../sample.txt")
    local seeds, maps = make_seedmap_table(lines)
    printf("<LOWEST LOCATION>: %.0f\n", get_lowest_location(seeds, maps))
    return 0
end

return main(#arg, arg)
