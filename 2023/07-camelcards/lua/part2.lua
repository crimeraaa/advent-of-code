require "camelcards" 

-- In part 2, Joker cards ('J') have the lowest indvidual card strength,
-- however they apply special rules when determining the hand type.
-- 
-- Jokers cause hand type to be considered the best possible case, e.g:
-- `"QJJQ2"` is treated as `"QQQQ2"` to get the type `FOUR_OF_A_KIND`.
-- 
-- HOWEVER, when breaking ties between cards with similar (determined) types,
-- 'J' is treated as is in terms of individual card strength.
CAMELCARDS.CARD_STRENGTH = table.invert(string.reverse("AKQT98765432J"):toarray())

---@param pairs int
---@param ofkinds int
---@param jokers int
local function adjust_counters(pairs, ofkinds, jokers)
    if (ofkinds == 0) and (pairs > 0) then
        local _NonJokerPairs = (jokers == 2 and pairs - 1) or pairs
        if _NonJokerPairs == 2 then
            ofkinds = 3
            pairs = 1
        else
            local _Type = (_NonJokerPairs * 2) + jokers
            ofkinds = ((_Type <= 2) and _Type + 1) or _Type
            pairs = 0
        end
    elseif (ofkinds > 0) and (pairs == 0) then
        -- Edge cases: "JJJJQ" and "JJJJJ" so clip calculation if over 5
        ofkinds = math.min(ofkinds + jokers, CAMELCARDS.HAND_LENGTH)
        pairs = 0
    elseif (ofkinds == 0) and (pairs == 0) then
        pairs = jokers 
        ofkinds = 0 
    else
        ofkinds = ofkinds + (pairs * 2)
        pairs = 0
    end
    return pairs, ofkinds
end

---@param chars str[] hand split into its individual chars for easier iteration.
local function parse_hand(chars)
    -- Character lookup table, same queries but set each value reset to 0.
    ---@type tbl<str, int> 
    local _CharToCount = table.copy(CAMELCARDS.CARD_STRENGTH, function(v) return 0 end)
    for _, char in ipairs(chars) do
        _CharToCount[char] = _CharToCount[char] + 1
    end
    local _OfKind, _Pairs, _Jokers = 0, 0, 0
    --? For more info see NOTES section in `./camelcards.lua`
    for char, count in pairs(_CharToCount) do
        if char == 'J' then
            _Jokers = count
        end
        if count >= 3 then
            _OfKind = count
        elseif count == 2 then 
            _Pairs = _Pairs + 1
        end
    end
    print(table.concat(chars), _OfKind .. " of a kind", _Pairs .. " pairs", _Jokers .. "  jokers")
    if _Jokers > 0 then
        _Pairs, _OfKind = adjust_counters(_Pairs, _OfKind, _Jokers)
    end
    return get_hand_type(_Pairs, _OfKind)
end
--[[-------------------------------- NOTES -------------------------------------
823J3 (ONE_PAIR) => 82333 (THREE_OF_A_KIND)

KTJJT (ONE_PAIR) => KTTTT (FOUR_OF_A_KIND)
    1 pairs, 0 ofkind, 2 Jokers => 0 pairs, 4 ofkind

QTTJ4 (ONE_PAIR) => QTTT4 (THREE_OF_A_KIND)
    1 pairs, 0 ofkind, 1 Jokers => 0 pairs, 3 ofkind

KKJ77 (TWO_PAIR) => KKK77 (FULL_HOUSE)
    2 pairs, 0 ofkind, 2 Jokers => (1 pair, 3 ofkind)

3KKJ3 (2 NON-JOKER PAIRS) => 3KK33 (FULL_HOUSE)

KTJJT (1 NON-JOKER PAIR) => KTTTT (FOUR_OF_A_KIND)
3ATJJ (0 NON-JOKER PAIRs) => 3ATTT (THREE_OF_A_KIND)

QQQJA (THREE_OF_A_KIND) => QQQQA (FOUR_OF_A_KIND)

468JA (HIGH_CARD) => 468AA (ONE_PAIR)
J2KA4 (HIGH_CARD) => 22KA4 (ONE_PAIR)
J5JJ5 (FULL_HOUSE) => 55555 (FIVE_OF_A_KIND)
------------------------------------------------------------------------------]]

-- Answer: 250665248
---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    local hands = {} ---@type CamelCardsHand[]

    for i, line in ipairs(lines) do
        local _Hand, _Bid = line:match("(%w+) (%w+)") ---@type str, str
        local _CharArray = _Hand:toarray()
        -- TODO: determine preferred type if Joker found
        hands[#hands+1] = {
            chars = _CharArray,
            bid = tonumber(_Bid),
            type = parse_hand(_CharArray) 
        }
    end

    -- Sort from weakest to strongest card hand types.
    table.sort(hands, function(x, y)
        local _HandStrengths = CAMELCARDS.HAND_STRENGTHS
        -- printf("Comparing %s and %s\n", table.concat(x.chars), table.concat(y.chars))
        -- print(x.type, y.type)
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
