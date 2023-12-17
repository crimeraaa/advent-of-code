require "camelcards" 

---@param chars str[] hand split into its individual chars for easier iteration.
local function parse_hand(chars)
    -- Character lookup table, same queries but set each value reset to 0.
    ---@type LookupTable  
    local _Counter = table.copy(CAMELCARDS.CARD_STRENGTH, function(v) return 0 end)
    for _, char in ipairs(chars) do
        _Counter[char] = _Counter[char] + 1
    end
    local _OfKind, _Pairs = 0, 0
    --? For more info see NOTES section in `./camelcards.lua`
    for _, count in pairs(_Counter) do
        if count >= 3 then
            _OfKind = count
        elseif count == 2 then
            _Pairs = _Pairs + 1
        end
    end
    return get_hand_type(_Pairs, _OfKind)
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    local hands = {} ---@type CamelCardsHand[]

    for _, line in ipairs(lines) do
        local _Hand, _Bid = line:match("(%w+) (%w+)") ---@type str, str
        local _CharArray = _Hand:toarray()
        hands[#hands+1] = {
            chars = _CharArray,
            bid = tonumber(_Bid),
            type = parse_hand(_CharArray)
        }
    end

    -- Sort from weakest to strongest card hand types.
    table.sort(hands, function(x, y)
        local _HandStrengths = CAMELCARDS.HAND_STRENGTHS
        if x.type == y.type then -- compare first unlike char for strength
            return compare_hands(x, y)
        end
        return _HandStrengths[x.type] < _HandStrengths[y.type]
    end)

    local winnings = 0
    for rank, hand in pairs(hands) do
        winnings = winnings + (hand.bid * rank)
        print(rank, table.concat(hand.chars), hand.bid, hand.type, winnings)
    end
    printf("Winnings: %.0f\n", winnings)
    return 0
end

return main(#arg, arg)
