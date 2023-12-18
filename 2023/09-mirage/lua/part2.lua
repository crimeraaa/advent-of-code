require "mirage"

---comment
---@param histories int[][]
local function extrapolate_history(histories)
    local sum = 0
    -- Need go backwards from bottom to top history
    local iter = array.ipairs.reverse
    -- local iter = ipairs
    for ii, history in iter(histories) do
        -- Fill in new first values of the previous sequence
        sum = history[1] - sum
        printf("list@%i: %i, %s\n", ii, sum, table.concat(history, ", "))
    end
    return sum
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    local sum = 0
    for _, line in ipairs(lines) do
        local history = table.copy(line:split("%s"), tonumber)
        local histories = get_history(history)
        sum = sum + extrapolate_history(histories)
        printf("\n")
    end
    print(sum)
    return 0
end

return main(#arg, arg)
