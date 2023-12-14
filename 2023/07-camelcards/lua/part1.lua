require "camelcards" 

local PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("/2023/07-camelcards/")
local FALLBACK = PROJECT_DIR .. "sample.txt"

---@param hand str Full string as retrieved from `string.match`.
---@param cards str[] Array of `hand` split into chars, use `string.toarray`.
local function parse_hand(hand, cards)
    printf("%s: ", hand)
    local _Counter = {}
    for _, char in ipairs(cards) do
        printf("['%s' => %i], ", char, CAMELCARDS.CARD_STRENGTH[char])
        if not _Counter[char] then
            _Counter[char] = 0
        end
        _Counter[char] = _Counter[char] + 1
    end
    printf("\n")
    for char, count in pairs(_Counter) do
        printf("\t'%s' occurs %i times.\n", char, count)
    end
    printf("\n")
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    print_table(CAMELCARDS.CARD_STRENGTH, "CAMELCARDS.CARDS")
    print_table(CAMELCARDS.HAND_STRENGTH, "CAMELCARDS.HAND_STRENGTH")
    for i, line in ipairs(lines) do
        local hand, bid = line:match("(%w+) (%w+)") ---@type str, str
        -- printf("| Hand[%i]: %s | Bid: %s\n", i, hand, bid)
        parse_hand(hand, hand:toarray())
    end
    return 0
end

return main(#arg, arg)
