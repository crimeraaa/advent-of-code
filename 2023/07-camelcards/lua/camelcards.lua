-- requires will cascade (i.e. be available) to the caller
require "util/common"
require "util/stdio"
require "util/string"
require "util/table"
require "util/prettyprint"

PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("/2023/07-camelcards/")
FALLBACK = PROJECT_DIR .. "sample.txt"

---@class CamelCardsHand
---@field chars str[] Original hand string split into individual characters.
---@field bid num Bid amount to be multiplied by the card's ranking.
---@field type str Hand's type, see: `get_hand_type`.

---@alias LookupTable { [str]:int}

CAMELCARDS = {}

-- "Query-able" hashtable for card strength values. 
-- `Highest: 'A' => Lowest: '2'`.
-- 
-- We reverse the string first so looking up 'A' gives a higher value than '2'.
---@type LookupTable
CAMELCARDS.CARD_STRENGTH = table.invert(string.reverse("AKQJT98765432"):toarray())

-- I typed out the arrangement in order of how they appear on the website.
-- It doesn't really matter but: https://adventofcode.com/2023/day/7
CAMELCARDS.HAND_TYPES = {
    PAIR = {"ONE_PAIR", "TWO_PAIR"},
    OF_A_KIND = {
        "ONE_OF_A_KIND", -- unused but I just want indexes to make sense
        "TWO_OF_A_KIND", -- ^ditto
        "THREE_OF_A_KIND", 
        "FOUR_OF_A_KIND", 
        "FIVE_OF_A_KIND"
    }
}

-- Written deliberately in reverse order. String array turned into lookup table.
---@type LookupTable
CAMELCARDS.HAND_STRENGTHS = table.invert{
    "HIGH_CARD",
    "ONE_PAIR",
    "TWO_PAIR",
    "THREE_OF_A_KIND", 
    "FULL_HOUSE",
    "FOUR_OF_A_KIND", 
    "FIVE_OF_A_KIND"
}

-- Assume all our hands have 5 cards.
CAMELCARDS.HAND_LENGTH = 5

---@param pairs_count 0|1|2
---@param of_a_kind_count 0|3|4|5
function get_hand_type(pairs_count, of_a_kind_count)
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

---@param x CamelCardsHand
---@param y CamelCardsHand
function compare_hands(x, y)
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

--[[------------------ HAND STRENGTHS AND PRIORITIES NOTES ---------------------

HIGH_CARD 
    DESCRIPTION:
    Hand contains no repeated cards.

    EXAMPLE/S:
    None    in  "23456"
    None    in  "AKQTJ"
    None    in  "39QK6"

    PRIORITY:
    Lowest.

ONE_PAIR, TWO_PAIR:        
    DESCRIPTION:
    1 or 2 cards in the hand occurs exactly 2 times.

    EXAMPLE/S:
    33      in  "32T3K"
    33, 22  in  "3223T" 
    KK, 77  in  "KK677"

    PRIORITY:
    Higher than HIGH_CARD, but lower than categories seen below.

THREE_OF_A_KIND, FOUR_OF_A_KIND, FIVE_OF_A_KIND:
    DESCRIPTION:
    3 up to 5 occurences of any 1 character.

    EXAMPLE/S:
    QQQ     in  "QQQJA"
    AAAAA   in  "AAAAA"
    KKKK    in  "KKTKK"
    555     in  "T55J5"

    EXCEPTIONS:
    333     in  "32233"
    This is because there is ONE_PAIR (22) and THREE_OF_A_KIND (333),
    this matches FULL_HOUSE which takes priority over THREE_OF_A_KIND.

FULL_HOUSE:          
    DESCIPRTION:
    3 cards in the hand are the same card, and the remaining 2 match each other.

    EXAMPLE/S:
    333, 22 in  "23332"
    QQQ, TT in  "QQQJJ"

    PRIORITY:
    Highest.

------------------------------------------------------------------------------]]
