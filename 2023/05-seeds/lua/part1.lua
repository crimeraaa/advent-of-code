require("helpers")

---@param lines str[]
---@param parse_fn fun(line: str)
local function loop_lines(lines, parse_fn)
    for i, line in pairs(lines) do
        parse_fn(line)
    end
end

---@param maps Map[]
---@param loop_body fun(map: Map)
function loop_maps(maps, loop_body)
    for i, map in ipairs(maps) do
        local name = string.format("map[%i]", i)
        -- print_table(map, name, 0) --? not a lot of control over output
        --? Finer control over output
        print_table(map.label, name .. ".label") 
        print_array(map.data, name .. ".data")
        loop_body(map)
        printf("\n")
    end
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or "../sample.txt")
    local maps = {} ---@type Map[]
    local ii, jj = 0, 0 -- start off empty
    -- Explicit capture groups are the only return values of string.match.
    -- otherwise, if no capture groups specified, it'll return the whole match.
    loop_lines(lines, function(line)
        -- Skip this line, it's not a map
        if line:find("seeds: [%d%s]+") then
            printf("seeds: ")
            for seed in line:gmatch("%d+") do
                printf("%s, ", seed)
            end
            printf("\n")
            return
        end
        local src, dst = line:match("(%w+)%-to%-(%w+) map:") -- label format: `"{src}-to-{dst} map:"`
        local x, y, z = line:match("(%d+) (%d+) (%d+)") -- data format: `"{int} {int} {int}"`
        if src and dst then -- found a new map
            ii = ii + 1 -- only incr. when new map found
            maps[ii] = {label = {src = src, dst = dst}, data = {}}
            jj = 0 -- reset writing index for new data table
        elseif x and y and z then -- found new data line for the current map
            jj = jj + 1 
            maps[ii].data[jj] = {
                src = tonumber(y), 
                dst = tonumber(x), 
                range = tonumber(z)
            }
        end
    end)
    
    loop_maps(maps, function(map)
        local count = 0
        ---@type int[] Maps `src` values to `dst`.
        local mapped = {} 
        for _, data in ipairs(map.data) do
            -- print_mappings(map.label, data)
            local range = data.src + data.range
            if range > count then
                count = range
            end
            -- Lua numeric `for` loops are inclusive of the condition
            for i = 0, data.range - 1 do
                -- Will not necessarily fill up index 1 though!
                mapped[data.src + i] = data.dst + i
            end
        end
        
        for i = 0, count - 1 do
            -- printf("%i=>%i\n", i, mapped[i] or i)
            -- unmapped destination values are the same as source
            if not mapped[i] then
                mapped[i] = i
            end
        end
        print_array(mapped, "mapped")
        printf("Total Elements: %i\n", count)
    end)
    
    return 0
end

return main(#arg, arg)
