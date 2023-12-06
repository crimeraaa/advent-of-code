require("helpers")

ELF_BAG = {RED = 12, GREEN = 13, BLUE = 14}

---@param line string
local function tokenize_game(line)
    local colon = line:find(":") -- 1-based index of `:` in subject string `line`
    local id = tonumber(line:sub(0, colon - 1):match("[%d]+"))
    local data = line:sub(colon + 1):trim()
    printf("Game <%i>: \"%s\"\n", id, data)
    local sets = data:split(";")
    for i, set in ipairs(sets) do
        printf("\tSet %i: \"%s\"\n", i, set)
        local cubes = set:split(",")
        for j, cube in ipairs(cubes) do
            local count = tonumber(cube:match("%d+"))
            local color = cube:match("[^%a](%a+)")
            printf("\t\tCube %i: count = %i, color = %s\n", j, count, color)
            if count > ELF_BAG[color:upper()] then
                printf("\t\tNot valid!\n")
                return 0
            end
        end
    end
    printf("\n")
    return id
end

-- I can't let go of C and C can't let go of me
---@param argc integer
---@param argv string[]
local function main(argc, argv)
    -- TODO Replace fallback with "../part2.txt"
    local fallback = "C:/Users/crimeraaa/repos/advent-of-code/2023/02-cube-conundrum/part1.txt"
    local file = assert(io.open(argv[1] or fallback, "r"))
    local sum = 0
    for line in file:lines("*l") do
        sum = sum + tokenize_game(line)
    end
    printf("Sum: %i\n", sum)
    file:close()
    return 0
end

return main(#arg, arg)
