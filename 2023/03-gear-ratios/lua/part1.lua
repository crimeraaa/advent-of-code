require("helpers")

---@class Symbol
---@field lineno integer 
---@field column integer Index into the line.
---@field value string|integer Symbol character or constructed part number.

-- Think in terms of an array of horizontal lines.
---@param width integer
---@param height integer
local function new_2Dboolarray(width, height)
    ---@type boolean[][]
    local bool_array = {}
    for y = 1, height do -- vertical indexes
        bool_array[y] = {}
        for x = 1, width do -- horizontal indexes
            bool_array[y][x] = false
        end
    end
    return bool_array
end

---@param matrix string[][]
---@param loop_body fun(row: integer, col: integer, char: string)
local function loop_matrix(matrix, loop_body)
    for row, line in ipairs(matrix) do -- vertical indexes
        for col, char in ipairs(line) do -- horizontal indexes
            loop_body(row, col, char)
        end
        printf("\n")
    end
end

---@param gearsymbols Symbol[]
---@param loop_body fun(lineno: integer, column: integer)
local function loop_gearsymbols(gearsymbols, loop_body)
    for _, gear in ipairs(gearsymbols) do
        printf("line:%i:%i = '%s': Got ", gear.lineno, gear.column, gear.value)
        for y = -1, 1 do -- For offset reasons we need to start at -1
            local lineno = y + gear.lineno -- offset lineno index into matrix
            for x = -1, 1 do 
                local column = x + gear.column -- offset col index into line
                loop_body(lineno, column)
            end
        end
        printf("\n")
    end
end

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
    local matrix = readfile((argc == 1 and argv[1]) or "./part1.txt")

    -- Symbol characters, line numbers and column numbers
    local gearsymbols = {} ---@type Symbol[]
    -- Constructed part numbers, line numbers and line index
    local partnumbers = {} ---@type Symbol[]

    local is_writing_partnumber = false
    local partnumber = 0
    
    printf("\nPrinting out <schematic>, constructing <symbols> and <numbers>:\n")
    loop_matrix(matrix, function(row, col, char) -- Create a closure!
        printf("%s ", char)
        local digit = tonumber(char) -- convert so we can do math
        if (not digit) and (char ~= ".") then
            ---@type Symbol
            local t = {lineno = row, column = col, value = char}
            -- insert in order of [L]eft to [R]ight, going [U]p to [D]own.
            table.insert(gearsymbols, t)
        elseif digit then
            -- Not writing a part number but found the first digit
            if not is_writing_partnumber then
                is_writing_partnumber = true
            end
            -- Prev is number place greater than current
            partnumber = (partnumber * 10) + digit
        else
            -- Writing a part number but found first non-digit, so end here
            if is_writing_partnumber then
                is_writing_partnumber = false
                ---@type Symbol
                local t = {lineno = row, column = col, value = partnumber}
                table.insert(partnumbers, t)
                partnumber = 0 -- Need else we mess up the next partnumber!
            end
        end
        -- Newline is printed right outside this loop body's scope
    end)
    printf("\n")

    ---@param line string[]
    ---@param column integer Index into `line` as if that were a char array.
    ---@param direction -1|1 -1 is for left, 1 is for right.
    ---@param partnumber_constructor fun(digit: integer)
    local function loop_peek(line, column, direction, partnumber_constructor)
        local digit = tonumber(line[column])
        if not digit then
            return
        end
        local peekoffset = direction -- internal so we don't lose the original
        while (digit) do
            digit = tonumber(line[column - peekoffset])
            peekoffset = peekoffset + direction -- works for both + and -
        end
    end
    
    -- Walk back to adjacent cells
    printf("Printing <symbols>:\n")
    loop_gearsymbols(gearsymbols, function(lineno, column) -- create a closure!
        -- Walk back through the main matrix
        local digit = tonumber(matrix[lineno][column])
        if not digit then
            return
        end
        -- Construct part number to the left
        local offset = 1
        local part_number = 0
        local place_value = 1
        while (digit) do
            -- Constructing a number backwards requires a bit of work!
            part_number = part_number + (digit * place_value)
            digit = tonumber(matrix[lineno][column - offset])
            -- Digit ot left of us is more significant
            offset = offset + 1
            place_value = place_value * 10
        end

        -- Construct part number to the right
        offset = 1
        while (true) do
            digit = tonumber(matrix[lineno][column + offset])
            if not digit then
                break
            end
            part_number = (part_number * 10) + digit
            -- Digit to right of us is more significant
            offset = offset + 1
        end

        -- Combine
        printf("%i (line%i:%i), ", part_number, lineno, column)
    end)
    printf("\n")

    printf("Printing <numbers>:\n")
    for _, partnum in ipairs(partnumbers) do
        printf("line:%i:%i = %i\n", partnum.lineno, partnum.column, partnum.value)
    end
    printf("\n")
    return 0
end

return main(#arg, arg)
