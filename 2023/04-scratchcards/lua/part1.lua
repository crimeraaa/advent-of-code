require("helpers")

---@param winning str
---@param playing str
local function tally_winnings(winning, playing)
    local score = 0
    printf("\nMatch : ")
    -- Need word boundary, otherwise we'll match single digits everywhere!
    -- Frontier patterns kinda work: https://stackoverflow.com/a/32854326
    -- "%f[%d]%d*"
    -- EDIT: Nevermind
    for jackpot in winning:gmatch("%d+") do
        -- Probably inefficient but this gets the job done
        -- formerly: no loop, just `if playing:match(jackpot)` which was wrong!
        for number in playing:gmatch("%d+") do
            if number == jackpot then
                printf("\"%2s\" ", jackpot)
                if score == 0 then -- Terse: (score > 0) and (score * 2) or 1
                    score = 1
                else
                    score = score * 2
                end
            end
        end
    end
    printf("\n")
    return score
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    -- argv[0] not counted by the `#` operator
    if (argc ~= 0) and (argc ~= 1) then
        printf("Usage: lua %s [puzzle_input].txt", argv[0])
        return 1
    end
    
    local lines = get_cardlist((argc == 1 and argv[1]) or "sample.txt")
    local points = 0
    
    -- Left side is winning numbers, right side is your numbers.
    -- first match is worth 1 point, every match afterwards doubles that value.
    for _, line in ipairs(lines) do
        local colon = line:find(":")
        local barline = line:find("|")
        local winning = line:sub(colon + 1, barline - 1)
        local playing = line:sub(barline + 1)
        local score = tally_winnings(winning, playing)
        printf("%s - score: %i\n", line, score)
        points = points + score
    end
    -- 27742 not right, too high
    -- EDIT: Was 26346
    printf("\n<POINTS>: %i\n", points)
    return 0
end

return main(#arg, arg)
