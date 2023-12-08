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
    local matrix = readfile((argc == 1 and argv[1]) or "../part1.txt")

    -- Keep track of symbol characters, line numbers and column numbers
    local symbols = {}
    printf("\nPrinting out <schematic>:\n")
    for row = 1, #matrix, 1 do
        local line = matrix[row]
        for col = 1, #line, 1 do
            local char = line[col]
            printf("%s ", char)
            -- Any non-dot and non-digit is considered a symbol in the problem
            if char:find("[^.%d]") then
                -- columns = horizontal, rows = vertical
                -- Insert symbols in order of left-right, top-down
                table.insert(symbols, {x = col, y = row, char = char})
            end
        end
        printf("\n")
    end
    printf("\n")
    
    for _, symbol in ipairs(symbols) do
        printf("line_%i:%i = '%s': Adjacent is ", symbol.y, symbol.x, symbol.char)
        -- Walk back to adjacent cells
        for y = symbol.y - 1, symbol.y + 1 do
            for x = symbol.x - 1, symbol.x + 1 do
                local digit = tonumber(matrix[y] and matrix[y][x])
                if digit then
                    printf("%i, ", digit)
                end
            end
        end
        printf("\n")
    end
    return 0
end

return main(#arg, arg)
