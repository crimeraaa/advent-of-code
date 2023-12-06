require("helpers")

-- What games would be possible if this were the contents of the bag?
BAG = {RED = 12, GREEN = 13, BLUE = 14}

-- Get the game number if it fits the criteria of the bag, else 0.
---@param line string
function tokenize_game(line)
    -- Pass explicit capture group to string.match to extract that
    -- local number = tonumber(line:match("Game ([%d]+):"))
    
    -- 3rd arg onwards are any capture groups specified
    local start, stop, gamenumber = line:find("Game ([%d]+):")
    gamenumber = tonumber(gamenumber) -- was returned as a string

    -- Remainder of string after game# + remove leading/trailing whitespace
    local sets = line:sub(stop + 1):trim()
    
    -- Don't capture leading/trailing whitepaces!
    -- Semicolon separated sets
    for set in sets:gmatch("%s?([^;]+)%s?") do 
        local results = {red = 0, green = 0, blue = 0}
        -- Comma spearated elements
        for cubes in set:gmatch("%s?([^,]+)%s?") do 
            -- match char-only seq and given our inputs it can only be a color.
            ---@type "red"|"green"|"blue"
            local color = cubes:match("[^%a]-(%a+)") 
            -- match digit-only seq.
            local count = cubes:match("([%d]+)[^%d]-") 
            -- Spelling of colors are the same so adjust for case
            if tonumber(count) > BAG[string.upper(color)] then
                return 0 -- still need to update `sum` in main
            end
        end
    end
    return gamenumber
end

---@param argc integer
---@param argv string[]
function main(argc, argv)
    local file = assert(io.open(argv[1] or "../part1.txt", "r"))
    local sum = 0 -- sum of game numbers that fit criteria of bag
    for line in file:lines("*l") do
        sum = sum + tokenize_game(line) -- update w/ current game no. or 0
    end
    printf("Sum: %i\n", sum)
    file:close()
    return 0
end

return main(#arg, arg)
