require("helpers")


---@class Partnumber
---@field index integer[] Indexes into the original line of constructed number.
---@field value integer[] Constructed part number itself.

local function dump_numbers(row, line)
    printf("%-2i : %s : ", row, line)
    local is_partnumber = false
    ---@type Partnumber[]
    local pnums = {}

    local column = 0 -- Current line column/char being read
    local pcount = 1 -- Current table in `pnums` being written
    for char in line:gmatch(".") do
        column = column + 1 -- sort of a hack but prolly works
        if (tonumber(char)) then
            if not is_partnumber then
                is_partnumber = true
            end
            if not pnums[pcount] then
                pnums[pcount] = {index = {}, value = {}}
            end
            -- Construct our partnumber from left to right
            table.insert(pnums[pcount].index, column)
            table.insert(pnums[pcount].value, tonumber(char))
        else
            if is_partnumber then
                pcount = pcount + 1
                is_partnumber = false
            end
        end
    end
    for _, partnumber in ipairs(pnums) do
        local value = tonumber(table.concat(partnumber.value, ""))
        local index = tonumber(table.concat(partnumber.index, ""))
        printf("%i (@ %i), ", value, index)
    end
    printf("\n")
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
    local schematic = readfile((argc == 1 and argv[1]) or "../part1.txt")
    for row, line in ipairs(schematic) do
        dump_numbers(row, line)
    end
    return 0
end

return main(#arg, arg)
