require("util/common")
require("util/stdio")
require("util/string")
require("util/prettyprint")

require("boatrace")

local PROJECT_DIR = WORKSPACE_DIR .. convert_OSpath("/2023/06-waitforit/")
local FALLBACK = PROJECT_DIR .. "sample.txt"

---@param lines str[]
local function make_races_db(lines)
    local times = extract_numbers(lines[1])
    local distances = extract_numbers(lines[2])
    
    -- If the equality check fails, this is assigned `false` automatically.
    local count = (#times == #distances) and #times
    assert(count, "times and distances tables cannot be merged!")

    local db = {} ---@type BoatRace[]
    for i = 1, #times do
        db[i] = {distance = distances[i], duration = times[i]}
    end
    return db
end

local _TimeUsed = "Elapsed (ms)"
local _Speed = "Speed (mm/ms)"
local _TimeLeft = "Time Left (ms)"
local _Traveled = "Traveled (mm)"
local _Beaten = "Beats Record?"
local _FormatPad = math.max(#_TimeUsed, #_Speed, #_TimeLeft, #_Traveled, #_Beaten)
local _FormatStr = make_padded_format(_FormatPad, 5)

---@param index int
---@param race BoatRace
local function find_race_timings(index, race)
    local _Duration = string.format("duration: %3i milliseconds", race.duration)
    local _Distance = string.format("record: %3i millimeters", race.distance)
    printf("%i: %-30s %-30s\n", index, _Duration, _Distance)
    -- For each millisecond you hold the button, speed increases by 1.
    local boat = {
        speed = 0,
        timeleft = race.duration,
        distance = 0,
    }
    printf(_FormatStr, _TimeUsed, _Speed, _TimeLeft, _Traveled, _Beaten)
    local wins = 0
    -- Multiples are mirrored about the midpoint of the entire range.
    local _SearchArea = math.floor(race.duration / 2)
    for i = 0, _SearchArea do
        local beats = "false"
        if boat.distance > race.distance then
            wins = wins + 1
            beats = "true"
        end
        printf(_FormatStr, i, boat.speed, boat.timeleft, boat.distance, beats)
        boat.speed = boat.speed + 1
        boat.timeleft = boat.timeleft - 1
        boat.distance = boat.timeleft * boat.speed
    end
    wins = wins * 2 -- adjust since we only searched 1 half of the mirror
    
    -- e.g. 30 milliseconds [0ms...30ms] results in searching [0ms...15ms].
    -- We cannot search this block's halves evenly as we have a midpoint.
    --      [11ms...14ms]   (upper half)
    --      [15ms]          (midpoint, ends up on one half but not the other!)
    --      [16ms...19ms]   (lower half, mirror of upper)
    -- So when race duration is even, gotta adjust for the midpoint.
    if (race.duration % 2) == 0 then
        wins = wins - 1
    end
    printf("Ways to win: %.0f\n\n", wins)
    return wins
end

---@param argc int
---@param argv str[]
local function main(argc, argv)
    local lines = readfile((argc == 1 and argv[1]) or FALLBACK)
    local races_db = make_races_db(lines)
    local total = 1 -- Start at 1 since we need to multiply
    for i, race in ipairs(races_db) do
        total = total * find_race_timings(i, race)
    end
    printf("\nTotal ways to win: %i\n", total)
    return 0
end

return main(#arg, arg)
