require("util/common")
require("util/stdio")
require("util/string")
require("util/prettyprint")

require("boatrace")

local PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("/2023/06-waitforit/")
local FALLBACK = PROJECT_DIR .. "sample.txt"

---@param lines str[]
local function make_races_db(lines)
    ---@type BoatRace
    return {
        duration = make_compoundnumber(lines[1]),
        distance = make_compoundnumber(lines[2])
    }
end

---@param race BoatRace
local function count_ways_to_win(race)
    local boat = {
        speed = 0,
        timeleft = race.duration,
        distance = 0,
    }
    local wins = 0
    -- See explanation in part 1 :)
    local _SearchArea = math.floor(race.duration / 2)
    for i = 0, _SearchArea do
        if boat.distance > race.distance then
            wins = wins + 1
        end
        boat.speed = boat.speed + 1
        boat.timeleft = boat.timeleft - 1
        boat.distance = boat.speed * boat.timeleft
    end
    wins = wins * 2 -- account for the fact we only searched a mirror / a half
    -- See explanation in part 1 :)
    return ((race.duration % 2) == 0 and wins - 1) or wins
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    local race = make_races_db(lines)
    print(race.duration.."ms", race.distance.."mm/ms")
    printf("%.0f ways to win\n", count_ways_to_win(race))
    return 0
end

return main(#arg, arg)
