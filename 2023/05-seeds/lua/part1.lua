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
        -- printf("%s-to-%s map:\n", map.label.src, map.label.dst)
        -- for _, data in ipairs(map.data) do
        --     -- Numbers are way too big to fit in %i
        --     printf("dst=%.0f, src=%.0f, range=%.0f\n", 
        --             data.dst, 
        --             data.src, 
        --             data.range)
        -- end
        -- printf("\n")
        findmap[map.label.src] = map
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
    local lines = readfile((argc == 1 and argv[1]) or "../sample.txt")
    local input = make_seedmap_table(lines)

    -- Map source name strings to lookup tables.
    local findmap = make_lookup_table(input.maps)
    local lowest -- Probably from humidity-to-location` map.
    for _, seed in ipairs(input.seeds) do
        local source = seed
        local mapped = 0 -- need outside of loop so can poke at afterwards
        local category = "seed"
        local map = findmap[category] ---@type Map
        printf("<SEED> %.0f:\n", seed)
        while map do
            local label_src, label_dst = map.label.src, map.label.dst
            -- If doesn't exist just use source value as the mapping
            -- TODO: Determine mapping mathematically
            for _, data in pairs(map.data) do
                local limit_src = data.src + data.range - 1
                local limit_dst = data.dst + data.range - 1
                printf(
                    "\t%s (%.0f...%.0f) => %s (%.0f...%.0f)\n", 
                    label_src, data.src, limit_src,
                    label_dst, data.dst, limit_dst
                )
                -- Determine if current value is in range
                if (source >= data.src) and (source <= limit_src) then
                    mapped = source + (data.dst - data.src)
                    break -- No need to match anymore so get outta here!
                else
                    mapped = source
                end
            end
            printf(
                "\t<MAPPED> %s %s => %s: %.0f\n", 
                label_src, 
                source, 
                label_dst, 
                mapped
            )
            printf("\n")
            source = mapped -- need for the next comparison
            category = label_dst
            map = findmap[category]
        end
        lowest = (not lowest or mapped < lowest) and mapped or lowest
    end
    printf("<LOWEST>: %.0f\n", lowest)
    return 0
end

return main(#arg, arg)
