require("helpers")

---@param line string
local function tokenize_game(line)
    -- 1-based index of `:` in subject string `line`
    local colon = line:find(":") 
    -- digit part of `"Game <number>:"`
    local id = tonumber(line:sub(0, colon - 1):match("[%d]+")) 
    -- Remainder of string after "Game <number>:", leading whitespace removed
    local data = line:sub(colon + 1):trim()
    -- Minimum #cubes of each color needed for this game to be possible
    local game = {red = 0, green = 0, blue = 0}
    printf("Game <%i>: \"%s\"\n", id, data)
    -- Semicolon separated sets
    for i, set in ipairs(data:split(";")) do
        printf("\tSet %i: \"%s\"\n", i, set)
        -- Comma separated elements in each set
        for j, cube in ipairs(set:split(",")) do
            -- Regex-heavy but less so than before
            local count = tonumber(cube:match("%d+")) -- digit-only sequence
            local color = cube:match("%a+") -- (C `char` type)-only sequence
            printf("\t\tCube %i: count = %i, color = %s\n", j, count, color)
            -- Update game minimum #cube requirement for this game to be possible
            if count > game[color] then
                game[color] = count
            end
        end
    end
    print("\tMinimum Required Cubes for this game to be possible:")
    for k, v in pairs(game) do
        print("\t", k, v)
    end
    printf("\n")
    -- Power of set of cubes
    return game.red * game.green* game.blue
end

-- I can't let go of C and C can't let go of me
---@param argc integer
---@param argv string[]
local function main(argc, argv)
    -- TODO Replace fallback with "../part2.txt"
    local file = assert(io.open(argv[1] or "../part1.txt", "r"))
    local sum = 0 -- sum of all games's cube power
    for line in file:lines("*l") do
        sum = sum + tokenize_game(line)
    end
    printf("Total Cube Power: %i\n", sum)
    file:close()
    return 0
end

return main(#arg, arg)
