require("helpers")

---@param lines str[]
---@param parse_fn fun(line: str)
local function loop_lines(lines, parse_fn)
    for _, line in pairs(lines) do
        parse_fn(line)
    end
end

---@param maps Map[]
---@param loop_body fun(map: Map)
function loop_maps(maps, loop_body)
    for _, map in ipairs(maps) do
        --[[ local name = string.format("map[%i]", i)
        -- print_table(map, name) --? not a lot of control over output
        --? Finer control over output
        print_table(map.label, name .. ".label") 
        print_array(map.data, name .. ".data") ]]
        loop_body(map)
        -- printf("\n")
    end
end

local function make_lookup_table(maps)
    
    ---@type tbl<str, Map> Map source name strings to lookup tables.
    local findmap = {} 
    -- local ii, jj = 0, 0
    loop_maps(maps, function(map)
        local finalmap = {
            label = {src = map.label.src, dst = map.label.dst},
            data = {}
        }
        printf("%s-to-%s map:\n", map.label.src, map.label.dst)
        for _, data in ipairs(map.data) do
            -- Numbers are way too big to fit in %i
            printf("dst=%.0f, src=%.0f, range=%.0f\n", 
                    data.dst, 
                    data.src, 
                    data.range)
            -- print_mapdata(map.label, data)
            -- local range = data.src + data.range
            --! This is the bottleneck!
            --! Rather than create tables, determine lookup mathematically?
            -- Lua numeric `for` loops are inclusive of the condition
            -- for i = 0, data.range - 1 do
            --     -- Will not necessarily fill up index 0, 1, 10, etc. though!
            --     finalmap.data[data.src + i] = data.dst + i
            -- end
        end
        printf("\n")

        findmap[map.label.src] = finalmap
        -- print_mapped(map.label, mapped, count)
        -- ii = ii + 1
        -- print_array(mapped, "mapped[".. ii .."]")
    end)
    return findmap
end

---@param lines str[]
local function make_seedmap_table(lines)
    local input = {}
    input.seeds = {} ---@type int[]
    input.maps = {} ---@type Map[]

    local ii, jj = 0, 0 -- start off empty, incr. as needed

    -- Explicit capture groups are the only return values of string.match.
    -- otherwise, if no capture groups specified, it'll return the whole match.
    loop_lines(lines, function(line)
        -- Skip this line, it's not a map, but do get the information in here
        if line:find("seeds: [%d%s]+") then
            local index = 0
            for seed in line:gmatch("%d+") do
                index = index + 1
                input.seeds[index] = tonumber(seed)
            end
            return
        end
        -- Format: `{src-name}-to-{dst-name} map:`
        local src, dst = line:match("(%w+)%-to%-(%w+) map:") 
        -- Format: `{dst-start} {src-start} {range}`
        local x, y, z = line:match("(%d+) (%d+) (%d+)")
        -- Assume cannot have `src && dst && x && y && z`
        if src and dst then -- found a new map
            ii = ii + 1
            input.maps[ii] = {
                label = {src = src, dst = dst}, 
                data = {}
            }
            jj = 0
        elseif x and y and z then -- found new data line for the current map
            -- Lua pattern matching usually returns strings
            x, y, z = tonumber(x), tonumber(y), tonumber(z)
            jj = jj + 1 
            --! Watch the order! Remember dst comes before src in the input.
            input.maps[ii].data[jj] = {src = y, dst = x, range = z}
        end
    end)
    return input
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or "../input.txt")
    local input = make_seedmap_table(lines)
    -- if input then
    --     print_array(input.seeds, "input.seeds")
    --     -- print_table(input.maps, "input.maps")
    --     for i, v in ipairs(input.maps) do
    --         print_table(v, "input.maps[".. i .."]")
    --     end
    --     return 0
    -- end

    -- Map source name strings to lookup tables.
    local findmap = make_lookup_table(input.maps)
    local lowest -- Probably from humidity-to-location` map.
    for _, seed in ipairs(input.seeds) do
        local source = seed
        local mapped = 0 -- need outside of loop so can poke at afterwards
        local category = "seed"
        local map = findmap[category] ---@type LookupTable
        -- printf("<SEED> %i:\n", seed)
        while map do
            local _src, _dst = map.label.src, map.label.dst
            -- If doesn't exist just use source value as the mapping
            mapped = map.data[source] or source
            -- printf("\t%s %s => %s:  %i\n", _src, source, _dst, mapped)
            source = mapped -- need for the next comparison
            category = _dst
            map = findmap[category]
        end
        lowest = (not lowest or mapped < lowest) and mapped or lowest
    end
    printf("<LOWEST>: %i\n", lowest)
    return 0
end

return main(#arg, arg)
