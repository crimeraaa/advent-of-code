require("helpers")

---@class CardTracker
---@field cardnumber int
---@field instances int

---@param winning str
---@param playing str
local function get_matches(winning, playing)
    local score = 0
    printf("Match : ")
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
                score = score + 1
            end
        end
    end
    printf("\n")
    return score
end

---@param lines str[]
---@param loop_body fun(lineno: int, line: str)
local function tally_winnings(lines, loop_body)
    for lineno, line in ipairs(lines) do
        loop_body(lineno, line)
    end
end

---@param linecount integer
local function init_cardtable(linecount)
    local copies = {} ---@type CardTracker[]
    for i = 1, linecount do
        copies[i] = {cardnumber = i, instances = 1}
    end
    return copies
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    -- argv[0] not counted by the `#` operator
    if (argc ~= 0) and (argc ~= 1) then
        printf("Usage: lua %s [puzzle_input].txt", argv[0])
        return 1
    end
    -- Left side is winning numbers, right side is your numbers.
    -- first match is worth 1 point, every match afterwards doubles that value.
    local lines = get_cardlist((argc == 1 and argv[1]) or "sample.txt")
    local cards = init_cardtable(#lines)
    tally_winnings(lines, function(lineno, line)
        local colon = line:find(":")
        local barline = line:find("|")
        local winning = line:sub(colon + 1, barline - 1)
        local playing = line:sub(barline + 1)
        local matches = get_matches(winning, playing)
        printf("%s - matches: %i\n", line, matches)
        for i = 1, matches do
            -- Current lineno always keeps track of #copies to be made
            local copies = cards[lineno].instances
            local target = cards[lineno + i] -- offset/cascade downwards
            target.instances = copies + target.instances
        end
        printf("\n")
    end)
    -- 27742 not right, too high
    -- EDIT: Was 26346
    local points = 0
    for _, card in ipairs(cards) do
        points = points + card.instances
    end
    printf("\n<POINTS>: %i\n", points)
    return 0
end

return main(#arg, arg)
