-- What games would be possible if this were the contents of the bag?
BAG = {RED = 12, GREEN = 13, BLUE = 14}

---@param fmt string
---@param ... string|number
function printf(fmt, ...)
    io.stdout:write(fmt:format(...))
end

-- Remove leading and trailing whitespaces from the string.
---@param subject string
---@nodiscard
function string.trim(subject)
    return subject:gsub("^%s+", ""):gsub("%s+$", "")
end

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
    local game = {}
    
    -- Don't capture leading/trailing whitepaces!
    -- Semicolon separated sets
    for set in sets:gmatch("%s?([^;]+)%s?") do 
        local results = {red = 0, green = 0, blue = 0}
        -- Comma spearated elements
        for cubes in set:gmatch("%s?([^,]+)%s?") do 
            -- match a char-only sequence
            local color = cubes:match("[^%a]-(%a+)") 
            -- match a digit-only squence
            local count = tonumber(cubes:match("([%d]+)[^%d]-")) 
            results[color] = results[color] + count
        end
        table.insert(game, results)
    end

    for i, set in ipairs(game) do
        -- printf("Set %i: %i red, %i green, %i blue\n", 
        --         i, 
        --         set.red, 
        --         set.green, 
        --         set.blue)
        for color, count in pairs(set) do
            if (count > BAG[string.upper(color)]) then
                -- printf("Game %i is not possible!\n", gamenumber)
                -- printf("Set %i has %i %s cubes.\n", i, count, color)
                return 0
            end
        end
    end
    return gamenumber
end

---@param argc integer
---@param argv string[]
function main(argc, argv)
    local file = assert(io.open(argv[1] or "../part1.txt", "r"))
    local sum = 0
    local number = 1
    for line in file:lines("*l") do
        -- printf("Line %i:\t\"%s\"\n", number, line)
        sum = sum + tokenize_game(line)
        -- printf("\n")
        number = number + 1
    end
    printf("Sum: %i\n", sum)
    file:close()
    return 0
end

return main(#arg, arg)
