-- requires will cascade (i.e. be available) to the caller
require "util/common"
require "util/stdio"
require "util/string"
require "util/table"
require "util/prettyprint"

CAMELCARDS = {}

-- "Query-able" hashtable for card strength values. 
-- `Highest: 'A' => Lowest: '2'`
-- 
-- TODO: We can hardcode AKQJT, then 9-2 then can be mathematically determined?
---@type tbl<str, int> 
CAMELCARDS.CARD_STRENGTH = table.invert(("AKQJT98765432"):reverse():toarray())

-- I typed out the arrangement in order of how they appear on the website.
-- It doesn't really matter but: https://adventofcode.com/2023/day/7
-- 
-- TODO: Determine mapping strength based on card counts?
-- HIGH_CARD:           No repeated cards.
-- 
--      23456
--      HIGH_CARD has the lowest priority, all others take precedence over it.
-- 
-- [1...2]_PAIR:        [1...2] pairs of [1...2] characters.
-- 
--      33, 22 in "3223T"; 
--      KK, 77 in "KK677";
--      ONE_PAIR and TWO_PAIR are higher priority than HIGH_CARD,
--      but lower than every other category below.
-- 
-- [3..5]_OF_A_KIND:    [3..5] occurences of any 1 character.
-- 
--      QQQ   in "QQQJA"; 
--      AAAAA in "AAAAA"; 
--      555   in "T55J5";
-- 
-- FULL_HOUSE:          3 cards w/ same label, remaining 2 another same label.
-- 
--      "23332"
--      FULL_HOUSE has the highest priority.
CAMELCARDS.HAND_STRENGTH = table.invert{
    FIVE_OF_A_KIND = 6,
    FOUR_OF_A_KIND = 5,
    FULL_HOUSE = 4,
    THREE_OF_A_KIND = 3,
    TWO_PAIR = 2,
    ONE_PAIR = 1,
    HIGH_CARD = 0,
}
