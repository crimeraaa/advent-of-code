-- What games would be possible if this were the contents of the bag?
BAG = {RED = 12, GREEN = 13, BLUE = 15}

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

---@param line string
function tokenize_game(line)
    -- Pass explicit capture group to string.match to extract that
    -- local number = tonumber(line:match("Game ([%d]+):"))
    
    -- 3rd arg onwards are any capture groups specified
    local start, stop, number = line:find("Game ([%d]+):")
    number = tonumber(number)
    printf("Game Number: %i (start: %i, stop: %i)\n", number, start, stop)

    -- Remainder of string after game# + remove leading/trailing whitespace
    local sets = line:sub(stop + 1):trim()

    -- Separate sets
    for set in sets:gmatch("([^;]+)") do
        set = set:trim() -- Remove leading whitespace
        printf("Set:\t\"%s\"\n", set)
        -- Separate individual elements of a given set
        for cubes in set:gmatch("[^,]+") do
            cubes = cubes:trim()
            printf("\tColor: \"%s\"\n", cubes)
        end
    end
end

---@param argc integer
---@param argv string[]
function main(argc, argv)
    local file = assert(io.open(argv[1] or "../part1.txt", "r"))
    local number = 1
    for line in file:lines("*l") do
        printf("Line %i:\n\"%s\"\n", number, line)
        tokenize_game(line)
        printf("\n")
        number = number + 1
    end
    file:close()
    return 0
end

return main(#arg, arg)
