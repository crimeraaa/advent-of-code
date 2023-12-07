require("helpers")

---@class Matrix
---@field line integer Line number.
---@field upper string
---@field middle string
---@field lower string

local Matrix = {}

Matrix.mt = {
    -- If, say, `self.upper` is `nil`, use the protoype as fallback!
    __index = function(tbl, key)
        return Matrix.prototype[key]
    end
}

---@type Matrix
Matrix.prototype = {line = -1,
                    upper = "(nil)",
                    middle = "(nil)",
                    lower = "(nil)"}

function Matrix.new(line, upper, middle, lower)
    ---@type Matrix
    local obj = {line = line,
                 upper = upper,
                 middle = middle,
                 lower = lower}
    return setmetatable(obj, Matrix.mt)
end

-- TODO: Get part numbers directly adjacent/diagonal to non '.' symbols
---@param argc integer
---@param argv string[]
local function main(argc, argv)
    local file = assert(io.open(argv[1] or "../part1.txt", "r"))
    local schematic = {} ---@type string[]
    -- Store whole file as string array first so we can check previous lines
    for line in file:lines("*l") do
        table.insert(schematic, line)
    end
    
    ---@type Matrix[]
    local matrices = {}
    for i, line in ipairs(schematic) do
        printf("%-2i : \"%s\", ", i, line)
        local start = 0
        local stop = 0
        local match = "" 
        local write_once = false
        while true do
            start, stop, match = line:find("([^%d.])", start)
            if match == nil then
                break
            end
            write_once = true
            start = stop + 1
        end
        if write_once == true then
            local obj = Matrix.new(i, 
                                   schematic[i - 1], 
                                   line, 
                                   schematic[i + 1])
            table.insert(matrices, obj)
        end
        -- Prolly need to do matrix math or some 2D array magic
        printf("\n")
    end
    for i, matrix in ipairs(matrices) do
        print("Grid@ line "..matrix.line)
        print(matrix.upper)
        print(matrix.middle)
        print(matrix.lower, "\n")
    end
    file:close()
    return 0
end

return main(#arg, arg)
