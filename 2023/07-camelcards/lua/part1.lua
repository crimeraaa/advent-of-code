require "camelcards" 

local PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("/2023/07-camelcards/")
local FALLBACK = PROJECT_DIR .. "sample.txt"

---@param pairs_count 0|1|2
---@param of_a_kind_count 0|3|4|5
local function get_hand_type(pairs_count, of_a_kind_count)
    if (pairs_count > 0) and (of_a_kind_count == 0) then
        return CAMELCARDS.HAND_TYPES.PAIR[pairs_count] 
    elseif (pairs_count == 1) and (of_a_kind_count == 3) then
        return "FULL_HOUSE" -- stronger than THREE_OF_A_KIND so prioritize it
    elseif (pairs_count == 0) and (of_a_kind_count > 0) then
        -- has indexes [1...5] tho [1...2] are unused
        -- I'm assuming we'll only ever be able to index [3...5] by this point.
        return CAMELCARDS.HAND_TYPES.OF_A_KIND[of_a_kind_count]
    end
    return "HIGH_CARD" -- prolly no cards were similar, no pairs, no of a kinds
end

---@param chars str[] hand split into its individual chars for easier iteration.
local function parse_hand(chars)
    -- Character lookup table, same queries but set each value reset to 0.
    ---@type tbl<str, int> 
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

---@param x CamelCardsHand
---@param y CamelCardsHand
local function compare_hands(x, y)
    local _CardStrengths = CAMELCARDS.CARD_STRENGTH
    -- Compare both hands, char by char
    for ii = 1, #x.chars do
        local c, d = x.chars[ii], y.chars[ii]
        -- Skip same chars as we don't want early return just yet
        if c ~= d then
            return _CardStrengths[c] < _CardStrengths[d] 
        end
    end
    -- with return true:  250118459
    -- with return false: 250120186, this one is correct!
    return false -- I suppose it makes sense since no strength is greater
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
