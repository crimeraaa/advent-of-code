require "mirage"

---comment
---@param histories int[][]
local function extrapolate_history(histories)
    local sum = 0
    -- Need go backwards from bottom to top history
    for ii, history in array.ipairs.reverse(histories) do
        -- Add last sequence values together to get this list's total history
        sum = sum + history[#history]
        printf("list@%i: %s, %i\n", ii, table.concat(history, ", "), sum)
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
