require("helpers")

-- TODO: Get part numbers directly adjacent/diagonal to non '.' symbols
---@param argc integer
---@param argv string[]
local function main(argc, argv)
    -- argv[0] isn't counted in the `#` operator count
    if (argc ~= 0) and (argc ~= 1) then
        printf("Too many arguments! Got %i, need 0 or 1.\n", argc)
        printf("Usage: lua %s [filepath]\n", argv[0])
        return 1
    end
    local schematic = readfile((argc == 1 and argv[1]) or "../part1.txt")
    local matrix = {} ---@type string[][]
    -- Populate matrix as if it were a 2D char array
    for row, line in ipairs(schematic) do
        matrix[row] = {} -- new line
        for char in line:gmatch(".") do
            table.insert(matrix[row], char)
        end
    end

    printf("\nPrinting out <matrix>:\n")
    for row = 1, #matrix, 1 do
        local line = matrix[row]
        local has_symbol = false
        local x, y = 0, 0
        for col = 1, #line, 1 do
            local char = line[col]
            printf("%s ", char)
            -- Any non-dot and non-digit is considered a symbol in the problem
            if char:find("[^.%d]") then
                has_symbol = true
                x, y = col, row -- columns = horizontal, rows = vertical
            end
        end
        if has_symbol then
            printf(" <--LINE WITH SYMBOL {%i, %i}", x, y)
        end
        -- TODO: Walk back to all 8 adjacent cells
        printf("\n")
    end
    return 0
end

return main(#arg, arg)
