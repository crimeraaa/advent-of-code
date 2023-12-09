require("helpers")

PEEK_LEFT = -1
PEEK_RIGHT = 1

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
---@param closure fun(row: integer, col: integer, char: string) Update external values.
local function loop_matrix(matrix, closure)
    for row, line in ipairs(matrix) do -- vertical indexes
        for col, char in ipairs(line) do -- horizontal indexes
            closure(row, col, char)
        end
        printf("\n")
    end
end

---@param gears Symbol[]
---@param closure fun(lineno: integer, column: integer) Update external values.
local function loop_gearsymbols(gears, closure)
    for _, gear in ipairs(gears) do
        printf("line:%i:%i = '%s': ", gear.lineno, gear.column, gear.value)
        for y = -1, 1 do -- For offset reasons we need to start at -1
            local lineno = y + gear.lineno -- offset lineno index into matrix
            for x = -1, 1 do 
                local column = x + gear.column -- offset col index into line
                closure(lineno, column)
            end
        end
        printf("\n")
    end
end

-- Look at cells to the left and right of our part number.
---@param line string[] An element indexed from `matrix` (`string[][]`)
---@param column integer Index into `line` as if that were a char array.
---@param direction -1|1 Pass `PEEK_LEFT` or `PEEK_RIGHT`.
---@param closure fun(digit: integer, offset: integer) Update extarnal values.
local function loop_peekoffset(line, column, direction, closure)
    local digit = 1
    local offset = direction -- start off -1 or 1 so we offset already
    while (true) do
        digit = tonumber(line[column + offset])
        if not digit then
            break
        end
        closure(digit, offset)
        offset = offset + direction -- works for both + and -
    end
end

---@param argc integer
---@param argv string[]
local function main(argc, argv)
    -- argv[0] isn't counted in the `#` operator count
    if (argc ~= 0) and (argc ~= 1) then
        printf("Got %i arguments, please only pass exactly 0 or 1.\n", argc)
        printf("Usage: lua %s [filepath]\n", argv[0])
        return 1
    end
    local matrix = readfile((argc == 1 and argv[1]) or "./part1.txt")

    -- Gear symbol characters, line numbers and column numbers
    local gears = {} ---@type Symbol[]
    
    printf("\n<SCHEMATIC>:\n")
    loop_matrix(matrix, function(row, col, char) -- Create a closure!
        printf("%s ", char)
        local is_digit = tonumber(char) -- convert so we can do math
        if (not is_digit) and (char ~= ".") then
            ---@type Symbol
            local t = {lineno = row, column = col, value = char}
            -- insert in order of [L]eft to [R]ight, going [U]p to [D]own.
            table.insert(gears, t)
        end
        -- Newline is printed right outside this loop body's scope
    end)
    printf("\n")
    
    -- Walk back through cells adjacent to the current cell (matrix)
    printf("Printing <SYMBOLS> and adjacent <PARTNUMBERS>:\n")

    -- If we visited a part number already, don't reconstruct it.
    local visited = new_2Dboolarray(#matrix, #matrix[1])
    local sum = 0
    loop_gearsymbols(gears, function(lineno, column) -- create a closure!
        local part_number = tonumber(matrix[lineno][column])
        -- Current cell (our "center") isn't even a number? Don't bother!
        if (not part_number) or visited[lineno][column] then
            return
        end
        local place_value = 10 -- part_number is the 1's place already

        -- Update part number going left requires correct place value.
        loop_peekoffset(matrix[lineno], column, PEEK_LEFT, function(digit, offset)
            visited[lineno][column + offset] = true
            part_number = part_number + (digit * place_value)
            place_value = place_value * 10
        end)

        -- Update part number going right, which is much simpler than left!
        loop_peekoffset(matrix[lineno], column, PEEK_RIGHT, function(digit, offset)
            visited[lineno][column + offset] = true
            part_number = (part_number * 10) + digit
        end)
        sum = sum + part_number
        printf("%i (@%i:%i), ", part_number, lineno, column)
    end)
    printf("\n<SUM>: %s\n", sum)
    return 0
end

return main(#arg, arg)
