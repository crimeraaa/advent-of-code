require("helpers")

-- Think in terms of an array of horizontal lines.
---@param width int
---@param height int
local function new_2Dboolarray(width, height)
    ---@type bool[][]
    local bool_array = {}
    for y = 1, height do -- vertical indexes
        bool_array[y] = {}
        for x = 1, width do -- horizontal indexes
            bool_array[y][x] = false
        end
    end
    return bool_array
end

---@param matrix str[][]
---@param update_gears fun(row: int, col: int, char: str) Update external values.
local function loop_matrix(matrix, update_gears)
    for row, line in ipairs(matrix) do -- vertical indexes
        for col, char in ipairs(line) do -- horizontal indexes
            update_gears(row, col, char)
        end
        printf("\n")
    end
end

---@param gears Symbol[]
---@param create_partnumber fun(lineno: int, column: int):int Update external values.
local function loop_gearsymbols(gears, create_partnumber)
    local sum = 0
    for _, gear in ipairs(gears) do
        printf("line:%i:%i = '%s': ", gear.lineno, gear.column, gear.value)
        local partnum_count = 0
        -- Start w/ 1 so first partnum mul'd returns itself instead of 0
        local gear_ratio = 1 

        -- For offset reasons we need to start at -1
        for y = -1, 1 do 
            local lineno = y + gear.lineno -- offset lineno index into matrix
            for x = -1, 1 do 
                local column = x + gear.column -- offset col index into line
                local partnum_value = create_partnumber(lineno, column)
                if partnum_value > 0 then
                    partnum_count = partnum_count + 1
                    -- sum = sum + partnum_value
                    gear_ratio = gear_ratio * partnum_value
                end
            end
        end

        printf(": %i adjacent part numbers.\n", partnum_count)
        if (partnum_count == 2) then
            sum = sum + gear_ratio
        end
    end
    return sum
end

-- Look at cells to the left and right of our part number.
-- This is meant to be used inside of the closure of `loop_gearsymbols`.
---@param matrix str[][]
---@param lineno int
---@param column int Index into `line` as if that were a char array.
---@param direction -1|1 Pass `PEEK_LEFT` or `PEEK_RIGHT`.
---@param peek_adjacent fun(digit: int, offset: int) Create part number based on adjacent cells.
local function loop_peekoffset(matrix, lineno, column, direction, peek_adjacent)
    local digit = 1
    local offset = direction -- start off -1 or 1 so we offset already
    while (true) do
        digit = tonumber(matrix[lineno][column + offset])
        if not digit then
            break
        end
        peek_adjacent(digit, offset)
        offset = offset + direction -- works for both + and -
    end
end

-- * Part 2: A gear is any '*' character with 2 adjacent part numbers.
-- * No more, no less!
---@param argc integer
---@param argv string[]
local function main(argc, argv)
    -- argv[0] isn't counted in the `#` operator count
    if (argc ~= 0) and (argc ~= 1) then
        printf("Got %i arguments, please only pass exactly 0 or 1.\n", argc)
        printf("Usage: lua %s [filepath]\n", argv[0])
        return 1
    end
    local matrix = readfile((argc == 1 and argv[1]) or "./sample.txt")

    -- Gear symbol characters, line numbers and column numbers
    local gears = {} ---@type Symbol[]
    
    printf("\n<SCHEMATIC>:\n")
    loop_matrix(matrix, function(row, col, char) -- Create a closure!
        printf("%s ", char)
        local is_digit = tonumber(char) -- convert so we can do math
        if (not is_digit) and (char ~= ".") and (char == "*") then
            ---@type Symbol
            local t = {lineno = row, column = col, value = char}
            -- insert in order of [L]eft to [R]ight, going [U]p to [D]own.
            table.insert(gears, t)
        end
        -- Newline is printed right outside this loop body's scope
    end)
    printf("\n")
    
    -- Walk back through cells adjacent to the current cell (matrix)
    printf("Printing <SYMBOLS> and adjacent <PARTNUM>:\n")

    -- If we visited a part number already, don't reconstruct it.
    local visited = new_2Dboolarray(#matrix, #matrix[1])
    local sum = loop_gearsymbols(gears, function(lineno, column) -- create a closure!
        local partnum = tonumber(matrix[lineno][column])
        -- Current cell (our "center") isn't even a number so don't bother
        if (not partnum) or visited[lineno][column] then
            return 0
        end

        local placeval = 10 -- part_number is the 1's place already
        -- Update part number going left requires correct place value.
        loop_peekoffset(matrix, lineno, column, PEEK_LEFT, function(digit, offset)
            visited[lineno][column + offset] = true
            partnum = partnum + (digit * placeval)
            placeval = placeval * 10
        end)

        -- Update part number going right, which is much simpler than left!
        loop_peekoffset(matrix, lineno, column, PEEK_RIGHT, function(digit, offset)
            visited[lineno][column + offset] = true
            partnum = (partnum * 10) + digit
        end)

        printf("%i (@%i:%i), ", partnum, lineno, column)
        return partnum
    end)
    printf("\n<SUM>: %s\n", sum)
    return 0
end

return main(#arg, arg)
